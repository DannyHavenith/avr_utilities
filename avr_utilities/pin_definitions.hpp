
/// pin_definitions.hpp - template classes and functions to support pin definitons as
/// a combination of both a port and a bit of that port

#if !defined(PIN_DEFINITIONS_HPP_)
#define PIN_DEFINITIONS_HPP_
#include <stdint.h>
#include <avr/io.h>

namespace pin_definitions
{

    enum PortPlaceholder {
        port_A,
        port_B,
        port_C,
        port_D,
        port_E,
        port_F
    };

    /// tags to help select between port (output port), pin (input port) or
    /// ddr (data direction port).
    struct tag_port {};
    struct tag_pin  {};
    struct tag_ddr  {};

    template< PortPlaceholder port>
    struct port_traits {};

#define DECLARE_PORT_TRAITS( p_)                                    \
        template<>                                                  \
        struct port_traits<port_##p_>                               \
        {                                                           \
            static volatile uint8_t &get( const tag_port &) { return PORT##p_;} \
            static volatile uint8_t &get( const tag_pin  &) { return PIN##p_; } \
            static volatile uint8_t &get( const tag_ddr  &) { return DDR##p_; } \
        };                                                          \
        /**/


#if defined(PORTA)
    DECLARE_PORT_TRAITS( A)
#endif
#if defined(PORTB)
    DECLARE_PORT_TRAITS( B)
#endif
#if defined(PORTC)
    DECLARE_PORT_TRAITS( C)
#endif
#if defined(PORTD)
    DECLARE_PORT_TRAITS( D)
#endif
#if defined(PORTE)
    DECLARE_PORT_TRAITS( E)
#endif
#if defined(PORTF)
    DECLARE_PORT_TRAITS( F)
#endif

template< PortPlaceholder port_, uint8_t bit_>
struct pin_definition
{
    static const PortPlaceholder    port = port_;
    static const uint8_t            bit  = bit_;
    static const uint8_t            mask = 1 << bit;
};

template< uint8_t bit_>
struct bit_type 
{
    static const uint8_t    value = bit_;
};

/// this metafunction converts a bit numer to a mask (1 << bit_nr).
template< uint8_t bit_>
struct bit_to_mask
{
    static const uint8_t value = 1 << bit_;
};

template <>
struct bit_to_mask< 255>
{
    static const uint8_t value = 0;
};


struct null_def {};

/// a set of pins or pin groups
template< typename head_, typename tail_ = null_def>
struct pin_cons_list
{
    typedef head_ head;
    typedef tail_ tail;

    typedef pin_cons_list< head_, tail_> as_cons_list;
};

/// a set of bits in one port
template< PortPlaceholder port_, 
            uint8_t bit0,
            uint8_t bit1 = 255,
            uint8_t bit2 = 255,
            uint8_t bit3 = 255,
            uint8_t bit4 = 255,
            uint8_t bit5 = 255,
            uint8_t bit6 = 255,
            uint8_t bit7 = 255
            >
struct pin_group
{
    static const uint8_t port = port_;
    static const uint8_t mask = 
            bit_to_mask< bit0>::value
        |   bit_to_mask< bit1>::value
        |   bit_to_mask< bit2>::value
        |   bit_to_mask< bit3>::value
        |   bit_to_mask< bit4>::value
        |   bit_to_mask< bit5>::value
        |   bit_to_mask< bit6>::value
        |   bit_to_mask< bit7>::value;
};

template< PortPlaceholder port, typename port_tag>
inline volatile uint8_t &get_port( const port_tag &tag)
{
    return port_traits<port>::get( tag);
}

template< typename pins_type>
inline void set( const pins_type &)
{
    get_port<pins_type::port>( tag_port()) |= pins_type::mask;
}

template< typename pins_type>
inline void reset( const pins_type &)
{
    get_port<pins_type::port>( tag_port()) &= ~pins_type::mask;
}


}

#endif //PIN_DEFINITIONS_HPP_
