/*=============================================================================
    Copyright (c) 2016 Lee Clagett

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#ifndef FUSION_AND_07152016_1625
#define FUSION_AND_07152016_1625

#include <boost/type_traits/integral_constant.hpp>

namespace boost { namespace fusion { namespace detail {
    template<typename ...Cond>
    struct and_impl : false_type {};

    template<typename ...T>
    struct and_impl<integral_constant<bool, true>, integral_constant<T, true>...>
        : true_type
    {};
   
    /* fusion::detail::and_ differs from mpl::and_ in the following ways:
       - The empty set is valid and returns false
       - A single element set is valid and returns the identity
       - There is no upper bound on the set size
       - The conditions are evaluated at once, and are not short-circuited. This
         reduces instantations when returning true; the implementation is not
         recursive. */
    template<typename ...Cond>
    struct and_ : and_impl<integral_constant<bool, Cond::value>...>
    {};
}}}

#endif // FUSION_AND_07152016_1625
