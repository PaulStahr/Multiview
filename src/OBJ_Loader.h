// OBJ_Loader.h - A Single Header OBJ Model Loader

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

// Namespace: OBJL
//
// Description: The namespace that holds eveyrthing that
//    is needed and used for the OBJ Model Loader
namespace objl
{
    struct Vertex
    {
        vec3f_t Position;
        vec3f_t Normal;
        vec2us_t TextureCoordinate;
        Vertex(){}

        Vertex(vec3f_t const & pos_, vec3f_t const & normal_, vec2us_t const & texture_coord_);
    };

    struct Material
    {
        Material();
        // Material Name
        std::string name;
        // Ambient Color
        vec3f_t Ka;
        // Diffuse Color
        vec3f_t Kd;
        // Specular Color
        vec3f_t Ks;
        // Specular Exponent
        float Ns;
        // Optical Density
        float Ni;
        // Dissolve
        float d;
        // Illumination
        int illum;
        // Ambient Texture Map
        std::string map_Ka;
        // Diffuse Texture Map
        std::string map_Kd;
        // Specular Texture Map
        std::string map_Ks;
        // Specular Hightlight Map
        std::string map_Ns;
        // Alpha Texture Map
        std::string map_d;
        // Bump Map
        std::string map_bump;
    };

    struct Mesh
    {
        Mesh(){}
        Mesh(std::vector<Vertex> const & _Vertices, std::vector<uint32_t> const & _Indices);
        std::string MeshName;
        std::vector<Vertex> Vertices;
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
        bool inTriangle(vec3f_t point, vec3f_t tri1, vec3f_t tri2, vec3f_t tri3);

        // Split a String into a string array at a given token
        inline void split(const std::string &in,
            std::vector<std::string> &out,
            std::string const & token)
        {
            out.clear();
            std::string::const_iterator beg = in.cbegin();
            for (std::string::const_iterator i = in.cbegin(); i < in.cend(); i++)
            {
                if (std::equal(token.begin(), token.end(), i))
                {
                    if (beg != i)
                    {
                        out.emplace_back(beg, i);
                        i += token.size() - 1;
                        beg = i + 1;
                    }
                    else
                    {
                        out.emplace_back("");
                    }
                }
                else if (i + token.size() >= in.end())
                {
                    out.emplace_back(beg, in.end());
                    break;
                }
            }
        }

        
        inline void split(std::string::const_iterator beg,
            std::string::const_iterator end,
            std::vector<std::string> &out,
            char token)
        {
            out.clear();
            if (beg == end){return;}
            std::string::const_iterator i = beg;
            while (true)
            {
                if (*i == token)
                {
                    out.emplace_back(beg, i);
                    ++i;
                    beg = i;
                }
                else if (++i == end)
                {
                    out.emplace_back(beg, end);
                    return;
                }
            }
        }
        
        // Split a String into a string array at a given token
        inline void split(const std::string &in,
            std::vector<std::string> &out,
            char token)
        {
            split(in.begin(), in.end(), out, token);
        }
        
        // Get tail of string after first token and possibly following spaces
        inline std::string & tail(const std::string &in, std::string &out)
        {
            size_t token_start = in.find_first_not_of(" \t");
            size_t space_start = in.find_first_of(" \t", token_start);
            size_t tail_start = in.find_first_not_of(" \t", space_start);
            size_t tail_end = in.find_last_not_of(" \t");
            if (tail_start != std::string::npos)
            {
                return out.assign(tail_start + in.begin(), tail_end == std::string::npos ? in.end() :  tail_end + 1 + in.begin());
            }
            return out = "";
        }

        // Get element at given index position
        template <class T>
        inline const T & getElement(const std::vector<T> &elements, int idx)
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
    // Class: Loader
    //
    // Description: The OBJ Model Loader
    class Loader
    {
    public:
        // Default Constructor
        Loader(){}
        ~Loader()
        {
            LoadedMeshes.clear();
        }

        // Load a file into the loader
        //
        // If file is loaded return true
        //
        // If the file is unable to be found
        // or unable to be loaded return false
        bool LoadFile(std::string const & Path);

        // Loaded Mesh Objects
        std::vector<Mesh> LoadedMeshes;
        // Loaded Vertex Objects
        size_t LoadedVertices;
        // Loaded Index Positions
        size_t LoadedIndices;
        // Loaded Material Objects
        std::vector<Material> LoadedMaterials;

    private:
        // Generate vertices from a list of positions, 
        //    tcoords, normals and a face line
        int GenVerticesFromRawOBJ(std::vector<Vertex>& oVerts,
            const std::vector<vec3f_t>& iPositions,
            const std::vector<vec2us_t>& iTCoords,
            const std::vector<vec3f_t>& iNormals,
            std::vector<std::array<int64_t, 3> > const & indices);

        // Triangulate a list of vertices into a face by printing
        //    inducies corresponding with triangles within it
        size_t VertexTriangluation(std::vector<uint32_t>& oIndices,
            std::vector<Vertex>::const_iterator iVerts_begin,
            std::vector<Vertex>::const_iterator iVerts_end,
            std::vector<Vertex> & tVerts);

        // Load Materials from .mtl file
        bool LoadMaterials(std::string path);
    };
}
