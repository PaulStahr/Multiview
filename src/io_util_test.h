#include "io_util.h"
BOOST_AUTO_TEST_SUITE(io_util)

BOOST_AUTO_TEST_CASE( constructors_test )
{
    
}

BOOST_AUTO_TEST_CASE( split_iterator_test )
{
    std::string str("foo batz  bar 42");
    auto split_iter = IO_UTIL::make_split_iterator(str, [](char c){return c == ' ';});
    BOOST_CHECK_EQUAL(*split_iter, "foo" );
    ++split_iter;
    BOOST_CHECK_EQUAL(*split_iter, "batz" );
    ++split_iter;
    BOOST_CHECK_EQUAL(*split_iter, "bar" );
    ++split_iter;
    int x = 0;
    split_iter.parse(x);
    BOOST_CHECK_EQUAL(x, 42);
    ++split_iter;
    BOOST_TEST(!split_iter.valid());   
}

BOOST_AUTO_TEST_SUITE_END()
