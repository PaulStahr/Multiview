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

    // Angle between 2 vec3f_t Objects
    float AngleBetweenV3(const vec3f_t a, const vec3f_t b)
    {
        return acosf(dot(a, b) / sqrtf(a.dot() * b.dot()));
    }
    
    float normdot (const vec3f_t & a, const vec3f_t & b)
    {
        return dot(a, b) / sqrtf(a.dot() * b.dot());
    }

    // Projection Calculation of a onto b
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
    bool inTriangle(vec3f_t point, vec3f_t tri1, vec3f_t tri2, vec3f_t tri3)
    {
        // Test to see if it is within an infinite prism that the triangle outlines.
        bool within_tri_prisim = SameSide(point, tri1, tri2, tri3) && SameSide(point, tri2, tri1, tri3)
            && SameSide(point, tri3, tri1, tri2);

        // If it isn't it will never be on the triangle
        if (!within_tri_prisim)
            return false;

        // Calulate Triangle's Normal
        vec3f_t n = GenTriNormal(tri1, tri2, tri3);

        // Project the point onto this normal
        vec3f_t proj = math::ProjV3(point, n);

        // If the distance from the triangle to the point is 0
        //	it lies on the triangle
        return proj == vec3f_t(0);
    }
}

static int64_t undef_index = std::numeric_limits<int64_t>::max();

