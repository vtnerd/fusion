/*=============================================================================
    Copyright (c) 2016 Lee Clagett

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#ifndef FUSION_IS_CONVERSION_ASSIGNMENT_07152016_2347
#define FUSION_IS_CONVERSION_ASSIGNMENT_07152016_2347

#include <boost/fusion/support/detail/is_same_size.hpp>
#include <boost/type_traits/integral_constant.hpp>
#include <boost/type_traits/is_base_of.hpp>
#include <boost/type_traits/remove_cv.hpp>
#include <boost/type_traits/remove_reference.hpp>

namespace boost { namespace fusion { namespace detail {
    template<typename Dest, typename U>
    struct is_conversion_assignment_impl
        : integral_constant<
              bool,
              (/*!is_base_of<Dest, U>::value && */ is_same_size<Dest, U>::value)
          >
    {};

    template<typename Dest, typename U>
    struct is_conversion_assignment
        : is_conversion_assignment_impl<
              typename remove_cv<typename remove_reference<Dest>::type>::type
            , typename remove_cv<typename remove_reference<U>::type>::type
          >
    {};
}}}

#endif
