/*
 * simple_text_parsing.h
 *
 * Copyright (c) 2018 Danny Havenith
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 *   Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 *   Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef AVR_UTILITIES_SIMPLE_TEXT_PARSING_H_
#define AVR_UTILITIES_SIMPLE_TEXT_PARSING_H_
namespace text_parsing
{
    /**
     * Given a pointer to a buffer in "input" and the end pointer of the buffer
     * in "end", parse the string in the buffer into a uint16_t.
     *
     * Since the parameter "input" is given by reference, this function will
     * actually advance the first parameter to point just beyond the last recognized
     * numerical character.
     */
    uint16_t parse_uint16( const char *(&input), const char *end)
    {
        uint16_t value = 0;
        while ( input < end and *input and *input <= '9' and *input >= '0')
        {
            value = 10 * value + (*input - '0');
            ++input;
        }
        return value;
    }

    uint8_t to_decimal( char hex_digit)
    {
        // this could be slightly more optimal.
        if (hex_digit >= '0' and hex_digit <= '9') return hex_digit - '0';
        hex_digit |= 0x20; // to lower case
        if (hex_digit >= 'a' and hex_digit <= 'f') return hex_digit - 'a' + 10;

        return 0;
    }

    /**
     * Consume as many characters from the character array pointed to by "input" as
     * match the expectation. Return true if all characters of the expectation string
     * were matched by the input, return false otherwise.
     *
     * If this function returns false, then input will be unchanged, if it returns true
     * input points to the first character after the recognized input characters.
     */
    bool consume( const char *(&input), const char *end, const char *expectation)
    {
        const char *saved = input;
        while (*expectation and input < end and *input++ == *expectation++) /* continue */;

        if (*expectation != 0)
        {
            input = saved;
            return false;
        }
        else
        {
            return true;
        }
    }

}

#endif /* AVR_UTILITIES_SIMPLE_TEXT_PARSING_H_ */
