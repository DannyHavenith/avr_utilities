//
//  Copyright (C) 2015 Danny Havenith
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef AVR_UTILITIES_DATASHEET_WRITE_HPP_
#define AVR_UTILITIES_DATASHEET_WRITE_HPP_

#include <boost/mpl/vector.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl/sort.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/fold.hpp>
#include <boost/mpl/int.hpp>
#include <boost/mpl/front.hpp>
#include <boost/mpl/pop_front.hpp>
#include <boost/mpl/empty.hpp>

namespace datasheet
{
namespace detail
{
using boost::mpl::int_;
using boost::mpl::at;
using boost::mpl::push_back;
using boost::mpl::vector;


template<typename Range>
struct get_address{};

template< unsigned address>
struct get_address< register_type< address>>
{
    static constexpr unsigned value = address;
};

template<typename register_, unsigned highest_bit, unsigned lowest_bit, typename value_type, value_type default_value>
struct get_address< bit_range< register_, highest_bit, lowest_bit, value_type, default_value>>
{
    static constexpr unsigned value = get_address<register_>::value;
};

template< typename mapped_bit>
struct get_address< assigned_bit< mapped_bit>>
    : get_address< mapped_bit> {};

template< typename Range>
struct get_mask{};

template<typename register_, unsigned highest_bit, unsigned lowest_bit, typename value_type, value_type default_value>
struct get_mask< bit_range< register_, highest_bit, lowest_bit, value_type, default_value>>
{
    static constexpr unsigned value =
                (1 << (highest_bit + 1)) - 1
               -((1 << lowest_bit) -1);
};

template< typename mapped_bit>
struct get_mask< assigned_bit<mapped_bit>>
    : get_mask< mapped_bit> {};

template< typename Range>
struct get_shift {};

template<typename register_, unsigned highest_bit, unsigned lowest_bit, typename value_type, value_type default_value>
struct get_shift< bit_range< register_, highest_bit, lowest_bit, value_type, default_value>>
{
    static constexpr int value = lowest_bit;
};

template< typename mapped_bit>
struct get_shift< assigned_bit<mapped_bit>>
    : get_shift< mapped_bit> {};


template<typename Range>
struct get_highest_bit{};

template<typename register_, unsigned highest_bit, unsigned lowest_bit, typename value_type, value_type default_value>
struct get_highest_bit< bit_range< register_, highest_bit, lowest_bit, value_type, default_value>>
{
    static constexpr int value = highest_bit;
};

template< typename mapped_bit>
struct get_highest_bit< assigned_bit< mapped_bit>>
    : get_highest_bit< mapped_bit> {};


struct range_in_lower_register
{
    template<typename L, typename R>
    struct apply :
            boost::mpl::bool_<
                    (get_address<L>::value) < (get_address<R>::value)
                ||      ((get_address<L>::value) == (get_address<R>::value))
                    &&  ((get_highest_bit<L>::value) > (get_highest_bit<R>::value))
            >
    {
    };
};

struct group_by_register
{
    template<
        typename State, typename Assignment,
        uint8_t current_address = at<State, int_<0>>::type::value,
        uint8_t assignment_address = get_address<Assignment>::value
        >
    struct apply
    {
        typedef typename at< State, int_<1>>::type CurrentGroup;
        typedef typename at< State, int_<2>>::type GroupList;
        typedef vector<
                    int_<assignment_address>,
                    vector<Assignment>,
                    typename push_back<GroupList, CurrentGroup>::type
                > type;
    };

    template<
        typename State, typename Assignment,
        uint8_t address
        >
    struct apply<State, Assignment, address, address>
    {
        typedef typename at< State, int_<1>>::type CurrentGroup;
        typedef typename at< State, int_<2>>::type GroupList;
        typedef vector<
                    int_<address>,
                    typename push_back< CurrentGroup, Assignment>::type,
                    GroupList
                    >
                type;
    };
};


template< typename GroupSequence, bool empty = (boost::mpl::empty<GroupSequence>::type::value)>
struct ForEach
{
    template< typename Operation, typename... Arguments>
    static Operation Apply( Operation op, const Arguments &... arguments)
    {
        using boost::mpl::front;
        using boost::mpl::pop_front;

        typedef typename front< GroupSequence>::type FirstType;
        op.template Apply<FirstType>( arguments...);

        return ForEach< typename pop_front<GroupSequence>::type>::Apply( op, arguments...);
    }

};

template< typename GroupSequence>
struct ForEach< GroupSequence, true>
{
    template< typename Operation, typename... Arguments>
    static Operation Apply( Operation op, const Arguments &...)
    {
        // do nothing, empty sequence
        return op;
    }
};

template< typename SoughtAssignmentType>
struct ValueGetter
{
    template< typename... Assignments>
    static int GetValue( const SoughtAssignmentType &assignment, const Assignments &...)
    {
        return assignment.assigned_value;
    }

