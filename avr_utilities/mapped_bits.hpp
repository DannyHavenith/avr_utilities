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
    unsigned assigned_value;
};

template< unsigned address>
struct register_type
{
};

template< unsigned address, typename value_type>
struct custom_register_type
{
};

/**
 * Template meta function that defines a default type for bit ranges of specified size.
 *
 * If for example, the bit count is 1 then the default value type will be bool, if the bit
 * count is <=8 then the default value type will be uint8_t, etc.
 */
template<unsigned bit_count>
struct default_value_type
{
    typedef typename default_value_type<bit_count+1>::type type;
};

template<>
struct default_value_type<1>
{
    typedef bool type;
};

template<>
struct default_value_type<8>
{
    typedef uint8_t type;
};
template<>
struct default_value_type<16>
{
    typedef uint16_t type;
};
template<>
struct default_value_type<32>
{
    typedef uint32_t type;
};

/**
 * Type that represents a range of bits inside a register
 */
template<
    typename reg,
    unsigned highest_bit, unsigned lowest_bit = highest_bit,
    typename value_type = typename default_value_type<highest_bit-lowest_bit+1>::type,
    value_type default_value = value_type{}
    >
struct bit_range
{
    typedef bit_range<reg, highest_bit, lowest_bit, value_type, default_value> this_type;
    assigned_bit< this_type > operator=(value_type value) const
    {
        return assigned_bit<this_type>{ static_cast<unsigned>(value)};
    }
};

template<
    typename reg,
    unsigned bit,
    bool default_value>
assigned_bit< bit_range< reg, bit, bit, bool, default_value>> operator!( const bit_range< reg, bit, bit, bool, default_value> & )
{
    return assigned_bit< bit_range< reg, bit, bit, bool, default_value>>{0};
}

#endif /* AVR_UTILITIES_MAPPED_BITS_HPP_ */
