#include "OBJ_Loader.h"
#include "geometry.h"
#include "qt_util.h"
BOOST_AUTO_TEST_SUITE(OBJ_Loader)

BOOST_AUTO_TEST_CASE( constructors_test )
{
    
}

BOOST_AUTO_TEST_CASE( data_access_test, * boost::unit_test::timeout(2))
{
    objl::Loader loader;
    loader.LoadFile("meshes/cube.obj");
    BOOST_CHECK_EQUAL( loader.LoadedMeshes.size(), 1 );
    objl::Mesh & m = loader.LoadedMeshes[0];
    BOOST_CHECK_EQUAL( m._vertices->size(), 14 );
    BOOST_CHECK_EQUAL( m.Indices.size(), 12 );
    uint8_t permutation[] = {5,1,0,4,2,6,3,7};
    {
        std::vector<objl::VertexHighres> const & vertices = dynamic_cast<objl::VertexArrayHighres*>(m._vertices.get())->_data;
        BOOST_CHECK_EQUAL(m._vertices->_sizeofa, sizeof(objl::VertexHighres));
        for (uint8_t i= 0; i < 8; ++i)
        {
            uint8_t idx = permutation[i];
            vec3_t<objl::VertexHighres::pos_t> expected(idx & 1 ? -1 : 1, idx & 2 ? -1 : 1, idx & 4 ? -1 : 1);
            auto & pos = vertices[i].Position;
            BOOST_REQUIRE_MESSAGE(distQ(pos,expected) < 1e-10, size_t(i) << "->" << size_t(idx) << " expected " << expected.x() << ',' << expected.y() << ',' << expected.z() << " got " << pos.x() << ',' << pos.y() << ',' << pos.z());
        }
    }
    objl::compress(m);
    QMatrix4x4 mesh_transform;
    mesh_transform.setToIdentity();
    QT_UTIL::translate(mesh_transform, m._offset);
    QT_UTIL::scale(mesh_transform, m._scale);
    float mult = 1/static_cast<float>(std::numeric_limits<int16_t>::max());
    std::vector<objl::VertexLowres> const & vertices = dynamic_cast<objl::VertexArrayLowres*>(m._vertices.get())->_data;
    BOOST_CHECK_EQUAL(m._vertices->_sizeofa, sizeof(objl::VertexLowres));
    for (uint8_t i= 0; i < 8; ++i)
    {
        uint8_t idx = permutation[i];
        vec3_t<objl::VertexHighres::pos_t> expected(idx & 1 ? -1 : 1, idx & 2 ? -1 : 1, idx & 4 ? -1 : 1);
        auto & pl = vertices[i].Position;
        QVector4D tmp = mesh_transform * QVector4D(pl[0] * mult,pl[1] * mult,pl[2] * mult,1);
        vec3f_t pos(tmp[0],tmp[1],tmp[2]);
        BOOST_REQUIRE_MESSAGE(distQ(pos,expected) < 1e-5, size_t(i) << "->" << size_t(idx) << " expected " << expected.x() << ',' << expected.y() << ',' << expected.z() << " got " << pos.x() << ',' << pos.y() << ',' << pos.z());
    }

    /*for (uint8_t i = 0; i<m.Indices.size(), ++i)
    {
    }*/
}

BOOST_AUTO_TEST_SUITE_END()
