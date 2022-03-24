#include "OBJ_Loader.h"
#include "geometry.h"
BOOST_AUTO_TEST_SUITE(OBJ_Loader)

BOOST_AUTO_TEST_CASE( constructors_test )
{
    
}

BOOST_AUTO_TEST_CASE( data_access_test, * boost::unit_test::timeout(2))
{
    objl::Loader loader;
    loader.LoadFile("meshes/cube.obj");
    BOOST_CHECK_EQUAL( loader.LoadedMeshes.size(), 1 );
    objl::Mesh const & m = loader.LoadedMeshes[0];
    BOOST_CHECK_EQUAL( m._vertices->size(), 14 );
    BOOST_CHECK_EQUAL( m.Indices.size(), 12 );
    uint8_t permutation[] = {5,1,0,4,2,6,3,7};
    std::vector<objl::VertexCommon> const & vertices = dynamic_cast<objl::VertexArrayHighres*>(m._vertices.get())->_data;
    BOOST_CHECK_EQUAL(m._vertices->_sizeofa, sizeof(objl::VertexCommon));
    for (uint8_t i= 0; i < 8; ++i)
    {
        uint8_t idx = permutation[i];
        vec3_t<objl::VertexCommon::pos_t> expected(idx & 1 ? -1 : 1, idx & 2 ? -1 : 1, idx & 4 ? -1 : 1);
        BOOST_REQUIRE_MESSAGE(distQ(vertices[i].Position,expected) < 1e-10, size_t(i) << "->" << size_t(idx) << " expected " << expected.x() << ',' << expected.y() << ',' << expected.z() << " got " << vertices[i].Position.x() << ',' << vertices[i].Position.y() << ',' << vertices[i].Position.z());
    }
    /*for (uint8_t i = 0; i<m.Indices.size(), ++i)
    {
    }*/
}

BOOST_AUTO_TEST_SUITE_END()
