#ifndef _OPENGL_MESH_MESH_H_
#define _OPENGL_MESH_MESH_H_

typedef unsigned int uint;

#include <glm/glm.hpp>

#include <string>
#include <vector>

namespace CppUtil {
	namespace OpenGL {
		class Shader;

		class Mesh {
		public:
			struct Vertex {
				// position
				glm::vec3 Position;
				// normal
				glm::vec3 Normal;
				// texCoords
				glm::vec2 TexCoords;
				// tangent
				//glm::vec3 Tangent;
				// bitangent
				//glm::vec3 Bitangent;
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
