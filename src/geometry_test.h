#include "geometry.h"
#include "io_util.h"

namespace boost
{
template <typename T, size_t N>
inline wrap_stringstream& operator<<(wrap_stringstream& wrapped, matharray<T,N> const& item)
{
    wrapped << '[';
    bool first = true;
    for (auto const& element : item) {
        wrapped << (!first ? "," : "") << element;
        first = false;
    }
    return wrapped << ']';
}

template <typename T, size_t N>
inline wrap_stringstream& operator<<(wrap_stringstream& wrapped, vec3f_t const& item)
{
    wrapped << '[';
    bool first = true;
    for (auto const& element : item) {
        wrapped << (!first ? "," : "") << element;
        first = false;
    }
    return wrapped << ']';
}
}

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
    std::map<frameindex_t, vec3f_t> map;
    map[0] = {0,0,0};
    map[3] = {0,3,0};
    BOOST_TEST(smoothed(map, 2,-1,-1).sum() == 0,   tt::tolerance(0.000001));
    BOOST_TEST(smoothed(map, 2,-2,-2).sum() == 0,   tt::tolerance(0.000001));
    BOOST_TEST(smoothed(map, 1, 0, 0).sum() == 0,   tt::tolerance(0.000001));
    BOOST_TEST(smoothed(map, 2, 1, 1).sum() == 0.5, tt::tolerance(0.000001));
    BOOST_TEST(smoothed(map, 1, 1, 1).sum() == 1,   tt::tolerance(0.000001));
    BOOST_TEST(smoothed(map, 2, 3, 3).sum() == 1.5, tt::tolerance(0.000001));
    BOOST_TEST(smoothed(map, 1, 3, 3).sum() == 3,   tt::tolerance(0.000001));
    BOOST_TEST(smoothed(map, 1, 0, 3).sum() == 1.5, tt::tolerance(0.000001));
    BOOST_TEST(smoothed(map, 1, 3, 4).sum() == 3,   tt::tolerance(0.000001));
    map[10] = {0,0,10};
    BOOST_TEST(smoothed(map, 10, 0, 60).sum() == 3,     tt::tolerance(0.000001));
    BOOST_TEST(smoothed(map, 10, 10,40).sum() == 2.5,   tt::tolerance(0.000001));
    
    for (size_t i = 1; i < (1 << 7); ++i)
    {
        std::map<frameindex_t, float> m2;
        size_t min = 7, max = 0;
        for (size_t j = 0; j < 7; ++j)
        {
            if ((i >> j) & 1)
            {
                m2[j] = j * 2;
                min = std::min(min, j);
                max = std::max(max, j);
            }
        }
        size_t min_bound = std::max(min,size_t(2));
        size_t max_bound = std::min(max,size_t(4));
        size_t min_bbound = std::min(min_bound, size_t(4));
        size_t max_bbound = std::max(max_bound, size_t(2));
        BOOST_TEST(smoothed(m2, 1, 2, 4) ==  (float) ((min_bbound - 2)          * min_bound * 2 
                                                    + (max_bbound - min_bbound) * (max_bound + min_bound)
                                                    + (4          - max_bbound) * max_bound * 2) / 2,   tt::tolerance(0.000001));
    }
}

BOOST_AUTO_TEST_CASE( interpolate_quaternion_test )
{
    rotation_t r0(1,0,0,0);
    rotation_t r1(0,1,0,0);
    BOOST_TEST(distQ(lerp(r0,r1,0),  rotation_t(1, 0, 0, 0)) == 0, tt::tolerance(0.000001));
    BOOST_TEST(distQ(lerp(r0,r1,0.5),rotation_t(sqrt(0.5), sqrt(0.5), 0, 0)) == 0, tt::tolerance(0.000001));
    BOOST_TEST(distQ(lerp(r0,r1,1),  rotation_t(0, 1, 0, 0)) == 0, tt::tolerance(0.000001));
    BOOST_TEST(distQ(lerp(r0,r0,0.5),rotation_t(1, 0, 0, 0)) == 0, tt::tolerance(0.000001));
}


BOOST_AUTO_TEST_SUITE_END()
