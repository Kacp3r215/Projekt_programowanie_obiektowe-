#include "Mesh.h"

using namespace std;

Mesh::Mesh(const string& name, vector<Vertex> vertices, vector<unsigned int> indices)
	: name(name),  vertices(vertices), indices(indices) {
	setupMesh();
}

void Mesh::setupMesh() {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);


	//pozycja
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	//normalne 
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

	//UV
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

	glBindVertexArray(0);
}

void Mesh::Draw() const {
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

Mesh Mesh::CreateGrid(const std::string& name, float size, int divisions) {
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	float step = size / divisions;
	float halfSize = size * 0.5f;

	// Linie równoleg³e do X (poziome)
	for (int i = 0; i <= divisions; ++i) {
		float z = -halfSize + i * step;
		vertices.push_back({ glm::vec3(-halfSize, 0.0f, z), glm::vec3(0, 1, 0), glm::vec2(0) });
		vertices.push_back({ glm::vec3(halfSize, 0.0f, z), glm::vec3(0, 1, 0), glm::vec2(1) });
		indices.push_back(i * 2);
		indices.push_back(i * 2 + 1);
	}

	// Linie równoleg³e do Z (pionowe)
	for (int i = 0; i <= divisions; ++i) {
		float x = -halfSize + i * step;
		vertices.push_back({ glm::vec3(x, 0.0f, -halfSize), glm::vec3(0, 1, 0), glm::vec2(0) });
		vertices.push_back({ glm::vec3(x, 0.0f, halfSize), glm::vec3(0, 1, 0), glm::vec2(1) });
		indices.push_back((divisions + 1 + i) * 2);
		indices.push_back((divisions + 1 + i) * 2 + 1);
	}

	return Mesh(name, vertices, indices);
}