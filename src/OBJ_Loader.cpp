#include "OBJ_Loader.h"
#include "io_util.h"
#include <atomic>
#include <limits>

namespace objl
{
Vertex::Vertex(vec3f_t const & pos_, vec3f_t const & normal_, vec2us_t const & texture_coord_) : Position(pos_), Normal(normal_), TextureCoordinate(texture_coord_){}

Material::Material() : Ns(0.0f), Ni(0.0f), d(0.0f), illum(0){}

Mesh::Mesh(std::vector<Vertex> const & _Vertices, std::vector<uint32_t> const & _Indices) : Vertices(_Vertices), Indices(_Indices){}

void Mesh::swap(Mesh & m)
{
    MeshName.swap(m.MeshName);
    Vertices.swap(m.Vertices);
    Indices.swap(m.Indices);
    std::swap(MeshMaterial, m.MeshMaterial);
}

namespace math
{
    vec3f_t CrossV3(const vec3f_t a, const vec3f_t b)
    {
        return vec3f_t(a.y() * b.z() - a.z() * b.y(),
            a.z() * b.x() - a.x() * b.z(),
            a.x() * b.y() - a.y() * b.x());
    }
    
    float normdot (const vec3f_t & a, const vec3f_t & b)
    {
        return dot(a, b) / sqrtf(a.dot() * b.dot());
    }

    float AngleBetweenV3(const vec3f_t a, const vec3f_t b)
    {
        return acosf(normdot(a,b));
    }

    vec3f_t ProjV3(const vec3f_t a, const vec3f_t b)
    {
        return b * (dot(a, b) / b.dot());
    }
}

namespace algorithm
{
    template<class Iterator1, class Iterator2> Iterator1 find_first_not_of(
        Iterator1 const& begin1,
        Iterator1 const& end1,
        Iterator2 const& begin2,
        Iterator2 const& end2)
    {
        for(Iterator1 mid1 = begin1; mid1 != end1; ++mid1)
        {
            for(Iterator2 mid2 = begin2; mid2 != end2; ++mid2)
                if(*mid1 == *mid2)
                    goto FOUND;
            return mid1;
            FOUND:;
        }
        return end1;
    }

    bool SameSide(vec3f_t const & p1, vec3f_t const & p2, vec3f_t const & a, vec3f_t const & b)
    {
        vec3f_t cp1 = math::CrossV3(b - a, p1 - a);
        vec3f_t cp2 = math::CrossV3(b - a, p2 - a);
        return dot(cp1, cp2) >= 0;
    }

    // Generate a cross produect normal for a triangle
    vec3f_t GenTriNormal(vec3f_t const & t1, vec3f_t const & t2, vec3f_t const & t3)
    {
        return math::CrossV3(t2 - t1,t3 - t1);
    }

