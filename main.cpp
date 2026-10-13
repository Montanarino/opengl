#define STB_IMAGE_IMPLEMENTATION
#ifdef __EMSCRIPTEN__
    #include <emscripten/emscripten.h>
    #include <emscripten/html5.h>
    #include <glad/gles2.h>
#else
    #include <glad/gl.h>
#endif
#include <GLFW/glfw3.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <cmath>
#include "Shader.h"
#include "Texture.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float lastX = 400, lastY = 300;
bool firstMouse = true;
float yaw = -90.0f;
float pitch = 0.0f;
float fov = 45.0f;
bool isJumping = false, goingUp = false, isRunning = false;
int wwidth = 800, wheight = 600;

GLFWwindow* window = nullptr;
unsigned int VBO, VAO, EBO, lightVAO;
Shader* shaderProgram = nullptr;
Shader* lightProgram = nullptr;
Texture* t1 = nullptr;
Texture* t2 = nullptr;

float vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};

glm::vec3 cubePositions[] = {
    glm::vec3( 0.0f,  0.0f,  0.0f), 
    glm::vec3( 2.0f,  5.0f, -15.0f), 
    glm::vec3(-1.5f, -2.2f, -2.5f),  
    glm::vec3(-3.8f, -2.0f, -12.3f),  
    glm::vec3( 2.4f, -0.4f, -3.5f),  
    glm::vec3(-1.7f,  3.0f, -7.5f),  
    glm::vec3( 1.3f, -2.0f, -2.5f),  
    glm::vec3( 1.5f,  2.0f, -2.5f), 
    glm::vec3( 1.5f,  0.2f, -1.5f), 
    glm::vec3(-1.3f,  1.0f, -1.5f)  
};

