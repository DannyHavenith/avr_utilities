/*
 * display_buffer.hpp
 *
 *  Created on: Aug 17, 2014
 *      Author: danny
 */

#ifndef DISPLAY_BUFFER_HPP_
#define DISPLAY_BUFFER_HPP_
#include <stdint.h>
#include <string.h> // for memset

namespace max7219
{
static const uint16_t decode_mode  = 0x0900;
static const uint16_t intensity    = 0x0a00;
static const uint16_t scan_limit   = 0x0b00;
static const uint16_t shutdown     = 0x0c00;
static const uint16_t display_test = 0x0f00;

/**
 *
 * Represent an array of 8x8 led matrices driven by MAX7219 ICs.
 *
 * This implementation assumes that the matrices have been wired to map rows to bytes, which is not the most efficient
 * wiring for this particular implementation. All data to be sent to the display is buffered internally and
 * only transmitted when transmit() is called.
 *
 * The number of displays is the first template argument.
 * The second argument is the spi device type used to communicate with the MAX7219s.
 * Finally, a type must be given that represents the chip select pin.
 *
 */
template< int display_count, typename spi_type, typename csk_type>
class display_buffer
{
public:
	display_buffer()
    :auto_shift_enabled(true)
	{
		spi_type::init();
		init();
		clear();
	}

	/**
	 * clear the display buffer.
	 */
	void clear()
	{
		current_column = 0;
		memset( buffer, 0, sizeof buffer);
	}

	/**
	 * Transmit the contents of the display buffer to the MAX7219s to drive the
	 * LED matrices.
	 */
	void transmit() const
	{
		for (uint8_t line = 0; line < 8; ++line)
		{
			const uint8_t *lineptr = buffer[line];
			reset( csk);
			for (uint8_t display  = display_count; display; --display)
			{
				spi_type::transmit( (((line&0x0f)+1)<<8) | *lineptr++ );
			}
			set( csk);
		}
	}

	/**
	 * Push a column of data to the display buffer, this will advance an internal
	 * cursor to the next column automatically.
	 *
	 * If auto-shift is enabled, any column that is pushed beyond the rightmost
	 * column will automatically shift the data in the display buffer to the left.
	 */
	void push_column( uint8_t value)
	{
		if (current_column >= 8 * display_count)
		{
		    if (!auto_shift_enabled) return;
			shift_left();
			current_column = 8 * display_count -1;
		}

		const uint8_t mask = 1 << (7 - (current_column%8));
		const uint8_t offset = current_column / 8;
		for ( uint8_t bit = 0; bit < 8; ++bit)
		{
			if (value&0x01) buffer[bit][offset] |= mask;
			value >>= 1;
		}
		++current_column;

	}

	/**
	 * Shift the contents of the data buffer one column to the left.
	 */
	void shift_left()
	{
		for (int8_t row = 8; row>=0; --row)
		{
			uint8_t *end = (&buffer[row][0]) -1;
			uint8_t *begin = (&buffer[row][display_count-1]);
			uint8_t carry = 0;
			while (begin != end)
			{
				uint8_t new_carry = (*begin & 0x80)?1:0;
				*begin = (*begin << 1) + carry;
				carry = new_carry;
				--begin;
			}
		}
	}

	/**
	 * Set- or reset auto-shift mode.
	 */
	void auto_shift( bool value)
	{
	    auto_shift_enabled = value;
	}

private:

	/**
	 * send a SPI command to the chained MAX7219s
	 */
	static void send( uint16_t command, uint8_t count = display_count)
	{
		reset( csk);
		while (count--) spi_type::transmit( command);
		set( csk);
	}

	static void init( uint8_t count = display_count)
	{
		set( csk);
		make_output( csk);

		send( decode_mode | 0, count); // no decoding on all digits
		send( shutdown    | 1, count); // switch on
		send( intensity   | 0, count); // halfway intensity
		send( scan_limit  | 7, count); // all digits/columns
		send( display_test| 0, count);
	}

	bool    auto_shift_enabled;
	static csk_type csk;
	uint8_t  current_column;
	uint8_t  buffer[8][display_count];
};

}


#endif /* DISPLAY_BUFFER_HPP_ */
