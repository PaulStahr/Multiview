#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <math.h>
#include <string_view>
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

    struct Mesh
    {
        Mesh(){}
        Mesh(std::vector<VertexCommon> const & _Vertices, std::vector<uint32_t> const & _Indices);
        std::string MeshName;
        std::vector<VertexCommon> Vertices;
        std::vector<uint32_t> Indices;
        Material MeshMaterial;
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

    namespace algorithm
    {
        // A test to see if P1 is on the same side as P2 of a line segment ab
        bool SameSide(vec3f_t const & p1, vec3f_t const & p2, vec3f_t const & a, vec3f_t const & b);

        // Generate a cross produect normal for a triangle
        vec3f_t GenTriNormal(vec3f_t const & t1, vec3f_t const & t2, vec3f_t const & t3);

        // Check to see if a vec3f_t Point is within a 3 vec3f_t Triangle
        bool inTriangle(vec3f_t const & point, vec3f_t const & tri1, vec3f_t const & tri2, vec3f_t const & tri3);

        // Get element at given index position
        template <class T>
        inline const T & getElement(const std::vector<T> &elements, int64_t idx)
        {
            return elements[idx + (idx < 0 ? elements.size(): -1)];
        }

        template <class T>
        inline T & getElement(std::vector<T> &elements, int64_t idx)
        {
            return elements[idx + (idx < 0 ? elements.size(): -1)];
        }

        // Get element at given index position
        template <class T>
        inline const T & getElement(const std::vector<T> &elements, std::string const &index)
        {
            return getElement(elements, std::stoi(index));
        }
    }

    class Loader
    {
    public:
        // Default Constructor
        Loader(){}
        ~Loader()
        {
            LoadedMeshes.clear();
        }

        bool LoadFile(std::string const & Path);

        std::vector<Mesh> LoadedMeshes;
        size_t LoadedVertices;
        size_t LoadedIndices;
        std::vector<Material> LoadedMaterials;

    private:
        size_t VertexTriangluation(std::vector<uint32_t>& oIndices,
            std::vector<VertexCommon>::const_iterator iVerts_begin,
            std::vector<VertexCommon>::const_iterator iVerts_end,
            std::vector<uint64_t> & tVertInd,
            size_t offset);

        bool LoadMaterials(std::string path);
    };
}
