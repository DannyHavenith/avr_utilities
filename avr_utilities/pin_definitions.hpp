
/// pin_definitions.hpp - template classes and functions to support pin definitons as
/// a combination of both a port and a bit of that port
/// This header file allows a style of programming where a programmer defines an output-
/// or input bit as a combination of a port and a bit number and then use the set or reset
/// function to set or reset that particular bit, e.g.
///
/// DECLARE_PIN( my_output, C, 0)
/// set( my_output) // equivalent to PORTC |= _BV(0)
///
/// The templates allow the code to be as efficient as handwritten statements, at the cost of
/// some compilation time.

#if !defined(PIN_DEFINITIONS_HPP_)
#define PIN_DEFINITIONS_HPP_


#include <stdint.h>  // including a std C header in an obvious C++ header file. hmm.
#include <avr/io.h>

#define PIN_DEF_ALWAYS_INLINE __attribute__((always_inline))

namespace pin_definitions
{

    enum PortPlaceholder {
        port_Null,
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

    /// If this type is substituted for a real port, any operation on it will result in no
    /// action at all.
    struct null_port {};

    /// traits template that for a given PortPlaceholder returns the port, pin or ddr register
    template< PortPlaceholder port>
    struct port_traits
    {
    private:
        // if you get a compile error complaining that get returns void,
        // you may be using a port that has not been defined for your mcu (like port A on an atmega88).
        template<typename T>
        static void get( const T &){}
    };

    template< PortPlaceholder port>
    struct port_type
    {
        typedef volatile uint8_t &type;
    };

    template<>
    struct port_type<port_Null>
    {
        typedef null_port type;
    };

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


/// specialisation for null-port. Will always return the null-port type.
template<>
struct port_traits<port_Null>
{
    template<typename whatever_tag>
    static null_port get( const whatever_tag &) { return null_port();}
};

#undef DECLARE_PORT_TRAITS


// typedefs for a simple list-type
struct empty_list {};

/// Implementation of a simple cons list.
/// A cons lists consists of a single element, the head of the list and
/// another cons list, called tail, which contains the rest of the list.
template< typename head_, typename tail_ = empty_list>
struct cons
{
    typedef head_ head;
    typedef tail_ tail;

    typedef cons< head_, tail_> as_cons;
};

/// This type defines a pin. The type contains information about the port and the bit number within that port.
/// By declaring a variable of this type, you can give a name to one specific bit of one port, e.g.
/// @code
///     pin_definition< Port_B, 4> led1; // led 1 is attached to pin B4
/// @endcode.
template< PortPlaceholder port_, uint8_t bit_>
struct pin_definition
{

    static const PortPlaceholder    port = port_;
    static const uint8_t            bit  = bit_;
    static const uint8_t            mask = 1 << bit_;
    static const uint8_t 			shift = bit_;
    typedef cons< pin_definition< port_, bit_>, empty_list> as_cons;
};

typedef pin_definition<port_Null, 0> null_pin_type;

/// a contiguous set of bits in one port
template< PortPlaceholder port_,
            uint8_t first_bit,
            uint8_t bits
            >
struct pin_group
{
    static const PortPlaceholder    port = port_;
    static const uint8_t            mask = (0xff >> (8 - bits)) << first_bit;
    static const uint8_t            shift = first_bit;

    typedef cons< pin_group< port_, first_bit, bits>, empty_list> as_cons;
};


// here be meta-functions...
// the following functions are a very lightweight version of boost.mpl

/// true-value for if_ metafunction
struct true_type { typedef true_type type;};

/// false-value for if_ metafunction
struct false_type { typedef false_type type;};



/// generic if-metafunction. returns the 'if_true' type if the condition
/// is of type true_type.
template< typename condition, typename if_true, typename if_false>
struct if_
{
    typedef if_false type;
};

template< typename if_true, typename if_false>
struct if_< true_type, if_true, if_false>
{
    typedef if_true type;
};

template< typename left_, typename right_>
struct and_ : false_type {};

template<>
struct and_<true_type, true_type> : true_type {};

/// utility template to employ SFINAE to add or remove
/// certain functions from an overload set.
template< typename condition, typename t = void>
struct enable_if
{

};

template< typename t>
struct enable_if< true_type, t>
{
    typedef t type;
};

/// meta-function that tells us if a given type is a pin or pin group.
template <typename T>
struct is_pin_or_pin_group : false_type {};

template< PortPlaceholder port_, uint8_t bit_>
struct is_pin_or_pin_group< pin_definition<port_, bit_> >
    : true_type {};

template< PortPlaceholder port_,
            uint8_t first_bit,
            uint8_t bits
            >
struct is_pin_or_pin_group< pin_group< port_, first_bit, bits> >
    : true_type {};


/// meta-function that returns true if two ports are equal
template< PortPlaceholder left, PortPlaceholder right>
struct is_same_port : false_type {};

template< PortPlaceholder holder>
struct is_same_port< holder, holder> : true_type {};

struct null_mask
{
    static const uint8_t mask = 0;
};

/// meta-function that creates a mask of all pins of one port, given a (cons-)list of pin-
/// or pin group definitions for several ports.
template< PortPlaceholder port, typename list>
struct mask_for_port
{
    static const uint8_t value =
        if_<
            typename is_same_port<list::head::port, port>::type,
            typename list::head,
            null_mask
            >::type::mask
        | mask_for_port< port, typename list::tail>::value;
};

/// specialization of this meta function for an empty list
template< PortPlaceholder port>
struct mask_for_port< port, empty_list>
{
    static const uint8_t value = 0;
};


template< PortPlaceholder port, typename port_tag>
inline typename port_type<port>::type get_port( const port_tag &tag)
{
    // if you get a compile error complaining that get() returns void,
    // you may be using a port that has not been defined for your mcu (like port A on an atmega88).
    return port_traits<port>::get( tag);
}


