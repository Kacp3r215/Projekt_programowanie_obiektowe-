#include <glad/glad.h>
#include "Aplication.h"
#include <iostream>
#include <glm.hpp>

/* STEROWANIE ROBOTEM
* M - przełacza tryb sterowania
 * strzałki - obrót robota
 * W,A,S,D - poruszanie kamerą
 * lewy przycisk myszy - zmiana kierunku kamery
 * prawy przycisk myszy - reset kamery
 * scroll - zoom kamery
 * ESC - zamknięcie okna
 */


	
using namespace std;

//definicja kontruktora 
Aplication::Aplication(int width, int height, const string& title) {
	initGLFW();

	GLFWmonitor* primary = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(primary);
	int screenWidth = mode->width;
	int screenHeight = mode->height;

	CreateWindow(screenWidth, screenHeight, title);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	glfwSetTime(0.0);

	glEnable(GL_DEPTH_TEST);

	projection = glm::perspective(glm::radians(45.0f), float(width) / float(height), 0.1f, 100.0f);


	model = make_unique<Model>("..//Robot_SCARA//assets//model_SCAR.obj");
	model2 = make_unique<Model>("..//Robot_SCARA//assets//model_SCAR2.obj");
	model3 = make_unique<Model>("..//Robot_SCARA//assets//model_SCAR2.obj");
	model4 = make_unique<Model>("..//Robot_SCARA//assets//model_SCAR3.obj");


	if (model->meshes.empty()) {
		cout << "Blad: model nie awiera zadmych meshow" << endl;
	}
	else {
		cout << "Model za³¹dowany pomyœlnie, liczba meshow: " << model->meshes.size() << endl;
	}
	
	
	shader = make_unique<Shader>("..//Robot_SCARA//assets//vertex.glsl", "..//Robot_SCARA//assets//fragment.glsl");
	shader1 = make_unique<Shader>("..//Robot_SCARA//assets//vertex.glsl", "..//Robot_SCARA//assets//fragment2.glsl");

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
	glfwSetWindowUserPointer(window, this);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
}
float cameraSpeed = 0.01f;


//definicja obslugi scrolla

void Aplication::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	Aplication* app = static_cast<Aplication*>(glfwGetWindowUserPointer(window));
	if (!app) return;
	if (yoffset > 0) {
		app->cameraPos /= 1.1f;
		cameraSpeed *= (9.0/10.0);
	}
	else if (yoffset < 0) {
		app->cameraPos *= 1.1f;
		cameraSpeed *= (10.0/9.0);
	}
}

double lastX = 0, lastY = 400;
double offsetX = 0.0f, offsetY = 0.0f;
bool firstMouse = true;



void Aplication::mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	Aplication* app = static_cast<Aplication*>(glfwGetWindowUserPointer(window));

	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	offsetX = xpos - lastX;
	offsetY = lastY - ypos; 
	lastX = xpos;
	lastY = ypos;


	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		if (offsetX < 0) app->cameraFront.x += offsetX * 0.01;
		if (offsetX > 0) app->cameraFront.x += offsetX * 0.01;
		if (offsetY < 0) app->cameraFront.y += offsetY * 0.01;
		if (offsetY >0) app->cameraFront.y += offsetY * 0.01;
	}

}
void Aplication::processInput() {



	//sterowanie kamerą W,A,S,D
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		cameraPos.z += cameraSpeed;

		cameraPos.x += cameraSpeed;

	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {


		cameraPos.z -= cameraSpeed;
		cameraPos.x -= cameraSpeed;

	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {


		cameraPos.x += cameraSpeed;
		cameraPos.z -= cameraSpeed;

	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {

		cameraPos.z += cameraSpeed;
		cameraPos.x -= cameraSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {

		cameraPos.y -= cameraSpeed;
		
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {

		cameraPos.y += cameraSpeed;
	
	}


	//reset na prawy przycisk
	
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
		cameraPos= glm::vec3(3.0f, 2.0f, 3.0f);
		cameraFront = glm::vec3(3.0f, 2.0f, 3.0f);
		cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
		cameraSpeed = 0.005f;
	
	}


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
		float rotationSpeed1 = 5.0f * deltaTime;


		if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
			mode1 = true;
			mode2 = false;
			mode3 = false;
		}
		if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
			mode1 = false;
			mode2 = true;
			mode3 = false;
		}
		if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
			mode1 = false;
			mode2 = false;
			mode3 = true;
		}
			
		
		
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
			if (mode1)rotationY += rotationSpeed;
			if (mode2) rotationY1 += rotationSpeed;


			if (mode3) {
				rotationZ -= rotationSpeed1;

				if (rotationZ < -3)rotationZ = -3;
			}
		}
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
			if (mode1)rotationY -= rotationSpeed;
			if (mode2) rotationY1 -= rotationSpeed;

			if (mode3) {

				rotationZ += rotationSpeed1;
				if (rotationZ > 0)rotationZ = 0;

			}


			
		}
	}

}


