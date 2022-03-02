#include "geometry.h"
BOOST_AUTO_TEST_SUITE(geometry)
namespace utf = boost::unit_test;
namespace tt = boost::test_tools;

BOOST_AUTO_TEST_CASE( interpolate_float_test )
{
    std::map<frameindex_t, float> map;
    map[0] = 0;
    map[3] = 3;
    BOOST_TEST(interpolated(map, 0) == 0, tt::tolerance(0.000001));
    BOOST_TEST(interpolated(map, 1) == 1, tt::tolerance(0.000001));
    BOOST_TEST(interpolated(map, 3) == 3, tt::tolerance(0.000001));
}

BOOST_AUTO_TEST_CASE( smoothed_float_test )
{
    std::map<frameindex_t, float> map;
    map[0] = 0;
    map[3] = 3;
    BOOST_TEST(smoothed(map, 2,-1,-1) == 0,   tt::tolerance(0.000001));
    BOOST_TEST(smoothed(map, 2,-2,-2) == 0,   tt::tolerance(0.000001));
    BOOST_TEST(smoothed(map, 1, 0, 0) == 0,   tt::tolerance(0.000001));
    BOOST_TEST(smoothed(map, 2, 1, 1) == 0.5, tt::tolerance(0.000001));
    BOOST_TEST(smoothed(map, 1, 1, 1) == 1,   tt::tolerance(0.000001));
    BOOST_TEST(smoothed(map, 2, 3, 3) == 1.5, tt::tolerance(0.000001));
    BOOST_TEST(smoothed(map, 1, 3, 3) == 3,   tt::tolerance(0.000001));
    BOOST_TEST(smoothed(map, 1, 0, 3) == 1.5, tt::tolerance(0.000001));
    BOOST_TEST(smoothed(map, 1, 3, 4) == 3,   tt::tolerance(0.000001));
}

BOOST_AUTO_TEST_CASE( interpolate_quaternion_test )
{
    rotation_t r0(1,0,0,0);
    rotation_t r1(0,1,0,0);
    BOOST_TEST(lerp(r0,r1,0)  ==rotation_t(1, 0, 0, 0), tt::tolerance(0.000001));
    BOOST_TEST(lerp(r0,r1,0.5)==rotation_t(sqrt(0.5), sqrt(0.5), 0, 0), tt::tolerance(0.000001));
    BOOST_TEST(lerp(r0,r1,1)  ==rotation_t(0, 1, 0, 0), tt::tolerance(0.000001));
    BOOST_TEST(lerp(r0,r0,0.5)==rotation_t(1, 0, 0, 0), tt::tolerance(0.000001));
}


BOOST_AUTO_TEST_SUITE_END()
