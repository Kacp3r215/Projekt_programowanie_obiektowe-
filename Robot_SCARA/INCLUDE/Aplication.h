#pragma once


#include <string>
#include <memory>
#include "Model.h"
#include "Shader.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

using namespace std;


//deklaracja klasy 
class Aplication {
public: 
	Aplication(int width=800, int height=600, const string& title="SYMULACJA SCARA");
	~Aplication(); // dekonstruktor
	void run(); // g³ówna pêtla aplikacji 
	

private: 
	GLFWwindow* window; 
	unique_ptr<Model> model;
	unique_ptr<Shader> shader;
	glm::mat4 projection;

	bool controlMode = false;
	float rotationY = 0.0f;

	void initGLFW();
	void CreateWindow(int width, int height, const string& title);
	void processInput();
	

};