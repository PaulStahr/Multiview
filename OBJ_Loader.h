// OBJ_Loader.h - A Single Header OBJ Model Loader

#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <math.h>

// Print progress to console while loading (large models)
#define OBJL_CONSOLE_OUTPUT

// Namespace: OBJL
//
// Description: The namespace that holds eveyrthing that
//	is needed and used for the OBJ Model Loader
namespace objl
{
	// Structure: Vector2
	//
	// Description: A 2D Vector that Holds Positional Data
	struct Vector2
	{
		// Default Constructor
		Vector2()
		{
			X = 0.0f;
			Y = 0.0f;
		}
		// Variable Set Constructor
		Vector2(float X_, float Y_)
		{
			X = X_;
			Y = Y_;
		}
		// Bool Equals Operator Overload
		bool operator==(const Vector2& other) const
		{
			return (this->X == other.X && this->Y == other.Y);
		}
		// Bool Not Equals Operator Overload
		bool operator!=(const Vector2& other) const
		{
			return !(this->X == other.X && this->Y == other.Y);
		}
		// Addition Operator Overload
		Vector2 operator+(const Vector2& right) const
		{
			return Vector2(this->X + right.X, this->Y + right.Y);
		}
		// Subtraction Operator Overload
		Vector2 operator-(const Vector2& right) const
		{
			return Vector2(this->X - right.X, this->Y - right.Y);
		}
		// Float Multiplication Operator Overload
		Vector2 operator*(const float& other) const
		{
			return Vector2(this->X *other, this->Y * other);
		}

		// Positional Variables
		float X;
		float Y;
	};

	// Structure: Vector3
	//
	// Description: A 3D Vector that Holds Positional Data
	struct Vector3
	{
		// Default Constructor
		Vector3() : X(0),Y(0),Z(0)
		{}
		// Variable Set Constructor
		Vector3(float X_, float Y_, float Z_) : X(X_), Y(Y_), Z(Z_)
		{}
		// Bool Equals Operator Overload
		bool operator==(const Vector3& other) const
		{
			return (this->X == other.X && this->Y == other.Y && this->Z == other.Z);
		}
		// Bool Not Equals Operator Overload
		bool operator!=(const Vector3& other) const
		{
			return !(this->X == other.X && this->Y == other.Y && this->Z == other.Z);
		}
		// Addition Operator Overload
		Vector3 operator+(const Vector3& right) const
		{
			return Vector3(this->X + right.X, this->Y + right.Y, this->Z + right.Z);
		}
		// Subtraction Operator Overload
		Vector3 operator-(const Vector3& right) const
		{
			return Vector3(this->X - right.X, this->Y - right.Y, this->Z - right.Z);
		}
		// Float Multiplication Operator Overload
		Vector3 operator*(const float& other) const
		{
			return Vector3(this->X * other, this->Y * other, this->Z * other);
		}
		// Float Division Operator Overload
		Vector3 operator/(const float& other) const
		{
			return Vector3(this->X / other, this->Y / other, this->Z / other);
		}
		
		float dot() const{return X * X + Y * Y + Z * Z;}

		// Positional Variables
		float X;
		float Y;
		float Z;
	};

	// Structure: Vertex
	//
	// Description: Model Vertex object that holds
	//	a Position, Normal, and Texture Coordinate
	struct Vertex
	{
		// Position Vector
		Vector3 Position;

		// Normal Vector
		Vector3 Normal;

		// Texture Coordinate Vector
		Vector2 TextureCoordinate;
        Vertex(){}
        
        Vertex(Vector3 const & pos_, Vector3 const & normal_, Vector2 const & texture_coord_) : Position(pos_), Normal(normal_), TextureCoordinate(texture_coord_){}
	};

	struct Material
	{
		Material()
		{
			Ns = 0.0f;
			Ni = 0.0f;
			d = 0.0f;
			illum = 0;
		}

		// Material Name
		std::string name;
		// Ambient Color
		Vector3 Ka;
		// Diffuse Color
		Vector3 Kd;
		// Specular Color
		Vector3 Ks;
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

	// Structure: Mesh
	//
	// Description: A Simple Mesh Object that holds
	//	a name, a vertex list, and an index list
	struct Mesh
	{
		// Default Constructor
		Mesh(){}
		// Variable Set Constructor
		Mesh(std::vector<Vertex> const & _Vertices, std::vector<uint32_t> const & _Indices) : Vertices(_Vertices), Indices(_Indices)
		{}
		// Mesh Name
		std::string MeshName;
		// Vertex List
		std::vector<Vertex> Vertices;
		// Index List
		std::vector<uint32_t> Indices;

		// Material
		Material MeshMaterial;
	};

