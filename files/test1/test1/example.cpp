
#include <iostream>
#include <glad/glad.h>
#include "GLFW/glfw3.h"
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "utils.h"

//forward declarations
void renderTerrain(glm::mat4 view, glm::mat4 projection);
void renderSkybox(glm::mat4 view, glm::mat4 projection);
void setupResources();

glm::vec3 cameraPosition(100, 100, 100), cameraForward(0, 0, 1), cameraUp(0, 1, 0);

unsigned int plane, size, VAO, cubeSize;
unsigned int myProgram, skyProgram;

unsigned int diffuseTexID;

unsigned int heightmapID, heightNormalID, dirtID, sandID, grassID;

void handleInput(GLFWwindow* window, float deltaTime) {
    static bool w, s, a, d, space, ctrl;
    static double cursorX = -1, cursorY = -1, lastCursorX, lastCursorY;
    static float pitch, yaw;
    static float speed = 100.0f;
    float sensitivity = 100.0f * deltaTime;
    float step = speed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) w = true;
    else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_RELEASE) w = false;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) s = true;
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_RELEASE) s = false;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) a = true;
    else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_RELEASE) a = false;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) d = true;
    else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_RELEASE) d = false;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) space
        = true;
    else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) space =
        false;
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) ctrl = true;
    else if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE) ctrl =
        false;
    if (cursorX == -1) {
        glfwGetCursorPos(window, &cursorX, &cursorY);
    }
    lastCursorX = cursorX;
    lastCursorY = cursorY;
    glfwGetCursorPos(window, &cursorX, &cursorY);
    glm::vec2 mouseDelta(cursorX - lastCursorX, cursorY - lastCursorY);
    // TODO: calculate rotation & movement
    yaw -= mouseDelta.x * sensitivity;
    pitch += mouseDelta.y * sensitivity;
    if (pitch < -90.0f) pitch = -90.0f;
    else if (pitch > 90.0f) pitch = 90.0f;
    if (yaw < -180.0f) yaw += 360;
    else if (yaw > 180.0f) yaw -= 360;
    glm::vec3 euler(glm::radians(pitch), glm::radians(yaw), 0);
    glm::quat q(euler);
    // update camera position / forward & up
    glm::vec3 translation(0, 0, 0);
    cameraPosition += q * translation;
    cameraUp = q * glm::vec3(0, 1, 0);
    cameraForward = q * glm::vec3(0, 0, 1);
}
int main()
{
    static double previousT = 0;
    std::cout << "Hello World!\n";
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    int width = 800;
    int height = 800;
    GLFWwindow* window = glfwCreateWindow(width, height, "Hello OpenGL!", nullptr,
        nullptr);
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glViewport(0, 0, width, height);
    
    // OPENGL SETTINGS //
    glEnable(GL_CULL_FACE);
    while (!glfwWindowShouldClose(window)) {
        double t = glfwGetTime();
        float deltaTime = t - previousT;
        previousT = t;
        float r = (float)sin(t * 1.1f);
        float g = (float)sin(t * 1.3f);
        float b = (float)sin(t * 1.7f);
        handleInput(window, deltaTime);

        glm::mat4 view = glm::lookAt(cameraPosition, cameraPosition +
            cameraForward, cameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(65.0f), width /
            (float)height, 0.1f, 1000.0f);
        /*
        glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(world));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
        */
        // iets tekenen
        glClearColor(r, g, b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
       

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseTexID);

        glBindVertexArray(VAO);
        //glDrawArrays(GL_TRIANGLES, 0, 3);
        //glDrawArrays(GL_TRIANGLES, 3, 3);

        glDrawElements(GL_TRIANGLES, cubeSize, GL_UNSIGNED_INT, 0);


        renderSkybox(view, projection);
        renderTerrain(view, projection);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}

void renderTerrain(glm::mat4 view, glm::mat4 projection) { 
    glUseProgram(myProgram);
    glCullFace(GL_BACK);
    glm::mat4 world = glm::mat4(1.f);
    world = glm::translate(world, glm::vec3(0, 0, 0));
    glUniformMatrix4fv(glGetUniformLocation(myProgram, "world"), 1, GL_FALSE,
        glm::value_ptr(world));
    glUniformMatrix4fv(glGetUniformLocation(myProgram, "view"), 1, GL_FALSE,
        glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(myProgram, "projection"), 1,
        GL_FALSE, glm::value_ptr(projection));
    glUniform3fv(glGetUniformLocation(myProgram, "cameraPosition"), 1,
        glm::value_ptr(cameraPosition));


    double t = glfwGetTime();
    glUniform3f(glGetUniformLocation(myProgram, "lightDirection"), glm::cos(t), -0.5f, glm::sin(t));

    //glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D, diffuseTexID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, heightmapID);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, heightNormalID);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, dirtID); 
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, sandID);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, grassID);

    glBindVertexArray(plane);
    glDrawElements(GL_LINES, size, GL_UNSIGNED_INT, 0);
}
void renderSkybox(glm::mat4 view, glm::mat4 projection){ 
    // SKY BOX
    glUseProgram(skyProgram);
    glCullFace(GL_FRONT);
    glm::mat4 world = glm::mat4(1.f);
    world = glm::translate(world, cameraPosition);
    glUniformMatrix4fv(glGetUniformLocation(skyProgram, "world"), 1, GL_FALSE,
        glm::value_ptr(world));
    glUniformMatrix4fv(glGetUniformLocation(skyProgram, "view"), 1, GL_FALSE,
        glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(skyProgram, "projection"), 1,
        GL_FALSE, glm::value_ptr(projection));
    glUniform3fv(glGetUniformLocation(skyProgram, "cameraPosition"), 1,
        glm::value_ptr(cameraPosition));


    double t = glfwGetTime();
    glUniform3f(glGetUniformLocation(skyProgram, "lightDirection"), glm::cos(t), -0.5f, glm::sin(t));

    //glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D, diffuseTexID);
    glBindVertexArray(VAO);
    //glDrawArrays(GL_TRIANGLES, 0, 6);
    glDrawElements(GL_TRIANGLES, cubeSize, GL_UNSIGNED_INT, 0);
}

