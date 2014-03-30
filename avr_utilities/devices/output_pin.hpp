/*
 * output_pin.hpp
 *
 *  Created on: Nov 18, 2011
 *      Author: danny
 */

#ifndef OUTPUT_PIN_HPP_
#define OUTPUT_PIN_HPP_

template<typename pin_type>
class output_pin
{
public:
    template< typename OutputInitializer>
    static inline void init( const OutputInitializer &outputs)
    {
        outputs( pin);
    }

    void set() const
    {
        pin_definitions::set( pin);
    }

    void reset() const
    {
        pin_definitions::reset( pin);
    }

private:
    static pin_type pin;
};

#endif /* OUTPUT_PIN_HPP_ */
