/*
 * boxtel_transmitter.hpp
 *
 *  Created on: May 19, 2012
 *      Author: danny
 */

#ifndef BOXTEL_TRANSMITTER_HPP_
#define BOXTEL_TRANSMITTER_HPP_
#include <stdint.h>
#include "round_robin_buffer.h"

namespace boxtel_packets
{
/**
 * Class that sends data to the serial port in boxtel-encoded format.
 *
 * Boxtel encoding sends each byte of data as two bytes: one byte with the even bits and one byte with the odd bits.
 * The 'unused' bits in each byte contain the complement of the used bits. That way, boxtel-encoded data holds at most
 * two consecutive bits with the same value.
 *
 * Boxtel coding is suited for communications over unreliable media that require a certain frequency of bit-changes,
 * such as 433Mhz transmitters.
 *
 * To use this class, an application should:
 * - initialize the serial port in the usual way
 * - create an instance of this class.
 * - implement a UDRE-interrupt handler, that calls this class' uart_empty_interrupt member function, for instance as follows:
 * \code
 *  /// Data register empty interrupt
 *  ISR( USART_UDRE_vect)
 *  {
 *      my_transmitter.uart_empty_interrupt();
 *  }
 * \endcode
 *
 * After this has been done, an application can prepare messages to be send by calling the append member function and when finished,
 * should call the commit member function. After commit has been called, the appended data will be send in a packet as described in
 * the \see uart_empty_interrupt function.
 *
 * The class relies on the application program to make sure that uart_empty_interrupt is called whenever
 * the UART data register is empty, this could be done by calling that function from the UDRE interrupt or
 * after polling the UDRE flag.
 */
struct transmitter
{
public:

    /// returns true if the transmitter is in the middle of sending bytes.
    bool is_busy() const volatile
    {
        return !state == idle;
    }

    /// commit all appends since the previous commit.
    /// this will actually send the appended bytes to output.
    void commit() volatile
    {
        buffer.commit();
        cli();
        if (state == idle)
        {
            // re-enable interrupt
            UCSR0B |= (1 << UDRIE0);
            state = preamble;
            bytes_left = long_preamble_size;
            UDR0 = 0; // start the UART and its interrupts
        }
        sei();
    }

    /// cancel all appends since the last commit
    void abort() volatile
    {
        buffer.reset_tentative();
    }

    /**
     * Offer a byte for tentative write.
     *
     * Note that nothing is transmitted until commit is called.
     * Typically an application will add one logical 'packet' of byte data using this function and the call commit.
     */
    bool append( uint8_t byte) volatile
    {
        return buffer.write_tentative( byte);
    }

    /**
     * Offer a 16-bit word for tentative write. This word will be added in big-endian format to the buffer for transmit.
     *
     * Note that nothing is transmitted until commit is called.
     * Typically an application will add one logical 'packet' of data using this function and the call commit.
     */
    bool append( uint16_t word) volatile
    {
        bool result = buffer.write_tentative( word >> 8);
        return result && buffer.write_tentative( word);
    }

