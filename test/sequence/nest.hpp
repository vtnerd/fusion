/*=============================================================================
    Copyright (C) 2015 Kohei Takahshi

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/

#include <boost/config.hpp>
#include <boost/fusion/include/as_deque.hpp>
#include <boost/fusion/include/as_list.hpp>
#include <boost/fusion/include/as_vector.hpp>
#include <boost/fusion/include/deque.hpp>
#include <boost/fusion/include/is_sequence.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/mpl/if.hpp>
#include <boost/ref.hpp>
#include <boost/type_traits/add_const.hpp>
#include <boost/type_traits/add_reference.hpp>
#include <boost/type_traits/is_reference.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/type_traits/remove_const.hpp>
#include <boost/type_traits/remove_reference.hpp>

#define FUSION_TEST_dequedeque 1
#define FUSION_TEST_listlist 1
#define FUSION_TEST_tupletuple 1
#define FUSION_TEST_vectorvector 1

// do not expand directly, use FUSION_TEST_IS
#define FUSION_TEST_IS_(seq)                                        \
    BOOST_PP_CAT(FUSION_TEST_, BOOST_PP_CAT(seq, FUSION_SEQUENCE))

#define FUSION_TEST_IS(seq) FUSION_TEST_IS_(seq)

template<typename T>
struct add_const_reference
    : boost::add_reference<typename boost::add_const<T>::type>
{};

template<typename T, template<typename> class As>
struct if_sequence_convert 
    : boost::mpl::eval_if<
          boost::fusion::traits::is_sequence<T>
        , As<T>
        , boost::mpl::identity<T>
      >
{};

template<typename T>
T make()
{
    T t = T();
    return t;
}

template<typename Seq, typename Arg>
void test_construct_from_1rvalue()
{
    Seq dest(make<Arg>());
    (void)dest;
}

template<typename Seq, typename Arg1, typename Arg2>
void test_construct_from_2rvalue()
{
    Seq dest(make<Arg1>(), make<Arg2>());
    (void)dest;
}

template<typename Seq, typename Arg1, typename Arg2, typename Arg3>
void test_construct_from_3rvalue()
{
    Seq dest(make<Arg1>(), make<Arg2>(), make<Arg3>());
    (void)dest;
}

template<typename Seq, typename Arg>
void test_construct_from_1lvalue()
{
    Arg arg = make<Arg>();
    Seq dest(arg);
    (void)dest;
}

template<typename Seq, typename Arg1, typename Arg2>
void test_construct_from_2lvalue()
{
    Arg1 arg1 = make<Arg1>();
    Arg2 arg2 = make<Arg2>();
    Seq dest(arg1, arg2);
    (void)dest;
}

template<typename Seq, typename Arg1, typename Arg2, typename Arg3>
void test_construct_from_3lvalue()
{
    Arg1 arg1 = make<Arg1>();
    Arg2 arg2 = make<Arg2>();
    Arg3 arg3 = make<Arg3>();
    Seq dest(arg1, arg2, arg3);
    (void)dest;
}

template<typename Seq, typename Arg>
void test_construct_from_both_1lvalue()
{
    test_construct_from_1lvalue<Seq, typename boost::remove_const<Arg>::type>();
    test_construct_from_1lvalue<Seq, typename boost::add_const<Arg>::type>();
}

template<typename Seq, typename Arg1, typename Arg2>
void test_construct_from_both_2lvalue()
{
    test_construct_from_2lvalue<
        Seq
      , typename boost::remove_const<Arg1>::type
      , typename boost::remove_const<Arg2>::type>();

    test_construct_from_2lvalue<
        Seq
      , typename boost::add_const<Arg1>::type
      , typename boost::add_const<Arg2>::type
    >();
}

template<typename Seq, typename Arg1, typename Arg2, typename Arg3>
void test_construct_from_both_3lvalue()
{
    test_construct_from_3lvalue<
        Seq
      , typename boost::remove_const<Arg1>::type
      , typename boost::remove_const<Arg2>::type
      , typename boost::remove_const<Arg3>::type
    >();

    test_construct_from_3lvalue<
        Seq
      , typename boost::add_const<Arg1>::type
      , typename boost::add_const<Arg2>::type
      , typename boost::add_const<Arg3>::type
    >();
}

template<typename Seq, typename Arg>
void test_construct_from_1()
{
    test_construct_from_1rvalue<Seq, Arg>();
    test_construct_from_both_1lvalue<Seq, Arg>();
}

template<typename Seq, typename Arg1, typename Arg2>
void test_construct_from_2()
{
    test_construct_from_2rvalue<Seq, Arg1, Arg2>();
    test_construct_from_both_2lvalue<Seq, Arg1, Arg2>();
}

template<typename Seq, typename Arg1, typename Arg2, typename Arg3>
void test_construct_from_3()
{
    test_construct_from_3rvalue<Seq, Arg1, Arg2, Arg3>();
    test_construct_from_both_3lvalue<Seq, Arg1, Arg2, Arg3>();
}

template<typename Seq, typename Arg>
void test_assign_from_rvalue()
{
    Seq dest;
    dest = make<Arg>();
}

template<typename Seq, typename Arg>
void test_assign_from_lvalue()
{
    Seq dest;
    Arg arg;
    dest = arg; 
}

template<typename Seq, typename Arg>
void test_assign_from_both_lvalue()
{
    test_assign_from_lvalue<Seq, typename boost::remove_const<Arg>::type>();
    test_assign_from_lvalue<Seq, typename::boost::add_const<Arg>::type>();
}

template<typename Seq, typename Arg>
void test_assign()
{
    test_assign_from_rvalue<Seq, Arg>();
    test_assign_from_both_lvalue<Seq, Arg>();
}

template <typename Seq, typename From>
void test_conversion()
{
#if !FUSION_TEST_IS(tuple) && \
    !(!defined(BOOST_FUSION_HAS_VARIADIC_DEQUE) && FUSION_TEST_IS(deque))

    // C++03 tuple does not provide conversion construction, and this appears
    // to be intended. However, conversion assignment is provided ... is that a
    // bug? C++11 is identical to 03 now ...
    // ... OR ...
    // C++03 deque conversion construction fails (bug)
    test_construct_from_1<Seq, From>();
#endif

#if !FUSION_TEST_IS(deque) &&                               \
    !(!defined(BOOST_FUSION_HAS_VARIADIC_VECTOR) &&         \
        (FUSION_TEST_IS(tuple) || FUSION_TEST_IS(vector)))
    // deque::base only has implicit assignment operator, so conversion
    // assignment fails in C++03 and C++11 (bug)
    // ... OR ...
    // C++03 tuple and vector only has implicit assignment operator, so
    // conversion assignment fails (bug)
    test_assign<Seq, From>();
#endif
}

template<typename Seq>
void test_all_conversion()
{
    test_conversion<
        Seq
      , typename boost::fusion::result_of::as_deque<Seq>::type
    >();
#if !(defined(BOOST_HAS_VARIADIC_VECTOR) && (FUSION_TEST_IS(vector) && FUSION_TEST_IS(tuple)))
    // cons conversion construction SFINAE needs to be updated
    test_conversion<
        Seq
      , typename boost::fusion::result_of::as_list<Seq>::type
    >();
#endif

    test_conversion<
        Seq
      , typename boost::fusion::result_of::as_vector<Seq>::type
    >();
}

template <typename T1>
void test_1_elem()
{
    using namespace boost::fusion;

    typedef FUSION_SEQUENCE<T1> seq;
    typedef FUSION_SEQUENCE<typename boost::add_reference<T1>::type> seq_refs;
    typedef FUSION_SEQUENCE<typename add_const_reference<T1>::type> seq_crefs;

    test_construct_from_1<seq, T1>();

//#if !(defined(BOOST_FUSION_HAS_VARIADIC_VECTOR) && FUSION_TEST_IS(tuple))
    // copying C++11 tuple of size 1 fails (bug)
    test_construct_from_1<seq, seq>();
//#endif

#if !(!defined(BOOST_NO_CXX11_RVALUE_REFERENCES) && FUSION_TEST_IS(deque))
    // deque with r-value references has implicitly deleted copy-assignment
    test_assign<seq, seq>();
#endif

#if !(!defined(BOOST_FUSION_HAS_VARIADIC_VECTOR) && \
    (FUSION_TEST_IS(vector) || FUSION_TEST_IS(tuple)))
    // C++03 vector of size 1 conversion construction fails (bug)
    test_all_conversion<seq>();
#endif

    test_construct_from_1lvalue<seq_refs, T1>();
//    test_construct_from_1lvalue<seq_refs, seq>();

    test_construct_from_both_1lvalue<seq_crefs, T1>();
//    test_construct_from_both_1lvalue<seq_crefs, seq>();
}

template <typename T1, typename T2>
void test_2_elem()
{
    using namespace boost::fusion;

    typedef FUSION_SEQUENCE<T1, T2> seq;
    typedef FUSION_SEQUENCE<
        typename boost::add_reference<T1>::type
      , typename boost::add_reference<T2>::type
    > seq_refs;
    typedef FUSION_SEQUENCE<
        typename add_const_reference<T1>::type
      , typename add_const_reference<T2>::type
    > seq_crefs;

    test_construct_from_2<seq, T1, T2>();
    test_construct_from_1<seq, seq>();

#if !(!defined(BOOST_NO_CXX11_RVALUE_REFERENCES) && FUSION_TEST_IS(deque))
    // deque with r-value references has implicitly deleted copy-assignment
    test_assign<seq, seq>();
#endif

    test_all_conversion<seq>();

    test_construct_from_2lvalue<seq_refs, T1, T2>();
//    test_construct_from_1lvalue<seq_refs, seq>();

    test_construct_from_both_2lvalue<seq_crefs, T1, T2>();
//    test_construct_from_both_1lvalue<seq_crefs, seq>();
}

template <typename T1, typename T2, typename T3>
void test_3_elem()
{
    using namespace boost::fusion;

    typedef FUSION_SEQUENCE<T1, T2, T3> seq;
    typedef FUSION_SEQUENCE<
        typename boost::add_reference<T1>::type
      , typename boost::add_reference<T2>::type
      , typename boost::add_reference<T3>::type
    > seq_refs;
    typedef FUSION_SEQUENCE<
        typename add_const_reference<T1>::type
      , typename add_const_reference<T2>::type
      , typename add_const_reference<T3>::type
    > seq_crefs;

    test_construct_from_3<seq, T1, T2, T3>();
    test_construct_from_1<seq, seq>();

#if !(!defined(BOOST_NO_CX11_RVALUE_REFERENCES) && FUSION_TEST_IS(deque))
    // deque with r-value references has implicitly deleted copy-assignment
    test_assign<seq, seq>();
#endif

    test_all_conversion<seq>();

    test_construct_from_3lvalue<seq_refs, T1, T2, T3>();
//    test_construct_from_1lvalue<seq_refs, seq>();

    test_construct_from_both_3lvalue<seq_crefs, T1, T2, T3>();
//    test_construct_from_both_1lvalue<seq_crefs, seq>();
}

template<typename T1>
void test_all_1_elem()
{
    using namespace boost::fusion::result_of;

    test_1_elem<T1>();

#if defined(BOOST_NO_CXX11_RVALUE_REFERENCES)
    // implicitly deleted assignment operator (bug)
    test_1_elem<typename if_sequence_convert<T1, as_deque>::type>();
#endif

#if !FUSION_TEST_IS(list) && !FUSION_TEST_IS(deque)
    // as_list will make inner element a `cons<Head, Tail>` type, and if
    // outter type is `list` or `deque` then construction from element will
    // fail (bug). i.e. cons<cons<int, nil>, nil> foo(con<int, nil>()); calls
    // copy-from-another-cons-type constructor but it should call construct
    // from element constructor.
    test_1_elem<typename if_sequence_convert<T1, as_list>::type>();
#endif

#if !defined(BOOST_FUSION_HAS_VARIADIC_VECTOR)
    // is_convertible is not supported; C++11 does not have proper SFINAE
    // of its constructors
    test_1_elem<typename if_sequence_convert<T1, as_vector>::type>();
#endif
}

template<typename T1, typename T2>
void test_all_2_elem()
{
    using namespace boost::fusion::result_of;

    test_2_elem<T1, T2>();

#if defined(BOOST_NO_CXX11_RVALUE_REFERENCES)
    // implicitly deleted assignment operator (bug) 
    test_2_elem<
        typename if_sequence_convert<T1, as_deque>::type
      , typename if_sequence_convert<T2, as_deque>::type
    >();
#endif

#if !FUSION_TEST_IS(list) && !FUSION_TEST_IS(deque)
    // as_list will make inner element a `cons<Head, Tail>` type, and if
    // outter type is `list` or `deque` then construction from element will
    // fail (bug). i.e. cons<cons<int, nil>, nil> foo(con<int, nil>()); calls
    // copy-from-another-cons-type constructor but it should call construct
    // from element constructor.
    test_2_elem<
        typename if_sequence_convert<T1, as_list>::type
      , typename if_sequence_convert<T2, as_list>::type
    >();
#endif

#if !defined(BOOST_FUSION_HAS_VARIADIC_VECTOR)
    // is_convertible is not supported; C++11 does not have proper SFINAE
    // of its constructors 
    test_2_elem<
        typename if_sequence_convert<T1, as_vector>::type
      , typename if_sequence_convert<T2, as_vector>::type
    >();
#endif
}

template<typename T1, typename T2, typename T3>
void test_all_3_elem()
{
    using namespace boost::fusion::result_of;

    test_3_elem<T1, T2, T3>();

#if defined(BOOST_NO_CXX11_RVALUE_REFERENCES)
    // implicitly deleted assignment operator (bug)  
    test_3_elem<
        typename if_sequence_convert<T1, as_deque>::type
      , typename if_sequence_convert<T2, as_deque>::type
      , typename if_sequence_convert<T3, as_deque>::type
    >();
#endif

#if !FUSION_TEST_IS(list) && !FUSION_TEST_IS(deque)
    // as_list will make inner element a `cons<Head, Tail>` type, and if
    // outter type is `list` or `deque` then construction from element will
    // fail (bug). i.e. cons<cons<int, nil>, nil> foo(con<int, nil>()); calls
    // copy-from-another-cons-type constructor but it should call construct
    // from element constructor.
    test_3_elem<
        typename if_sequence_convert<T1, as_list>::type
      , typename if_sequence_convert<T2, as_list>::type
      , typename if_sequence_convert<T3, as_list>::type
    >();
#endif

#if !defined(BOOST_FUSION_HAS_VARIADIC_VECTOR)
    // is_convertible is not supported; C++11 does not have proper SFINAE
    // of its constructors 
    test_3_elem<
        typename if_sequence_convert<T1, as_vector>::type
      , typename if_sequence_convert<T2, as_vector>::type
      , typename if_sequence_convert<T3, as_vector>::type
    >();
#endif
}

void test()
{
    using namespace boost::fusion;

    test_all_1_elem<FUSION_SEQUENCE<> >();
    test_all_2_elem<FUSION_SEQUENCE<>, int>();
    test_all_2_elem<int, FUSION_SEQUENCE<> >();
    test_all_3_elem<int, FUSION_SEQUENCE<>, float>();

    test_all_1_elem<FUSION_SEQUENCE<int> >();
    test_all_2_elem<FUSION_SEQUENCE<int>, int>();
    test_all_2_elem<int, FUSION_SEQUENCE<int> >();
    test_all_3_elem<int, FUSION_SEQUENCE<int>, float>();

    test_all_1_elem<FUSION_SEQUENCE<int, float> >();
    test_all_2_elem<FUSION_SEQUENCE<int, float>, int>();
    test_all_2_elem<int, FUSION_SEQUENCE<int, float> >();
    test_all_3_elem<int, FUSION_SEQUENCE<int, float>, float>();

    test_all_2_elem<FUSION_SEQUENCE<>, FUSION_SEQUENCE<> >();
    test_all_2_elem<FUSION_SEQUENCE<int>, FUSION_SEQUENCE<> >();
    test_all_2_elem<FUSION_SEQUENCE<>, FUSION_SEQUENCE<int> >();
    test_all_2_elem<FUSION_SEQUENCE<int>, FUSION_SEQUENCE<float> >();
    test_all_2_elem<FUSION_SEQUENCE<int, float>, FUSION_SEQUENCE<float> >();
    test_all_2_elem<FUSION_SEQUENCE<int>, FUSION_SEQUENCE<float, int> >();
    test_all_2_elem<
        FUSION_SEQUENCE<int, float>, FUSION_SEQUENCE<float, int> 
    >();
}

