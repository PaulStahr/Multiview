#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <math.h>
#include <string_view>
#include <memory>
#include <cstddef>
#include "geometry.h"

// Print progress to console while loading (large models)
#define OBJL_CONSOLE_OUTPUT
#define OFFSET_OF(m) offset_of<decltype(get_class_type(m)), \
                     decltype(get_member_type(m)), m>()

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

    typedef Vertex<float, int16_t, uint16_t> VertexCommon;

    struct VertexArrayCommon{
        const size_t _offsetp;
        const size_t _sizeofp;
        const size_t _offsetn;
        const size_t _sizeofn;
        const size_t _offsett;
        const size_t _sizeoft;
        const size_t _sizeofa;

        VertexArrayCommon(
            size_t offsetp,
            size_t sizeofp,
            size_t offsetn,
            size_t sizeofn,
            size_t offsett,
            size_t sizeoft,
            size_t sizeofa) : 
                _offsetp(offsetp),
                _sizeofp(sizeofp),
                _offsetn(offsetn),
                _sizeofn(sizeofn),
                _offsett(offsett),
                _sizeoft(sizeoft),
                _sizeofa(sizeofa){}

        virtual bool empty() const = 0;

        virtual void* data() = 0;

        virtual size_t size() const = 0;

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
            offsetof(Vert, Normal),
            sizeof(vec3_t<N>),
            offsetof(Vert, TextureCoordinate),
            sizeof(vec2_t<T>),
            sizeof(Vert)),
            _data(data_){}

        VertexArray(std::vector<Vertex<P,N,T> > const & data_) : VertexArrayCommon(
            offsetof(Vert,Position),
            sizeof(vec3_t<P>),
            offsetof(Vert, Normal),
            sizeof(vec3_t<N>),
            offsetof(Vert, TextureCoordinate),
            sizeof(vec2_t<T>),
            sizeof(Vert)),
            _data(data_){}

        bool empty() const {return _data.empty();}

        size_t size() const {return _data.size();}

        void* data(){return _data.data();}

        ~VertexArray() = default;
    };

    typedef VertexArray<float, int16_t, uint16_t> VertexArrayHighres;

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
    };

    struct Mesh
    {
        Mesh(){}
        Mesh(std::vector<VertexCommon> const & _Vertices, std::vector<triangle_t> const & _Indices);
        Mesh(std::string && name, std::vector<VertexCommon> &&, std::vector<triangle_t> && indices);
        std::string MeshName;
        std::unique_ptr<VertexArrayCommon> _vertices;
        std::vector<triangle_t> Indices;
        Material MeshMaterial;
        octree_t octree;
        Mesh(Mesh &&other) = default;
        void swap(Mesh & m);
    };

    namespace math
    {
        // vec3f_t Cross Product
        vec3f_t CrossV3(const vec3f_t a, const vec3f_t b);

        // Angle between 2 vec3f_t Objects
        float AngleBetweenV3(const vec3f_t a, const vec3f_t b);

        // Projection Calculation of a onto b
        vec3f_t ProjV3(const vec3f_t a, const vec3f_t b);
    }
    
    octree_t create_naive_octree(Mesh & m);
    
    octree_t create_octree(Mesh & m, size_t vertex_begin, size_t vertex_end, size_t max_vertices);

    class Loader
    {
    public:
        // Default Constructor
        Loader(){}
        bool LoadFile(std::string const & Path);

        std::vector<Mesh> LoadedMeshes;
        size_t LoadedVertices;
        size_t loaded_faces;
        std::vector<Material> LoadedMaterials;
        void swap(Loader & other);
        Loader(Loader&&other)=default;
        Loader & operator=(Loader &&) = default;
    private:
        size_t VertexTriangluation(std::vector<triangle_t>& oIndices,
            std::vector<VertexCommon> const & iVerts,
            std::vector<uint32_t> & tVertInd);

        bool LoadMaterials(std::string path);
    };
}
