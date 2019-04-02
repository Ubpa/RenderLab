#ifndef _OPENGL_MESH_MESH_H_
#define _OPENGL_MESH_MESH_H_

typedef unsigned int uint;

#include <CppUtil/Basic/Point3.h>
#include <CppUtil/Basic/Normal.h>
#include <CppUtil/Basic/Vector2.h>

#include <string>
#include <vector>

namespace CppUtil {
	namespace OpenGL {
		class Shader;

		class Mesh {
		public:
			struct Vertex {
				// position
				Point3 Position;
				// normal
				Normalf Normal;
				// texCoords
				Vec2 TexCoords;
				// tangent
				//Normalf Tangent;
				// bitangent
				//Normalf Bitangent;
			};

			struct TextureInfo {
				uint id;
				std::string type;
				std::string path;
			};

			/*  Functions  */
			// constructor
			Mesh(const std::vector<Vertex> & vertices, const std::vector<uint> & indices, const std::vector<TextureInfo> & textureInfos);

			// render the mesh
			void Draw(const Shader & shader, const std::string & materialPrefix = "");

		private:
			/*  Mesh Data  */
			std::vector<Vertex> vertices;
			std::vector<uint> indices;
			std::vector<TextureInfo> textureInfos;
			uint VAO;

			/*  Render data  */
			uint VBO, EBO;

			/*  Functions    */
			// initializes all the buffer objects/arrays
			void SetupMesh();
		};
	}
}

#endif// !_MESH_H_
