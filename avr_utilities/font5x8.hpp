//
//  Copyright (C) 2014 Danny Havenith
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//

/**
 *  This file contains an implementation of a font that is 8 pixels high and at most 5 pixels wide.
 *
 *  Characters are stored in program memory. The function find_character() retreives the in-memory
 *  address for characters based on ascii code, which can then be read using the AVR pgm_read_byte()
 *  function.
 *
 *  The font consists of upper case and lower case letters, digits and a limited set of symbols.
 */

#ifndef FONT5X8_HPP_
#define FONT5X8_HPP_
#include <avr/pgmspace.h>

namespace font5x8 {
namespace detail {
namespace {
/**
 * Linear search for the n-th character in the font buffer.
 * characters have different widths. Each character is separated from the next by a
 * zero.
 */
inline const uint8_t*find_nth( const uint8_t *font, char character)
{
    const uint8_t *character_start = font;

    while (character)
    {
        while (pgm_read_byte(character_start++));// search for the next zero.
        character--;
    }

    return character_start;
}
}
}

/// given a character code, return the font buffer address for the corresponding character.
///
/// Most characters map onto their ASCII-code. Some symbols are missing. If there is no character in the
/// font buffer for the given character code, this function returns the pointer to a question mark.
inline const uint8_t *find_character( char character)
{
    // column-wise definition of character pixels.
    // These are declared inside the inline function to
    // assure that the compiler will instantiate them only once.


    // symbols from space to '0' in the ascii table
    static const uint8_t symbols1[] PROGMEM = {
            0,         // space
            0b10111111,// !
            0,
            0b00000111,// "
            0b00000111,
            0,
            0b00100100, // #
            0b11111111,
            0b00100100,
            0b11111111,
            0b00100100,
            0,
            0b01001000, // $
            0b01010100,
            0b11111110,
            0b01010100,
            0b00100100,
            0,
            0b01000100, // %
            0b00100000,
            0b00010000,
            0b00001000,
            0b01000100,
            0,
            0b01101110, // &
            0b10010001,
            0b10101001,
            0b01000110,
            0b10100000,
            0,
            0b00000111, // '
            0,
            0b00111100, // (
            0b01000010,
            0b10000001,
            0,
            0b10000001, // )
            0b01000010,
            0b00111100,
            0,
            0b00010000, // *
            0b01010100,
            0b00111000,
            0b01010100,
            0b00010000,
            0,
            0b00010000, // +
            0b00010000,
            0b01111100,
            0b00010000,
            0b00010000,
            0,
            0b10000000, // ,
            0b01000000,
            0,
            0b00010000, // -
            0b00010000,
            0b00010000,
            0b00010000,
            0b00010000,
            0,
            0b10000000, // .
            0,
            0b01000000, // /
            0b00100000,
            0b00010000,
            0b00001000,
            0b00000100,
            0
    };

    // digits '0'-'9'
    static const uint8_t digits[] PROGMEM = {
        0b01111110,
        0b10010001,
        0b10001001,
        0b10000101,
        0b01111110,
        0,
        0b10000010,
        0b11111111,
        0b10000000,
        0,
        0b11000010,
        0b10100001,
        0b10010001,
        0b10001001,
        0b10000110,
        0,
        0b01000010,
        0b10000001,
        0b10001001,
        0b10001001,
        0b01110110,
        0,
        0b00011000,
        0b00010100,
        0b00010010,
        0b11111111,
        0b00010000,
        0,
        0b01001111,
        0b10001001,
        0b10001001,
        0b10001001,
        0b01110001,
        0,
        0b01111000,
        0b10010100,
        0b10010010,
        0b10010001,
        0b01100001,
        0,
        0b00000001,
        0b11110001,
        0b00001001,
        0b00000101,
        0b00000011,
        0,
        0b01110110,
        0b10001001,
        0b10001001,
        0b10001001,
        0b01110110,
        0,
        0b00001110,
        0b10010001,
        0b10010001,
        0b01010001,
        0b00111110,
        0,
    };

    // symbols between '9' and 'A'
    static const uint8_t symbols2[] PROGMEM = {
            0b01001000, // :
            0,
            0b10000000, // ;
            0b01001000,
            0,
            0b00010000, // <
            0b00101000,
            0b01000100,
            0,
            0b00101000, // =
            0b00101000,
            0b00101000,
            0b00101000,
            0b00101000,
            0,
            0b01000100, // >
            0b00101000,
            0b00010000,
            0,
            0b00000110, // ?
            0b00000001,
            0b10110001,
            0b00001001,
            0b00000110,
            0,
            0b01100100, // @
            0b10010010,
            0b11110010,
            0b10000010,
            0b01111100,
            0,
    };


    static const uint8_t uppercase[] PROGMEM = {
        0b11111110,
        0b00010001,
        0b00010001,
        0b00010001,
        0b11111110,
        0,
        0b11111111,
        0b10001001,
        0b10001001,
        0b10001001,
        0b01110110,
        0,
        0b01111110,
        0b10000001,
        0b10000001,
        0b10000001,
        0b01000010,
        0,
        0b11111111,
        0b10000001,
        0b10000001,
        0b01000010,
        0b00111100,
        0,
        0b11111111,
        0b10001001,
        0b10001001,
        0b10001001,
        0b10000001,
        0,
        0b11111111,
        0b00001001,
        0b00001001,
        0b00001001,
        0,
        0b01111110,
        0b10000001,
        0b10010001,
        0b10010001,
        0b01110010,
        0,
        0b11111111,
        0b00001000,
        0b00001000,
        0b00001000,
        0b11111111,
        0,
        0b10000001,
        0b10000001,
        0b11111111,
        0b10000001,
        0b10000001,
        0,
        0b01100000,
        0b10000000,
        0b10000000,
        0b10000000,
        0b01111111,
        0,
        0b11111111,
        0b00011000,
        0b00100100,
        0b01000010,
        0b10000001,
        0,
        0b11111111,
        0b10000000,
        0b10000000,
        0b10000000,
        0b10000000,
        0,
        0b11111111,
        0b00000010,
        0b00001100,
        0b00000010,
        0b11111111,
        0,
        0b11111111,
        0b00000010,
        0b00001100,
        0b00010000,
        0b11111111,
        0,
        0b01111110,
        0b10000001,
        0b10000001,
        0b10000001,
        0b01111110,
        0,
        0b11111111,
        0b00010001,
        0b00010001,
        0b00010001,
        0b00001110,
        0,
        0b01111110,
        0b10000001,
        0b10100001,
        0b01000001,
        0b10111110,
        0,
        0b11111111,
        0b00010001,
        0b00110001,
        0b01010001,
        0b10001110,
        0,
        0b10000110,
        0b10001001,
        0b10001001,
        0b10001001,
        0b01110001,
        0,
        0b00000001,
        0b00000001,
        0b11111111,
        0b00000001,
        0b00000001,
        0,
        0b01111111,
        0b10000000,
        0b10000000,
        0b10000000,
        0b01111111,
        0,
        0b00001111,
        0b00110000,
        0b11000000,
        0b00110000,
        0b00001111,
        0,
        0b11111111,
        0b01000000,
        0b00110000,
        0b01000000,
        0b11111111,
        0,
        0b11000011,
        0b00100100,
        0b00011000,
        0b00100100,
        0b11000011,
        0,
        0b00000111,
        0b00001000,
        0b11110000,
        0b00001000,
        0b00000111,
        0,
        0b11000001,
        0b10100001,
        0b10011001,
        0b10000101,
        0b10000011,
        0,
    };

    // special symbol for unsupported character
    static const uint8_t unsupported[] PROGMEM = {
            0b10101010,
            0
    };

    // symbols between 'Z' and 'a'
    static const uint8_t symbols3[] PROGMEM = {
            0b11111111, // [
            0b10000001,
            0,
            0b00000100,
            0b00001000,
            0b00010000,
            0b00100000,
            0b01000000, // \ (adding this to prevent it interpreted as escape)
            0,
            0b10000001, // ]
            0b11111111,
            0,
            0b00000010, // ^
            0b00000001,
            0b00000010,
            0,
            0b10000000, // _
            0b10000000,
            0b10000000,
            0b10000000,
            0,
            0b00000001, // `
            0b00000010,
            0
    };

    static const uint8_t lowercase[] PROGMEM = {
        0b01100000,
        0b10010100,
        0b10010100,
        0b10010100,
        0b11111000,
        0,
        0b11111111,
        0b10010000,
        0b10001000,
        0b10001000,
        0b01110000,
        0,
        0b01110000,
        0b10001000,
        0b10001000,
        0b10001000,
        0,
        0b01110000,
        0b10001000,
        0b10001000,
        0b10010000,
        0b11111111,
        0,
        0b01110000,
        0b10101000,
        0b10101000,
        0b10101000,
        0b00110000,
        0,
        0b00010000,
        0b11111110,
        0b00010001,
        0b00000001,
        0b00000010,
        0,
        0b00011000,
        0b10100100,
        0b10100100,
        0b10100100,
        0b01111100,
        0,
        0b11111111,
        0b00010000,
        0b00001000,
        0b00001000,
        0b11110000,
        0,
        0b11111010,
        0,
        0b01000000,
        0b10000000,
        0b10000100,
        0b01111101,
        0,
        0b11111111,
        0b00100000,
        0b01010000,
        0b10001000,
        0,
        0b10000001,
        0b11111111,
        0b10000000,
        0,
        0b11111000,
        0b00001000,
        0b00110000,
        0b00001000,
        0b11110000,
        0,
        0b11111000,
        0b00010000,
        0b00001000,
        0b00001000,
        0b11110000,
        0,
        0b01110000,
        0b10001000,
        0b10001000,
        0b10001000,
        0b01110000,
        0,
        0b11111100,
        0b00100100,
        0b00100100,
        0b00100100,
        0b00011000,
        0,
        0b00001000,
        0b00010100,
        0b00010100,
        0b00011000,
        0b11111100,
        0,
        0b11111000,
        0b00010000,
        0b00001000,
        0b00001000,
        0b00010000,
        0,
        0b10010000,
        0b10101000,
        0b10101000,
        0b10101000,
        0b01000000,
        0,
        0b00001000,
        0b01111110,
        0b10001000,
        0b10000000,
        0,
        0b01111000,
        0b10000000,
        0b10000000,
        0b01000000,
        0b11111000,
        0,
        0b00111000,
        0b01000000,
        0b10000000,
        0b01000000,
        0b00111000,
        0,
        0b01111000,
        0b10000000,
        0b01110000,
        0b10000000,
        0b01111000,
        0,
        0b10001000,
        0b01010000,
        0b00100000,
        0b01010000,
        0b10001000,
        0,
        0b10011100,
        0b10100000,
        0b10100000,
        0b01111100,
        0,
        0b10001000,
        0b11001000,
        0b10101000,
        0b10011000,
        0b10001000,
        0,
    };

    // symbols after 'z'
    static const uint8_t symbols4[] PROGMEM = {
            0b00001000, // {
            0b01110110,
            0b10000001,
            0,
            0b11111111, // |
            0,
            0b10000001, // }
            0b01110110,
            0b00001000,
            0,
            0b00000010, // ~
            0b00000001,
            0b00000010,
            0b00000100,
            0b00000010,
            0
    };

    static const uint8_t other_symbols[] PROGMEM = {
            0b00111000, // copyright
            0b01000100,
            0b10010010,
            0b10101010,
            0b10000010,
            0b01000100,
            0b00111000,
            0,
            0b00001000, // ->
            0b00001000,
            0b00101010,
            0b00011100,
            0b00001000,
            0,
            0b00010000, // <-
            0b00111000,
            0b01010100,
            0b00010000,
            0b00010000,
            0,
            0b00100010, // smiley
            0b01000000,
            0b01001000,
            0b01000000,
            0b00100010,
            0,
    };

    using detail::find_nth;
    if (character == 0) return 0;
    if (character <  ' ') return unsupported;
    if (character <  '0') return find_nth( symbols1, character - ' ');
    if (character <= '9') return find_nth( digits, character - '0');
    if (character <  'A') return find_nth( symbols2, character - ':');
    if (character <= 'Z') return find_nth( uppercase, character - 'A');
    if (character <  'a') return find_nth( symbols3, character - '[');
    if (character <= 'z') return find_nth( lowercase, character - 'a');
    if (character <= '~') return find_nth( symbols4, character - '{');
    switch (character)
    {
        case 169: // copyright symbol
            return find_nth( other_symbols, 0);
            break;
        default:
        {
            return unsupported; // question mark also for unknown symbol character
        }
    }
}
} // end namespace




#endif /* FONT5X8_HPP_ */