    // Check to see if a vec3f_t Point is within a 3 vec3f_t Triangle
    bool inTriangle(vec3f_t const & point, vec3f_t const & tri1, vec3f_t const & tri2, vec3f_t const & tri3)
    {
        // Test to see if it is within an infinite prism that the triangle outlines.
        bool within_tri_prisim = SameSide(point, tri1, tri2, tri3) && SameSide(point, tri2, tri1, tri3)
            && SameSide(point, tri3, tri1, tri2);

        if (!within_tri_prisim)
            return false;

        vec3f_t n = GenTriNormal(tri1, tri2, tri3);
        return math::ProjV3(point, n) == vec3f_t(0);
    }
}

static int64_t undef_index = std::numeric_limits<int64_t>::max();

template<typename SplitIter, size_t len>
bool read_vec(SplitIter & split_iter, matharray<float, len> & vec)
{
    for (size_t i = 0; i < vec.size(); ++i)
    {
        split_iter.parse(vec[i]);
        ++split_iter;
    }
    return true;
}

template <typename SplitIter>
void create_absolute_path(SplitIter & split_iter, std::string const & folder, std::string & result)
{
    if(split_iter[0] != '/'){result = folder;}
    result += (++split_iter).remaining();
    while (result.back() == 13){result.pop_back();}
}

bool Loader::LoadFile(std::string const & Path)
{
    // If the file is not an .obj file return false
    if (Path.substr(Path.size() - 4, 4) != ".obj")
        return false;

    std::ifstream file(Path);

    if (!file.is_open())
        return false;

    LoadedMeshes.clear();
    LoadedVertices = 0;
    LoadedIndices = 0;

    std::vector<vec3f_t> Positions;
    std::vector<vec2us_t> TCoords;
    std::vector<vec3f_t> Normals;

    std::vector<std::string> meshMatNames;
    std::string curMeshMatName;

    bool listening = false;
    Mesh cur_mesh;

    #ifdef OBJL_CONSOLE_OUTPUT
    const uint32_t outputEveryNth = 10000;
    uint32_t outputIndicator = outputEveryNth;
    #endif

    std::vector<uint64_t> tVertInd;
    std::string curline;
    std::vector<std::array<int64_t, 3> > indices;
    auto split_iter = IO_UTIL::make_split_iterator("", [](char c){return c == ' ' || c == '\t';});
    auto split_iter2= IO_UTIL::make_split_iterator("", [](char c){return c == '/';});
    while (std::getline(file, curline))
    {
            #ifdef OBJL_CONSOLE_OUTPUT
        if ((outputIndicator = ((outputIndicator + 1) % outputEveryNth)) == 1)
        {
            if (!cur_mesh.MeshName.empty())
            {
                std::cout
                    << "\r- " << cur_mesh.MeshName
                    << "\t| vertices > " << Positions.size()
                    << "\t| texcoords > " << TCoords.size()
                    << "\t| normals > " << Normals.size()
                    << "\t| triangles > " << (cur_mesh.Vertices.size() / 3)
                    << ( "\t| material: " + curMeshMatName);
            }
        }
        #endif
        split_iter.str(curline);
        if (split_iter.size()== 2 && split_iter[0] == 'v')
        {
            if (split_iter[1] == 't')
            {
                float u, v;
                (++split_iter).parse(u);
                (++split_iter).parse(v);
                TCoords.emplace_back(static_cast<uint16_t>(u * std::numeric_limits<uint16_t>::max()),static_cast<uint16_t>(v * std::numeric_limits<uint16_t>::max()));
            }
            else if (split_iter[1] == 'n')
            {
                float x, y, z;
                (++split_iter).parse(x);
                (++split_iter).parse(y);
                (++split_iter).parse(z);
                Normals.emplace_back(x,y,z);
            }
        }
        else if (split_iter.size() == 1)
        {
            if (split_iter[0] == 'v')
            {
                float x, y, z;
                (++split_iter).parse(x);
                (++split_iter).parse(y);
                (++split_iter).parse(z);
                Positions.emplace_back(x,y,z);
            }
            else if (split_iter[0] == 'f')
            {
                size_t oldVertexSize = cur_mesh.Vertices.size();
                while ((++split_iter).valid())
                {
                    split_iter2.str(split_iter.begin(), split_iter.end());
                    std::array<int64_t, 3> fVertex({undef_index, undef_index, undef_index});
                    split_iter2.parse(fVertex[0]);
                    ++split_iter2;
                    if (split_iter2.valid())
                    {
                        if (split_iter2.begin() != split_iter2.end())
                        {
                            split_iter2.parse(fVertex[2]);
                        }
                        ++split_iter2;
                        if (split_iter2.valid())
                        {
                            split_iter2.parse(fVertex[1]);
                        }
                    }
                    indices.emplace_back(fVertex);
                }
                GenVerticesFromRawOBJ(cur_mesh.Vertices, Positions, TCoords, Normals, indices);
                LoadedVertices += indices.size();
                size_t old_indice_count = cur_mesh.Indices.size();
                size_t addedIndices = VertexTriangluation(cur_mesh.Indices, cur_mesh.Vertices.cend() - indices.size(), cur_mesh.Vertices.cend(), tVertInd);
                indices.clear();
                LoadedIndices += addedIndices;
                std::transform(cur_mesh.Indices.begin() + old_indice_count, cur_mesh.Indices.end(), cur_mesh.Indices.begin() + old_indice_count, UTIL::plus(oldVertexSize));
            }
            else if (split_iter[0] == 'o' || split_iter[0] == 'g')
            {
                if (listening && !cur_mesh.Indices.empty() && !cur_mesh.Vertices.empty())
                {
                    LoadedMeshes.emplace_back();
                    LoadedMeshes.back().swap(cur_mesh);
                    meshMatNames.emplace_back(curMeshMatName);
                }
                listening = true;
                if ((++split_iter).valid())
                {
                    split_iter.get(cur_mesh.MeshName);
                }
                else
                {
                    cur_mesh.MeshName = "unnamed";
                }
                #ifdef OBJL_CONSOLE_OUTPUT
                std::cout << std::endl;
                outputIndicator = 0;
                #endif
            }
        }
        // Get Mesh Material Name
        else if (*split_iter == "usemtl")
        {
            curMeshMatName = (++split_iter).remaining();

            // Create new Mesh, if Material changes within a group
            if (!cur_mesh.Indices.empty() && !cur_mesh.Vertices.empty())
            {
                std::string tmp = cur_mesh.MeshName;
                for (size_t i = 1; std::find_if(LoadedMeshes.begin(), LoadedMeshes.end(), [&tmp](Mesh const &m){return m.MeshName == tmp;}) != LoadedMeshes.end(); ++i)
                {
                    tmp = cur_mesh.MeshName + "_" + std::to_string(i);
                }
                cur_mesh.MeshName = tmp;
                LoadedMeshes.emplace_back();
                LoadedMeshes.back().swap(cur_mesh);
                meshMatNames.emplace_back(curMeshMatName);
            }
            #ifdef OBJL_CONSOLE_OUTPUT
            outputIndicator = 0;
            #endif
        }
        // Load Materials
        else if (*split_iter == "mtllib")
        {
            std::string pathtomat;
            std::string folder = "";
            if (Path.rfind('/') != std::string::npos)
            {
                folder.assign(Path.begin(), Path.begin() + Path.rfind('/') + 1);
            }
            create_absolute_path(split_iter, folder, pathtomat);
            #ifdef OBJL_CONSOLE_OUTPUT
            std::cout << std::endl << "- find materials in: " << pathtomat << std::endl;
            #endif
            LoadMaterials(pathtomat);
        }
    }

    #ifdef OBJL_CONSOLE_OUTPUT
    std::cout << std::endl;
    #endif

    // Deal with last mesh
    if (!cur_mesh.Indices.empty() && !cur_mesh.Vertices.empty())
    {
        LoadedMeshes.emplace_back();
        LoadedMeshes.back().swap(cur_mesh);
        meshMatNames.emplace_back(curMeshMatName);
    }

    file.close();

    // Set Materials for each Mesh
    for (size_t i = 0; i < meshMatNames.size(); ++i)
    {
        std::string const & matname = meshMatNames[i];
        auto iter = std::find_if(LoadedMaterials.begin(), LoadedMaterials.end(), [matname](Material const & m){return m.name == matname;});
        if (iter == LoadedMaterials.end())
        {
            std::cout << "Error, couldn't find Material" << std::endl;
        }
        else
        {
            LoadedMeshes[i].MeshMaterial = *iter;
        }
    }
    return !(LoadedMeshes.empty() && LoadedVertices == 0 && LoadedIndices == 0);
}

void Loader::GenVerticesFromRawOBJ(std::vector<Vertex>& oVerts,
    const std::vector<vec3f_t>& iPositions,
    const std::vector<vec2us_t>& iTCoords,
    const std::vector<vec3f_t>& iNormals,
    std::vector<std::array<int64_t, 3> > const & indices)
{    
    bool noNormal = false;
    size_t oldSize = oVerts.size();

    for (std::array<int64_t, 3> const & idx : indices)
    {
        oVerts.emplace_back(
            algorithm::getElement(iPositions, idx[0]), idx[1] == undef_index ? vec3f_t(0,0,0) : algorithm::getElement(iNormals, idx[1]), idx[2] == undef_index ? vec2us_t(0, 0) : algorithm::getElement(iTCoords, idx[2]));
        noNormal |= idx[1] == undef_index;
    }
    if (noNormal)
    {
        vec3f_t normal = algorithm::GenTriNormal(oVerts[oldSize+1].Position, oVerts[oldSize+2].Position, oVerts[oldSize+0].Position);
        for (auto iter = oVerts.begin() + oldSize; iter != oVerts.end(); ++iter)
        {
            iter->Normal = normal;
        }
    }
}

size_t Loader::VertexTriangluation(std::vector<uint32_t>& oIndices,
    std::vector<Vertex>::const_iterator iVerts_begin,
    std::vector<Vertex>::const_iterator iVerts_end,
    std::vector<uint64_t> & tVertInd)
{
    size_t iSize = std::distance(iVerts_begin, iVerts_end);
    if (iSize < 3)
    {
        return 0;
    }
    if (iSize == 3)
    {
        oIndices.push_back(0);
        oIndices.push_back(1);
        oIndices.push_back(2);
        return 3;
    }

    size_t oldSize = oIndices.size();
    tVertInd.clear();
    for (size_t i = 0; i < iSize; ++i)
    {
        tVertInd.emplace_back(i);
    }
    do
    {
        for (size_t i = 0; i < tVertInd.size(); i++)
        {
            uint64_t iPrev = tVertInd[(i + tVertInd.size() - 1) % tVertInd.size()];
            uint64_t iCur = tVertInd[i];
            uint64_t iNext = tVertInd[(i + 1) % tVertInd.size()]; 
            if (tVertInd.size() == 3)
            {
                oIndices.push_back(iPrev);
                oIndices.push_back(iCur);
                oIndices.push_back(iNext);
                tVertInd.clear();
                break;
            }
            if (tVertInd.size() == 4)
            {
                oIndices.push_back(iPrev);
                oIndices.push_back(iCur);
                oIndices.push_back(iNext);
                oIndices.push_back(iNext);
                oIndices.push_back(tVertInd[(i + 2) % tVertInd.size()]);
                oIndices.push_back(iPrev);
                tVertInd.clear();
                break;
            }
            vec3f_t const & pPrev = iVerts_begin[iPrev].Position;
            vec3f_t const & pCur = iVerts_begin[iCur].Position;
            vec3f_t const & pNext = iVerts_begin[iNext].Position;
            float dot = math::normdot(pPrev - pCur, pNext - pCur);
            if (dot <= 0 || dot >= 1)
                continue;

            bool inTri = false;
            for (size_t j = 0; j < iSize; j++)
            {
                if (j != iPrev && j != iCur && j != iNext && algorithm::inTriangle(iVerts_begin[j].Position, pPrev, pCur, pNext))
                {
                    inTri = true;
                    break;
                }
            }
            if (inTri)
                continue;
            oIndices.push_back(iPrev);
            oIndices.push_back(iCur);
            oIndices.push_back(iNext);
            tVertInd.erase(tVertInd.begin() + i);
            i = std::numeric_limits<size_t>::max();
        }
    }
    while (!tVertInd.empty());
    return oIndices.size() - oldSize;
}

bool Loader::LoadMaterials(std::string path)
{
    std::cout << "load materials "<< path << std::endl;
    while (path.back() == '\13' || path.back() == '\0'){path.pop_back();}
    if (path.substr(path.size() - 4, path.size()) != ".mtl")
    {
        std::cout << "unexpected file ending: " << path.substr(path.size() - 4, path.size()) << std::endl;
        return false;
    }
    std::ifstream file(path);
    std::string folder = "";
    if (path.rfind('/') != std::string::npos)
    {
        folder.assign(path.begin(), path.begin() + path.rfind('/') + 1);
    }

    if (!file.is_open())
    {
        std::cout << "cant't open material file" << std::endl;
        return false;

    }
    Material *material = nullptr;

    std::string curline;
    auto split_iter = IO_UTIL::make_split_iterator("", [](char c){return c == ' ' || c == '\t';});

    while (std::getline(file, curline))
    {
        split_iter.str(curline);
        if (*split_iter == "newmtl")
        {
            LoadedMaterials.emplace_back();
            material = &LoadedMaterials.back();
            if (curline.size() > 7)
            {
                material->name = (++split_iter).remaining();
            }
            else
            {
                material->name = "none";
            }
        }
        else if (*split_iter == "Ka")   {read_vec(++split_iter, material->Ka);}
        else if (*split_iter == "Kd")   {read_vec(++split_iter, material->Kd);}
        else if (*split_iter == "Ks")   {read_vec(++split_iter, material->Ks);}
        else if (*split_iter == "Ns")   {(++split_iter).parse(material->Ns);}// Optical Density
        else if (*split_iter == "Ni")   {(++split_iter).parse(material->Ni);}// Dissolve
        else if (*split_iter == "d")    {(++split_iter).parse(material->d);}// Illumination
        else if (*split_iter == "illum"){(++split_iter).parse(material->illum);}// Ambient Texture Map
        else if (*split_iter == "map_Ka")   {create_absolute_path(split_iter, folder, material->map_Ka);}   // Diffuse Texture Map
        else if (*split_iter == "map_Kd")   {create_absolute_path(split_iter, folder, material->map_Kd);}
        else if (*split_iter == "map_Ks")   {create_absolute_path(split_iter, folder, material->map_Ks);}// Specular Texture Map
        else if (*split_iter == "map_Ns")   {create_absolute_path(split_iter, folder, material->map_Ns);}// Specular Hightlight Map
        else if (*split_iter == "map_d")    {create_absolute_path(split_iter, folder, material->map_d);}// Alpha Texture Map
        else if (*split_iter == "map_Bump" || *split_iter == "map_bump" || *split_iter == "bump"){create_absolute_path(split_iter, folder, material->map_bump);}
    }
    std::cout << (LoadedMaterials.empty() ? "No materials found" :"Sucess") << std::endl;
    return !LoadedMaterials.empty();
}
}
