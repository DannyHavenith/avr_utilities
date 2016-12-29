//
//  Copyright (C) 2015 Danny Havenith
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
/**
 * @file datasheet_registers.hpp
 * Macros and templates to convert data sheets into C++ compilable code.
 *
 * The macros in this file can be used to describe registers as they are
 * described in a typical datasheet. A macro is provided to describe a register
 * and to give a name to ranges of bits in such a register.
 *
 * All of this leads to C++ type definitions where the types encode the address
 * and structure of the registers.
 *
 */
#ifndef AVR_UTILITIES_DATASHEET_REGISTERS_HPP_
#define AVR_UTILITIES_DATASHEET_REGISTERS_HPP_
#include <stdint.h>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/tuple/elem.hpp>
#include "mapped_bits.hpp"

// helper macros for AVRUTIL_SEMI_SEQUENCE_TO_SEQUENCE
#define AVRUTIL_WRAP_SEQUENCE_0(...)            \
    ((__VA_ARGS__)) AVRUTIL_WRAP_SEQUENCE_1

#define AVRUTIL_WRAP_SEQUENCE_1(...)            \
    ((__VA_ARGS__)) AVRUTIL_WRAP_SEQUENCE_0

#define AVRUTIL_WRAP_SEQUENCE_0_END
#define AVRUTIL_WRAP_SEQUENCE_1_END

// turn a semi-sequence, e.g.  (a,b)(c)(d,e,f)(g,h) into a true
// sequence of pp tuples, e.g. ((a,b))((c))((d,e,f))((g,h))
#define AVRUTIL_SEMI_SEQUENCE_TO_SEQUENCE( semi_seq)        \
    BOOST_PP_CAT( AVRUTIL_WRAP_SEQUENCE_0 semi_seq, _END)   \
    /**/

#define AVRUTIL_DECLARE_REGISTER( name_, address_)  \
    typedef register_type<address_> name_;          \
    /**/

#define AVRUTIL_DECLARE_RANGES( regname_, ranges_seq)                   \
    BOOST_PP_SEQ_FOR_EACH( AVRUTIL_DECLARE_RANGE, regname_, ranges_seq) \
    /**/

#define AVRUTIL_DECLARE_RANGE( ignored_, regname_, variadic_tuple)          \
        bit_range< regname_, AVRUTIL_RANGE_FROM_VAR_TUPLE variadic_tuple> \
            AVRUTIL_NAME_FROM_VAR_TUPLE variadic_tuple __attribute__((unused))= {};                     \
    /**/

#define AVRUTIL_RANGE_FROM_VAR_TUPLE( name_, ...) __VA_ARGS__
#define AVRUTIL_NAME_FROM_VAR_TUPLE( name_, ...) name_

/**
 * Macro to describe a single register.
 */
#define AVRUTIL_DATASHEET_REGISTER( name_, address_, bits_semi_sequence)            \
    AVRUTIL_DECLARE_REGISTER( name_, address_)                                      \
    AVRUTIL_DECLARE_RANGES(name_, AVRUTIL_SEMI_SEQUENCE_TO_SEQUENCE( bits_semi_sequence))   \
    /**/


#endif /* AVR_UTILITIES_DATASHEET_REGISTERS_HPP_ */
