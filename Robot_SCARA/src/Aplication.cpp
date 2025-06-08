#include <glad/glad.h>
#include <glm.hpp>
#include <iostream>
#include "Aplication.h"
#include <iostream>
#include <sstream>

/* STEROWANIE ROBOTEM
 * M - przełacza tryb sterowania
 * strzałki - obrót robota
 * W,A,S,D - poruszanie kamerą
 * lewy przycisk myszy - zmiana kierunku kamery
 * prawy przycisk myszy - reset kamery
 * scroll - zoom kamery
 * ESC - zamknięcie okna
 * OBSŁUGA TRYBU UCZENIA
 * KLIKNAC M NASTEPNIE L WYKONAC JAKIES RUCHY PRZY POMOCY 1 2 3 I STRZALEK
 * ZNOWU KLIKNAC L ZEBY ZAKONCZYC NAGRYWANIE, NACISNAC K ZEBY URUCHOMIC ODTWARZANIE
 */

using namespace std;

glm::vec3 globalMin1, globalMax2, globalMin3, globalMax4;
// definicja kontruktora
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

    Base = make_unique<Model>("..//Robot_SCARA//assets//Podstawa.obj");
    Arm1 = make_unique<Model>("..//Robot_SCARA//assets//Ramie1.obj");
    Arm2 = make_unique<Model>("..//Robot_SCARA//assets//Ramie2.obj");
    Arm3 = make_unique<Model>("..//Robot_SCARA//assets//Ramie3.obj");
    prymityw = make_unique<Model>("..//Robot_SCARA//assets//prymityw.obj");

    prymitywy.push_back(make_unique<Primitive>("..//Robot_SCARA//assets//prymityw.obj", glm::vec3(4.0f, 0.0f, 3.0f)));
    pri.push_back(false);

    // informacja gdzie lezy najmniejszy i najwiekszy wierzcholek
    Model* model = Arm3.get();
    glm::vec3 globalMin(FLT_MAX), globalMax(-FLT_MAX);

    for (const Mesh& mesh : model->meshes) {
        for (const Vertex& v : mesh.vertices) {
            globalMin = glm::min(globalMin, v.position);
            globalMax = glm::max(globalMax, v.position);
        }
    }
    globalMin1 = globalMin;
    globalMax2 = globalMax;

    Model* model1 = prymityw.get();
    glm::vec3 globalMinx(FLT_MAX), globalMaxx(-FLT_MAX);

    for (const Mesh& mesh : model1->meshes) {
        for (const Vertex& v : mesh.vertices) {
            globalMinx = glm::min(globalMinx, v.position);
            globalMaxx = glm::max(globalMaxx, v.position);
        }
    }
    globalMin3 = globalMinx;
    globalMax4 = globalMaxx;

    if (Base->meshes.empty()) {
        cout << "Blad: model nie awiera zadmych meshow" << endl;
    }
    else {
        cout << "Model za³¹dowany pomyœlnie, liczba meshow: " << Base->meshes.size() << endl;
    }

    shader = make_unique<Shader>("..//Robot_SCARA//assets//vertex.glsl", "..//Robot_SCARA//assets//fragment.glsl");
    shader1 = make_unique<Shader>("..//Robot_SCARA//assets//vertex.glsl", "..//Robot_SCARA//assets//fragment2.glsl");

    groundGrid = make_unique<Mesh>(Mesh::CreateGrid("GroundGrid", 20.0f, 20));

    // modelMat51 = glm::mat4(1.0f);
    // modelMat51 = glm::translate(modelMat51, glm::vec3(4.0f, 0.0f, 3.0f));
}

// definicja destruktora
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
    window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
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

// definicja obslugi scrolla

void Aplication::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    Aplication* app = static_cast<Aplication*>(glfwGetWindowUserPointer(window));
    if (!app) return;
    if (yoffset > 0) {
        app->cameraPos /= 1.1f;
        cameraSpeed *= (9.0 / 10.0);
    }
    else if (yoffset < 0) {
        app->cameraPos *= 1.1f;
        cameraSpeed *= (10.0 / 9.0);
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
        if (offsetY > 0) app->cameraFront.y += offsetY * 0.01;
    }
}

