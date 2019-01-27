#include <CppUtil/OpenGL/Mesh.h>

#include <glad/glad.h> // holds all OpenGL type declarations

#include <CppUtil/OpenGL/Shader.h>

using namespace CppUtil::OpenGL;

using namespace std;

// constructor
Mesh::Mesh(const vector<Vertex> & vertices, const vector<size_t> & indices, const vector<TextureInfo> & textureInfos)
{
	this->vertices = vertices;
	this->indices = indices;
	this->textureInfos = textureInfos;

	// now that we have all the required data, set the vertex buffers and its attribute pointers.
	SetupMesh();
}

// initializes all the buffer objects/arrays
void Mesh::SetupMesh()
{
	// create buffers/arrays
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	// load data into vertex buffers
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// A great thing about structs is that their memory layout is sequential for all its items.
	// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
	// again translates to 3/2 floats which translates to a byte array.
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(size_t), &indices[0], GL_STATIC_DRAW);

	// set the vertex attribute pointers
	// vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	// vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
	// vertex tangent
	//glEnableVertexAttribArray(3);
	//glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
	// vertex bitangent
	//glEnableVertexAttribArray(4);
	//glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

	glBindVertexArray(0);
}

// render the mesh
void Mesh::Draw(const Shader & shader, const string & materialPrefix)
{
	// bind appropriate textures
	size_t diffuseNr = 1;
	size_t specularNr = 1;
	size_t normalNr = 1;
	size_t heightNr = 1;
	for (size_t i = 0; i < textureInfos.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
		// retrieve texture number (the N in diffuse_textureN)
		string number;
		string name = textureInfos[i].type;
		if (name == "texture_diffuse")
			number = std::to_string(diffuseNr++);
		else if (name == "texture_specular")
			number = std::to_string(specularNr++); // transfer size_t to stream
		else if (name == "texture_normal")
			number = std::to_string(normalNr++); // transfer size_t to stream
		else if (name == "texture_height")
			number = std::to_string(heightNr++); // transfer size_t to stream

												 // now set the sampler to the correct texture unit
		shader.SetInt(materialPrefix + name + number, i);
		// and finally bind the texture
		glBindTexture(GL_TEXTURE_2D, textureInfos[i].id);
	}

	// draw mesh
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	// always good practice to set everything back to defaults once configured.
	glActiveTexture(GL_TEXTURE0);
}