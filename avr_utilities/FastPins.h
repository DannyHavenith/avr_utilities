//
//  Copyright (C) 2014 Danny Havenith
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef AVR_UTILITIES_ARDUINO_LIKE_HPP_
#define AVR_UTILITIES_ARDUINO_LIKE_HPP_

#include "pin_definitions.hpp"
#include "devices/bitbanged_spi.h"
#include "stdint.h"

/// This macro is used internally to create a specialisation of the DigitalPin traits template.
#define MAP_ARDUINO_DIGITAL_PIN( arduinoPin_, port_, bit_)  \
    template<>                                              \
    struct DigitalPin <arduinoPin_>                         \
    {                                                       \
        typedef PIN_TYPE( port_, bit_) type;                \
    };                                                      \
    /**/


namespace FastPinDefinitions {

    /// PinType is a traits type that maps an Arduino pin number to a pin type from pin_definitions.
    /// This can then be used to declare variables of some arduino pin type by using the arduino pin number,
    /// e.g.:
    /// @code
    /// DigitalPin<8>::type myLedPin; // led pin is arduino digital pin #8, which is AVR pin PB0
    /// @endcode
    template< int pin>
    struct DigitalPin {};

    // for those lucky enough to have a compiler that supports C++11 or higher:
#   if (__cplusplus >= 201103L)
    template< int pin>
    using DigitalPinType = DigitalPin<pin>::type;
#   endif

    // this could have been done with less lines and some more metaprogramming, but
    // I think it's a lot more readable as it is below.
    MAP_ARDUINO_DIGITAL_PIN( 0, D, 0);
    MAP_ARDUINO_DIGITAL_PIN( 1, D, 1);
    MAP_ARDUINO_DIGITAL_PIN( 2, D, 2);
    MAP_ARDUINO_DIGITAL_PIN( 3, D, 3);
    MAP_ARDUINO_DIGITAL_PIN( 4, D, 4);
    MAP_ARDUINO_DIGITAL_PIN( 5, D, 5);
    MAP_ARDUINO_DIGITAL_PIN( 6, D, 6);
    MAP_ARDUINO_DIGITAL_PIN( 7, D, 7);
    MAP_ARDUINO_DIGITAL_PIN( 8, B, 0);
    MAP_ARDUINO_DIGITAL_PIN( 9, B, 1);
    MAP_ARDUINO_DIGITAL_PIN(10, B, 2);
    MAP_ARDUINO_DIGITAL_PIN(11, B, 3);
    MAP_ARDUINO_DIGITAL_PIN(12, B, 4);
    MAP_ARDUINO_DIGITAL_PIN(13, B, 5);

    // define some keywords as tag types;
    struct InputType    {};
    struct InputPullUpType {};
    struct OutputType   {};
    struct MsbFirstType {};
    struct LsbFirstType {};

    struct HighType
    {
        operator int()
        {
            return 1;
        }
    };

    struct LowType
    {
        operator int()
        {
            return 0;
        }
    };

    // implementations of pinMode. Note that both the pin argument and
    // mode argument are now coded as different types instead of different constants.

    template<typename PinType>
    inline void pinMode( const PinType &pin, const InputType &)
    {
        pin_definitions::make_input( pin);
    }

    template<typename PinType>
    inline void pinMode( const PinType &pin, const InputPullUpType &)
    {
        make_input( pin);
        set( pin); // enable pull-up.
    }

    template<typename PinType>
    inline void pinMode( const PinType &pin, const OutputType &)
    {
        make_output( pin);
    }

    // implementations of digitalWrite.
    // the pin and value are different types instead of variables.

    template< typename PinType>
    inline void digitalWrite( const PinType &pin, const HighType &)
    {
        set( pin);
    }

    template< typename PinType>
    inline void digitalWrite( const PinType &pin, const LowType &)
    {
        reset( pin);
    }

    template< typename PinType>
    inline void digitalWrite( const PinType &pin, uint8_t value)
    {
        write( pin, value != 0);
    }

    template< typename PinType>
    inline int digitalRead( const PinType &pin)
    {
        return pin_definitions::read( pin);
    }

    namespace Detail
    {
        /// Template meta function that translates the LsbFirstType or MsbFirstType
        /// to a spi strategy type as used by the bitbanged_spi template.
        template< typename Direction>
        struct DirectionToSPIStrategy
        {
            typedef msb_first_direction type;
        };

        /// Specialization of this metafunction for LsbFirstType.
        template<>
        struct DirectionToSPIStrategy<LsbFirstType>
        {
            typedef lsb_first_direction type;
        };

        template< typename InputPin, typename OutputPin, typename ClockPin, typename Direction>
        struct SelectSpi
        {

            struct PinDefinitions
            {
                OutputPin mosi;
                InputPin  miso;
                ClockPin  clk;
            };

            typedef bitbanged_spi<PinDefinitions, typename DirectionToSPIStrategy<Direction>::type > type;
        };
    }


    template< typename DataPin, typename ClockPin, typename Direction>
    inline void shiftOut( const DataPin &, const ClockPin &, const Direction &, uint8_t data)
    {
        typedef typename Detail::SelectSpi< pin_definitions::null_pin_type, DataPin, ClockPin, Direction>::type SpiType;
        SpiType::transmit_receive( data);
    }

    template< typename DataPin, typename ClockPin, typename Direction>
    inline uint8_t shiftIn( const DataPin &, const ClockPin &, const Direction &)
    {
        typedef typename Detail::SelectSpi< DataPin, pin_definitions::null_pin_type, ClockPin, Direction>::type SpiType;
        return SpiType::transmit_receive( 0);
    }

    // declare local constants to be used as arguments in the arduino-like functions. Each of these
    // constants has their own (tag-) type, in contrast to real arduino code where these constants are all of some integer type.
    namespace {
        InputPullUpType __attribute__((unused)) INPUT_PULLUP;
        InputType       __attribute__((unused)) INPUT;
        OutputType      __attribute__((unused)) OUTPUT;

        HighType        __attribute__((unused)) HIGH;
        LowType         __attribute__((unused)) LOW;

        MsbFirstType    __attribute__((unused)) MSBFIRST;
        LsbFirstType    __attribute__((unused)) LSBFIRST;

        DigitalPin<13>::type __attribute__((unused))  LED_BUILTIN;
    }
}

#undef MAP_ARDUINO_DIGITAL_PIN

#endif /* AVR_UTILITIES_ARDUINO_LIKE_HPP_ */
