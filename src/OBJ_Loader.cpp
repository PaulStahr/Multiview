#include "OBJ_Loader.h"
#include <atomic>
#include <limits>
#include <charconv>

namespace objl
{
Vertex::Vertex(vec3f_t const & pos_, vec3f_t const & normal_, vec2f_t const & texture_coord_) : Position(pos_), Normal(normal_), TextureCoordinate(texture_coord_){}

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
                    return end1;
            return mid1;
        }
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

    split_iterator::split_iterator (const std::string &in_, std::string const & tokens_) : _in_beg(in_.begin()), _in_end(in_.end()), _beg(in_.begin()), _end(in_.begin()), _tokens(tokens_){
        ++(*this);
    }

    std::string& split_iterator::get(std::string & ptr) {ptr.assign(_beg, _end); return ptr;}

    split_iterator & split_iterator::operator++(){
        _beg = find_first_not_of(_end, _in_end, _tokens.begin(), _tokens.end());
        _end = std::find_first_of(_beg, _in_end, _tokens.begin(), _tokens.end());
        return *this;
    }

    void split_iterator::str(const std::string& str)
    {
        _end = _beg = _in_beg = str.begin();
        _in_end = str.end();
        ++(*this);
    }

    std::string::const_iterator split_iterator::begin(){return _beg;}

    std::string::const_iterator split_iterator::end(){return _end;}

    bool split_iterator::valid() const
    {
        return _beg != _in_end;
    }

    /*float split_iterator::to_float(){
        float result;
        auto success = std::from_chars(&(*_beg), &(*_end), result);
        if (success == std::errc())
        {
            throw std::runtime_error();
        }
        return result;
    }*/

    split_iterator::~split_iterator(){}
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
    std::vector<vec2f_t> TCoords;
    std::vector<vec3f_t> Normals;

    std::vector<std::string> MeshMatNames;

    bool listening = false;
    Mesh cur_mesh;

    #ifdef OBJL_CONSOLE_OUTPUT
    const uint32_t outputEveryNth = 1000;
    uint32_t outputIndicator = outputEveryNth;
    #endif

    std::atomic<size_t> read_line;
    read_line = 0;
    std::atomic<size_t> write_line;
    write_line = std::numeric_limits<size_t>::max();
    std::vector<Vertex> tVerts;
