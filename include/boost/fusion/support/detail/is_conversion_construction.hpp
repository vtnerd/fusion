/*=============================================================================
    Copyright (c) 2016 Lee Clagett

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#ifndef FUSION_IS_CONVERSION_CONSTRUCTION_07162016_0257
#define FUSION_IS_CONVERSION_CONSTRUCTION_07162016_0257

#include <boost/core/enable_if.hpp>
#include <boost/mpl/and.hpp>
#include <boost/mpl/not.hpp>
#include <boost/fusion/iterator/value_of.hpp>
#include <boost/fusion/sequence/intrinsic/begin.hpp>
#include <boost/fusion/sequence/intrinsic/size.hpp>
#include <boost/fusion/support/detail/is_same_size.hpp>
#include <boost/type_traits/integral_constant.hpp>
#include <boost/type_traits/is_base_of.hpp>
#include <boost/type_traits/remove_cv.hpp>
#include <boost/type_traits/remove_reference.hpp>

namespace boost { namespace fusion { namespace detail {
    /* Do not be tempted to make the check:
           not_<is_convertible<Sequence, begin<Dest>>>`
       The conversion expression:
           vector<vector<int>> foo(list<vector<int>>())
       will fail because the SFINAE check incorrectly reduces to:
           not_<not_<is_convertible<list<vector<int>>, int>>> */

    template<typename Sequence>
    struct first_element_
        : fusion::result_of::value_of<
              typename fusion::result_of::begin<Sequence>::type
          >
    {};

    template<typename Dest, typename Source, typename Enable = void>
    struct not_one_element_construction_ : true_type {};

    template<typename Dest, typename Source>
    struct not_one_element_construction_<
        Dest
      , Source
      , typename boost::enable_if_c<
            fusion::result_of::size<Dest>::value == 1
        >::type
    > : is_convertible<
            typename first_element_<Source>::type
          , typename first_element_<Dest>::type
        >
    {};

    template<typename Dest, typename Source>
    struct is_conversion_construction_impl
        : mpl::and_<
              mpl::not_< is_base_of<Dest, Source> >
            , fusion::detail::is_same_size<Dest, Source>
            , not_one_element_construction_<Dest, Source>
          >
    {};
     
    template<typename Dest, typename Source>
    struct is_conversion_construction
        : is_conversion_construction_impl<
              typename remove_cv<typename remove_reference<Dest>::type>::type
            , typename remove_cv<typename remove_reference<Source>::type>::type
          >
    {};
}}}

#endif