bool playback_c = false, playback_space = false;
void Aplication::updatePlayback(float deltaTime) {
    if (playbackMode == PlaybackMode::RECORDING) {
        bool key_c = false;
        bool space = false;

        if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) key_c = true;
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) space = true;

        movementSequence.emplace_back(rotationY, rotationY1, rotationZ, key_c, space);
    }
    else if (playbackMode == PlaybackMode::PLAYBACK && !movementSequence.empty()) {
        // Tymczasowo wyłącz controlMode podczas odtwarzania
        bool wasControlMode = controlMode;
        controlMode = false;

        playbackTimer += deltaTime * playbackSpeed * 5.0f;

        // Bez interpolacji - dokładne odtwarzanie
        auto& frame = movementSequence[currentPlaybackIndex];
        rotationY = std::get<0>(frame);
        rotationY1 = std::get<1>(frame);
        rotationZ = std::get<2>(frame);
        playback_c = get<3>(frame);
        playback_space = get<4>(frame);

        playbackTimer += deltaTime;
        if (playbackTimer >= (1.0f / 120.0f)) {  // 60 FPS
            playbackTimer = 0.0f;
            currentPlaybackIndex = (currentPlaybackIndex + 1) % movementSequence.size();
            if (currentPlaybackIndex == 0) {
                // modelMat51 = glm::mat4(1.0f);
                // modelMat51 = glm::translate(modelMat51, glm::vec3(4.0f, 0.0f, 3.0f));
                prim = false;
                first = false;
            }

            // Dodajemy debugowy output
            std::cout << "Playing frame " << currentPlaybackIndex << ": Y=" << rotationY << ", Y1=" << rotationY1 << ", Z=" << rotationZ << std::endl;
        }

        controlMode = wasControlMode;
    }
}

void Aplication::calculateInverseKinematics(const glm::vec3& target, float& outY, float& outY1, float& outZ) {
    const float L1 = 4.9f;  // Długość pierwszego ramienia
    const float L2 = 4.1f;  // Długość drugiego ramienia
    const float Z_MIN = -1.9f;
    const float Z_MAX = -0.2f;

    // Konwersja współrzędnych do układu robota
    float x = target.x;
    float y = target.z;  // Zamiana osi Z na Y w układzie robota
    float z = target.y;  // Wysokość

    // Oblicz odległość w płaszczyźnie XY
    float D = sqrt(x * x + y * y);

    // Sprawdź czy punkt jest osiągalny
    if (D > L1 + L2 || D < fabs(L1 - L2)) {
        std::cout << "Punkt poza zasięgiem robota!" << std::endl;
        return;
    }

    // Oblicz kąty
    float cosTheta2 = (D * D - L1 * L1 - L2 * L2) / (2 * L1 * L2);
    cosTheta2 = glm::clamp(cosTheta2, -1.0f, 1.0f);
    float theta2 = acos(cosTheta2);

    float theta1 = atan2(y, x) - atan2(L2 * sin(theta2), L1 + L2 * cos(theta2));

    // Konwersja na stopnie i ustawienie ograniczeń
    outY = glm::degrees(theta1);
    outY1 = glm::degrees(theta2) - 90.0f;  // Korekta dla układu robota

    // Normalizacja wysokości
    float z_norm = (z - 1.79f) / 2.0f;  // 1.79 to wysokość bazowa
    outZ = glm::mix(Z_MIN, Z_MAX, z_norm);

    // Ograniczenia ruchów
    outY = glm::clamp(outY, -130.0f, 130.0f);
    outY1 = glm::clamp(outY1, -130.0f, 130.0f);
    outZ = glm::clamp(outZ, Z_MIN, Z_MAX);
}

void Aplication::setPositioningMode(bool enable) {
    positioningMode = enable;
    if (enable) {
        cout << "Tryb pozycjonowania Włączony. Wprowadź współrzędne (x,y,z): ";
    }
    else {
        cout << "Tryb pozycjonowania wyłączony." << endl;
    }
}

//float Aplication::moveTowards(float current, float target, float maxDelta) {
   // if (fabs(target - current) <= maxDelta) {
  //      return target;
  //  }
 //   return current + ((target > current) ? maxDelta : -maxDelta);
//}

