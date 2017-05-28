//
// The following code is essentially a copy of a part of Arduino's WString.h, which is
//
// Copyright (c) 2009-10 Hernando Barragan.  All right reserved.
// Copyright 2011, Paul Stoffregen, paul@pjrc.com
//
// and distributed under LGPL version 2.1 or later
//
#ifndef AVR_UTILITIES_FLASH_STRING_HPP_
#define AVR_UTILITIES_FLASH_STRING_HPP_
#include <avr/pgmspace.h>

namespace flash_string
{
    class helper;
    inline const helper *as_pstring( const char *ptr)
    {
        return reinterpret_cast<const helper *>(ptr);
    }

}

// I hate to define short macro names because they pollute the global "token space",
// but at least I'm adding an underscore, which makes me feel a little better.
#define F_(string_literal) (flash_string::as_pstring(PSTR(string_literal)))

#endif /* AVR_UTILITIES_FLASH_STRING_HPP_ */
