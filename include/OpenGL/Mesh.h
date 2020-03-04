#ifndef _OPENGL_MESH_MESH_H_
#define _OPENGL_MESH_MESH_H_

typedef unsigned int unsigned;

#include <UGM/point.h>
#include <UGM/Normal.h>
#include <UGM/vec.h>

#include <string>
#include <vector>

namespace CppUtil {
	namespace OpenGL {
		class Shader;

		class Mesh {
		public:
			struct Vertex {
				// position
				Ubpa::pointf3 Position;
				// normal
				Normalf Normal;
				// texCoords
				Ubpa::vecf2 TexCoords;
				// tangent
				//Normalf Tangent;
				// bitangent
				//Normalf Bitangent;
			};

			struct TextureInfo {
				unsigned id;
				std::string type;
				std::string path;
			};

			/*  Functions  */
			// constructor
			Mesh(const std::vector<Vertex> & vertices, const std::vector<unsigned> & indices, const std::vector<TextureInfo> & textureInfos);

			// render the mesh
			void Draw(const Shader & shader, const std::string & materialPrefix = "");

		private:
			/*  Mesh Data  */
			std::vector<Vertex> vertices;
			std::vector<unsigned> indices;
			std::vector<TextureInfo> textureInfos;
			unsigned VAO;

			/*  Render data  */
			unsigned VBO, EBO;

			/*  Functions    */
			// initializes all the buffer objects/arrays
			void SetupMesh();
		};
	}
}

#endif// !_MESH_H_