	// Namespace: Math
	//
	// Description: The namespace that holds all of the math
	//	functions need for OBJL
	namespace math
	{
		// Vector3 Cross Product
		Vector3 CrossV3(const Vector3 a, const Vector3 b)
		{
			return Vector3(a.Y * b.Z - a.Z * b.Y,
				a.Z * b.X - a.X * b.Z,
				a.X * b.Y - a.Y * b.X);
		}

		// Vector3 Magnitude Calculation
		float MagnitudeV3(const Vector3 in)
		{
			return sqrtf(in.dot());
		}

		// Vector3 DotProduct
		float DotV3(const Vector3 a, const Vector3 b)
		{
			return a.X * b.X + a.Y * b.Y + a.Z * b.Z;
		}

		// Angle between 2 Vector3 Objects
		float AngleBetweenV3(const Vector3 a, const Vector3 b)
		{
			return acosf(DotV3(a, b) / sqrtf(a.dot() * b.dot()));
		}

		// Projection Calculation of a onto b
		Vector3 ProjV3(const Vector3 a, const Vector3 b)
		{
			return b * (DotV3(a, b) / b.dot());
		}
	}

	// Namespace: Algorithm
	//
	// Description: The namespace that holds all of the
	// Algorithms needed for OBJL
	namespace algorithm
	{
		// Vector3 Multiplication Opertor Overload
		Vector3 operator*(const float& left, const Vector3& right)
		{
			return Vector3(right.X * left, right.Y * left, right.Z * left);
		}

		// A test to see if P1 is on the same side as P2 of a line segment ab
		bool SameSide(Vector3 const & p1, Vector3 const & p2, Vector3 const & a, Vector3 const & b)
		{
			Vector3 cp1 = math::CrossV3(b - a, p1 - a);
			Vector3 cp2 = math::CrossV3(b - a, p2 - a);

			if (math::DotV3(cp1, cp2) >= 0)
				return true;
			else
				return false;
		}

		// Generate a cross produect normal for a triangle
		Vector3 GenTriNormal(Vector3 const & t1, Vector3 const & t2, Vector3 const & t3)
		{
			Vector3 u = t2 - t1;
			Vector3 v = t3 - t1;

			return math::CrossV3(u,v);
		}

		// Check to see if a Vector3 Point is within a 3 Vector3 Triangle
		bool inTriangle(Vector3 point, Vector3 tri1, Vector3 tri2, Vector3 tri3)
		{
			// Test to see if it is within an infinite prism that the triangle outlines.
			bool within_tri_prisim = SameSide(point, tri1, tri2, tri3) && SameSide(point, tri2, tri1, tri3)
				&& SameSide(point, tri3, tri1, tri2);

			// If it isn't it will never be on the triangle
			if (!within_tri_prisim)
				return false;

			// Calulate Triangle's Normal
			Vector3 n = GenTriNormal(tri1, tri2, tri3);

			// Project the point onto this normal
			Vector3 proj = math::ProjV3(point, n);

			// If the distance from the triangle to the point is 0
			//	it lies on the triangle
			return math::MagnitudeV3(proj) == 0;
		}

