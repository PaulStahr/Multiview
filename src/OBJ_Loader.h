#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <math.h>
#include <string_view>
#include <memory>
#include "geometry.h"

// Print progress to console while loading (large models)
#define OBJL_CONSOLE_OUTPUT

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
        std::string MeshName;
        std::vector<VertexCommon> Vertices;
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
