//
//  Copyright (C) 2014 Danny Havenith
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef TWO_WIRE_HPP_
#define TWO_WIRE_HPP_
#include "avr_utilities/pin_definitions.hpp"

namespace two_wire {

/// Simple two-wire device.
/// This class controls the clock and io line and supports read and write operations
/// The template argument pin_definitions must have two members of type pin_definition<...>
/// with names "io" and "clk".
template< typename pin_definitions>
class two_wire {
public:
	static void init()
	{
		reset( pins.clk);
		make_output( pins.clk);
		make_input( pins.io); // start io in high-Z mode.
	}

	static uint8_t read_byte( uint8_t command)
	{
		make_output( pins.io);
		send_byte( command);
		make_input( pins.io);
		uint8_t result = receive_byte();
		reset( pins.clk);
		return result;
	}

	static void write_byte( uint8_t command, uint8_t value)
	{
		make_output( pins.io);
		send_byte( command);
		send_byte( value);
		reset( pins.clk);
		make_input( pins.io);
	}

private:
	static pin_definitions pins;

	/// Send a byte and leave the clock in high state.
	///
	/// This function sends 8 bits, setting the clock after each bit and reseting it
	/// just before changing the bits. After 8 bits have been sent, the clock is left in high
	/// state.
	/// This function also assumes that the data direction of pins.io has been set to
	/// input.
	static void send_byte( uint8_t command)
	{
		for (uint8_t mask = 1; mask; mask <<= 1)
		{
			reset( pins.clk);
			write( pins.io, command & mask);
			set( pins.clk);
		}
	}

	/// Read a byte, assuming a command byte was sent and the clock is in the high state.
	static uint8_t receive_byte()
	{
		uint8_t result = 0;
		for (uint8_t mask = 1; mask; mask <<= 1)
		{
			reset( pins.clk);
			asm volatile("nop\n\t" ::);
			asm volatile("nop\n\t" ::);

			// assume we're slow enough to read the io line now.
			if (is_set( pins.io))
			{
				result |= mask;
			}

			set( pins.clk);
		}
		return result;
	}
};

}  // namespace two_wire
#endif /* TWO_WIRE_HPP_ */
