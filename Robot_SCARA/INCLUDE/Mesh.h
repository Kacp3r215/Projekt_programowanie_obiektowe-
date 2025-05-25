#pragma once

#include <glad/glad.h>
#include <glm.hpp>
#include <vector>
#include <string>

using namespace std;

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
};

class Mesh {
public:
	string name;
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	unsigned int VAO;

	Mesh(const string& name, vector<Vertex> vertices, vector<unsigned int> indices);
	void Draw() const;

private:
	unsigned int VBO, EBO;
	void setupMesh();

};