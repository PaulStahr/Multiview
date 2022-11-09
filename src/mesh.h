#ifndef MESH_H
#define MESH_H

#include <memory>

#include "geometry.h"

namespace objl
{
template <typename P, typename N, typename T>
struct Vertex
{
    typedef P pos_t;
    typedef N normal_t;
    typedef T texture_t;
    vec3_t<P> Position;
    vec3_t<N> Normal;
    vec2_t<T> TextureCoordinate;
    Vertex(){}

    inline Vertex(vec3_t<P> const & pos_, vec3_t<N> const & normal_,vec2_t<T> const & texture_coord_) : Position(pos_), Normal(normal_), TextureCoordinate(texture_coord_){}
};

typedef Vertex<float, int16_t, uint16_t> VertexHighres;
typedef Vertex<int16_t, int16_t, uint16_t> VertexLowres;

struct VertexArrayCommon{
    const size_t _offsetp;
    const size_t _sizeofp;
    const PRIMITIVE_TYPE _typeofp;
    const size_t _offsetn;
    const size_t _sizeofn;
    const PRIMITIVE_TYPE _typeofn;
    const size_t _offsett;
    const size_t _sizeoft;
    const PRIMITIVE_TYPE _typeoft;
    const size_t _sizeofa;

    VertexArrayCommon(
        size_t offsetp,
        size_t sizeofp,
        PRIMITIVE_TYPE typeofp,
        size_t offsetn,
        size_t sizeofn,
        PRIMITIVE_TYPE typeofn,
        size_t offsett,
        size_t sizeoft,
        PRIMITIVE_TYPE typeoft,
        size_t sizeofa);

    virtual bool empty() const = 0;

    virtual void* data() = 0;

    virtual size_t size() const = 0;
    
    virtual VertexArrayCommon* copy() const = 0;

    virtual ~VertexArrayCommon() = default;
};

template <typename P, typename N, typename T>
struct VertexArray : VertexArrayCommon
{
    std::vector<Vertex<P,N,T> > _data;
    typedef Vertex<P,N,T> Vert;

    VertexArray(std::vector<Vertex<P,N,T> > && data_) : VertexArrayCommon(
        offsetof(Vert,Position),
        sizeof(vec3_t<P>),
        primitive_type_enum<P>,
        offsetof(Vert, Normal),
        sizeof(vec3_t<N>),
        primitive_type_enum<N>,
        offsetof(Vert, TextureCoordinate),
        sizeof(vec2_t<T>),
        primitive_type_enum<T>,
        sizeof(Vert)),
        _data(data_){}

    VertexArray(std::vector<Vertex<P,N,T> > const & data_) : VertexArrayCommon(
        offsetof(Vert,Position),
        sizeof(vec3_t<P>),
        primitive_type_enum<P>,
        offsetof(Vert, Normal),
        sizeof(vec3_t<N>),
        primitive_type_enum<N>,
        offsetof(Vert, TextureCoordinate),
        sizeof(vec2_t<T>),
        primitive_type_enum<T>,
        sizeof(Vert)),
        _data(data_){}

    VertexArray(VertexArray<P,N,T> const & other) = default;

    Vertex<P,N,T> & operator[](size_t idx){return _data[idx];}
    
    Vertex<P,N,T> const & operator[] (size_t idx) const {return _data[idx];}

    bool empty() const {return _data.empty();}

    size_t size() const {return _data.size();}

    void* data(){return _data.data();}

    VertexArrayCommon* copy() const{return new VertexArray<P,N,T>(*this);}

    ~VertexArray() = default;
};

typedef VertexArray<float, int16_t, uint16_t> VertexArrayHighres;
typedef VertexArray<int16_t, int16_t, uint16_t> VertexArrayLowres;

struct Material
{
    Material();         // Material Name
    std::string name;   // Ambient Color
    vec3f_t Ka;         // Diffuse Color
    vec3f_t Kd;         // Specular Color
    vec3f_t Ks;         // Specular Exponent
    float Ns;           // Optical Density
    float Ni;           // Dissolve
    float d;            // Illumination
    int illum;          // Ambient Texture Map
    std::string map_Ka; // Diffuse Texture Map
    std::string map_Kd; // Specular Texture Map
    std::string map_Ks; // Specular Hightlight Map
    std::string map_Ns; // Alpha Texture Map
    std::string map_d;  // Bump Map
    std::string map_bump;
};

struct octree_t
{
    std::unique_ptr<octree_t> _lhs, _rhs;
    size_t _begin, _end;
    size_t _cut_begin, _cut_end;
    vec3f_t _min, _max;
    
    octree_t() = default;
    octree_t(octree_t && other) = default;
    octree_t(octree_t const & other);

    octree_t & operator=(octree_t && other) = default; 
};

struct Mesh
{
    Mesh(){}
    Mesh(std::vector<VertexHighres> const & _Vertices, std::vector<triangle_t> const & _Indices);
    Mesh(std::string && name, std::vector<VertexHighres> &&, std::vector<triangle_t> && indices);
    Mesh(Mesh const & other);
    std::string MeshName;
    std::unique_ptr<VertexArrayCommon> _vertices;
    std::vector<triangle_t> Indices;
    std::shared_ptr<Material> _material;
    octree_t octree;
    scale_t _scale;
    vec3f_t _offset;
    Mesh(Mesh &&other) = default;
    void swap(Mesh & m);
};

void compress(Mesh & m);

octree_t create_naive_octree(Mesh & m);

octree_t create_octree(Mesh & m, size_t vertex_begin, size_t vertex_end, size_t max_vertices);
}

#endif
