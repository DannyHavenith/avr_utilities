//
//  Copyright (C) 2017 Danny Havenith
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef ESP_LINK_CLIENT_HPP_
#define ESP_LINK_CLIENT_HPP_
#include "command.hpp"

#include <stdint.h>
#include <avr_utilities/devices/uart.h>
#include <avr_utilities/function/function.hpp>

#include <string.h>



namespace flash_string
{
    class helper;
}
namespace esp_link
{
    struct packet
    {
        uint16_t cmd;            /**< Command to execute */
        uint16_t argc;           /**< Number of arguments */
        uint32_t value;          /**< Callback to invoke, NULL if none; or response value */
        uint8_t  args[0];        /**< Arguments */
    };

    struct string_ref
    {
        const char *buffer;
        uint16_t len;
    };

    struct packet_parser
    {
        packet_parser( const packet *p)
        :m_packet{p},
         m_argument{ reinterpret_cast<const char*>( &p->args[0])}
        {
        }

        template< typename T>
        void get( T &value)
        {
            return get( get_len(), value);
        }

        void get( string_ref &value)
        {
            value.len = get_len();
            value.buffer =  m_argument;
            advance( value.len);
        }

    private:
        template< typename T>
        void get( uint16_t len, T &value)
        {
            if (len >= sizeof( T))
            {
                value = *reinterpret_cast< T*>( m_argument);
            }
            advance( len);
        }

        void advance( uint16_t len)
        {
            m_argument += (len + ((4-((len+2)&3))&3));
        }

        uint16_t get_len()
        {
            auto val = *reinterpret_cast<const uint16_t*>( m_argument);
            m_argument +=2;
            return val;
        }

        const packet *m_packet;
        const char   *m_argument;
    };


    class client
    {
    public:

        using callback_type = function::function<void (const packet *, uint16_t)>;
        client( serial::uart<> &uart)
        : m_uart{&uart}
        {
        }

        /**
         * Execute a command.
         *
         * The first argument must be an instantiation of the command<> template. This instantiation should have
         * a command code as its first argument (Cmd) and then a function prototype as its second argument. The parameters
         * of this function prototype (Parameters...) determine what other arguments are expected of the caller of execute():
         * each type in Parameters... is matched with a function argument in args... Then, the type of Parameters determines how
         * the argument will be translated into data in the packet that will be sent to the esp-link serial port.
         *
         */
        template< uint16_t Cmd, typename ReturnType, typename... Parameters, typename... Arguments>
        void execute( command<Cmd, ReturnType( Parameters...)> /*ignore*/, const Arguments &... args)
        {
            static_assert( sizeof...(Parameters) <= sizeof...(Arguments), "Too few arguments provided for this command");
            static_assert( sizeof...(Parameters) >= sizeof...(Arguments), "Too many arguments provided for this command");

            constexpr uint16_t argc = send_parameter_count( tag<Parameters>{}...);
            send_request_header( Cmd, 0x142, argc);
            (void)((int[]){0, (add_parameter(tag<Parameters>{}, args),0)...});
            finalize_request();
        }

        const packet* receive(uint32_t timeout = 50000L);
        const packet* try_receive();

        void log_packet(const esp_link::packet *p);


        void send(const char* str);
        void send(const char* str, uint16_t len);

        bool sync();
        void send_padding(uint16_t length);
        void send_hex( uint8_t value);

    private:

        template <typename T>
        struct tag {};

        // constexpr functions to determine how many parameters to send to the
        // esp-link, given the list of function parameters.
        // This is not simply the count of the function parameters, because parameters
        // of type 'string_with_extra_len' are represented as a single argument, but will
        // result in two parameters being sent to the esp-link.
        template< typename T>
        static constexpr uint16_t send_parameter_count( tag<T>)
        {
            return 1;
        }

        static constexpr uint16_t send_parameter_count( tag<string_with_extra_len>)
        {
            return 2;
        }

        template< typename Head, typename... Tail>
        static constexpr uint16_t send_parameter_count( tag<Head> head, Tail... tail)
        {
            return send_parameter_count( head) + send_parameter_count( tail...);
        }

        static constexpr uint16_t send_parameter_count()
        {
            return 0;
        }

        // metafunction to exclude arguments from parameter type deduction
        template<typename T>
        struct literally
        {
            using type = T;
        };

        template<typename T>
        using literally_t = typename literally<T>::type;


        // sending parameters...
        void add_parameter_bytes(const uint8_t* data, uint16_t length);
        void add_parameter(tag<callback>,   callback_type f);
        void add_parameter(tag<string>,     const char* string);
        void add_parameter(tag<string>,     const flash_string::helper* string);
        void add_parameter(tag<string_with_extra_len>, const char* string);

        // send a parameter of any type T, represented by a value that can be converted
        // to type T. By using the literally_t metafunction, we assure that the actual
        // argument will be cast to type T before further processing.
        template< typename T>
        void add_parameter( tag<T>,  literally_t<T> value)
        {
            add_parameter( value);
        }

        void add_parameter( tag<bool>, bool value)
        {
        	add_parameter( static_cast<uint8_t>(value?1:0));
        }


        template< typename T>
        void add_parameter( T value)
        {
            add_parameter_bytes(
                    reinterpret_cast<const uint8_t *>( &value),
                    sizeof( value));
        }


        uint32_t register_callback(callback_type f);

        void send_direct(uint8_t value);
        void send_byte(uint8_t value);
        void send_bytes(const uint8_t* buffer, uint8_t size);

        /// sent a value as a sequence of bytes to the esp. This will send
        /// the memory bytes of this value
        template< typename T>
        void send_binary( const T &value)
        {
            send_bytes( reinterpret_cast< const uint8_t *>( &value), sizeof value);
        }


        void send_request_header(uint16_t command, uint32_t value, uint16_t argcount);
        void finalize_request();

        void clear_input();

        bool receive_byte(uint8_t& value, uint32_t timeout = 100000L);
        uint8_t receive_byte_w();

        static void crc16_add(uint8_t value, uint16_t &accumulator);

        const packet* decode_packet(const uint8_t* buffer, uint8_t size);
        const packet* check_packet(const uint8_t* buffer, uint8_t size);

        uint16_t        m_runningCrc = 0;
        serial::uart<>  *m_uart;
        static constexpr uint8_t buffer_size = 128;
        uint8_t m_buffer[buffer_size];
        uint8_t m_buffer_index = 0;
        bool    m_last_was_esc = false;
        bool    m_syncing = false;

        static constexpr uint8_t callbacks_size = 8;
        callback_type m_callbacks[callbacks_size];
    };

}

#endif /* ESP_LINK_CLIENT_HPP_ */
