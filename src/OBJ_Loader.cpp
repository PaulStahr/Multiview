#include "OBJ_Loader.h"

namespace objl
{
Vertex::Vertex(vec3f_t const & pos_, vec3f_t const & normal_, vec2f_t const & texture_coord_) : Position(pos_), Normal(normal_), TextureCoordinate(texture_coord_){}

Material::Material() : Ns(0.0f), Ni(0.0f), d(0.0f), illum(0){}

Mesh::Mesh(std::vector<Vertex> const & _Vertices, std::vector<uint32_t> const & _Indices) : Vertices(_Vertices), Indices(_Indices){}

namespace math
{
    vec3f_t CrossV3(const vec3f_t a, const vec3f_t b)
    {
        return vec3f_t(a.y() * b.z() - a.z() * b.y(),
            a.z() * b.x() - a.x() * b.z(),
            a.x() * b.y() - a.y() * b.x());
    }

    // vec3f_t Magnitude Calculation
    float MagnitudeV3(const vec3f_t in)
    {
        return sqrtf(in.dot());
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
    bool SameSide(vec3f_t const & p1, vec3f_t const & p2, vec3f_t const & a, vec3f_t const & b)
    {
        vec3f_t cp1 = math::CrossV3(b - a, p1 - a);
        vec3f_t cp2 = math::CrossV3(b - a, p2 - a);
        return dot(cp1, cp2) >= 0;
    }

    // Generate a cross produect normal for a triangle
    vec3f_t GenTriNormal(vec3f_t const & t1, vec3f_t const & t2, vec3f_t const & t3)
    {
        vec3f_t u = t2 - t1;
        vec3f_t v = t3 - t1;
        return math::CrossV3(u,v);
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
        return math::MagnitudeV3(proj) == 0;
    }
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

    std::vector<Vertex> Vertices;
    std::vector<uint32_t> Indices;

    std::vector<std::string> MeshMatNames;

    bool listening = false;
    std::string meshname;

    #ifdef OBJL_CONSOLE_OUTPUT
    const uint32_t outputEveryNth = 1000;
    uint32_t outputIndicator = outputEveryNth;
    #endif

    std::string curline;
    std::vector<std::string> split;
    std::vector<std::string> sVert;
    std::vector<std::string> sFace;
    std::string tail;
    std::string first_token;
    std::vector<Vertex> tVerts;
    while (std::getline(file, curline))
    {
        #ifdef OBJL_CONSOLE_OUTPUT
        if ((outputIndicator = ((outputIndicator + 1) % outputEveryNth)) == 1)
        {
            if (!meshname.empty())
            {
                std::cout
                    << "\r- " << meshname
                    << "\t| vertices > " << Positions.size()
                    << "\t| texcoords > " << TCoords.size()
                    << "\t| normals > " << Normals.size()
                    << "\t| triangles > " << (Vertices.size() / 3)
                    << (!MeshMatNames.empty() ? "\t| material: " + MeshMatNames.back() : "");
            }
        }
        #endif

        // Generate a Mesh Object or Prepare for an object to be created
        algorithm::firstToken(curline, first_token);
        if (first_token == "o" || first_token == "g" || curline[0] == 'g')
        {
            if (!listening)
            {
                listening = true;
                meshname = first_token == "o" || first_token == "g" ? algorithm::tail(curline, tail) : "unnamed";
            }
            else
            {
                // Generate the mesh to put into the array

                if (!Indices.empty() && !Vertices.empty())
                {
                    // Create Mesh
                    LoadedMeshes.emplace_back();
                    LoadedMeshes.back().MeshName = meshname;

                    LoadedMeshes.back().Vertices.swap(Vertices);
                    LoadedMeshes.back().Indices.swap(Indices);
                    meshname.clear();

                    algorithm::tail(curline, meshname);
                }
                else
                {
                    meshname = first_token == "o" || first_token == "g" ? algorithm::tail(curline, tail) : meshname = "unnamed";
                }
            }
            #ifdef OBJL_CONSOLE_OUTPUT
            std::cout << std::endl;
            outputIndicator = 0;
            #endif
        }
        // Generate a Vertex Position
        else if (first_token == "v")
        {
            algorithm::split(algorithm::tail(curline, tail), split, ' ');
            Positions.emplace_back(std::stof(split[0]),std::stof(split[1]),std::stof(split[2]));
        }
        // Generate a Vertex Texture Coordinate
        else if (first_token == "vt")
        {
            algorithm::split(algorithm::tail(curline, tail), split, ' ');
            TCoords.emplace_back(std::stof(split[0]), std::stof(split[1]));
        }
        // Generate a Vertex Normal;
        else if (first_token == "vn")
        {
            algorithm::split(algorithm::tail(curline, tail), split, ' ');
            Normals.emplace_back(std::stof(split[0]), std::stof(split[1]), std::stof(split[2]));
        }
        // Generate a Face (vertices & indices)
        else if (first_token == "f")
        {
            // Generate the vertices
            size_t oldVertexSize = Vertices.size();
            size_t num_added = GenVerticesFromRawOBJ(Vertices, Positions, TCoords, Normals, sFace, sVert, algorithm::tail(curline, tail));
            sFace.clear();
            sVert.clear();
            LoadedVertices += num_added;
            size_t old_indice_count = Indices.size();
            size_t addedIndices = VertexTriangluation(Indices, Vertices.cend() - num_added, Vertices.cend(), tVerts);
            
            // Add Indices
            LoadedIndices += addedIndices;
            for (size_t i = old_indice_count; i < Indices.size(); ++i)
            {
                Indices[i] += oldVertexSize;
            }
        }
        // Get Mesh Material Name
        else if (first_token == "usemtl")
        {
            MeshMatNames.push_back(algorithm::tail(curline, tail));

            // Create new Mesh, if Material changes within a group
            if (!Indices.empty() && !Vertices.empty())
            {
                // Create Mesh
                LoadedMeshes.emplace_back();
                LoadedMeshes.back().Vertices.swap(Vertices);
                LoadedMeshes.back().Indices.swap(Indices);
                LoadedMeshes.back().MeshName = meshname;
                for(size_t i = 1; true; ++i) {
                    if (i != 1)
                    {
                        LoadedMeshes.back().MeshName = meshname + "_" + std::to_string(i);
                    }
                    for (auto m = LoadedMeshes.begin(); m + 1 < LoadedMeshes.end(); ++m)
                    {
                        if (m->MeshName == LoadedMeshes.back().MeshName)
                        {
                            continue;
                        }
                    }
                    break;
                }
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
            algorithm::split(Path, split, '/');

            std::string pathtomat = "";

            if (split.size() != 1)
            {
                for (size_t i = 0; i < split.size() - 1; i++)
                {
                    pathtomat += split[i];
                    pathtomat += "/";
                }
            }


            pathtomat += algorithm::tail(curline, tail);

            #ifdef OBJL_CONSOLE_OUTPUT
            std::cout << std::endl << "- find materials in: " << pathtomat << std::endl;
            #endif

            // Load Materials
            LoadMaterials(pathtomat);
        }
    }

    #ifdef OBJL_CONSOLE_OUTPUT
    std::cout << std::endl;
    #endif

    // Deal with last mesh

    if (!Indices.empty() && !Vertices.empty())
    {
        LoadedMeshes.emplace_back();
        LoadedMeshes.back().Vertices.swap(Vertices);
        LoadedMeshes.back().Indices.swap(Indices);
        LoadedMeshes.back().MeshName = meshname;
    }

    file.close();

    // Set Materials for each Mesh
    for (size_t i = 0; i < MeshMatNames.size(); i++)
    {
        std::string const & matname = MeshMatNames[i];

        // Find corresponding material name in loaded materials
        // when found copy material variables into mesh material
        for (size_t j = 0; j < LoadedMaterials.size(); j++)
        {
            if (LoadedMaterials[j].name == matname)
            {
                LoadedMeshes[i].MeshMaterial = LoadedMaterials[j];
                break;
            }
        }
    }
    return !(LoadedMeshes.empty() && LoadedVertices == 0 && LoadedIndices == 0);
}

int Loader::GenVerticesFromRawOBJ(std::vector<Vertex>& oVerts,
    const std::vector<vec3f_t>& iPositions,
    const std::vector<vec2f_t>& iTCoords,
    const std::vector<vec3f_t>& iNormals,
    std::vector<std::string> & sface,
    std::vector<std::string> & svert,
    std::string const & icurline)
{
    algorithm::split(icurline, sface, ' ');

    bool noNormal = false;
    size_t oldSize = oVerts.size();

    for (size_t i = 0; i < sface.size(); i++)
    {
        algorithm::split(sface[i], svert, '/');

        // Check for just position - v1
        if (svert.size() == 1)// P
        {
            oVerts.emplace_back(algorithm::getElement(iPositions, svert[0]), vec3f_t(0, 0,0), vec2f_t(0, 0));
            noNormal = true;
        }

        // Check for position & texture - v1/vt1
        if (svert.size() == 2)// P/T
        {
            oVerts.emplace_back(algorithm::getElement(iPositions, svert[0]), vec3f_t(0,0,0), algorithm::getElement(iTCoords, svert[1]));
            noNormal = true;
        }

        // Check for Position, Texture and Normal - v1/vt1/vn1
        // or if Position and Normal - v1//vn1
        if (svert.size() == 3)
        {
            if (svert[1] == "")// P//N
            {
                oVerts.emplace_back(algorithm::getElement(iPositions, svert[0]), algorithm::getElement(iNormals, svert[2]), vec2f_t(0, 0));
            }
            else// P/T/N
            {
                oVerts.emplace_back(algorithm::getElement(iPositions, svert[0]), algorithm::getElement(iNormals, svert[2]), algorithm::getElement(iTCoords, svert[1]));
            }
        }
    }

    // take care of missing normals
    // these may not be truly acurate but it is the 
    // best they get for not compiling a mesh with normals	
    if (noNormal)
    {
        vec3f_t A = oVerts[oldSize+1].Position;
        vec3f_t B = oVerts[oldSize+2].Position - oVerts[oldSize+1].Position;

        vec3f_t normal = math::CrossV3(A, B);

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
                    if (iVerts_begin[j].Position == pCur.Position)
                        oIndices.push_back(j);
                    if (iVerts_begin[j].Position == pPrev.Position)
                        oIndices.push_back(j);
                    if (iVerts_begin[j].Position == pNext.Position)
                        oIndices.push_back(j);
                }

                tVerts.clear();
                break;
            }
            if (tVerts.size() == 4)
            {
                // Create a triangle from pCur, pPrev, pNext
                for (size_t j = 0; j < iSize; j++)
                {
                    if (iVerts_begin[j].Position == pCur.Position)
                        oIndices.push_back(j);
                    if (iVerts_begin[j].Position == pPrev.Position)
                        oIndices.push_back(j);
                    if (iVerts_begin[j].Position == pNext.Position)
                        oIndices.push_back(j);
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
                    if (iVerts_begin[j].Position == pPrev.Position)
                        oIndices.push_back(j);
                    if (iVerts_begin[j].Position == pNext.Position)
                        oIndices.push_back(j);
                    if (iVerts_begin[j].Position == tempVec)
                        oIndices.push_back(j);
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
                if (iVerts_begin[j].Position == pCur.Position)
                    oIndices.push_back(j);
                if (iVerts_begin[j].Position == pPrev.Position)
                    oIndices.push_back(j);
                if (iVerts_begin[j].Position == pNext.Position)
                    oIndices.push_back(j);
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

        // if no triangles were created
        if (oIndices.size() - oldSize == 0)
            break;

        // if no more vertices
        if (tVerts.empty())
            break;
    }
    return oIndices.size() - oldSize;
}

bool Loader::LoadMaterials(std::string path)
{
    std::cout << "load materials"<< path << std::endl;
    // If the file is not a material file return false
    if (path[path.size() - 1] == 13)
    {
        path.pop_back();
    }
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
        // Ambient Color
        else if (first_token == "Ka")
        {
            algorithm::split(algorithm::tail(curline, tail), split, ' ');

            if (split.size() != 3)
                continue;

            material->Ka = vec3f_t(std::stof(split[0]),std::stof(split[1]),std::stof(split[2]));
        }
        // Diffuse Color
        else if (first_token == "Kd")
        {
            algorithm::split(algorithm::tail(curline, tail), split, ' ');

            if (split.size() != 3)
                continue;

            material->Kd = vec3f_t(std::stof(split[0]),std::stof(split[1]),std::stof(split[2]));
        }
        // Specular Color
        else if (first_token == "Ks")
        {
            algorithm::split(algorithm::tail(curline, tail), split, ' ');

            if (split.size() != 3)
                continue;

            material->Ks = vec3f_t(std::stof(split[0]),std::stof(split[1]),std::stof(split[2]));
        }
        // Specular Exponent
        else if (first_token == "Ns")
        {
            material->Ns = std::stof(algorithm::tail(curline, tail));
        }
        // Optical Density
        else if (first_token == "Ni")
        {
            material->Ni = std::stof(algorithm::tail(curline, tail));
        }
        // Dissolve
        else if (first_token == "d")
        {
            material->d = std::stof(algorithm::tail(curline, tail));
        }
        // Illumination
        else if (first_token == "illum")
        {
            material->illum = std::stoi(algorithm::tail(curline, tail));
        }
        // Ambient Texture Map
        else if (first_token == "map_Ka")
        {
            algorithm::tail(curline, material->map_Ka);
        }
        // Diffuse Texture Map
        else if (first_token == "map_Kd")
        {
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
        // Specular Texture Map
        else if (first_token == "map_Ks")
        {
            algorithm::tail(curline, material->map_Ks);
        }
        // Specular Hightlight Map
        else if (first_token == "map_Ns")
        {
            algorithm::tail(curline, material->map_Ns);
        }
        // Alpha Texture Map
        else if (first_token == "map_d")
        {
            algorithm::tail(curline, material->map_d);
        }
        // Bump Map
        else if (first_token == "map_Bump" || first_token == "map_bump" || first_token == "bump")
        {
            algorithm::tail(curline, material->map_bump);
        }
    }
    std::cout << (LoadedMaterials.empty() ? "No materials found" :"Sucess") << std::endl;
    return !LoadedMaterials.empty();
}
}
