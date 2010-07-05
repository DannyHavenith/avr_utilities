
/// pin_definitions.hpp - template classes and functions to support pin definitons as
/// a combination of both a port and a bit of that port
/// This header file allows a style of programming where a programmer defines an output-
/// or input bit as a combination of a port and a bit number and then use the set or reset
/// function to set or reset that particular bit, e.g.
///
/// DEFINE_PIN( my_output, C, 0)
/// set( my_output) // equivalent to PORTC |= _BV(0)
///
/// The templates allow the code to be as efficient as handwritten statements, at the cost of
/// some compilation time.

#if !defined(PIN_DEFINITIONS_HPP_)
#define PIN_DEFINITIONS_HPP_
#include <stdint.h>  // including a std C header in an obvious C++ header file. hmm.
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

#undef DECLARE_PORT_TRAITS


// typedefs for a simple list-type
struct empty_list {};

/// a set of pins or pin groups
template< typename head_, typename tail_ = empty_list>
struct cons
{
    typedef head_ head;
    typedef tail_ tail;

    typedef cons< head_, tail_> as_cons;
};


template< PortPlaceholder port_, uint8_t bit_>
struct pin_definition
{
    static const PortPlaceholder    port = port_;
    static const uint8_t            bit  = bit_;
    static const uint8_t            mask = 1 << bit_;
	static const uint8_t 			shift = bit_;
    typedef cons< pin_definition< port_, bit_>, empty_list> as_cons;
};

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


/// true-value for if_ metafunction
struct true_type { typedef true_type type;};

/// false-value for if_ metafunction
struct false_type { typedef false_type type;};

/// meta-function that returns true if two ports are equal
template< PortPlaceholder left, PortPlaceholder right>
struct is_same_port : false_type {};

template< PortPlaceholder holder>
struct is_same_port< holder, holder> : true_type {};


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



struct null_mask
{
    static const uint8_t mask = 0;
};

/// meta-function that creates a mask of all pins of one port, given a list of pin-
/// or pin group definitions.
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
inline volatile uint8_t &get_port( const port_tag &tag)
{
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

    /// template meta function that removes all pin- and pin group definitions
    /// for a given port from a list.
    /// This meta function is used to remove all pin definitions for a given port,
    /// after the mask for that port has already been constructed and used.
    template< PortPlaceholder port, typename list_type>
    struct remove_port
    {
        typedef typename list_type::head head;
        typedef typename list_type::tail tail;
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
            static inline void operate()
            {
                // initialize the port of the first element of the list
                operation()( get_port<port>( port_tag()), mask_for_port< port, list>::value);

                // now remove all elements for this port and continue to initialize
                for_each_port_operator< typename remove_port< port, typename list::tail>::type, operation, port_tag>::operate();
            }
        };

        /// specialization of the for_each_port_operator metafunction for empty lists.
        template< typename operation, typename port_tag>
        struct for_each_port_operator< empty_list, operation, port_tag>
        {
            static inline void operate()
            {
                // do nothing for the empty list...
            }
        };

    }

    struct assign
    {
        void operator()( volatile uint8_t &reg, uint8_t value) const
        {
            reg = value;
        }
    };

    struct set_bits
    {
        void operator()( volatile uint8_t &reg, uint8_t value) const
        {
            reg |= value;
        }
    };

    struct reset_bits
    {
        void operator()( volatile uint8_t &reg, uint8_t value) const
        {
            reg &= ~value;
        }
    };

    // the following functions use pin definitions to perform common tasks

    /// initialize all ports of the given pin definitions, turning all given pins to output and making all 
    /// bits that are not mentioned in those ports inputs.
    template< typename list_builder>
    inline void init_as_output( const list_builder &)
    {
        detail::for_each_port_operator< typename list_builder::as_cons, assign, tag_ddr>::operate();
    }

    /// make the given pins outputs. This does not affect other pins on the same ports
    template< typename list_builder>
    inline void make_output( const list_builder &)
    {
        detail::for_each_port_operator< typename list_builder::as_cons, set_bits, tag_ddr>::operate();
    }

    /// explicitly make the given pins inputs. This does not affect other pins on the same ports.
    template< typename list_builder>
    inline void make_input( const list_builder &)
    {
        detail::for_each_port_operator< typename list_builder::as_cons, reset_bits, tag_ddr>::operate();
    }

    /// set the given bits to 1, this changes the output ports
    template< typename list_builder>
    inline void set( const list_builder &)
    {
        detail::for_each_port_operator< typename list_builder::as_cons, set_bits, tag_port>::operate();
    }

    /// resets the given bits to zero.
    template< typename list_builder>
    inline void reset( const list_builder &)
    {
        detail::for_each_port_operator< typename list_builder::as_cons, reset_bits, tag_port>::operate();
    }

    /// write a value to the given output pin or pin-group.
    template< typename pins_type>
    inline void write( const pins_type &, uint8_t value)
    {
    	uint8_t shifted = (value << pins_type::shift) & pins_type::mask;
    	volatile uint8_t &port = get_port<pins_type::port>( tag_port());
    	port = (port & ~pins_type::mask) | shifted;
    }

    /// read a value from the given input pin or pin-group
    template< typename pins_type>
    inline uint8_t read( const pins_type &)
    {
    	return (get_port<pins_type::port>( tag_pin()) & pins_type::mask)
    				>> pins_type::shift;
    }

    /// returns true iff at least one of the bits in the pin-definition or pin-group is set.
    template< typename pins_type>
    inline bool is_set( const pins_type &)
    {
    	return (get_port<pins_type::port>( tag_pin()) & pins_type::mask) != 0;
    }
    
}

#define DEFINE_PIN( name_, p_, bit_) \
	__attribute__((unused)) pin_definitions::pin_definition< pin_definitions::port_##p_, bit_>  name_ ;
#define DEFINE_PIN_GROUP( name_, p_, first_bit_, bit_count_) \
	__attribute__((unused)) pin_definitions::pin_group< pin_definitions::port_##p_, first_bit_, bit_count_> name_;

#endif //PIN_DEFINITIONS_HPP_
