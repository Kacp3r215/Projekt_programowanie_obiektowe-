#include "Shader.h"
#include <fstream>
#include <sstream>
#include <iostream>
 
using namespace std;

Shader::Shader(const char* vertexPath, const char* fragmentPath) {
	string vertexCode, fragmentCode;
	ifstream vShaderFile(vertexPath), fShaderFile(fragmentPath);
	stringstream vStream, fStream;

	vStream << vShaderFile.rdbuf();
	fStream << fShaderFile.rdbuf();
	vertexCode = vStream.str();
	fragmentCode = fStream.str();

	const char* vCode = vertexCode.c_str();
	const char* fCode = fragmentCode.c_str();

	unsigned int vertex, fragment;
	int success;
	char infoLog[512];


	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vCode, NULL);
	glCompileShader(vertex);
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		cout << "Vertex shader error" << infoLog << endl;
	}



	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fCode, NULL);
	glCompileShader(fragment);
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		cout << "Vertex shader error" << infoLog << endl;
	}



	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);

	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		cout << "Shader program error" << infoLog << endl;
	}

	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void Shader::use() const {
	glUseProgram(ID);
}

void Shader::setMat4(const string& name, const glm::mat4& mat) const {
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}