unsigned int indices[] = {
    0, 1, 3,
    1, 2, 3
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void main_loop();
void updateCameraVectors(float xoffset, float yoffset);
#ifdef __EMSCRIPTEN__
EM_BOOL em_mousedown_callback(int eventType, const EmscriptenMouseEvent *e, void *userData);
EM_BOOL em_mousemove_callback(int eventType, const EmscriptenMouseEvent *e, void *userData);
#endif

void main_loop() {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;  
    processInput(window);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float color = (std::sin(glfwGetTime()) / 2.0f) + 0.5f;
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    glm::mat4 projection = glm::perspective(glm::radians(fov), (float)wwidth / (float)wheight, 0.1f, 100.0f);

    glm::vec3 lightPos = glm::vec3(5.0f, 2.0f, 5.0f);

    shaderProgram->use();
    shaderProgram->setVec3("objectColor", 1.0f, 0.5f, 0.31f);
    shaderProgram->setVec3("lightColor", 1.0f, 1.0f, 1.0f);
    shaderProgram->setVec3("lightPos", lightPos.x, lightPos.y, lightPos.z);
    shaderProgram->setMatrix4fv("view", view);
    shaderProgram->setMatrix4fv("projection", projection);

    glActiveTexture(GL_TEXTURE0);
    t1->use();
    glActiveTexture(GL_TEXTURE1);
    t2->use();
    glBindVertexArray(VAO);

    for (unsigned int i = 0; i < 10; i++)
    {
        model = glm::mat4(1.0f);
        model = glm::translate(model, cubePositions[i]);
        float angle = glfwGetTime() * 25.0f;
        model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
        shaderProgram->setMatrix4fv("model", model);

        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    float angle = glfwGetTime() * 25.0f;
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(150.0f, 1.0f, 150.0f));
    model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
    shaderProgram->setMatrix4fv("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 10.0f, 0.0f));
    model = glm::scale(model, glm::vec3(150.0f, 1.0f, 150.0f));
    model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
    shaderProgram->setMatrix4fv("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    lightProgram->use();
    glBindVertexArray(lightVAO);
    model = glm::mat4(1.0f);
    model = glm::translate(model, lightPos);
    model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
    lightProgram->setMatrix4fv("view", view);
    lightProgram->setMatrix4fv("projection", projection);
    lightProgram->setMatrix4fv("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    
    glfwSwapBuffers(window);
    glfwPollEvents();
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    #ifdef __EMSCRIPTEN__
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    #else
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #endif

    window = glfwCreateWindow(800, 600, "ciao", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    #ifdef __EMSCRIPTEN__
    // Lock the pointer on canvas click and use HTML5 relative mouse movement
    emscripten_set_mousedown_callback("#canvas", nullptr, true, em_mousedown_callback);
    emscripten_set_mousemove_callback("#canvas", nullptr, true, em_mousemove_callback);
    if (!gladLoadGLES2((GLADloadfunc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD (GLES)" << std::endl;
        return -1;
    }
    #else
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD (GL)" << std::endl;
        return -1;
    }
    #endif

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback); 
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Paths updated to match your CMake "--preload-file ${CMAKE_SOURCE_DIR}/assets@/assets"
    shaderProgram = new Shader("assets/vshader.glsl", "assets/fshader.glsl");
    lightProgram  = new Shader("assets/l_vshader.glsl", "assets/l_fshader.glsl");
    t1 = new Texture("assets/container.jpg");
    t2 = new Texture("assets/poletti.png");

    glViewport(0, 0, 800, 600);

    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenVertexArrays(1, &VAO);
    glGenVertexArrays(1, &lightVAO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); 
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    shaderProgram->use();
    shaderProgram->setInt("texture1", 0);
    shaderProgram->setInt("texture2", 1);
    
    glEnable(GL_DEPTH_TEST);

    #ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(main_loop, 0, 1);
    #else
    while (!glfwWindowShouldClose(window)) {
        main_loop();
    }
    #endif

    delete shaderProgram;
    delete lightProgram;
    delete t1;
    delete t2;

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    wwidth = (width > 0) ? width : 1;
    wheight = (height > 0) ? height : 1;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    fov -= (float)yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f)
        fov = 45.0f; 
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    float cameraSpeed = 2.5f * deltaTime;
    glm::vec3 lockedFront = glm::vec3(cameraFront.x, 0.0f, cameraFront.z);
    if (glm::length(lockedFront) > 0.0001f)
        lockedFront = glm::normalize(lockedFront);
    if (isRunning)
        lockedFront *= 4.0f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * lockedFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * lockedFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(lockedFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(lockedFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !isJumping) {
        isJumping = true;
        goingUp = true;
    }
    if (isJumping && goingUp) {
        cameraPos += cameraSpeed * glm::vec3(0.0f, -4.0f * cameraPos.y + 4.0f, 0.0f);
        if (cameraPos.y >= 0.95f) {
            goingUp = false;
        }
    } else if (isJumping && !goingUp) {
        cameraPos -= cameraSpeed * glm::vec3(0.0f, -4.0f * cameraPos.y + 4.0f, 0.0f);
        if (cameraPos.y <= 0.05f) {
            cameraPos.y = 0.0f;
            isJumping = false;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        isRunning = true;
    } else {
        isRunning = false;
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; 
    lastX = xpos;
    lastY = ypos;

    updateCameraVectors(xoffset, yoffset);
}

void updateCameraVectors(float xoffset, float yoffset) {
    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw   += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)  pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);
}

#ifdef __EMSCRIPTEN__
// Lock pointer when the user clicks the canvas
EM_BOOL em_mousedown_callback(int eventType, const EmscriptenMouseEvent *e, void *userData) {
    emscripten_request_pointerlock("#canvas", true);
    return EM_TRUE;
}

// Read infinite relative mouse movement (movementX / movementY) while locked
EM_BOOL em_mousemove_callback(int eventType, const EmscriptenMouseEvent *e, void *userData) {
    EmscriptenPointerlockChangeEvent status;
    if (emscripten_get_pointerlock_status(&status) == EMSCRIPTEN_RESULT_SUCCESS && status.isActive) {
        updateCameraVectors((float)e->movementX, -(float)e->movementY);
    }
    return EM_TRUE;
}
#endif