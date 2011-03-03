/*
 * nrf24l01.h
 *
 *  Created on: Feb 18, 2011
 *      Author: danny
 */

#ifndef NRF24L01_H_
#define NRF24L01_H_

#include <inttypes.h>
#include <util/delay.h>

#include "nrf24L01_registers.h"

template<typename pin_definitions, typename spi_device>
class nrf24l01
{
private:
    static pin_definitions pins;
    typedef spi_device spi;

public:

    static void init()
    {
        spi::init();
        reset( pins.ce);
        set( pins.csn);
        make_output( pins.ce | pins.csn);
    }

    static uint8_t write_register( uint8_t reg, uint8_t value)
    {
        reset( pins.csn);
        uint8_t status = spi::transmit_receive( W_REGISTER | (REGISTER_MASK & reg));
        spi::transmit_receive( value);
        set( pins.csn);
        return status;
    }

    static uint8_t read_register( uint8_t reg)
    {
        reset( pins.csn);
        spi::transmit_receive( R_REGISTER | (REGISTER_MASK & reg));
        uint8_t result = spi::transmit_receive(0);
        set( pins.csn);
        return result;
    }

    static uint8_t write_register( uint8_t reg, const uint8_t *values, uint8_t size)
    {
        reset( pins.csn);
        uint8_t status = spi::transmit_receive( W_REGISTER | (REGISTER_MASK & reg));
        spi::transmit( values, size);
        set( pins.csn);
        return status;
    }

    static uint8_t set_receive_address( const uint8_t *address, uint8_t size)
    {
        return write_register( RX_ADDR_P0, address, size);
    }

    template< uint8_t size>
    static uint8_t set_receive_address( const uint8_t (&address)[size])
    {
        return set_receive_address( address, size);
    }

    static uint8_t set_transmit_address( const uint8_t *address, uint8_t size)
    {
        return write_register( TX_ADDR, address, size);
    }

    template< uint8_t size>
    static uint8_t set_transmit_address( const uint8_t (&address)[size])
    {
        return set_transmit_address( address, size);
    }

    static void start_listen()
    {
        set( pins.ce);
    }

    static uint8_t get_status()
    {
        reset( pins.csn);
        uint8_t status = spi::transmit_receive( 0xff);
        set( pins.csn);
        return status;
    }

    static bool ready_to_send()
    {
        uint8_t status = get_status();
        if(status & (1<<TX_DS))
        {
            write_register(STATUS,(1<<TX_DS)|(1<<MAX_RT)); // Reset status register
            return true;
        }
        else
        {
            return false;
        }
    }

    static bool data_ready()
    {
        const uint8_t status = read_register( FIFO_STATUS);
        return (status & _BV( RX_EMPTY )) == 0;
    }

    static bool send( const uint8_t *buffer, uint8_t buffer_size)
    {
        reset( pins.ce);


//        reset( pins.csn);
//        spi::transmit_receive( FLUSH_TX);
//        set( pins.csn);

        reset( pins.csn);
        uint8_t status = spi::transmit_receive( W_TX_PAYLOAD);
        if ( status & _BV( TX_FULL))
        {
            return false;
        }

        spi::transmit( buffer, buffer_size);
        set( pins.csn);
        set( pins.ce);
        _delay_us( 10);
        reset( pins.ce);
        return true;
    }

    static void receive( uint8_t *buffer, uint8_t buffer_size)
    {
        reset( pins.csn);
        spi::transmit_receive( R_RX_PAYLOAD);
        spi::receive( buffer, buffer_size);
        set( pins.csn);
    }

    template< uint8_t size>
    static void send( const char (&buffer)[size])
    {
        send( reinterpret_cast<const uint8_t *>(buffer), size);
    }

};

#endif /* NRF24L01_H_ */
