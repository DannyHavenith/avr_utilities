//
//  Copyright (C) 2015 Danny Havenith
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef AVR_UTILITIES_MAPPED_BITS_HPP_
#define AVR_UTILITIES_MAPPED_BITS_HPP_


/* A type that holds some bit values and a bit range to which these values should be
 * assigned, AND-ed or OR-ed.
 */
template< typename mapped_bit>
struct assigned_bit
{
    uint8_t value;
};

template< uint8_t address>
struct register_type
{
};

template< uint8_t address, typename value_type>
struct custom_register_type
{
};

/**
 * Type that represents a range of bits inside a register
 */
template< typename reg, uint8_t highest_bit, uint8_t lowest_bit = highest_bit>
struct mapped_bits
{
    assigned_bit< mapped_bits<reg, highest_bit, lowest_bit> > operator=(uint8_t value) const
    {
        return assigned_bit<mapped_bits<reg,highest_bit,lowest_bit> >( value);
    }
};


#endif /* AVR_UTILITIES_MAPPED_BITS_HPP_ */