/*void Aplication::updateAnimation(float deltaTime) {
    if (!isAnimating) return;

    // Stała szybkości animacji (stopnie na sekundę)
    const float ROTATION_SPEED = 90.0f;
    const float Z_SPEED = 1.0f;

    // Oblicz maksymalną zmianę na podstawie czasu
    float maxRotationChange = ROTATION_SPEED * deltaTime;
    float maxZChange = Z_SPEED * deltaTime;

    // Interpoluj każdy kąt osobno
    rotationY = moveTowards(rotationY, targetAngles.x, maxRotationChange);
    rotationY1 = moveTowards(rotationY1, targetAngles.y, maxRotationChange);
    rotationZ = moveTowards(rotationZ, targetAngles.z, maxZChange);

    // Sprawdź czy osiągnięto cel z większą tolerancją
    float epsilon = 0.5f; // Zwiększona tolerancja
    if (fabs(rotationY - targetAngles.x) < epsilon &&
        fabs(rotationY1 - targetAngles.y) < epsilon &&
        fabs(rotationZ - targetAngles.z) < epsilon) {
        isAnimating = false;
        // Dokładne ustawienie na cel
        rotationY = targetAngles.x;
        rotationY1 = targetAngles.y;
        rotationZ = targetAngles.z;
    }
}
*/
void Aplication::updatePositioning() {
    if (!positioningMode) return;

    cout << "Wprowadz wspolrzedne (x y z), oddzielone spacjami: ";
    string input;
    getline(std::cin, input);

    std::istringstream iss(input);
    float x, y, z;
    if (iss >> x >> y >> z) {
        inputCoords = glm::vec3(x, y, z);

        // Sprawdź czy punkt jest w zasięgu robota
        float distance = glm::length(glm::vec2(x, z));
        if (distance > 9.0f || distance < 1.0f || y < 0.0f || y > 3.0f) {
            std::cout << "Punkt poza zakresem robota!" << std::endl;
            positioningMode = false;
            return;
        }

        std::cout << "Obliczam kinematykę odwrotną dla punktu: ("
            << x << ", " << y << ", " << z << ")" << std::endl;

        // Bezpośrednie przypisanie kątów bez animacji
        calculateInverseKinematics(inputCoords, rotationY, rotationY1, rotationZ);

        std::cout << "Ustawiono kąty: Y=" << rotationY
            << ", Y1=" << rotationY1
            << ", Z=" << rotationZ << std::endl;
    }
    else {
        std::cout << "Nieprawidłowy format danych!" << std::endl;
    }

    positioningMode = false;
}

bool Aplication::processInput() {
    bool output = false;
    static bool lKeyPressed = false;
    static bool kKeyPressed = false;

    // sterowanie kamerą W,A,S,D
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

    bool static pKeyPressed = false;

    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS && !pKeyPressed) {
        prymitywy.push_back(make_unique<Primitive>("..//Robot_SCARA//assets//prymityw.obj", glm::vec3(4.0f, 0.0f, 3.0f)));
        pri.push_back(false);
        pKeyPressed = true;
    }
    else if (glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE) {
        pKeyPressed = false;
    }

    bool static oKeyPressed = false;

    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS && !oKeyPressed) {
        if (!prymitywy.empty() && !pri.empty()) {
            prymitywy.erase(prymitywy.begin());
            pri.erase(pri.begin());
        }
        output = true;
        oKeyPressed = true;
    }
    else if (glfwGetKey(window, GLFW_KEY_O) == GLFW_RELEASE) {
        oKeyPressed = false;
    }

    // reset na prawy przycisk

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        cameraPos = glm::vec3(15.0f, 15.0f, 18.0f);
        cameraFront = glm::vec3(3.0f, 3.0f, 3.0f);
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

    if (controlMode && playbackMode != PlaybackMode::PLAYBACK) {
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
            if (mode1) {
                rotationY += rotationSpeed;
                rotationY = glm::clamp(rotationY, -130.0f, 130.0f);
            }
            if (mode2) {
                rotationY1 += rotationSpeed;
                rotationY1 = glm::clamp(rotationY1, -130.0f, 130.0f);
            }

            if (mode3) {
                rotationZ -= rotationSpeed1;

                rotationZ = glm::clamp(rotationZ, -1.9f, -0.2f);
            }
        }

        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            if (mode1) {
                rotationY -= rotationSpeed;
                rotationY = glm::clamp(rotationY, -130.0f, 130.0f);
            }
            if (mode2) {
                rotationY1 -= rotationSpeed;
                rotationY1 = glm::clamp(rotationY1, -130.0f, 130.0f);
            }
            if (mode3) {
                rotationZ += rotationSpeed1;

                rotationZ = glm::clamp(rotationZ, -1.9f, -0.2f);
            }
        }
    }

    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS && !lKeyPressed) {
        if (playbackMode == PlaybackMode::NORMAL) {
            playbackMode = PlaybackMode::RECORDING;
            movementSequence.clear();
            cout << "Ropoczęto nagrywanie sekwencji" << endl;
        }
        else if (playbackMode == PlaybackMode::RECORDING) {
            playbackMode = PlaybackMode::NORMAL;
            cout << "Zakonczono nagrywanie. Sekwencja ma: " << movementSequence.size() << endl;
        }
        lKeyPressed = true;
    }
    else if (glfwGetKey(window, GLFW_KEY_L) == GLFW_RELEASE) {
        lKeyPressed = false;
    }

    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS && !kKeyPressed) {
        if (!movementSequence.empty()) {
            if (playbackMode == PlaybackMode::NORMAL) {
                playbackMode = PlaybackMode::PLAYBACK;
                currentPlaybackIndex = 0;
                playbackTimer = 0.0f;
                prim = false;
                first = false;

                // modelMat51 = glm::mat4(1.0f);
                // modelMat51 = glm::translate(modelMat51, glm::vec3(4.0f, 0.0f, 3.0f));

                std::cout << "Rozpoczęto odtwarzanie sekwencji. Liczba klatek: " << movementSequence.size() << std::endl;
            }
            else if (playbackMode == PlaybackMode::PLAYBACK) {
                playbackMode = PlaybackMode::NORMAL;
                std::cout << "Zatrzymano odtwarzanie" << std::endl;
            }
            kKeyPressed = true;
        }
        else {
            std::cout << "Brak sekwencji do odtworzenia!" << std::endl;
        }
    }
    else if (glfwGetKey(window, GLFW_KEY_K) == GLFW_RELEASE) {
        kKeyPressed = false;
    }

    // włączenie trybu pozycjonowania:
    static bool nKeyPressed = false;
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS && !nKeyPressed) {
        setPositioningMode(!positioningMode);
        if (positioningMode) {
            // Wymuszenie odczytu z konsoli
            glfwPollEvents(); // Opróżnij bufor zdarzeń
            updatePositioning();
        }
        nKeyPressed = true;
    }
    else if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {
        nKeyPressed = false;
    }

    return output;
}

