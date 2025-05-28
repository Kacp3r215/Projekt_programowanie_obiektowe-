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
	unique_ptr<Model> model2;
	unique_ptr<Model> model3;
	//unique_ptr<Model> model4;

	unique_ptr<Shader> shader;
	glm::mat4 projection;
glm::vec3 cameraPos   = glm::vec3(3.0f, 2.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(3.0f, 2.0f,3.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);

	bool controlMode = false;
	bool mode1 = true;
	bool mode2 = false;
	bool mode3 = false;

	float rotationY = 0.0f;
	float rotationY1 = 0.0f;


	void initGLFW();
	void CreateWindow(int width, int height, const string& title);
	void processInput();
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	

};
