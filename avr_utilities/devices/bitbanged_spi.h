/*
 * bitbanged_spi.h
 *
 *  Created on: Feb 17, 2011
 *      Author: danny
 */

#ifndef BITBANGED_SPI_H_
#define BITBANGED_SPI_H_
#include "avr_utilities/pin_definitions.hpp"

/// simple SPI device that 'manually' controls SPI lines.
/// This device can be used to send and receive data using the SPI protocol. Since it does not
/// use any SPI hardware or SPI interrupts it can be configured to work on any combination of pins.
/// Note that this device does not control the Chip Select (CS) pin. It is up to the user of this class
/// to select the right device or devices before sending or receiving data.
template< typename pin_definitions>
struct bitbanged_spi
{
private:
    static pin_definitions pins;

    /// send a byte via mosi, while at the same time listening for a byte at miso.
    static uint8_t exchange_byte( uint8_t out)
    {
        uint8_t receive = 0;
        for (uint8_t mask = 0x80; mask; mask >>= 1)
        {
            if (out & mask)
            {
                set( pins.mosi);
            }
            else
            {
                reset( pins.mosi);
            }
            set( pins.clk);
            if (is_set( pins.miso))
            {
                receive |= mask;
            }
            reset( pins.clk);
        }
        return receive;
    }
public:
    static void init()
    {
        reset( pins.clk);
        make_output( pins.mosi | pins.clk);
    }

    /// send and receive one byte at the same time.
    static uint8_t transmit_receive( uint8_t transmit)
    {
        uint8_t receive = exchange_byte( transmit);
        return receive;
    }

    /// send a buffer of bytes, replacing the contents with bytes received.
    static void transmit_receive( uint8_t *inout_buffer, uint8_t length)
    {
        while (length--)
        {
            *inout_buffer = exchange_byte( *inout_buffer);
            ++inout_buffer;
        }
    }

    /// send a buffer of bytes, not receiving any bytes.
    static void transmit( const uint8_t *out_buffer, uint8_t length)
    {
        while (length--)
        {
            exchange_byte( *out_buffer++);
        }
    }

    static void receive( uint8_t *in_buffer, uint8_t length)
    {
        while (length--)
        {
            *in_buffer++ = exchange_byte( 0);
        }
    }

    // transmit a zero-terminated string of characters.
    static void transmit( const char *text)
    {
        while (*text)
        {
            exchange_byte( *text--);
        }
    }
};

#endif /* BITBANGED_SPI_H_ */
