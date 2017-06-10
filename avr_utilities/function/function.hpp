//
//  Copyright (C) 2017 Danny Havenith
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
/**
 * This file implements a very rudimentary version of something akin to std::function<>
 */
#ifndef FUNCTION_FUNCTION_HPP_
#define FUNCTION_FUNCTION_HPP_

namespace function
{
    template <typename RT>
    class if_void
    {};

    template<>
    struct if_void<void>
    {
        typedef void type;
    };

    template <typename RT>
    struct if_not_void
    {
        typedef RT type;
    };

    template<>
    struct if_not_void<void>
    {
    };

    // simplified implementation of forward, I'll always remember to explicitly
    // specify the template type, I promise.
    template< typename T>
    T&& forward( T && val)
    {
        return static_cast<T&&>(val);
    }

    template<typename T>
    class function{};

    template< typename ReturnType, typename... Args>
    class function< ReturnType (Args...)>
    {
    public:

        /// construct a function object with a free function
        function( ReturnType (*f)(Args...))
        :m_object{nullptr}, m_f{ f}
        {}

        /// construct a functin object with a member function and
        /// an object pointer.
        template<typename ObjectType>
        function( ObjectType *object, ReturnType (ObjectType::*mf)(Args...))
        :m_object{ reinterpret_cast<PlaceHolder *>(object)},
         m_mf{ reinterpret_cast<MemberFunction>( mf)}
        {
        }

        function()
        :m_object{nullptr}, m_f{ nullptr}
        {}


        /// function call operator for non-void function types
        template<typename RT = ReturnType, typename... Arguments>
        typename if_not_void<RT>::type operator()( Arguments&&... args)
        {
            return m_object ?
                    (m_object->*m_mf)( forward<Arguments>(args)...)
                    :m_f(forward<Arguments>(args)...);
        }

        /// function call operator for void function types
        template<typename RT = ReturnType, typename... Arguments>
        typename if_void<RT>::type operator()( Arguments &&... args)
        {
            if (m_object)
            {
                    (m_object->*m_mf)(forward<Arguments>(args)...);
            }
            else
            {
                    m_f(forward<Arguments>(args)...);
            }
        }

        explicit operator bool() const
        {
            return m_object != nullptr || m_f != nullptr;
        }

    private:

        class PlaceHolder;
        using MemberFunction = ReturnType (PlaceHolder::*)( Args...);
        using FreeFunction = ReturnType (*)( Args...);

        PlaceHolder *m_object;
        union {
            FreeFunction m_f;
            MemberFunction m_mf;
        };
    };
}

#endif /* FUNCTION_FUNCTION_HPP_ */