#pragma omp parallel num_threads(1)
    {
        std::string curline;
        std::string tail;
        std::string first_token;
        std::vector<std::string> sVert;
        std::vector<std::array<int64_t, 3> > indices; 
        std::string word;
        algorithm::split_iterator split_iter(curline, " \t");
        while (true)
        {
            bool success;
            size_t linenumber;
            #pragma omp critical
            {
                success = std::getline(file, curline) ? true : false;
                linenumber = read_line;
                ++read_line;
            }
            if (!success)
            {
                std::cout << "break" << std::endl;
                break;
            }
             #ifdef OBJL_CONSOLE_OUTPUT
            if ((outputIndicator = ((outputIndicator + 1) % outputEveryNth)) == 1)
            {
                while (write_line != linenumber - 1);
                if (!cur_mesh.MeshName.empty())
                {
                    std::cout
                        << "\r- " << cur_mesh.MeshName
                        << "\t| vertices > " << Positions.size()
                        << "\t| texcoords > " << TCoords.size()
                        << "\t| normals > " << Normals.size()
                        << "\t| triangles > " << (cur_mesh.Vertices.size() / 3)
                        << (!MeshMatNames.empty() ? "\t| material: " + MeshMatNames.back() : "");
                }
            }
            #endif

            // Generate a Mesh Object or Prepare for an object to be created
            split_iter.str(curline);
            split_iter.get(first_token);
            //algorithm::firstToken(curline, first_token);
            if (first_token == "o" || first_token == "g" || curline[0] == 'g')
            {
                while (write_line != linenumber - 1);
                if (listening && !cur_mesh.Indices.empty() && !cur_mesh.Vertices.empty())
                {
                    // Create Mesh
                    LoadedMeshes.emplace_back();
                    LoadedMeshes.back().swap(cur_mesh);
                }
                listening = true;
                cur_mesh.MeshName = first_token == "o" || first_token == "g" ? algorithm::tail(curline, tail) : "unnamed";
                #ifdef OBJL_CONSOLE_OUTPUT
                std::cout << std::endl;
                outputIndicator = 0;
                #endif
            }
            // Generate a Vertex Position
            else if (first_token == "v")
            {
                float x = std::stof((++split_iter).get(word));
                float y = std::stof((++split_iter).get(word));
                float z = std::stof((++split_iter).get(word));
                while (write_line != linenumber - 1);
                Positions.emplace_back(x,y,z);
            }
            // Generate a Vertex Texture Coordinate
            else if (first_token == "vt")
            {
                float u = std::stof((++split_iter).get(word));
                float v = std::stof((++split_iter).get(word));
                while (write_line != linenumber - 1);
                TCoords.emplace_back(u,v);
            }
            // Generate a Vertex Normal;
            else if (first_token == "vn")
            {
                float x = std::stof((++split_iter).get(word));
                float y = std::stof((++split_iter).get(word));
                float z = std::stof((++split_iter).get(word));
                while (write_line != linenumber - 1);
                Normals.emplace_back(x,y,z);
            }
            // Generate a Face (vertices & indices)
            else if (first_token == "f")
            {
                // Generate the vertices
                size_t oldVertexSize = cur_mesh.Vertices.size();
                while ((++split_iter).valid())
                {
                    algorithm::split(split_iter.begin(), split_iter.end(), sVert, '/');
                    switch (sVert.size())
                    {
                        case 1: indices.emplace_back(std::array<int64_t, 3>({(int64_t)std::stoi(sVert[0]), undef_index, undef_index}));break;
                        case 2: indices.emplace_back(std::array<int64_t, 3>({(int64_t)std::stoi(sVert[0]), undef_index, (int64_t)std::stoi(sVert[1])}));break;
                        case 3:
                            if (sVert[1].empty())// P//N
                            {
                                indices.emplace_back(std::array<int64_t, 3>({(int64_t)std::stoi(sVert[0]), (int64_t)std::stoi(sVert[2]), undef_index}));
                            }
                            else// P/T/N
                            {
                                indices.emplace_back(std::array<int64_t, 3>({(int64_t)std::stoi(sVert[0]), (int64_t)std::stoi(sVert[2]), (int64_t)std::stoi(sVert[1])}));
                            }
                            break;
                    }
                }
                
                while (write_line != linenumber - 1);
                size_t num_added = GenVerticesFromRawOBJ(cur_mesh.Vertices, Positions, TCoords, Normals, indices);
                indices.clear();
                LoadedVertices += num_added;
                size_t old_indice_count = cur_mesh.Indices.size();
                size_t addedIndices = VertexTriangluation(cur_mesh.Indices, cur_mesh.Vertices.cend() - num_added, cur_mesh.Vertices.cend(), tVerts);
                
                // Add Indices
                LoadedIndices += addedIndices;
                std::transform(cur_mesh.Indices.begin() + old_indice_count, cur_mesh.Indices.end(), cur_mesh.Indices.begin() + old_indice_count, UTIL::plus(oldVertexSize));
            }
            // Get Mesh Material Name
            else if (first_token == "usemtl")
            {
                while (write_line != linenumber - 1);
                MeshMatNames.push_back(algorithm::tail(curline, tail));

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
                }
            
                #ifdef OBJL_CONSOLE_OUTPUT
                outputIndicator = 0;
                #endif
            }
            // Load Materials
            else if (first_token == "mtllib")
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

                while (write_line != linenumber - 1);
                // Load Materials
                LoadMaterials(pathtomat);
            }
            else
            {
                while (write_line != linenumber - 1);
            }
            //std::cout << 'c'<< linenumber << std::endl;
            if (write_line != linenumber - 1)
            {
                throw std::runtime_error("error: " + std::to_string(write_line) + " " + std::to_string(linenumber) + " -> ");
            }
            write_line = linenumber;
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
    }

    file.close();

    // Set Materials for each Mesh
    for (size_t i = 0; i < MeshMatNames.size(); ++i)
    {
        std::string const & matname = MeshMatNames[i];
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
    const std::vector<vec2f_t>& iTCoords,
    const std::vector<vec3f_t>& iNormals,
    std::vector<std::array<int64_t, 3> > const & indices)
{    
    bool noNormal = false;
    size_t oldSize = oVerts.size();

    for (size_t i = 0; i < indices.size(); i++)
    {
        std::array<int64_t, 3> const & idx = indices[i];
        oVerts.emplace_back(
            algorithm::getElement(iPositions, idx[0]), idx[1] == undef_index ? vec3f_t(0,0,0) : algorithm::getElement(iNormals, idx[1]), idx[2] == undef_index ? vec2f_t(0, 0) : algorithm::getElement(iTCoords, idx[2]));
        noNormal |= idx[1] == undef_index;
    }
    if (noNormal)
    {
        vec3f_t normal = -algorithm::GenTriNormal(oVerts[oldSize+1].Position, oVerts[oldSize+0].Position, oVerts[oldSize+2].Position);

        for (size_t i = oldSize; i < oVerts.size(); i++)
        {
            oVerts[i].Normal = normal;
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
            Vertex pPrev = i == 0 ? tVerts[tVerts.size() - 1] : tVerts[i - 1];

            // pCur = the current vertex;
            Vertex pCur = tVerts[i];

            // pNext = the next vertex in the list
            Vertex pNext = i == tVerts.size() - 1 ? tVerts[0] : tVerts[i + 1];
            
            // Check to see if there are only 3 verts left
            // if so this is the last triangle
            if (tVerts.size() == 3)
            {
                // Create a triangle from pCur, pPrev, pNext
                for (size_t j = 0; j < tVerts.size(); j++)
                {
                    if (iVerts_begin[j].Position == pCur.Position)  oIndices.push_back(j);
                    if (iVerts_begin[j].Position == pPrev.Position) oIndices.push_back(j);
                    if (iVerts_begin[j].Position == pNext.Position) oIndices.push_back(j);
                }

                tVerts.clear();
                break;
            }
            if (tVerts.size() == 4)
            {
                // Create a triangle from pCur, pPrev, pNext
                for (size_t j = 0; j < iSize; j++)
                {
                    if (iVerts_begin[j].Position == pCur.Position)  oIndices.push_back(j);
                    if (iVerts_begin[j].Position == pPrev.Position) oIndices.push_back(j);
                    if (iVerts_begin[j].Position == pNext.Position) oIndices.push_back(j);
                }

                vec3f_t tempVec;
                for (size_t j = 0; j < tVerts.size(); j++)
                {
                    if (tVerts[j].Position != pCur.Position
                        && tVerts[j].Position != pPrev.Position
                        && tVerts[j].Position != pNext.Position)
                    {
                        tempVec = tVerts[j].Position;
                        break;
                    }
                }

                // Create a triangle from pCur, pPrev, pNext
                for (size_t j = 0; j < iSize; j++)
                {
                    if (iVerts_begin[j].Position == pPrev.Position) oIndices.push_back(j);
                    if (iVerts_begin[j].Position == pNext.Position) oIndices.push_back(j);
                    if (iVerts_begin[j].Position == tempVec)        oIndices.push_back(j);
                }

                tVerts.clear();
                break;
            }

            // If Vertex is not an interior vertex
            float angle = math::AngleBetweenV3(pPrev.Position - pCur.Position, pNext.Position - pCur.Position);//TODO
            if (angle <= 0 && angle >= 1 / 3.14159265359)
                continue;

            // If any vertices are within this triangle
            bool inTri = false;
            for (size_t j = 0; j < iSize; j++)
            {
                if (algorithm::inTriangle(iVerts_begin[j].Position, pPrev.Position, pCur.Position, pNext.Position)
                    && iVerts_begin[j].Position != pPrev.Position
                    && iVerts_begin[j].Position != pCur.Position
                    && iVerts_begin[j].Position != pNext.Position)
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
                if (iVerts_begin[j].Position == pCur.Position)  oIndices.push_back(j);
                if (iVerts_begin[j].Position == pPrev.Position) oIndices.push_back(j);
                if (iVerts_begin[j].Position == pNext.Position) oIndices.push_back(j);
            }

            // Delete pCur from the list
            for (size_t j = 0; j < tVerts.size(); j++)
            {
                if (tVerts[j].Position == pCur.Position)
                {
                    tVerts.erase(tVerts.begin() + j);
                    break;
                }
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
    std::string first_token;
    while (std::getline(file, curline))
    {
        //std::cout << curline << std::endl;
        algorithm::firstToken(curline, first_token);
        // new material and material name
        if (first_token == "newmtl")
        {
            LoadedMaterials.emplace_back();
            material = &LoadedMaterials.back();
            material->name = curline.size() > 7 ? algorithm::tail(curline, tail) : "none";
        }
        else if (first_token == "Ka"){read_vec(algorithm::tail(curline, tail), material->Ka ,split);}
        else if (first_token == "Kd"){read_vec(algorithm::tail(curline, tail), material->Kd ,split);}
        else if (first_token == "Ks"){read_vec(algorithm::tail(curline, tail), material->Ks ,split);}
        else if (first_token == "Ns")   {material->Ns = std::stof(algorithm::tail(curline, tail));}// Optical Density
        else if (first_token == "Ni")   {material->Ni = std::stof(algorithm::tail(curline, tail));}// Dissolve
        else if (first_token == "d")    {material->d = std::stof(algorithm::tail(curline, tail));}// Illumination
        else if (first_token == "illum"){material->illum = std::stoi(algorithm::tail(curline, tail));}// Ambient Texture Map
        else if (first_token == "map_Ka"){algorithm::tail(curline, material->map_Ka);}   // Diffuse Texture Map
        else if (first_token == "map_Kd"){
            algorithm::split(path, split, '/');
            std::string & pathtotex = material->map_Kd;
            pathtotex = "";
            
            if (split.size() != 1)
            {
                for (size_t i = 0; i < split.size() - 1; ++i)
                {
                    pathtotex += split[i];
                    pathtotex += "/";
                }
            }

            pathtotex += algorithm::tail(curline, tail);
            
            if (pathtotex[pathtotex.size() - 1] == 13)
            {
                pathtotex.pop_back();
            }
        }
        else if (first_token == "map_Ks")   {algorithm::tail(curline, material->map_Ks);}// Specular Texture Map
        else if (first_token == "map_Ns")   {algorithm::tail(curline, material->map_Ns);}// Specular Hightlight Map
        else if (first_token == "map_d")    {algorithm::tail(curline, material->map_d);}// Alpha Texture Map
        else if (first_token == "map_Bump" || first_token == "map_bump" || first_token == "bump")// Bump Map
        {
            algorithm::tail(curline, material->map_bump);
        }
    }
    std::cout << (LoadedMaterials.empty() ? "No materials found" :"Sucess") << std::endl;
    return !LoadedMaterials.empty();
}
}