template<size_t len>
bool read_vec(std::string const & str, matharray<float, len> & vec, std::vector<std::string> & split)
{
    algorithm::split(str, split, ' ');
    if (split.size() != vec.size()){return false;}
    for (size_t i = 0; i < vec.size(); ++i)
    {
        vec[i] = std::stof(split[i]);
    }
    return true;
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

    std::vector<Vertex> tVerts;
    {
        std::string curline;
        std::string tail;
        std::vector<std::array<int64_t, 3> > indices; 
        std::string word;
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

            // Generate a Mesh Object or Prepare for an object to be created
            split_iter.str(curline);
            // Generate a Vertex Texture Coordinate
            if (split_iter.size()== 2 && split_iter[0] == 'v')
            {
                if (split_iter[1] == 't')
                {
                    float u, v;
                    (++split_iter).parse(u);
                    (++split_iter).parse(v);
                    TCoords.emplace_back(static_cast<uint16_t>(u * std::numeric_limits<uint16_t>::max()),static_cast<uint16_t>(v * std::numeric_limits<uint16_t>::max()));
                }
                // Generate a Vertex Normal;
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
                if (split_iter[0] == 'o' || split_iter[0] == 'g')
                {
                    if (listening && !cur_mesh.Indices.empty() && !cur_mesh.Vertices.empty())
                    {
                        // Create Mesh
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
                // Generate a Vertex Position
                else if (split_iter[0] == 'v')
                {
                    float x, y, z;
                    (++split_iter).parse(x);
                    (++split_iter).parse(y);
                    (++split_iter).parse(z);
                    Positions.emplace_back(x,y,z);
                }
                // Generate a Face (vertices & indices)
                else if (split_iter[0] == 'f')
                {
                    // Generate the vertices
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
                    
                    size_t num_added = GenVerticesFromRawOBJ(cur_mesh.Vertices, Positions, TCoords, Normals, indices);
                    indices.clear();
                    LoadedVertices += num_added;
                    size_t old_indice_count = cur_mesh.Indices.size();
                    size_t addedIndices = VertexTriangluation(cur_mesh.Indices, cur_mesh.Vertices.cend() - num_added, cur_mesh.Vertices.cend(), tVerts);
                    
                    // Add Indices
                    LoadedIndices += addedIndices;
                    std::transform(cur_mesh.Indices.begin() + old_indice_count, cur_mesh.Indices.end(), cur_mesh.Indices.begin() + old_indice_count, UTIL::plus(oldVertexSize));
                }
            }
            // Get Mesh Material Name
            else if (*split_iter == "usemtl")
            {
                algorithm::tail(curline, curMeshMatName);

                // Create new Mesh, if Material changes within a group
                if (!cur_mesh.Indices.empty() && !cur_mesh.Vertices.empty())
                {
                    std::string tmp = cur_mesh.MeshName;
                    for (size_t i = 1; std::find_if(LoadedMeshes.begin(), LoadedMeshes.end(), [tmp](Mesh const &m){return m.MeshName == tmp;}) != LoadedMeshes.end(); ++i)
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
                // Generate LoadedMaterial
                // Generate a path to the material file
                std::string pathtomat;
                if (Path.rfind('/') != std::string::npos)
                {
                    pathtomat.assign(Path.begin(), Path.begin() + Path.rfind('/') + 1);
                }
                pathtomat += algorithm::tail(curline, tail);

                #ifdef OBJL_CONSOLE_OUTPUT
                std::cout << std::endl << "- find materials in: " << pathtomat << std::endl;
                #endif

                // Load Materials
                LoadMaterials(pathtomat);
            }
            //std::cout << 'c'<< linenumber << std::endl;
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

int Loader::GenVerticesFromRawOBJ(std::vector<Vertex>& oVerts,
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

        for (auto iter = oVerts.begin() + oldSize; iter != oVerts.end(); iter++)
        {
            iter->Normal = normal;
        }
    }
    return oVerts.size() - oldSize;
}

// Triangulate a list of vertices into a face by printing
//	inducies corresponding with triangles within it
size_t Loader::VertexTriangluation(std::vector<uint32_t>& oIndices,
    std::vector<Vertex>::const_iterator iVerts_begin,
    std::vector<Vertex>::const_iterator iVerts_end,
    std::vector<Vertex> & tVerts)
{
    // If there are 2 or less verts,
    // no triangle can be created,
    // so exit
    size_t iSize = std::distance(iVerts_begin, iVerts_end);
    if (iSize < 3)
    {
        return 0;
    }
    // If it is a triangle no need to calculate it
    if (iSize == 3)
    {
        oIndices.push_back(0);
        oIndices.push_back(1);
        oIndices.push_back(2);
        return 3;
    }

    size_t oldSize = oIndices.size();
    // Create a list of vertices
    tVerts.assign(iVerts_begin, iVerts_end);
    while (true)
    {
        // For every vertex
        for (size_t i = 0; i < tVerts.size(); i++)
        {
            // pPrev = the previous vertex in the list
            vec3f_t pPrev = i == 0 ? tVerts[tVerts.size() - 1].Position : tVerts[i - 1].Position;

            // pCur = the current vertex;
            vec3f_t pCur = tVerts[i].Position;

            // pNext = the next vertex in the list
            vec3f_t pNext = i == tVerts.size() - 1 ? tVerts[0].Position : tVerts[i + 1].Position;
            
            // Check to see if there are only 3 verts left
            // if so this is the last triangle
            if (tVerts.size() == 3)
            {
                // Create a triangle from pCur, pPrev, pNext
                for (size_t j = 0; j < tVerts.size(); j++)
                {
                    vec3f_t const & curpos = iVerts_begin[j].Position;
                    if (curpos == pCur)  oIndices.push_back(j);
                    if (curpos == pPrev) oIndices.push_back(j);
                    if (curpos == pNext) oIndices.push_back(j);
                }

                tVerts.clear();
                break;
            }
            if (tVerts.size() == 4)
            {
                // Create a triangle from pCur, pPrev, pNext
                for (size_t j = 0; j < iSize; j++)
                {
                    vec3f_t const & curpos = iVerts_begin[j].Position;
                    if (curpos == pCur)  oIndices.push_back(j);
                    if (curpos == pPrev) oIndices.push_back(j);
                    if (curpos == pNext) oIndices.push_back(j);
                }

                vec3f_t tempVec;
                for (size_t j = 0; j < tVerts.size(); j++)
                {
                    vec3f_t const & curpos = iVerts_begin[j].Position;
                    if (curpos != pCur
                        && curpos != pPrev
                        && curpos != pNext)
                    {
                        tempVec = curpos;
                        break;
                    }
                }

                // Create a triangle from pCur, pPrev, pNext
                for (size_t j = 0; j < iSize; j++)
                {
                    vec3f_t const & curpos = iVerts_begin[j].Position;
                    if (curpos == pPrev) oIndices.push_back(j);
                    if (curpos == pNext) oIndices.push_back(j);
                    if (curpos == tempVec)        oIndices.push_back(j);
                }

                tVerts.clear();
                break;
            }

            // If Vertex is not an interior vertex
            ;
            float dot = math::normdot(pPrev - pCur, pNext - pCur);
            if (dot <= 0 && dot >= 1)
                continue;

            // If any vertices are within this triangle
            bool inTri = false;
            for (size_t j = 0; j < iSize; j++)
            {
                vec3f_t const & curpos = iVerts_begin[j].Position;
               if (algorithm::inTriangle(curpos, pPrev, pCur, pNext)
                    && curpos != pPrev
                    && curpos != pCur
                    && curpos != pNext)
                {
                    inTri = true;
                    break;
                }
            }
            if (inTri)
                continue;

            // Create a triangle from pCur, pPrev, pNext
            for (size_t j = 0; j < iSize; j++)
            {
                vec3f_t const & curpos = iVerts_begin[j].Position;
                if (curpos == pCur)  oIndices.push_back(j);
                if (curpos == pPrev) oIndices.push_back(j);
                if (curpos == pNext) oIndices.push_back(j);
            }

            // Delete pCur from the list
            auto iter = std::find_if(tVerts.begin(), tVerts.end(), [&pCur](Vertex v){return v.Position == pCur;});
            if (iter != tVerts.end()){
                tVerts.erase(iter);
            }

            // reset i to the start
            // -1 since loop will add 1 to it
            i = -1;
        }

        // if no triangles were created or no more vertices
        if (oIndices.size() - oldSize == 0 || tVerts.empty())
            break;
    }
    return oIndices.size() - oldSize;
}

bool Loader::LoadMaterials(std::string path)
{
    std::cout << "load materials"<< path << std::endl;
    // If the file is not a material file return false
    if (path.back() == 13){path.pop_back();}
    if (path.substr(path.size() - 4, path.size()) != ".mtl" && path.substr(path.size() - 5, path.size()) != ".mtl\0")
    {
        std::cout << "wrong file ending: " << path.substr(path.size() - 4, path.size()) << std::endl;
        return false;
    }
    std::ifstream file(path);

    // If the file is not found return false
    if (!file.is_open())
    {
        std::cout << "cant't open material file" << std::endl;
        return false;

    }
    Material *material = nullptr;

    // Go through each line looking for material variables
    std::string curline;
    std::vector<std::string> split;
    std::string tail;
    auto split_iter = IO_UTIL::make_split_iterator("", [](char c){return c == ' ' || c == '\t';});

    while (std::getline(file, curline))
    {
        //std::cout << curline << std::endl;
        split_iter.str(curline);
        // new material and material name
        if (*split_iter == "newmtl")
        {
            LoadedMaterials.emplace_back();
            material = &LoadedMaterials.back();
            material->name = curline.size() > 7 ? algorithm::tail(curline, tail) : "none";
        }
        else if (*split_iter == "Ka"){read_vec(algorithm::tail(curline, tail), material->Ka ,split);}
        else if (*split_iter == "Kd"){read_vec(algorithm::tail(curline, tail), material->Kd ,split);}
        else if (*split_iter == "Ks"){read_vec(algorithm::tail(curline, tail), material->Ks ,split);}
        else if (*split_iter == "Ns")   {(++split_iter).parse(material->Ns);}// Optical Density
        else if (*split_iter == "Ni")   {(++split_iter).parse(material->Ni);}// Dissolve
        else if (*split_iter == "d")    {(++split_iter).parse(material->d);}// Illumination
        else if (*split_iter == "illum"){(++split_iter).parse(material->illum);}// Ambient Texture Map
        else if (*split_iter == "map_Ka"){algorithm::tail(curline, material->map_Ka);}   // Diffuse Texture Map
        else if (*split_iter == "map_Kd"){
            std::string & pathtotex = material->map_Kd;
            pathtotex = "";
            if (path.rfind('/') != std::string::npos)
            {
                pathtotex.assign(path.begin(), path.begin() + path.rfind('/') + 1);
            }
            pathtotex += algorithm::tail(curline, tail);
            if (pathtotex.back() == 13)
            {
                pathtotex.pop_back();
            }
        }
        else if (*split_iter == "map_Ks")   {algorithm::tail(curline, material->map_Ks);}// Specular Texture Map
        else if (*split_iter == "map_Ns")   {algorithm::tail(curline, material->map_Ns);}// Specular Hightlight Map
        else if (*split_iter == "map_d")    {algorithm::tail(curline, material->map_d);}// Alpha Texture Map
        else if (*split_iter == "map_Bump" || *split_iter == "map_bump" || *split_iter == "bump")// Bump Map
        {
            algorithm::tail(curline, material->map_bump);
        }
    }
    std::cout << (LoadedMaterials.empty() ? "No materials found" :"Sucess") << std::endl;
    return !LoadedMaterials.empty();
}
}
