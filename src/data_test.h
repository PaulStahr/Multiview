#include "data.h"
BOOST_AUTO_TEST_SUITE(data)
namespace utf = boost::unit_test;
namespace tt = boost::test_tools;

BOOST_AUTO_TEST_CASE( pointer_cleanup )
{
    size_t removed = 0;
    {
        rendered_framebuffer_t framebuffer;
        for (size_t i = 0; i < 5; ++i)
        {
            framebuffer.begin()[i] = std::make_shared<gl_texture_id>(i + 1, [&removed](GLuint ){++removed;});
        }
    }
    BOOST_TEST(removed == 5);
    removed = 0;
    {
        std::vector<std::shared_ptr<gl_texture_id> > tmp(5);
        {
            rendered_framebuffer_t framebuffer;
            for (size_t i = 0; i < 5; ++i)
            {
                framebuffer.begin()[i] = std::make_shared<gl_texture_id>(i + 1, [&removed](GLuint ){++removed;});
            }
            for (size_t i = 0; i < 5; ++i)
            {
                tmp[i] = reinterpret_cast<std::shared_ptr<gl_texture_id> * >(&framebuffer)[i];
            }
        }
        BOOST_TEST(removed == 0);            
        tmp.clear();
        BOOST_TEST(removed == 5);
    }
}

BOOST_AUTO_TEST_SUITE_END()
