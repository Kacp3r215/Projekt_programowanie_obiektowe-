#pragma once

#include <string>
#include <glad/glad.h>
#include <glm.hpp>

using namespace std;

class Shader {
public:
	unsigned int ID;

	Shader(const char* vertexPath, const char* fragmentPath);
	void use() const;
	void setVec3(const string& name, const glm::vec3 &value) const;
	void setMat4(const string& name, const glm::mat4& mat) const;
	

};
