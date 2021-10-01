#include "OBJ_Loader.h"
BOOST_AUTO_TEST_SUITE(OBJ_Loader)

BOOST_AUTO_TEST_CASE( constructors_test )
{
    
}

BOOST_AUTO_TEST_CASE( data_access_test, * boost::unit_test::timeout(2))
{
    objl::Loader loader;
    loader.LoadFile("meshes/cube.obj");
    BOOST_CHECK_EQUAL( loader.LoadedMeshes.size(), 1 );
}

BOOST_AUTO_TEST_SUITE_END()