    /// this helper class is used to construct cons lists.
    template< typename head, typename tail = empty_list>
    struct cons_builder
    {
        typedef cons< head, tail> as_cons;

        template< typename new_head>
        cons_builder< new_head, as_cons > operator()( const new_head &) const
        {
            return cons_builder< new_head, cons< head, tail> >();
        }
    };

    /// this function allows an expression of the form:
    ///     list_of( pindef0)(pindef1)(pindef3)... etc.
    /// list_of(...) returns a cons builder that itself defines an operator().
    template< typename head>
    cons_builder< head> list_of( const head&)
    {
        return cons_builder<head, empty_list>();
    }

    /// definition of an operator|() that allows us to create lists of
    /// pins with an expression like:
    ///    pindef0 | pindef1 | pindef2...
    /// enable_if is used to make sure each argument is either a pin or a
    /// pin group.
    template< typename left_type, typename right_type>
    typename enable_if<
        typename and_<
            typename is_pin_or_pin_group<left_type>::type,
            typename is_pin_or_pin_group<right_type>::type
        >::type,
        cons<left_type, cons<right_type> >
    >::type operator|( const left_type &, const right_type &)
    {
        return cons<left_type, cons<right_type> >();
    }

    /// this definition of operator|() allows us to combine a list of pins
    /// with either a pin definition or a pin group.
    template< typename right_type, typename head, typename tail>
    typename enable_if<
        typename is_pin_or_pin_group<right_type>::type,
        cons< right_type, cons< head, tail> >
    >::type operator|( const cons<head, tail> &, const right_type &)
    {
        return cons< right_type, cons< head, tail> >();
    }

    /// template meta function that removes all pin- and pin group definitions
    /// for a given port from a list.
    /// This meta function is used to remove all pin definitions for a given port,
    /// after the mask for that port has already been constructed and used.
    template< PortPlaceholder port, typename list_type>
    struct remove_port
    {
        typedef typename list_type::head head;
        typedef typename list_type::tail tail;
        // if the head type is associated with 'port', create a list without the head type and recurse on the tail-type
        // if the head type is not associated with 'port' create a list that consists of the head type at the front of
        // the result of recursion on the tail type
        typedef typename if_<
                    typename is_same_port< port, head::port>::type,
                    typename remove_port< port, tail>::type,
                    cons< head, typename remove_port< port, tail>::type>
                    >::type type;
    };

    template< PortPlaceholder port>
    struct remove_port< port, empty_list>
    {
        typedef empty_list type;
    };

    namespace detail
    {
        template< typename operation, typename port_tag>
        inline void set_as_output( empty_list &, const operation &, const port_tag &)
        {
            // do nothing for an empty list;
        }

        /// perform an operation on every port in a list of pins.
        /// For each port in the list, this functor will aggregate all pins into a mask and then
        /// call the given operation with the port and the mask as arguments.
        /// The operation will be called exactly one time for each port that appears in the list.
        /// The tag parameter determines whether the operation will be performed on the port input, the port
        /// output, or the port data direction registers.
        template< typename list, typename operation, typename port_tag>
        struct for_each_port_operator
        {
            static const PortPlaceholder port = list::head::port;
            static inline void operate() __attribute__((always_inline))
            {
                // perform the operation on the port of the first element of the list
                operation()( get_port<port>( port_tag()), mask_for_port< port, list>::value);

                // now remove all elements for this port from 'list' and recurse.
                for_each_port_operator< typename remove_port< port, typename list::tail>::type, operation, port_tag>::operate();
            }
        };

        /// specialization of the for_each_port_operator meta function for empty lists.
        template< typename operation, typename port_tag>
        struct for_each_port_operator< empty_list, operation, port_tag>
        {
            static inline void operate() __attribute__((always_inline))
            {
                // do nothing for the empty list...
            }
        };

    }

    /// This operator will assign the value to the given register.
    /// operator to be used with for_each_port_operator
    struct assign
    {
        void operator()( volatile uint8_t &reg, uint8_t value) const
        {
            reg = value;
        }

        void operator()( const null_port &, uint8_t ) const
        {
            // do nothing;
        }
    };

    /// This operator will logical-or the value with the given register.
    /// operator to be used with for_each_port_operator
    struct set_bits
    {
        void operator()( volatile uint8_t &reg, uint8_t value) const
        {
            reg |= value;
        }

