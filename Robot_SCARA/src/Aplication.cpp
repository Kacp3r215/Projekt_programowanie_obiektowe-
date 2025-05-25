#include <glad/glad.h>
#include "Aplication.h"
#include <iostream>
#include <glm.hpp>



	
using namespace std;

//definicja kontruktora 
Aplication::Aplication(int width, int height, const string& title) {
	initGLFW();
	CreateWindow(width, height, title);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	glfwSetTime(0.0);

	glEnable(GL_DEPTH_TEST);

	projection = glm::perspective(glm::radians(45.0f), float(width) / float(height), 0.1f, 100.0f);


	model = make_unique<Model>("..//Robot_SCARA//assets//model_SCAR.obj");
	if (model->meshes.empty()) {
		cout << "Blad: model nie awiera zadmych meshow" << endl;
	}
	else {
		cout << "Model za��dowany pomy�lnie, liczba meshow: " << model->meshes.size() << endl;
	}
	
	
	shader = make_unique<Shader>("..//Robot_SCARA//assets//vertex.glsl", "..//Robot_SCARA//assets//fragment.glsl");

}

//definicja destruktora
Aplication::~Aplication() {
	glfwDestroyWindow(window);
	glfwTerminate();
}

void Aplication::initGLFW() {
	if (!glfwInit()) {
		cout << "Nie udalo sie zainicjalizowac GLFW" << endl;
	}
}

void Aplication::CreateWindow(int width, int height, const string& title) {
	window = glfwCreateWindow(width, height, title.c_str(),NULL,NULL);
	if (!window) {
		cout << "Nie udalo sie stworzyc okienka" << endl;
		glfwTerminate();

	}
	glfwMakeContextCurrent(window);
}

void Aplication::processInput() {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}


	static bool mKeyPressed = false;
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS && !mKeyPressed) {
		controlMode = !controlMode;
		mKeyPressed = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_M) == GLFW_RELEASE) {
		mKeyPressed = false;
	}

	if (controlMode) {
		static float lastFrame = 0.0f;
		float currentFrame = glfwGetTime();
		float deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		float rotationSpeed = 45.0f * deltaTime;

		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
			rotationY += rotationSpeed;
		}
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
			rotationY -= rotationSpeed;
		}
	}

}


void Aplication::run() {
	while (!glfwWindowShouldClose(window)) {
		
		processInput();

		glfwPollEvents();

		//kolor t�a
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader->use();

		glm::mat4 view = glm::lookAt(
			glm::vec3(3.0f, 2.0f, 3.0f),
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f)
		);
		
		


		glm::mat4 modelMat = glm::rotate(glm::mat4(1.0f), glm::radians(rotationY), glm::vec3(0, 1, 0));
		modelMat = glm::scale(modelMat, glm::vec3(0.1f));//skalowanie modelu 
		
		glm::mat4 mvp = projection * view * modelMat;
		shader->setMat4("mvp", mvp);

		model->Draw();


		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}