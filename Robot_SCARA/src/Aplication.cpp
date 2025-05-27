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
	if (model->meshes.empty()) {
		cout << "Blad: model nie awiera zadmych meshow" << endl;
	}
	else {
		cout << "Model za³¹dowany pomyœlnie, liczba meshow: " << model->meshes.size() << endl;
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
	glfwSetWindowUserPointer(window, this);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
}
float cameraSpeed = 0.005f;

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

	//cout << offsetX << " " << offsetY << endl;

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


	
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
		cameraPos= glm::vec3(3.0f, 2.0f, 3.0f);
		cameraFront = glm::vec3(3.0f, 2.0f, 3.0f);
		cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
		cameraSpeed = 0.005f;
	
	}
/*	
	if (cameraPos.x < 1.5 && cameraPos.z>4.3) {
		cameraPos.x = 1.5;
		cameraPos.z = 4.3;
	}

	if (cameraPos.x > 4.5 && cameraPos.z<1.3) {
		cameraPos.x = 4.5;
		cameraPos.z = 1.3;
	}

	if (cameraPos.x < 1.3 && cameraPos.z<1.4) {
		cameraPos.x = 1.3;
		cameraPos.z = 1.4;
	}
*/ ///tutaj bedzie ograniczenie na poruszanie po planszy

	

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	//klikniecie prawego przycisku myszki robot powraca na ekran




	static bool mKeyPressed = false;
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS && !mKeyPressed) {
		controlMode = !controlMode;
		activeArm = 0;
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

		if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) activeArm = 1;
		if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) activeArm = 2;
		if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) activeArm = 3;

		float rotationSpeed = 45.0f * deltaTime;

		if (activeArm > 0) {
			if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
				if (activeArm == 1) arm1Angle -= rotationSpeed;
				else if (activeArm == 2) arm2Angle -= rotationSpeed;
				else if (activeArm == 3) arm3Angle -= rotationSpeed;
			}
			if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
				if (activeArm == 1) arm1Angle += rotationSpeed;
				else if (activeArm == 2) arm2Angle += rotationSpeed;
				else if (activeArm == 3) arm3Angle += rotationSpeed;
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
			cameraPos-cameraFront, //tutaj mozna kierunek chodzenia kamery wziac ten wektor
			cameraUp
		);
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

		Mesh* podstawa = model->getMesh("Podstawa");
		Mesh* ramie1 = model->getMesh("Ramie1");
		Mesh* ramie2 = model->getMesh("Ramie2");
		Mesh* ramie3 = model->getMesh("Ramie3");

		glm::mat4 modelMat = glm::mat4(1.0f);

		if (podstawa) {
			shader->setMat4("mvp", projection * view * modelMat);
			podstawa->Draw();
		}

		if (ramie1) {
			glm::mat4 arm1Mat = modelMat;
			arm1Mat = glm::translate(arm1Mat, glm::vec3(0.0f, 0.0f, 0.0f)); // Pozycja względem podstawy
			arm1Mat = glm::rotate(arm1Mat, glm::radians(arm1Angle), glm::vec3(0.0f, 1.0f, 0.0f)); // Obrót wokół osi Z
			shader->setMat4("mvp", projection * view * arm1Mat);
			ramie1->Draw();

			// Ramie2 jako dziecko Ramie1
			if (ramie2) {
				glm::mat4 arm2Mat = arm1Mat;
				arm2Mat = glm::translate(arm2Mat, glm::vec3(0.0f, 0.0f, 0.0f)); // Pozycja względem Ramie1
				arm2Mat = glm::rotate(arm2Mat, glm::radians(arm2Angle), glm::vec3(0.0f, 1.0f, 0.0f));
				shader->setMat4("mvp", projection * view * arm2Mat);
				ramie2->Draw();

				// Ramie3 jako dziecko Ramie2
				if (ramie3) {
					glm::mat4 arm3Mat = arm2Mat;
					arm3Mat = glm::translate(arm3Mat, glm::vec3(0.0f, 0.0f, 0.0f)); // Pozycja względem Ramie2
					arm3Mat = glm::rotate(arm3Mat, glm::radians(arm3Angle), glm::vec3(0.0f, 1.0f, 0.0f));
					shader->setMat4("mvp", projection * view * arm3Mat);
					ramie3->Draw();
				}
			}
		}

		
		//glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraPos + cameraUp);
		//shader->setMat4("view", view);
		//cout << cameraPos.x << " " << cameraPos.y << " " << cameraPos.z << endl;


		//glm::mat4 modelMat = glm::rotate(glm::mat4(1.0f), glm::radians(rotationY), glm::vec3(0, 1, 0));
		//modelMat = glm::scale(modelMat, glm::vec3(0.1f));//skalowanie modelu 
		
		//glm::mat4 mvp = projection * view * modelMat;
		//shader->setMat4("mvp", mvp);

		


		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}