void Aplication::run() {
	while (!glfwWindowShouldClose(window)) {
		
		processInput();

		glfwPollEvents();

		//kolor t³a
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader->use();

		glm::mat4 view = glm::lookAt(
			cameraPos,
			cameraPos-cameraFront, 
			cameraUp
		);

		shader->setMat4("view", view);
		shader->setVec3("lightDir", lightDir);
		shader->setVec3("lightColor", lightColor);


		shader1->use();

		glm::mat4 view1 = glm::lookAt(
			cameraPos,
			cameraPos - cameraFront, 
			cameraUp
		);
		



		shader1->setMat4("view", view1);
		shader1->setVec3("lightDir", lightDir);
		shader1->setVec3("lightColor", lightColor);



		// --- Rysowanie pierwszego modelu  ---
		glm::vec3 pivot = glm::vec3(-1.5f, 0.25f, 0.0f);
		glm::mat4 modelMat1 = glm::mat4(1.0f);
		modelMat1 = glm::scale(modelMat1, glm::vec3(0.1f));
		glm::mat4 mvp1 = projection * view * modelMat1;
		shader->use();
		shader->setMat4("mvp", mvp1);
		shader->setMat4("model", modelMat1);
		model->Draw();





		// --- Rysowanie drugiego modelu  ---
		glm::mat4 modelMat2 = glm::mat4(1.0f);
		modelMat2 = glm::translate(modelMat2, pivot); //przesunięcie do punktu obrotu
		modelMat2 = glm::rotate(modelMat2, glm::radians(rotationY), glm::vec3(0, 1, 0)); //obrót wokół osi Y
		modelMat2 = glm::translate(modelMat2, -pivot); //przesunięcie w górę
		modelMat2 = glm::translate(modelMat2, glm::vec3(-3.0f, 0.5f, 0.0f)); //przesuniecie do koncowki ramienia
		glm::mat4 mvp2 = projection * view * modelMat1 * modelMat2;

		if (mode1) {
			shader1->use();
			shader1->setMat4("mvp", mvp2);
			shader1->setMat4("model", modelMat2);

		}
		
		else {
			shader->use();
			shader->setMat4("mvp", mvp2);
			shader->setMat4("model", modelMat2);
		}
		
		
		model2->Draw();

		// --- Rysowanie trzeciego modelu  ---

		glm::mat4 modelMat3 = glm::mat4(1.0f);
		modelMat3 = glm::translate(modelMat3, pivot); //przesunięcie do punktu obrotu
		modelMat3 = glm::rotate(modelMat3, glm::radians(rotationY1), glm::vec3(0, 1, 0)); //obrót wokół osi Y
		modelMat3 = glm::translate(modelMat3, -pivot); //przesunięcie w górę
		modelMat3 = glm::translate(modelMat3, glm::vec3(-3.1f, 0.7f, 0.0f)); //przesuniecie do koncowki ramienia
		glm::mat4 mvp3 = projection * view * modelMat1* modelMat2 * modelMat3;
		if (mode2) {
			shader1->use();
		shader1->setMat4("mvp", mvp3);
		shader1->setMat4("model", modelMat3);
		}
		else {
			shader->use();
			shader->setMat4("mvp", mvp3);
			shader->setMat4("model", modelMat3);
		}
		model3->Draw();


		// --- Rysowanie czwartego modelu  ---
		glm::mat4 modelMat4 = glm::mat4(1.0f);
		modelMat4 = glm::translate(modelMat4, glm::vec3(-1.4f, 0.2f+rotationZ, 0.0f));
		modelMat4 = glm::scale(modelMat4, glm::vec3(0.7f));
		glm::mat4 mvp4 = projection * view * modelMat1 * modelMat2 * modelMat3 * modelMat4;
		


		if (mode3) {
			shader1->use();
			shader1->setMat4("mvp", mvp4);
			shader1->setMat4("model", modelMat4);
		}
		else {
			shader->use();
			shader->setMat4("mvp", mvp4);
			shader->setMat4("model", modelMat4);
		}
		model4->Draw();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}