    /**
     * This function must be called by the application program whenever the UART data register is empty.
     * Normally, this is done from the UDRE-interrupt, for example as follows:
     *
     * \code
     *  /// Data register empty interrupt
     *  ISR( USART_UDRE_vect)
     *  {
     *      my_transmitter.uart_empty_interrupt();
     *  }
     * \endcode
     *
     *
     * This function implements a simple state machine that sends packets.
     * The packets consist of:
     * - n (n >= 1) preamble bytes
     * - end-of-preamble (0x55)
     * - an address-nibble
     * - a size-byte
     * - at most 16 bytes of data
     *
     * Each byte is sent as a boxtel-encoded word of 16 bits. The address nibble is sent as a specially coded boxtel-nibble.
     * see transmitter::boxtel_odd, boxtel_even and boxtel_nibble.
     *
     */
    void uart_empty_interrupt() volatile
    {
        switch (state)
        {
        case end_of_message:
            // stop transmission if there are no more bytes to send
            // otherwise start a new package
            if (buffer.empty())
            {
                state = idle;
                UCSR0B &= ~(1 << UDRIE0); // disable interrupt, we're idle
            }
            else
            {
                // no preamble, we're sending this packet right after a previous one, marked with an end-of-preamble byte
                state = send_addr_even;
                UDR0 = 0x55;
            }
            break;
        case preamble:
            // more preamble bytes to send?
            if (--bytes_left)
            {
                UDR0 = 0x33;
            }
            else
            {
                // send end-of-preamble (start-of-packet)
                UDR0 = 0x55;
                state = send_addr_even;
            }
            break;
        case send_addr_even:
            // send the even bits of the packet address.
            UDR0 = boxtel_even( address);
            state = send_addr_odd;
            break;
        case send_addr_odd:
            // send the odd bits of the packet address.
            UDR0 = boxtel_odd( address);
            state = size;
            bytes_left = buffer.size();
            break;
        case size:
            // send the packet size.
            // The packet size is 1-16 and is encoded in 4 bits, which get send in a special nibble-version of boxtel-encoding.
            {
                // at most 16 bytes per packet.
                uint8_t size = min( (uint8_t)16, buffer.size());
                bytes_left = size;
                UDR0 = boxtel_nibble( bytes_left - 1); // the protocol requires size-1 to be sent
                state = payload_even;
            }
            break;
        case payload_even:
            {
                // transmit the even bits of a payload byte
                uint8_t byte = 0;
                buffer.get_first( &byte);
                UDR0 = boxtel_even( byte);
                state = payload_odd;
            }
            break;
        case payload_odd:
            {
                // transmit the odd bits of a payload byte
                uint8_t byte = 0;
                buffer.read( &byte);
                UDR0 = boxtel_odd( byte);
                if (!--bytes_left)
                {
                    state = payload_even;
                }
                else
                {
                    state = end_of_message;
                }
            }
            break;
        default:
            // do nothing
            break;
        }
    }

private:

    /// Create a boxtel-coded byte with the even bits of a byte
    /// In a boxtel-coded byte each even bit is the complement of the bit to
    /// its left. A boxtel-coded byte has only 4 bits of information, padded with complementing
    /// bits. This acts both as a checksum and a way of creating a nicely alternating bit-pattern.
    /// In boxtel-coding, for each byte first the even bits are sent, with odd bits as padding and then
    /// the odd bits are sent, with even bits as padding.
    static inline uint8_t boxtel_even( uint8_t value)
    {
        return (value & 0x55) | (((~value)<<1) & 0xAA);
    }

    /// Create a boxtel-coded byte with the odd bits of a byte
    /// In a boxtel-coded byte each even bit is the complement of the bit to
    /// its left. A boxtel-coded byte has only 4 bits of information, padded with complementing
    /// bits. This acts both as a checksum and a way of creating a nicely alternating bit-pattern.
    /// In a boxtel_even byte, the even bits hold the true values and the odd bits are padding, in a boxtel_odd
    /// byte, the roles are reversed: odd bits have true values and even bits are padding.
    /// In boxtel-coding, for each byte first the even bits are sent, with odd bits as padding and then
    /// the odd bits are sent, with even bits as padding.
    static inline uint8_t boxtel_odd( uint8_t value)
    {
        return (value & 0xAA) | (((~value)>>1) & 0x55);
    }

    /// create a boxtel-coded byte out of the lower 4 bits of the input byte.
    /// given a byte with the following bit-values ( '3' stands for the bit value at position 3):
    ///  xxxx3210, this function creates a byte with bit values 2x0x3x1x and then computes a boxtel_odd
    /// for that value.
    static inline uint8_t boxtel_nibble( uint8_t value)
    {
        value &= 0x0f;
        return boxtel_odd( value | (value <<5));
    }

    // we don't have <algorithm> available
    template< typename T>
    static T min( T left, T right)
    {
        return (left<right)?left:right;
    }

    enum {
        idle = 0, // explicitly 0, to make use of default zero-initialisation
        end_of_message,
        preamble,
        send_addr_even,
        send_addr_odd,
        size,
        payload_even,
        payload_odd
    } state;


    // use a 32-byte buffer to prepare data for the transmitter
    round_robin_buffer<32> buffer;

    static const uint8_t long_preamble_size = 10;
    uint8_t bytes_left;
    uint8_t address;
};
}



#endif /* BOXTEL_TRANSMITTER_HPP_ */
