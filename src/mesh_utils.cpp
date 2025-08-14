#include "mesh_utils.h"
#include "data.h"

namespace objl
{
void compress(mesh_object_t & mo, bool uniform)
{
    if (uniform)
    {
        matharray<float, 3> min_vec(std::numeric_limits<float>::infinity());
        matharray<float, 3> max_vec(-std::numeric_limits<float>::infinity());
        for (objl::Mesh & me : mo._meshes)
        {
            VertexArrayHighres* vah = dynamic_cast<VertexArrayHighres* >(me._vertices.get());
            if (!vah){continue;}
            std::vector<VertexHighres> & vertices = vah->_data;
            auto index_iter_begin = me.Indices.begin();
            auto index_iter_end = me.Indices.end();
            minmax_sse(&**index_iter_begin, &**index_iter_end, vertices.cbegin(), min_vec, max_vec);
        }
        for (objl::Mesh & me : mo._meshes)
        {
            compress_impl(me, min_vec, max_vec);
        }   
    }
    else
    {
        for (objl::Mesh & me : mo._meshes)
        {
            objl::compress(me);
        }
    }
}

void compress_impl(Mesh & me, matharray<float, 3> const & min_vec, matharray<float, 3> const & max_vec)
{
    VertexArrayHighres* vah = dynamic_cast<VertexArrayHighres* >(me._vertices.get());
    std::vector<VertexHighres> & vertices = vah->_data;
    std::vector<VertexLowres> vertices_result;
    vertices_result.reserve(vertices.size());
    me._scale = scale_t(max_vec - min_vec);
    matharray<float, 3> mult = static_cast<float>(std::numeric_limits<VertexLowres::pos_t>::max()) / (max_vec - min_vec);
    matharray<float, 3> offset = mult * (min_vec + max_vec) * (-0.5);
    me._offset = vec3f_t((min_vec + max_vec) * 0.5);
    for (VertexHighres const & cur : vertices)
    {
        vec3_t<VertexLowres::pos_t> pos(cur.Position[0] * mult[0] + offset[0], cur.Position[1] * mult[1] + offset[1], cur.Position[2] * mult[2] + offset[2]);
        vertices_result.emplace_back(pos, cur.Normal, cur.TextureCoordinate);
    }
    me._vertices = std::make_unique<VertexArrayLowres>(std::move(vertices_result));
}

void compress(Mesh & m)
{
    VertexArrayHighres* vah = dynamic_cast<VertexArrayHighres* >(m._vertices.get());
    if (!vah){return;}
    matharray<float, 3> min_vec(std::numeric_limits<float>::infinity());
    matharray<float, 3> max_vec(-std::numeric_limits<float>::infinity());
    std::vector<VertexHighres> & vertices = vah->_data;
    auto index_iter_begin = m.Indices.begin();
    auto index_iter_end = m.Indices.end();
    minmax_sse(&**index_iter_begin, &**index_iter_end, vertices.cbegin(), min_vec,max_vec);
    compress_impl(m, min_vec, max_vec);
}
}