    template<typename FirstAssignment, typename... Assignments>
    static int GetValue( const FirstAssignment &, const Assignments&... assignments)
    {
        return GetValue( assignments...);
    }
};

/**
 * Special case: a bit range of a single bit was given as an argument. This should result in
 * the simple value "1" being returned (mentioning a single bit range means "set it to true").
 */
template<
    typename reg,
    unsigned bit,
    bool default_value>
struct ValueGetter< bit_range< reg, bit, bit, bool, default_value>>
{
    template<typename FirstAssignment, typename... Assignments>
    static int GetValue( const FirstAssignment &, const Assignments&... assignments)
    {
        return 1;
    }
};

struct HarvestAssignments
{
    uint8_t value{0};
    uint8_t mask{0};

    template< typename AssignmentType, typename... Assignments>
    void Apply( const Assignments &... assignments)
    {
        value |= (ValueGetter< AssignmentType>::GetValue( assignments...)
                    << get_shift<AssignmentType>::value)
                        & get_mask<AssignmentType>::value;
        mask |= get_mask<AssignmentType>::value;
    }
};

template< typename Device>
struct ExecuteAssignmenGroups
{
    ExecuteAssignmenGroups( Device &device)
    :m_device( &device)
    {}

    template< typename GroupType, typename... Assignments>
    void Apply( const Assignments &... assignments)
    {
        HarvestAssignments result =
                ForEach<GroupType>::Apply( HarvestAssignments{}, assignments...);
        typedef typename boost::mpl::front< GroupType>::type FirstAssignmentType;

        m_device->template WriteMasked<get_address<FirstAssignmentType>::value>( result.value, result.mask);
    }

    // using a pointer here instead of a reference,
    // to avoid warnings about non-pod members not being packed.
    Device *m_device;
};

template<typename AssignmentGroups, typename Device, typename... Assignments>
void execute_assignments( Device &device, const Assignments &... assignments)
{
    ExecuteAssignmenGroups<Device> execute{ device};
    ForEach<AssignmentGroups>::Apply( execute, assignments...);
}
} // namespace detail


template<typename Device, typename... Assignments>
void let(  Device &device, const Assignments &... assignments)
{
    using boost::mpl::sort;
    using boost::mpl::vector;
    using boost::mpl::int_;
    using boost::mpl::front;
    using boost::mpl::pop_front;
    using boost::mpl::fold;
    using boost::mpl::push_back;
    using boost::mpl::at;

    using detail::range_in_lower_register;

    // Sort all the register range types in the tuple by register address,
    // increasing and then by bit position, decreasing.
    typedef typename sort<vector<Assignments...>, range_in_lower_register>::type
            SortedRanges;

    // Now group the ranges by register.
    // First take the first assignment...
    typedef typename front<SortedRanges>::type Front;
    // ...and the other assignments.
    typedef typename pop_front<SortedRanges>::type Tail;

    // Create an accumulator state for the fold meta function. This state
    // consists of an address, a group of assignments that assign to that register and
    // a vector of vectors that contains the groups of previously combined assignments.
    typedef vector<
            int_<detail::get_address<Front>::value>,
            vector<Front>,
            vector<>
        > InitialState;

    // Now start grouping
    typedef typename fold< Tail, InitialState , detail::group_by_register>::type
            PartiallyGrouped;

    // The last group was not added to the vector of groups yet. do that now.
    typedef typename push_back<
            typename at< PartiallyGrouped, int_<2>>::type,
            typename at< PartiallyGrouped, int_<1>>::type
            >::type Grouped;

    // at this point, Grouped is a vector that contains groups of assigment types.
    // Where each group consists of assignments to the same register.
    detail::execute_assignments<Grouped>( device, assignments...);
}
}
#endif /* AVR_UTILITIES_DATASHEET_WRITE_HPP_ */
