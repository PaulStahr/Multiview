#pragma once
#include <vector>
#include <string>
#include <iosfwd>
#include <memory>
#include <cstddef>
#include "types.h"
#include "mesh.h"
// Print progress to console while loading (large models)
//#define OBJL_CONSOLE_OUTPUT
#define OFFSET_OF(m) offset_of<decltype(get_class_type(m)), \
                     decltype(get_member_type(m)), m>()

namespace objl
{
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
        std::vector<VertexHighres> const & iVerts,
        std::vector<uint32_t> & tVertInd);

    bool LoadMaterials(std::string path);
};

void print_models(objl::Loader & Loader, std::ostream & file);
}
