/*
 * hd44780.hpp
 *
 *  Created on: Nov 11, 2011
 *      Author: danny
 */

#ifndef HD44780_HPP_
#define HD44780_HPP_

template< typename pin_e, typename pin_rw, typename pin_rs, typename data_pins>
class hd44780_lcd
{
public:
    typedef uint8_t byte;

    template<typename OutputInitializer>
    static void init( OutputInitializer &initializer)
    {
        initializer( e | rw | rs | data);
        reset( rs);
        _delay_ms( 15);
        outnibble
    }

    static void data_out( byte data)
    {
        outbyte( data, true);
    }

    static void command_out( byte command)
    {
        outbyte( command, false);
        wait_ready();
    }

    static byte data_in()
    {
        return inbyte( true);
    }

    static byte command_in()
    {
        return inbyte( false);
    }

private:

    // pin definitions.
    static pin_e    e;
    static pin_rw   rw;
    static pin_rs   rs;
    static data_pins data;

    static const byte BUSY_FLAG = 0x80;

    static void wait_ready()
    {
        while (command_in() & BUSY_FLAG);
    }

    static void outbyte( byte byte, bool set_rs)
    {
        if (set_rs)
        {
            set( rs);
        }
        else
        {
            reset( rs);
        }
        outnibble( byte >> 4, rs);
        outnibble( byte & 0x0f, rs);
    }

    static void outnibble( byte nibble)
    {
        reset( rw);
        write( data, nibble);
        set( e);
        delay_500ns();
        reset( e);
    }

    static byte inbyte( bool set_rs)
    {
        if (set_rs)
        {
            set( rs);
        }
        else
        {
            reset( rs);
        }
        byte result =innibble() << 4;
        return result | innibble();
    }

    static byte innibble()
    {
        byte result;
        set( rw);
        make_inputs( data);
        set( e);
        delay_500ns();
        result = read( data);
        reset( e);
        return result;
    }

    static void delay_500ns()
    {
#if F_CPU > 4000000UL
        _delay_us(0.5);
#else
  /*
   * When reading back, we need one additional NOP, as the value read
   * back from the input pin is sampled close to the beginning of a
   * CPU clock cycle, while the previous edge on the output pin is
   * generated towards the end of a CPU clock cycle.
   */
    __asm__ volatile("nop");
#  if F_CPU > 1000000UL
    __asm__ volatile("nop");
#    if F_CPU > 2000000UL
    __asm__ volatile("nop");
    __asm__ volatile("nop");
#    endif /* F_CPU > 2000000UL */
#  endif /* F_CPU > 1000000UL */
#endif

    }
};

#endif /* HD44780_HPP_ */