        void operator()( const null_port &, uint8_t ) const
        {
            // do nothing;
        }
    };

    /// This operator will reset all bits given in the second argument in the register
    /// operator to be used with for_each_port_operator
    struct reset_bits
    {
        void operator()( volatile uint8_t &reg, uint8_t value) const
        {
            reg &= ~value;
        }

        void operator()( const null_port &, uint8_t ) const
        {
            // do nothing;
        }
    };

    // the following functions use pin definitions to perform common tasks

    /// initialize all ports of the given pin definitions, turning all given pins to output and making all
    /// bits that are not mentioned in those ports inputs.
    /// This template receives a list_builder, or list as template argument. A list contains (port, bits)-pairs
    /// and this function will combine all bits for each port mentioned in the list and then assign the accumulated
    /// bits value to the data direction register for those ports.
    template< typename list_builder>
    inline extern void init_as_output( const list_builder &)
    {
        detail::for_each_port_operator< typename list_builder::as_cons, assign, tag_ddr>::operate();
    }

    /// make the given pins outputs. This does not affect other pins on the same ports.
    /// See also init_as_output.
    template< typename list_builder>
    inline void make_output( const list_builder &)
    {
        detail::for_each_port_operator< typename list_builder::as_cons, set_bits, tag_ddr>::operate();
    }

    /// explicitly make the given pins inputs. This does not affect other pins on the same ports.
    /// See also init_as_output.
    template< typename list_builder>
    inline void make_input( const list_builder &)
    {
        detail::for_each_port_operator< typename list_builder::as_cons, reset_bits, tag_ddr>::operate();
    }

    /// set the given bits to 1, this changes the output ports
    /// See also init_as_output.
    template< typename list_builder>
    inline PIN_DEF_ALWAYS_INLINE void set( const list_builder &)
    {
        detail::for_each_port_operator< typename list_builder::as_cons, set_bits, tag_port>::operate();
    }

    /// Toggle all the given output pins, or pin groups.
    /// This function uses the little known fact that writing a 1 to any bit of a PINx register will
    /// toggle the corresponding PORTx bits.
    /// For input ports this will have the effect of toggling the pull-up resistor.
    template< typename list_builder>
    inline PIN_DEF_ALWAYS_INLINE void toggle( const list_builder &)
    {
        detail::for_each_port_operator< typename list_builder::as_cons, assign, tag_pin>::operate();
    }

    /// resets the given bits to zero.
    /// See also init_as_output.
    template< typename list_builder>
    inline PIN_DEF_ALWAYS_INLINE void reset( const list_builder &)
    {
        detail::for_each_port_operator< typename list_builder::as_cons, reset_bits, tag_port>::operate();
    }

    /// clear() is an alias for reset().
    template< typename list_builder>
    inline PIN_DEF_ALWAYS_INLINE void clear( const list_builder &pins)
    {
        reset( pins);
    }

    /// write a value to the given output pin or pin-group.
    template< typename pins_type>
    inline void write( const pins_type &, uint8_t value)
    {
        uint8_t shifted = (value << pins_type::shift) & pins_type::mask;
        volatile uint8_t &port = get_port<pins_type::port>( tag_port());
        port = (port & ~pins_type::mask) | shifted;
    }

    /// overload of the write function for single pins.
    /// This helps the optimizer to create the shortest code possible for single pin writes.
    template< PortPlaceholder port_, uint8_t bit_>
    inline void write( const pin_definition<port_, bit_> pin, uint8_t value)
    {
        if (value) set( pin);
        else reset(pin);
    }

    /// read a value from the given input pin or pin-group
    template< typename pins_type>
    inline uint8_t read( const pins_type &)
    {
        return (get_port<pins_type::port>( tag_pin()) & pins_type::mask)
                    >> pins_type::shift;
    }

    inline uint8_t read( const null_pin_type &)
    {
        return 0;
    }

    /// returns true iff at least one of the bits in the pin-definition or pin-group is set.
    template< typename pins_type>
    inline bool is_set( const pins_type &)
    {
        return (get_port<pins_type::port>( tag_pin()) & pins_type::mask) != 0;
    }

    inline bool is_set( const null_pin_type &)
    {
        return false;
    }

}


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattributes"

#define PIN_TYPE( p_, bit_) \
    pin_definitions::pin_definition< pin_definitions::port_##p_, bit_>

#define DECLARE_PIN( name_, p_, bit_) \
    PIN_TYPE( p_, bit_) __attribute__((unused)) name_ ;

#define PIN_GROUP_TYPE( p_, first_bit_, bit_count_) \
    pin_definitions::pin_group< pin_definitions::port_##p_, first_bit_, bit_count_>

#define DECLARE_PIN_GROUP( name_, p_, first_bit_, bit_count_) \
    PIN_GROUP_TYPE( p_, first_bit_, bit_count_) __attribute__((unused)) name_ ;

#pragma GCC diagnostic pop

#endif //PIN_DEFINITIONS_HPP_
