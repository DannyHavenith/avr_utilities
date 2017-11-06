//
//  Copyright (C) 2017 Danny Havenith
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef ESP_LINK_COMMAND_HPP_
#define ESP_LINK_COMMAND_HPP_
#include "command_codes.hpp"
#include <stdint.h>

/**
 * This file contains command definitions.
 *
 * Each command describes
 * the contents of a packet that will be sent to esp-link via the uart.
 */


namespace esp_link {

/**
 * Instantiations of this template describe a command that can be sent to esp-link
 * The first argument is the command code (which will be sent as part of the header
 * of an esp-link packet). The second argument is a functionn prototype that describes
 * the arguments to be sent to the esp-link and the contents of the expected return
 * packet, if any.
 *
 * The following example declares a non-existent esp-link command with code 142 that
 * requires a string of some kind (either const char*, PROGMEM const char*) an int16_t and
 * an int32_t and that should trigger a return packet from esp-link with an int16_t as a value.
 *
 * @code{.cpp}
 * constexpr command< 142, int16_t ( string, int32_t, int16_t)> example_command;
 * @endcode
 *
 * The next example describes an esp-link command that takes a string and for which no return
 * packet is expected:
 *
 * @code {.cpp}
 * constexpr command< 143, void (string)> example2;
 * @endcode
 *
 * Objects of these types can be used with the esp_lin::client::execute() member function
 * template. This function template expects arguments that are comaptible with the given
 * argument types, so for instance the last example can be used as follows:
 *
 * @code {.cpp}
 * esp_link::client esp;
 * esp.execute( example2, "Hello world");
 * @endcode
 *
 * Using incompatible argument types will result in a compilation error, so the following
 * example will not compile:
 *
 * esp_link::client esp;
 * esp.execute( example2, 42);
 * @endcode
 *
 * @see client::execute()
 *
 */
template< uint16_t Command, typename FunctionPrototype>
struct command
{
};

template< typename T>
struct return_type
{
    using type = T;
};

// tag types for return values or parameters
// for most types, the type itself acts as the tag type, but
// especially for types that have multiple overloads or that require
// more processing, a tag type will act as a stand-in for the actual arguments
// given.
struct ack {};    /// return bool to indicate whether an ack package arrived
struct string {}; /// accept any string type as argument
struct string_with_extra_len {};
struct callback {};

template<>
struct return_type<ack>
{
    using type = bool;
};


namespace {
    constexpr
        command<
            commands::CMD_SYNC,
            ack()>
        sync;

    constexpr
        command<
            commands::CMD_GET_TIME,
            uint32_t()>
        get_time;
}

namespace mqtt
{
namespace {
    constexpr
        command<
            commands::CMD_MQTT_SUBSCRIBE,
            void ( string topic, uint8_t qos)>
        subscribe;

    constexpr
        command<
            commands::CMD_MQTT_SETUP,
            void ( callback connected, callback disconnected, callback published, callback data)>
        setup;

    constexpr
        command<
            commands::CMD_MQTT_PUBLISH,
            void ( string topic, string_with_extra_len message, uint8_t qos, bool retain)>
        publish;
    }
}
}



#endif /* ESP_LINK_COMMAND_HPP_ */
