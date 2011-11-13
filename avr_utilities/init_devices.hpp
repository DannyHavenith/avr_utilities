/*
 * init_devices.hpp
 *
 *  Created on: Nov 11, 2011
 *      Author: danny
 *
 *  A somewhat contorted way to initialize the output pins for all devices in one go.
 *
 *  Every device that implements the device concept is supposed to have a static init-function
 *  that takes an output_initializer as a single argument. The device is expected to invoke the
 *  function-call-operator on the output_initializer with the list of output pins for that device.
 *  An output initializer maintains a list of devices and when the function-call-operator is invoked
 *  it will combine the given list of output pins with those that it already new and delegate to
 *  a the next output initializer. The next output initializer will call the init function for
 *  the next device, which will call the function-call-operator, etc. until there are no devices
 *  left and all output pins are known. At that point the pin_definitions::init_as_output function
 *  will be called that will initialize the output pins an a minimal set of instructions.
 *
 *  All of this is because the output pins can be
 */

#ifndef INIT_DEVICES_HPP_
#define INIT_DEVICES_HPP_
#include "pin_definitions.hpp"

namespace init_devices
{
    template< typename device, typename pins = pin_definitions::empty_list>
    struct output_initializer
    {
        template< typename new_output_pins>
        void operator()( const new_output_pins &new_pins) const
        {
            device::init( output_initializer<pin_definitions::empty_list, pins>());
        }
    };

    template< typename head_device, typename tail_devices, typename pins>
    struct output_initializer<
        pin_definitions::cons< head_device, tail_devices>,
        pins>
    {
        template< typename new_output_pins>
        void operator()( const new_output_pins &) const
        {
            typedef output_initializer<
                    tail_devices,
                    typename pin_definitions::concatenate_cons< new_output_pins, pins>::type
                    > next_initializer;

            head_device::init( next_initializer());
        }
    };

    template< typename pins>
    struct output_initializer< pin_definitions::empty_list, pins>
    {
      template< typename new_output_pins>
      void operator()( const new_output_pins &) const
      {
          typedef typename pin_definitions::concatenate_cons< new_output_pins, pins>::type all_pins;
          pin_definitions::init_as_output( all_pins());
      }
    };

    template< typename device_cons>
    inline extern void init( const device_cons &)
    {
        output_initializer<device_cons>()( pin_definitions::empty_list());
    }
}
#endif /* INIT_DEVICES_HPP_ */