void Aplication::run() {
    float lastFrame = 0.0f;
    static bool firstRun = true;
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        if (processInput()) continue;

        if (playbackMode != PlaybackMode::NORMAL) {
            updatePlayback(deltaTime);
        }
        if (positioningMode) {
            updatePositioning();
        }

       // if (isAnimating) {
       //     updateAnimation(deltaTime);
      //  }

        glfwPollEvents();

        // kolor t³a
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader->use();

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos - cameraFront, cameraUp);

        shader->setMat4("view", view);
        shader->setVec3("lightDir", lightDir);
        shader->setVec3("lightColor", lightColor);

        // --- Rysowanie podlogi ---

        shader1->use();

        glm::mat4 gridModel = glm::mat4(1.0f);
        gridModel = glm::translate(gridModel, glm::vec3(0.0f, -0.01f, 0.0f));  // Lekko poniżej 0, aby uniknąć "z-fighting"
        // gridModel = glm::scale(gridModel, glm::vec3(0.5f));
        shader->setMat4("mvp", projection * view * gridModel);
        shader->setMat4("model", gridModel);
        shader->setVec3("lightColor", glm::vec3(0.2f, 0.2f, 0.2f));  // Szare światło dla siatki
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        groundGrid->Draw();
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glm::mat4 view1 = glm::lookAt(cameraPos, cameraPos - cameraFront, cameraUp);

        shader1->setMat4("view", view1);
        shader1->setVec3("lightDir", lightDir);
        shader1->setVec3("lightColor", lightColor);

        // --- Rysowanie pierwszego modelu  ---
        // glm::vec3 pivot = glm::vec3(-1.5f, 0.25f, 0.0f);

        glm::mat4 modelMat1 = glm::mat4(1.0f);
        // modelMat1 = glm::scale(modelMat1, glm::vec3(0.1f));
        glm::mat4 mvp1 = projection * view * modelMat1;
        shader->use();
        shader->setMat4("mvp", mvp1);
        shader->setMat4("model", modelMat1);
        Base->Draw();

        // --- Rysowanie drugiego modelu  ---
        glm::mat4 modelMat2 = glm::mat4(1.0f);
        glm::vec3 pivot1 = glm::vec3(0.0f, 0.25f, 1.8f);
        modelMat2 = glm::translate(modelMat2, pivot1);                                    // przesunięcie do punktu obrotu
        modelMat2 = glm::rotate(modelMat2, glm::radians(rotationY), glm::vec3(0, 1, 0));  // obrót wokół osi Y
        modelMat2 = glm::translate(modelMat2, -pivot1);                                   // przesunięcie w górę
        modelMat2 = glm::translate(modelMat2, glm::vec3(0.0f, 1.79f, 1.8f));              // przesuniecie do koncowki ramienia
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
        Arm1->Draw();

        // --- Rysowanie trzeciego modelu  ---
        glm::mat4 modelMat3 = glm::mat4(1.0f);
        glm::vec3 pivot2 = glm::vec3(0.0f, 0.25f, 2.6f);
        modelMat3 = glm::translate(modelMat3, pivot2);                                     // przesunięcie do punktu obrotu
        modelMat3 = glm::rotate(modelMat3, glm::radians(rotationY1), glm::vec3(0, 1, 0));  // obrót wokół osi Y
        modelMat3 = glm::translate(modelMat3, -pivot2);                                    // przesunięcie w górę
        modelMat3 = glm::translate(modelMat3, glm::vec3(0.0f, 0.5f, 6.7f));                // przesuniecie do koncowki ramienia
        glm::mat4 mvp3 = projection * view * modelMat1 * modelMat2 * modelMat3;
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
        Arm2->Draw();

        // --- Rysowanie czwartego modelu  ---
        glm::mat4 modelMat4 = glm::mat4(1.0f);
        if (!pri.empty()) {
            if (!pri[0])
                modelMat4 = glm::translate(modelMat4, glm::vec3(0.0f, rotationZ, 0.0f));
            else
                modelMat4 = glm::translate(modelMat4, glm::vec3(0.0f, -0.1f + rotationZ, 0.0f));
        }
        else
            modelMat4 = glm::translate(modelMat4, glm::vec3(0.0f, rotationZ, 0.0f));

        // modelMat4 = glm::scale(modelMat4, glm::vec3(0.7f));
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
        Arm3->Draw();

        glm::mat4 arm3modelMatrix = modelMat1 * modelMat2 * modelMat3 * modelMat4;
        glm::vec3 minWorld = glm::vec3(arm3modelMatrix * glm::vec4(globalMin1, 1.0f));
        glm::vec3 maxWorld = glm::vec3(arm3modelMatrix * glm::vec4(globalMax2, 1.0f));

        // --- Rysowanie prymitywa ---
        if (!prymitywy.empty()) {
            for (int i = 0; i < prymitywy.size(); ++i) {
                glm::mat4 modelMat5 = glm::mat4(1.0f);

                if (pri[i]) {
                    glm::vec3 arm3BottomPos = glm::vec3(arm3modelMatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
                    prymitywy[i]->SetPosition(arm3BottomPos + glm::vec3(0.0f, -0.25f, 0.0f));
                }
                else {
                    glm::vec3 pos = prymitywy[i]->GetPosition();
                    if (pos.y <= 0) {
                        pos.y = 0;
                        prymitywy[i]->SetPosition(pos);
                        first = false;
                    }
                    else {
                        pos.y -= 0.01f;
                        prymitywy[i]->SetPosition(pos);
                    }
                }

                modelMat51 = glm::translate(glm::mat4(1.0f), prymitywy[i]->GetPosition());

                modelMat5 = modelMat51;

                glm::mat4 mvp5 = projection * view * modelMat5;
                prymitywy[i]->Draw(mvp5, modelMat5, shader.get());

                glm::vec3 primMinWorld = glm::vec3(modelMat5 * glm::vec4(globalMin3, 1.0f));
                glm::vec3 primMaxWorld = glm::vec3(modelMat5 * glm::vec4(globalMax4, 1.0f));

                if (!pri[i] && minWorld.x <= primMaxWorld.x && maxWorld.x >= primMinWorld.x && minWorld.y <= primMaxWorld.y && maxWorld.y >= primMinWorld.y && minWorld.z <= primMaxWorld.z && maxWorld.z >= primMinWorld.z) {
                    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS || (playback_c && playbackMode == PlaybackMode::PLAYBACK)) {
                        pri[i] = true;
                        first = true;
                    }
                }

                if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS || (playback_space && playbackMode == PlaybackMode::PLAYBACK)) {
                    pri[i] = false;
                }
            }
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}
