/*=============================================================================
    Copyright (c) 2014-2015 Kohei Takahashi

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#ifndef FUSION_VECTOR_11052014_1625
#define FUSION_VECTOR_11052014_1625

#include <boost/config.hpp>
#include <boost/fusion/support/config.hpp>
#include <boost/fusion/container/vector/detail/config.hpp>
#include <boost/fusion/container/vector/vector_fwd.hpp>

///////////////////////////////////////////////////////////////////////////////
// Without variadics, we will use the PP version
///////////////////////////////////////////////////////////////////////////////
#if !defined(BOOST_FUSION_HAS_VARIADIC_VECTOR)
# include <boost/fusion/container/vector/detail/cpp03/vector.hpp>
#else

///////////////////////////////////////////////////////////////////////////////
// C++11 interface
///////////////////////////////////////////////////////////////////////////////
#include <boost/fusion/support/sequence_base.hpp>
#include <boost/fusion/support/is_sequence.hpp>
#include <boost/fusion/support/detail/index_sequence.hpp>
#include <boost/fusion/container/vector/detail/at_impl.hpp>
#include <boost/fusion/container/vector/detail/value_at_impl.hpp>
#include <boost/fusion/container/vector/detail/begin_impl.hpp>
#include <boost/fusion/container/vector/detail/end_impl.hpp>
#include <boost/fusion/iterator/deref.hpp>
#include <boost/fusion/sequence/intrinsic/begin.hpp>
#include <boost/fusion/sequence/intrinsic/end.hpp>
#include <boost/fusion/support/detail/and.hpp>
#include <boost/fusion/support/detail/is_conversion_construction.hpp>
#include <boost/fusion/support/detail/is_conversion_assignment.hpp>
#include <boost/core/enable_if.hpp>
#include <boost/mpl/and.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/int.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits/integral_constant.hpp>
#include <boost/type_traits/is_convertible.hpp>
#include <boost/type_traits/remove_cv.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <cstddef>
#include <utility>

namespace boost { namespace fusion
{
    struct vector_tag;
    struct random_access_traversal_tag;

    namespace vector_detail
    {
        struct from_sequence {};
        struct each_elem {};

        template<typename T>
        struct is_not_vector_impl : true_type {};

        template<typename ...U>
        struct is_not_vector_impl<fusion::vector<U...>> : false_type {};

        template<typename T>
        struct is_not_vector 
            : is_not_vector_impl<
                  typename remove_cv<typename remove_reference<T>::type>::type
              >
        {};

        template<typename ...T>
        void expand(T&&...) {}

        template<
            typename Sequence
          , typename Iterator
          , typename Deref = typename fusion::result_of::deref<Iterator>::type
          , typename Result = typename mpl::if_<
                is_lvalue_reference<Sequence>
              , Deref
              , Deref //, typename add_rvalue_reference<Deref>::type
            >::type
        >
        Result forward(Iterator const& iterator)
        {
            return static_cast<Result>(fusion::deref(iterator));
        }

        // Object proxy since preserve object order
        template <std::size_t N, typename T>
        struct store
        {
            BOOST_CONSTEXPR BOOST_FUSION_GPU_ENABLED
            store()
                : elem() // value-initialized explicitly
            {}

            BOOST_CONSTEXPR BOOST_FUSION_GPU_ENABLED
            store(store const& rhs)
                : elem(rhs.get())
            {}

            BOOST_CXX14_CONSTEXPR BOOST_FUSION_GPU_ENABLED
            store&
            operator=(store const& rhs)
            {
                elem = rhs.get();
                return *this;
            }

            BOOST_CONSTEXPR BOOST_FUSION_GPU_ENABLED
            store(store&& rhs)
                : elem(static_cast<T&&>(rhs.get()))
            {}

            BOOST_CXX14_CONSTEXPR BOOST_FUSION_GPU_ENABLED
            store&
            operator=(store&& rhs)
            {
                elem = static_cast<T&&>(rhs.get());
                return *this;
            }

            template <typename U>
            BOOST_CONSTEXPR BOOST_FUSION_GPU_ENABLED
            store(each_elem, U&& rhs)
                : elem(std::forward<U>(rhs))
            {}

            template <typename U>
            BOOST_CXX14_CONSTEXPR BOOST_FUSION_GPU_ENABLED
            store&
            assign_elem(U&& rhs)
            {
                elem = std::forward<U>(rhs);
                return *this;
            }
 
            template<typename U>
            BOOST_CONSTEXPR BOOST_FUSION_GPU_ENABLED
            store(store<N, U> const& rhs)
                : elem(rhs.get())
            {}

            template<typename U>
            BOOST_CXX14_CONSTEXPR BOOST_FUSION_GPU_ENABLED
            store&
            assign (store<N, U> const& rhs)
            {
                elem = rhs.get();
                return *this;
            }

            template<typename U>
            BOOST_CONSTEXPR BOOST_FUSION_GPU_ENABLED
            store(store<N, U>&& rhs)
                : elem(static_cast<U&&>(rhs.get()))
            {}

            template<typename U>
            BOOST_CXX14_CONSTEXPR BOOST_FUSION_GPU_ENABLED
            store&
            assign (store<N, U>&& rhs)
            {
                elem = static_cast<U&&>(rhs.get());
                return *this;
            }

            BOOST_CXX14_CONSTEXPR BOOST_FUSION_GPU_ENABLED
            T      & get()       { return elem; }
            BOOST_CONSTEXPR BOOST_FUSION_GPU_ENABLED
            T const& get() const { return elem; }

            T elem;
        };

        template <typename I, typename ...T>
        struct vector_data;

        template <std::size_t ...I, typename ...T>
        struct vector_data<detail::index_sequence<I...>, T...>
            : store<I, T>...
            , sequence_base<vector_data<detail::index_sequence<I...>, T...> >
        {
            typedef vector_tag                  fusion_tag;
            typedef fusion_sequence_tag         tag; // this gets picked up by MPL
            typedef mpl::false_                 is_view;
            typedef random_access_traversal_tag category;
            typedef mpl::int_<sizeof...(T)>     size;
            typedef vector<T...>                type_sequence;

            BOOST_CONSTEXPR BOOST_FUSION_GPU_ENABLED
            vector_data() : store<I, T>()...
            {}

            template<typename J, typename ...U>
            BOOST_CONSTEXPR BOOST_FUSION_GPU_ENABLED
            vector_data(vector_data<J, U...> const& rhs) : store<I, T>(rhs)...
            {
                // U is not expanded with I, check
                BOOST_STATIC_ASSERT(sizeof...(U) == sizeof...(I));
            }

            template<typename J, typename ...U>
            BOOST_CONSTEXPR BOOST_FUSION_GPU_ENABLED
            vector_data(vector_data<J, U...>&& rhs)
                : store<I, T>(std::move(rhs))...
            {
                // U is not expanded with I, check
                BOOST_STATIC_ASSERT(sizeof...(U) == sizeof...(I));
            }

            template <typename ...U>
            BOOST_CONSTEXPR BOOST_FUSION_GPU_ENABLED
            vector_data(each_elem, U&&... var)
                : store<I, T>(each_elem(), std::forward<U>(var))...
            {}

            template<typename J, typename ...U>
            BOOST_CXX14_CONSTEXPR BOOST_FUSION_GPU_ENABLED
            void
            assign(vector_data<J, U...> const& from)
            {
                // U is not expanded with I, check
                BOOST_STATIC_ASSERT(sizeof...(U) == sizeof...(I));
                expand(store<I, T>::assign(from)...);
            }

            template<typename J, typename ...U>
            BOOST_CXX14_CONSTEXPR BOOST_FUSION_GPU_ENABLED
            void
            assign(vector_data<J, U...>&& from)
            {
                // U is not expanded with I, check
                BOOST_STATIC_ASSERT(sizeof...(U) == sizeof...(I));
                expand(store<I, T>::assign(std::move(from))...);
            }

            template<typename ...U>
            BOOST_CXX14_CONSTEXPR BOOST_FUSION_GPU_ENABLED
            void
            assign_each(U&&... var)
            {
                expand(store<I, T>::assign_elem(std::forward<U>(var))...);
            }

            template <std::size_t N, typename U>
            static BOOST_CXX14_CONSTEXPR BOOST_FUSION_GPU_ENABLED
            auto at_detail(store<N, U>* this_) -> decltype(this_->get())
            {
                return this_->get();
            }

            template <std::size_t N, typename U>
            static BOOST_CONSTEXPR BOOST_FUSION_GPU_ENABLED
            auto at_detail(store<N, U> const* this_) -> decltype(this_->get())
            {
                return this_->get();
            }

            template <typename J>
            BOOST_CXX14_CONSTEXPR BOOST_FUSION_GPU_ENABLED
            auto at_impl(J) -> decltype(at_detail<J::value>(this))
            {
                return at_detail<J::value>(this);
            }

            template <typename J>
            BOOST_CONSTEXPR BOOST_FUSION_GPU_ENABLED
            auto at_impl(J) const -> decltype(at_detail<J::value>(this))
            {
                return at_detail<J::value>(this);
            }

            template <std::size_t N, typename U>
            static BOOST_FUSION_GPU_ENABLED
            mpl::identity<U> value_at_impl(store<N, U>*);
        };

        template<typename ...T>
        struct base {
            typedef vector_data<
                typename detail::make_index_sequence<sizeof...(T)>::type
              , T...
            > type;
        };
    } // namespace boost::fusion::vector_detail

    template <typename... T>
    struct vector : vector_detail::base<T...>::type
    {
        typedef typename vector_detail::base<T...>::type base;

        BOOST_CONSTEXPR BOOST_FUSION_GPU_ENABLED
        vector() : base()
        {}

        template<
            typename... U
          , typename = typename boost::enable_if<
                fusion::detail::and_<is_convertible<U, T>...>
            >::type
        >
        // XXX: constexpr become error due to pull-request #79, booooo!!
        //      In the (near) future release, should be fixed.
        /* BOOST_CONSTEXPR */ BOOST_FUSION_GPU_ENABLED
        explicit vector(U&&... u)
            : base(vector_detail::each_elem(), std::forward<U>(u)...)
        {}

        template<
            typename ...U
          , typename = typename boost::enable_if<
                fusion::detail::and_<is_convertible<U, T>...>
            >::type
        >
        BOOST_CONSTEXPR BOOST_FUSION_GPU_ENABLED
        vector(vector<U...> const& rhs) : base(rhs)
        {}

        template<
            typename ...U
          , typename = typename boost::enable_if<
                fusion::detail::and_<is_convertible<U, T>...>
            >::type
        >
        BOOST_CONSTEXPR BOOST_FUSION_GPU_ENABLED
        vector(vector<U...>&& rhs) : base(std::move(rhs))
        {}

        template<
            typename Sequence
          , typename = typename boost::enable_if<
                mpl::and_<
                    vector_detail::is_not_vector<Sequence>
                  , fusion::detail::is_conversion_construction<vector, Sequence>
                >
            >::type
        >
        BOOST_FUSION_GPU_ENABLED
        vector(Sequence&& seq)
            : vector(
                  vector_detail::from_sequence()
                , std::forward<Sequence>(seq)
                , typename detail::make_index_sequence<sizeof...(T)>::type())
        {}

        template<typename Sequence, std::size_t ...I>
        BOOST_FUSION_GPU_ENABLED
        vector(vector_detail::from_sequence, Sequence&& seq, detail::index_sequence<I...>)
            : base(
                  vector_detail::each_elem()
                , vector_detail::forward<Sequence>(advance_c<I>(begin(seq)))...)
        {}

        template<typename ...U>
        vector& operator=(vector<U...> const& rhs)
        {
            base::assign(rhs);
            return *this;
        }

        template<typename ...U>
        vector& operator=(vector<U...>&& rhs)
        {
            base::assign(std::move(rhs));
            return *this;
        }

        template <typename Sequence>
        BOOST_CXX14_CONSTEXPR BOOST_FUSION_GPU_ENABLED
        typename boost::enable_if<
            mpl::and_<
                vector_detail::is_not_vector<Sequence>
              , fusion::detail::is_conversion_assignment<vector, Sequence>
            >
          , vector&
        >::type
        operator=(Sequence&& rhs)
        {
            assign(
                std::forward<Sequence>(rhs)
              , typename detail::make_index_sequence<sizeof...(T)>::type());
            return *this;
        }

        template<typename Sequence, std::size_t ...I>
        BOOST_CXX14_CONSTEXPR BOOST_FUSION_GPU_ENABLED
        void assign(Sequence&& seq, detail::index_sequence<I...>)
        {
            base::assign_each(
                vector_detail::forward<Sequence>(advance_c<I>(begin(seq)))...
            );
        }
    };
}}

#endif
#endif

