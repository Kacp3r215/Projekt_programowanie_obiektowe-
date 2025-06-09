#pragma once

#include <string>
#include <memory>
#include <vector>
#include <tuple>
#include "Model.h"
#include "Shader.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include "Primitive.h"

using namespace std;

//tryb uczenia
enum class PlaybackMode {
	NORMAL,
	RECORDING,
	PLAYBACK
};


//deklaracja klasy 
class Aplication {
public:
	Aplication(int width = 800, int height = 600, const string& title = "SYMULACJA SCARA");
	~Aplication(); // dekonstruktor
	void run(); // glówna pêtla aplikacji 


private:
	GLFWwindow* window;
	unique_ptr<Model> Base;
	unique_ptr<Model> Arm1;
	unique_ptr<Model> Arm2;
	unique_ptr<Model> Arm3;
	unique_ptr<Model> prymityw;
	vector<unique_ptr<Primitive>> prymitywy;
	vector<bool>pri;

	unique_ptr<Shader> shader;
	unique_ptr<Shader> shader1;

	unique_ptr<Mesh> groundGrid;

	glm::mat4 projection;
	glm::vec3 cameraPos = glm::vec3(15.0f, 15.0f, 18.0f);
	glm::vec3 cameraFront = glm::vec3(3.0f, 3.0f, 3.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	bool controlMode = false;
	bool mode1 = false;
	bool mode2 = false;
	bool mode3 = false;



	float rotationY = 0.0f, rotationY_N = 0.0f;
	float rotationY1 = 0.0f, rotationY1_N = 0.0f;
	float rotationZ = 0.0f, rotationZ_N = 0.0f;
	bool Y_N=false, Y1_N=false, Z_N=false;


	glm::vec3 lightDir = glm::normalize(glm::vec3(-1.0f, -1.0f, -0.5f)); // kierunek padania światła
	glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 0.95f); // kolor światła

	// ZMIENNE TRYBU UCZENIA
	PlaybackMode playbackMode = PlaybackMode::NORMAL;
	vector <tuple<float, float, float, bool, bool>> movementSequence;
	float playbackSpeed = 1.0f;
	size_t currentPlaybackIndex = 0;
	float playbackTimer = 2.0f;
	bool prim = false;
	bool first = false;

	glm::mat4 modelMat51 = glm::mat4(1.0f);

	//Tryb automatycznego pozycjonowania 
	bool positioningMode = false;
	glm::vec3 targetPosition = glm::vec3(0.0f);
	glm::vec3 inputCoords = glm::vec3(0.0f);
	glm::vec3 targetAngles = glm::vec3(0.0f);

	bool isMovingToTarget = false;


	void initGLFW();
	void CreateWindow(int width, int height, const string& title);
	bool processInput();
	void updatePlayback(float deltaTime);
	//Tryb pozycjonowania:
	void calculateInverseKinematics(const glm::vec3& target, float& outY, float& outY1, float& outZ);
	void setPositioningMode(bool enable);
	void updatePositioning();
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);

};
