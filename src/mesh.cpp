#include "mesh.h"
#include <cstring>

namespace objl
{
VertexArrayCommon::VertexArrayCommon(
    size_t offsetp,
    size_t sizeofp,
    PRIMITIVE_TYPE typeofp,
    size_t offsetn,
    size_t sizeofn,
    PRIMITIVE_TYPE typeofn,
    size_t offsett,
    size_t sizeoft,
    PRIMITIVE_TYPE typeoft,
    size_t sizeofa) :
        _offsetp(offsetp),
        _sizeofp(sizeofp),
        _typeofp(typeofp),
        _offsetn(offsetn),
        _sizeofn(sizeofn),
        _typeofn(typeofn),
        _offsett(offsett),
        _sizeoft(sizeoft),
        _typeoft(typeoft),
        _sizeofa(sizeofa){}

Material::Material() : Ns(0.0f), Ni(0.0f), d(1.0f), illum(0){}

Mesh::Mesh(
    std::vector<VertexHighres> const & vertices_,
    std::vector<triangle_t> const & _Indices) :
    _vertices(std::make_unique<VertexArrayHighres>(vertices_)),
    Indices(_Indices),
    _scale(1,1,1),
    _offset(0,0,0)
{
    octree._begin = octree._cut_begin = 0;
    octree._cut_end = octree._end = Indices.size();
}

Mesh::Mesh(
    std::string && name_,
    std::vector<VertexHighres> && vertices_,
    std::vector<triangle_t> && indices_) :
    MeshName(name_),
    _vertices(std::make_unique<VertexArrayHighres>(vertices_)),
    Indices(indices_),
    _material(nullptr),
    _scale(1,1,1),
    _offset(0,0,0)
{
    octree._begin = octree._cut_begin = 0;
    octree._cut_end = octree._end = Indices.size();
}

Mesh::Mesh(Mesh const & other) :
    MeshName(other.MeshName),
    _vertices(other._vertices->copy()),
    Indices(other.Indices),
    _material(other._material),
    octree(other.octree),
    _scale(other._scale),
    _offset(other._offset)
    {}

void Mesh::swap(Mesh & m)
{
    MeshName.swap(m.MeshName);
    _vertices.swap(_vertices);
    Indices.swap(m.Indices);
    std::swap(octree, m.octree);
    std::swap(_material, m._material);
}

octree_t create_naive_octree(Mesh & m)
{
    octree_t result;
    result._begin = result._cut_begin = 0;
    result._end = result._cut_end = m.Indices.size();
    std::fill(result._min.begin(), result._min.end(), -10e20);
    std::fill(result._max.begin(), result._max.end(), 10e20);
    return result;
}

void print(__m128i var)
{
    uint32_t val[4];
    memcpy(val, &var, sizeof(val));
    printf("%i %i %i %i \n", val[0], val[1], val[2], val[3]);
}

template <typename IndexIter, typename VertexIter>
void count_cuts_sse(IndexIter index_begin, IndexIter index_end, VertexIter vertices, matharray<float, 3>& mid, matharray<uint32_t,3>& triangle_lhs_count, matharray<uint32_t,3> & triangle_rhs_count)
{
    __m128i triangle_lhs_count_sse = _mm_set1_epi32(0);
    __m128i triangle_rhs_count_sse = _mm_set1_epi32(0);
    __m128 mid_sse = _mm_setr_ps(mid[0],mid[1],mid[2],0);
    __m128i mask = _mm_set1_epi32(-1);
    __m128i onemask = _mm_set1_epi32(1);
    for (auto t = index_begin; t != index_end; ++t)
    {
        __m128i vertex_lhs = _mm_set1_epi32(0);
        __m128i vertex_rhs = _mm_set1_epi32(0);
        for (uint32_t vindex : *t)
        {
            auto & v = vertices[vindex].Position;
            __m128 vsse = _mm_setr_ps (v[0], v[1], v[2], 0);
            vertex_lhs = _mm_or_si128(vertex_lhs, _mm_castps_si128(_mm_cmp_ps(vsse, mid_sse,_CMP_LT_OQ)));
            vertex_rhs = _mm_or_si128(vertex_rhs, _mm_castps_si128(_mm_cmp_ps(vsse, mid_sse,_CMP_GT_OQ)));
        }
        triangle_lhs_count_sse += _mm_and_si128(onemask,_mm_and_si128(vertex_lhs, _mm_xor_si128(mask, vertex_rhs)));
        triangle_rhs_count_sse += _mm_and_si128(onemask,_mm_and_si128(vertex_rhs, _mm_xor_si128(mask, vertex_lhs)));
    }
    triangle_lhs_count = sse2matharray<uint32_t,3>(triangle_lhs_count_sse);
    triangle_rhs_count = sse2matharray<uint32_t,3>(triangle_rhs_count_sse);
}

template <typename IndexIter, typename VertexIter>
void count_cuts(IndexIter index_begin, IndexIter index_end, VertexIter vertices, matharray<float, 3> &mid, matharray<uint32_t,3>& triangle_lhs_count, matharray<uint32_t,3> & triangle_rhs_count)
{
    for (auto t = index_begin; t != index_end; ++t)
    {
        matharray<bool, 3> vertex_lhs({false, false, false});
        matharray<bool, 3> vertex_rhs({false, false, false});
        for (auto vindex : *t)
        {
            auto & v = vertices[vindex].Position;
            vertex_lhs[0] |= v[0] < mid[0];
            vertex_lhs[1] |= v[1] < mid[1];
            vertex_lhs[2] |= v[2] < mid[2];
            vertex_rhs[0] |= v[0] > mid[0];
            vertex_rhs[1] |= v[1] > mid[1];
            vertex_rhs[2] |= v[2] > mid[2];
        }
        triangle_lhs_count[0] += vertex_lhs[0] & !vertex_rhs[0];
        triangle_lhs_count[1] += vertex_lhs[1] & !vertex_rhs[1];
        triangle_lhs_count[2] += vertex_lhs[2] & !vertex_rhs[2];
        triangle_rhs_count[0] += vertex_rhs[0] & !vertex_lhs[0];
        triangle_rhs_count[1] += vertex_rhs[1] & !vertex_lhs[1];
        triangle_rhs_count[2] += vertex_rhs[2] & !vertex_lhs[2];
    }
}

template <typename IndexIter, typename VertexIter>
void minmax_sse(IndexIter index_begin, IndexIter index_end, VertexIter vertices, matharray<float,3> & min, matharray<float,3> & max)
{
    __m128 min_sse = _mm_set1_ps(std::numeric_limits<float>::infinity());
    __m128 max_sse = _mm_set1_ps(-std::numeric_limits<float>::infinity());
    for (auto t = index_begin; t != index_end; ++t)
    {
        auto & v = vertices[*t].Position;
        __m128 vsse = _mm_setr_ps (v[0], v[1], v[2], 0);
        min_sse = _mm_min_ps(min_sse,vsse);
        max_sse = _mm_max_ps(max_sse,vsse);
    }
    min = sse2matharray<float,3>(min_sse);
    max = sse2matharray<float,3>(max_sse);
}

template <typename IndexIter, typename VertexIter>
void minmax(IndexIter index_begin, IndexIter index_end, VertexIter vertices, matharray<float,3> & min, matharray<float,3> & max)
{
    std::fill(min.begin(), min.end(), std::numeric_limits<float>::infinity());
    std::fill(max.begin(), max.end(), -std::numeric_limits<float>::infinity());
    for (uint32_t *t = index_begin; t != index_end; ++t)
    {
        auto & v = vertices[*t].Position;
        min[0] = std::min(min[0], v[0]);
        max[0] = std::max(max[0], v[0]);
        min[1] = std::min(min[1], v[1]);
        max[1] = std::max(max[1], v[1]);
        min[2] = std::min(min[2], v[2]);
        max[2] = std::max(max[2], v[2]);
    }
}

void compress(Mesh & m)
{
    VertexArrayHighres* vah = dynamic_cast<VertexArrayHighres* >(m._vertices.get());
    if (!vah){return;}
    matharray<float, 3> min;
    matharray<float, 3> max;
    std::vector<VertexHighres> & vertices = vah->_data;
    auto index_iter_begin = m.Indices.begin();
    auto index_iter_end = m.Indices.end();
    minmax_sse(&**index_iter_begin, &**index_iter_end, vertices.cbegin(), min,max);
    std::vector<VertexLowres> vertices_result;
    vertices_result.reserve(vertices.size());
    m._scale = scale_t(max - min);
    matharray<float, 3> mult = static_cast<float>(std::numeric_limits<VertexLowres::pos_t>::max()) / (max - min);
    matharray<float, 3> offset = mult * (min + max) * (-0.5);
    m._offset = vec3f_t((min + max) * 0.5);
    for (VertexHighres const & cur : vertices)
    {
        vec3_t<VertexLowres::pos_t> pos(cur.Position[0] * mult[0] + offset[0], cur.Position[1] * mult[1] + offset[1], cur.Position[2] * mult[2] + offset[2]);
        vertices_result.emplace_back(pos, cur.Normal, cur.TextureCoordinate);
    }
    m._vertices = std::make_unique<VertexArrayLowres>(std::move(vertices_result));
}

octree_t::octree_t(const objl::octree_t& other) :
    _lhs(other._lhs ? new octree_t(*other._lhs) : nullptr),
    _rhs(other._rhs ? new octree_t(*other._rhs) : nullptr),
    _begin(other._begin),
    _end(other._end),
    _cut_begin(other._cut_begin),
    _cut_end(other._cut_end),
    _min(other._min),
    _max(other._max)
{}

octree_t create_octree(Mesh & m, size_t index_begin, size_t index_end, size_t max_triangles)
{
    std::vector<VertexHighres> & vertices = dynamic_cast<VertexArrayHighres* >(m._vertices.get())->_data;
    matharray<float,3> min, max;
    auto index_iter_begin = m.Indices.begin() + index_begin;
    auto index_iter_end = m.Indices.begin() + index_end;
    minmax_sse(&**index_iter_begin, &**index_iter_end, vertices.cbegin(), min,max);
    octree_t result;
    {
        std::copy(min.begin(), min.end(), result._min.begin());
        std::copy(max.begin(), max.end(), result._max.begin());
        result._begin = index_begin;
        result._end = index_end;
        size_t count = index_end - index_begin;
        if (count < max_triangles)
        {
            result._lhs = nullptr;
            result._rhs = nullptr;
            result._cut_begin = index_begin;
            result._cut_end = index_end;
            return result;
        }
        matharray<float,3> mid = (min + max) * (float)0.5;
        matharray<uint32_t,3> triangle_lhs_count({0,0,0}), triangle_rhs_count({0,0,0});
        count_cuts_sse(index_iter_begin, index_iter_end, vertices.cbegin(), mid, triangle_lhs_count, triangle_rhs_count);
        //count_cuts(index_iter_begin, index_iter_end, vertices.cbegin(), mid, triangle_lhs_count, triangle_rhs_count);
        matharray<uint32_t,3> triangle_both_count = matharray<uint32_t,3>({count, count, count}) - (triangle_lhs_count + triangle_rhs_count);
        matharray<float,3> score(max - min);
        score *= triangle_lhs_count;
        score *= triangle_rhs_count;
        score /= (triangle_both_count + matharray<uint32_t,3>({100,100,100}));
        size_t cut_dim = std::distance(score.begin(), std::max_element(score.begin(), score.end()));
        matharray<uint32_t, 3> sizes({triangle_lhs_count[cut_dim], triangle_both_count[cut_dim], triangle_rhs_count[cut_dim]});
        std::unique_ptr<triangle_t[]> tmp(new triangle_t[count]);

        auto lhs_iter = tmp.get();
        auto cut_iter = lhs_iter + sizes[0];
        auto rhs_iter = cut_iter + sizes[1];

        auto cut_plane = mid[cut_dim];
        for (auto t = index_iter_begin; t != index_iter_end; ++t)
        {
            bool vertex_lhs = false, vertex_rhs = false;
            for (uint32_t vindex : *t)
            {
                auto coord = vertices[vindex].Position[cut_dim];
                vertex_lhs |= coord < cut_plane;
                vertex_rhs |= coord > cut_plane;
            }
            if      (vertex_rhs && !vertex_lhs){assert(rhs_iter < tmp.get() + count);              *rhs_iter++ = *t;}
            else if (vertex_lhs && !vertex_rhs){assert(lhs_iter < tmp.get() + sizes[0]);           *lhs_iter++ = *t;}
            else                               {assert(cut_iter < tmp.get() + sizes[0] + sizes[1]);*cut_iter++ = *t;}
        }
        result._cut_begin=      index_begin + sizes[0];
        result._cut_end  =result._cut_begin + sizes[1];
        std::copy(tmp.get(), rhs_iter, index_iter_begin);
    }
    result._lhs = std::make_unique<octree_t>(create_octree(m, index_begin, result._cut_begin, max_triangles));
    result._rhs = std::make_unique<octree_t>(create_octree(m, result._cut_end, index_end, max_triangles));
    return result;
}
}