void setupResources() {
    ///SETUP OBJECT///
    // Vertices of our triangle!
    // need 24 vertices for normal/uv-mapped Cube
    float vertices[] = {
        // positions            //colors            // tex coords   // normals
        0.5f, -0.5f, -0.5f,     1.0f, 0.0f, 1.0f,   1.f, 0.f,       0.f, -1.f, 0.f,
        0.5f, -0.5f, 0.5f,      1.0f, 1.0f, 0.0f,   1.f, 1.f,       0.f, -1.f, 0.f,
        -0.5f, -0.5f, 0.5f,     0.0f, 1.0f, 1.0f,   0.f, 1.f,       0.f, -1.f, 0.f,
        -0.5f, -0.5f, -.5f,     1.0f, 1.0f, 1.0f,   0.f, 0.f,       0.f, -1.f, 0.f,
        0.5f, 0.5f, -0.5f,      1.0f, 1.0f, 1.0f,   2.f, 0.f,       1.f, 0.f, 0.f,
        0.5f, 0.5f, 0.5f,       1.0f, 1.0f, 1.0f,   2.f, 1.f,       1.f, 0.f, 0.f,
        0.5f, 0.5f, 0.5f,       1.0f, 1.0f, 1.0f,   1.f, 2.f,       0.f, 0.f, 1.f,
        -0.5f, 0.5f, 0.5f,      1.0f, 1.0f, 1.0f,   0.f, 2.f,       0.f, 0.f, 1.f,
        -0.5f, 0.5f, 0.5f,      1.0f, 1.0f, 1.0f,   -1.f, 1.f,      -1.f, 0.f, 0.f,
        -0.5f, 0.5f, -.5f,      1.0f, 1.0f, 1.0f,   -1.f, 0.f,      -1.f, 0.f, 0.f,
        -0.5f, 0.5f, -.5f,      1.0f, 1.0f, 1.0f,   0.f, -1.f,      0.f, 0.f, -1.f,
        0.5f, 0.5f, -0.5f,      1.0f, 1.0f, 1.0f,   1.f, -1.f,      0.f, 0.f, -1.f,
        -0.5f, 0.5f, -.5f,      1.0f, 1.0f, 1.0f,   3.f, 0.f,       0.f, 1.f, 0.f,
        -0.5f, 0.5f, 0.5f,      1.0f, 1.0f, 1.0f,   3.f, 1.f,       0.f, 1.f, 0.f,
        0.5f, -0.5f, 0.5f,      1.0f, 1.0f, 1.0f,   1.f, 1.f,       0.f, 0.f, 1.f,
        -0.5f, -0.5f, 0.5f,     1.0f, 1.0f, 1.0f,   0.f, 1.f,       0.f, 0.f, 1.f,
        -0.5f, -0.5f, 0.5f,     1.0f, 1.0f, 1.0f,   0.f, 1.f,       -1.f, 0.f, 0.f,
        -0.5f, -0.5f, -.5f,     1.0f, 1.0f, 1.0f,   0.f, 0.f,       -1.f, 0.f, 0.f,
        -0.5f, -0.5f, -.5f,     1.0f, 1.0f, 1.0f,   0.f, 0.f,       0.f, 0.f, -1.f,
        0.5f, -0.5f, -0.5f,     1.0f, 1.0f, 1.0f,   1.f, 0.f,       0.f, 0.f, -1.f,
        0.5f, -0.5f, -0.5f,     1.0f, 1.0f, 1.0f,   1.f, 0.f,       1.f, 0.f, 0.f,
        0.5f, -0.5f, 0.5f,      1.0f, 1.0f, 1.0f,   1.f, 1.f,       1.f, 0.f, 0.f,
        0.5f, 0.5f, -0.5f,      1.0f, 1.0f, 1.0f,   2.f, 0.f,       0.f, 1.f, 0.f,
        0.5f, 0.5f, 0.5f,       1.0f, 1.0f, 1.0f,   2.f, 1.f,       0.f, 1.f, 0.f
    };
    unsigned int indices[] = {  // note that we start from 0!
        // DOWN
        0, 1, 2,   // first triangle
        0, 2, 3,    // second triangle
        // BACK
        14, 6, 7,   // first triangle
        14, 7, 15,    // second triangle
        // RIGHT
        20, 4, 5,   // first triangle
        20, 5, 21,    // second triangle
        // LEFT
        16, 8, 9,   // first triangle
        16, 9, 17,    // second triangle
        // FRONT
        18, 10, 11,   // first triangle
        18, 11, 19,    // second triangle
        // UP
        22, 12, 13,   // first triangle
        22, 13, 23,    // second triangle
    };
    cubeSize = sizeof(indices);

    glGenVertexArrays(1, &VAO);
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
        GL_STATIC_DRAW);
    int stride = sizeof(float) * 11;
    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);
    // color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) *
        3));
    glEnableVertexAttribArray(1);
    // uv
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) *
        6));
    glEnableVertexAttribArray(2);
    // normal
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) *
        8));
    glEnableVertexAttribArray(3);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    ///END SETUP OBJECT///
    plane = GeneratePlane("Heightmap.png", GL_RGBA, 4, 1.0f, 1.0f,
        size, heightmapID);

    // terrain textures
    heightNormalID = loadTexture("NormalHeightmap.png", GL_RGBA, 4);
    dirtID = loadTexture("dirt.jpg", GL_RGBA, 3);
    sandID = loadTexture("sand.png", GL_RGBA, 4);
    grassID = loadTexture("grass.jpg", GL_RGBA, 3);

    ///SETUP SHADER PROGRAM///
    stbi_set_flip_vertically_on_load(true);
    char* vertexSource;
    loadFromFile("vertexShader.shader", &vertexSource);
    char* fragmentSource;
    loadFromFile("fragmentShader.shader", &fragmentSource);
    unsigned int vertSky, fragSky;
    CreateShader("vertexShaderSky.shader", GL_VERTEX_SHADER, vertSky);
    CreateShader("fragmentShaderSky.shader", GL_FRAGMENT_SHADER, fragSky);
    // LOAD & CREATE TEXTURES

    //unsigned int diffuseTexID = loadTexture("container2.png", GL_RGBA, 4);
    unsigned int diffuseTexID = loadTexture("abstractWoodTexture.jpg", GL_RGB, 3);
    // END
    unsigned int vertID, fragID;
    vertID = glCreateShader(GL_VERTEX_SHADER);
    fragID = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(vertID, 1, &vertexSource, nullptr);
    glShaderSource(fragID, 1, &fragmentSource, nullptr);
    int success;
    char infoLog[512];
    glCompileShader(vertID);
    glGetShaderiv(vertID, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertID, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog <<
            std::endl;
    };
    glCompileShader(fragID);
    glGetShaderiv(fragID, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragID, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog <<
            std::endl;
    };
    myProgram = glCreateProgram();
    glAttachShader(myProgram, vertID);
    glAttachShader(myProgram, fragID);
    glLinkProgram(myProgram);
    skyProgram = glCreateProgram();
    glAttachShader(skyProgram, vertSky);
    glAttachShader(skyProgram, fragSky);
    glLinkProgram(skyProgram);
    glDeleteShader(vertID);
    glDeleteShader(fragID);
    glDeleteShader(vertSky);
    glDeleteShader(fragSky);

    ///END SETUP SHADER PROGRAM///
    
    glUseProgram(myProgram);
    glUniform1i(glGetUniformLocation(myProgram, "heightmap"), 0);
    glUniform1i(glGetUniformLocation(myProgram, "normalMap"), 1);
    glUniform1i(glGetUniformLocation(myProgram, "dirt"), 2);
    glUniform1i(glGetUniformLocation(myProgram, "sand"), 3);
    glUniform1i(glGetUniformLocation(myProgram, "grass"), 4);

    /// MATRIX SETUP ///
    /*
    glUseProgram(myProgram);
    int worldLoc = glGetUniformLocation(myProgram, "world");
    int viewLoc = glGetUniformLocation(myProgram, "view");
    int projLoc = glGetUniformLocation(myProgram, "projection");
    */
    /// END MATRIX SETUP ///
}