		// Split a String into a string array at a given token
		inline void split(const std::string &in,
			std::vector<std::string> &out,
			std::string const & token)
		{
			out.clear();

            std::string::const_iterator beg = in.cbegin();
            for (std::string::const_iterator i = in.cbegin(); i < in.cend(); i++)
			{
				if (std::equal(token.begin(), token.end(), i))//test == token)
				{
					if (beg != i)
					{
                        out.emplace_back(beg, i);
						//out.push_back(in.substr(beg, i - beg));
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
					//out.push_back(in.substr(beg, in.size() - beg));
            		break;
				}
			}
		}
		
		// Split a String into a string array at a given token
		inline void split(const std::string &in,
			std::vector<std::string> &out,
			char token)
		{
			out.clear();
            if (in.size() == 0)
            {
                return;
            }

            std::string::const_iterator beg = in.begin();
            //std::cout << "in " << in << std::endl;
			std::string::const_iterator i = in.begin();
            while (true)
			{
                if (*i == token)
				{
                    out.emplace_back(beg, i);
                    ++i;
                    beg = i;
				}
				else if (++i == in.end())
				{
                    out.emplace_back(beg, in.end());
					return;
				}
			}
		}
		
		// Get tail of string after first token and possibly following spaces
		inline std::string & tail(const std::string &in, std::string &out)
		{
			size_t token_start = in.find_first_not_of(" \t");
			size_t space_start = in.find_first_of(" \t", token_start);
			size_t tail_start = in.find_first_not_of(" \t", space_start);
			size_t tail_end = in.find_last_not_of(" \t");
			if (tail_start != std::string::npos && tail_end != std::string::npos)
			{
				return out.assign(tail_start + in.begin(), tail_end + 1 + in.begin());
			}
			else if (tail_start != std::string::npos)
			{
				return out.assign(tail_start + in.begin(), in.end());
			}
			return out = "";
		}

		// Get tail of string after first token and possibly following spaces
		/*inline std::string tail(const std::string &in)
		{
			size_t token_start = in.find_first_not_of(" \t");
			size_t space_start = in.find_first_of(" \t", token_start);
			size_t tail_start = in.find_first_not_of(" \t", space_start);
			size_t tail_end = in.find_last_not_of(" \t");
			if (tail_start != std::string::npos && tail_end != std::string::npos)
			{
				return in.substr(tail_start, tail_end - tail_start + 1);
			}
			else if (tail_start != std::string::npos)
			{
				return in.substr(tail_start);
			}
			return "";
		}*/
		
		// Get first token of string
		inline std::string & firstToken(const std::string &in, std::string & out)
		{
			if (!in.empty())
			{
				size_t token_start = in.find_first_not_of(" \t");
				size_t token_end = in.find_first_of(" \t", token_start);
				if (token_start != std::string::npos && token_end != std::string::npos)
				{
					return out.assign(token_start + in.begin(), token_end + in.begin());
				}
				else if (token_start != std::string::npos)
				{
					return out.assign(token_start + in.begin(), in.end());
				}
			}
			return out.assign("");
		}

		/*// Get first token of string
		inline std::string firstToken(const std::string &in)
		{
			if (!in.empty())
			{
				size_t token_start = in.find_first_not_of(" \t");
				size_t token_end = in.find_first_of(" \t", token_start);
				if (token_start != std::string::npos && token_end != std::string::npos)
				{
					return in.substr(token_start, token_end - token_start);
				}
				else if (token_start != std::string::npos)
				{
					return in.substr(token_start);
				}
			}
			return "";
		}*/

		// Get element at given index position
		template <class T>
		inline const T & getElement(const std::vector<T> &elements, std::string &index)
		{
			int idx = std::stoi(index);
			if (idx < 0)
				idx += elements.size();
			else
				idx--;
			return elements[idx];
		}
	}

	// Class: Loader
	//
	// Description: The OBJ Model Loader
	class Loader
	{
	public:
		// Default Constructor
		Loader()
		{

		}
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
		bool LoadFile(std::string const & Path)
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

			std::vector<Vector3> Positions;
			std::vector<Vector2> TCoords;
			std::vector<Vector3> Normals;

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
		//	tcoords, normals and a face line
		int GenVerticesFromRawOBJ(std::vector<Vertex>& oVerts,
			const std::vector<Vector3>& iPositions,
			const std::vector<Vector2>& iTCoords,
			const std::vector<Vector3>& iNormals,
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
                    oVerts.emplace_back(algorithm::getElement(iPositions, svert[0]), Vector3(0, 0,0), Vector2(0, 0));
					noNormal = true;
				}

				// Check for position & texture - v1/vt1
				if (svert.size() == 2)// P/T
				{
                    oVerts.emplace_back(algorithm::getElement(iPositions, svert[0]), Vector3(0,0,0), algorithm::getElement(iTCoords, svert[1]));
					noNormal = true;
				}

				// Check for Position, Texture and Normal - v1/vt1/vn1
				// or if Position and Normal - v1//vn1
				if (svert.size() == 3)
				{
                    if (svert[1] == "")// P//N
                    {
                        oVerts.emplace_back(algorithm::getElement(iPositions, svert[0]), algorithm::getElement(iNormals, svert[2]), Vector2(0, 0));
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
				Vector3 A = oVerts[oldSize  ].Position - oVerts[oldSize+1].Position;
				Vector3 B = oVerts[oldSize+2].Position - oVerts[oldSize+1].Position;

				Vector3 normal = math::CrossV3(A, B);

				for (size_t i = oldSize; i < oVerts.size(); i++)
				{
					oVerts[i].Normal = normal;
				}
			}
			return oVerts.size() - oldSize;
		}

		// Triangulate a list of vertices into a face by printing
		//	inducies corresponding with triangles within it
		size_t VertexTriangluation(std::vector<uint32_t>& oIndices,
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

						Vector3 tempVec;
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

		// Load Materials from .mtl file
		bool LoadMaterials(std::string path)
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

					material->Ka = Vector3(std::stof(split[0]),std::stof(split[1]),std::stof(split[2]));
				}
				// Diffuse Color
				else if (first_token == "Kd")
				{
					algorithm::split(algorithm::tail(curline, tail), split, ' ');

					if (split.size() != 3)
						continue;

					material->Kd = Vector3(std::stof(split[0]),std::stof(split[1]),std::stof(split[2]));
				}
				// Specular Color
				else if (first_token == "Ks")
				{
					algorithm::split(algorithm::tail(curline, tail), split, ' ');

					if (split.size() != 3)
						continue;

					material->Ks = Vector3(std::stof(split[0]),std::stof(split[1]),std::stof(split[2]));
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
	};
}
