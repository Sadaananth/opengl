#pragma once

#include <vector>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../inc/Shader.hpp"

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};

struct Texture {
	unsigned int id;
	std::string type;
	std::string path;
};


class Mesh {
public:
	// mesh data
	std::vector<Vertex> mVerticesList;
	std::vector<unsigned int> mIndicesList;
	std::vector<Texture> mTexturesList;
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
		std::vector<Texture> textures);
	void Draw(Shader& shader);
private:
	// render data
	unsigned int VAO, VBO, EBO;
	void setupMesh();
};
