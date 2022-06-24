// test1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <glad/glad.h>
#include "GLFW/glfw3.h"
#include <fstream>

#include "model.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

#include "utils.h"

void drawSky(glm::mat4 view, glm::mat4 projection);
void renderTerrain(glm::mat4 view, glm::mat4 projection);
void renderWater(glm::mat4 view, glm::mat4 projection);
void renderModel(Model* model, unsigned int shader, glm::vec3 pos, glm::vec3 rot, float scale, glm::mat4 view, glm::mat4 projection);
void setupResources();
void renderCurlImage();
void setupFinalResources();


glm::vec3 cameraPosition(100, 100, 100), cameraForward(0, 0, 1), cameraUp(0, 1, 0);
glm::vec3 lightDir(-0.7f, 0.5f, -0.5f);
glm::mat4 view, projection;

int width, height;
double t;

/// huiswerkopdrachten
unsigned int myProgram, skyProgram, modelProgram, hutProgram, waterProgram;
unsigned int terrainSize,  plane, cubeSize;
unsigned int heightmapID, heightNormalID;
unsigned int dirt, sand, grass, snow, rock, water;
unsigned int waterSize;
unsigned int VAO;

Model* backpack;
Model* hut;

/// final assignment
unsigned int image;
unsigned int indexesSize;
glm::vec2 mousePos;


void handleInput(GLFWwindow* window, float deltaTime) {
    static bool w, s, a, d, space, ctrl;
    static double cursorX = -1, cursorY = -1, lastCursorX, lastCursorY;
    static float pitch, yaw;
    static float speed = 50.0f;

    float sensitivity = 80.0f * deltaTime;
    float step = speed * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)				w = true;
    else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_RELEASE)		w = false;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)				s = true;
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_RELEASE)		s = false;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)				a = true;
    else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_RELEASE)		a = false;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)				d = true;
    else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_RELEASE)		d = false;

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)				space = true;
    else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)		space = false;
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)		ctrl = true;
    else if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE)	ctrl = false;

    if (cursorX == -1) {
        glfwGetCursorPos(window, &cursorX, &cursorY);
    }

    lastCursorX = cursorX;
    lastCursorY = cursorY;
    glfwGetCursorPos(window, &cursorX, &cursorY);

    glm::vec2 mouseDelta(cursorX - lastCursorX, cursorY - lastCursorY);

    // calculate rotation & movement
    yaw += mouseDelta.x * sensitivity;
    pitch += mouseDelta.y * sensitivity;

    pitch = glm::max(glm::min(pitch, 90.0f),-90.0f);

    if (yaw < -180.0f) yaw += 360;
    else if (yaw > 180.0f) yaw -= 360;

    glm::vec3 euler(glm::radians(pitch), glm::radians(-yaw), 0);
    glm::quat q(euler);

    // update camera position / forward & up
    glm::vec3 translation(int(a)-int(d), int(space)-int(ctrl), int(w) - int(s));
    translation *= step;
    cameraPosition += q * translation;

    cameraUp = q * glm::vec3(0, 1, 0);
    cameraForward = q * glm::vec3(0, 0, 1);
}

glm::vec2 getMousePos(GLFWwindow* window, float deltaTime) {
    static double cursorX = -1, cursorY = -1;

    glfwGetCursorPos(window, &cursorX, &cursorY);

    return glm::vec2(cursorX, cursorY);
}

int main()
{
    static bool huiswerkopdrachten = false;
    static double previousT = 0;

    std::cout << "Hello darkness my old friend!\n";
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    //create a window
    width = 1080;
    height = 1080;

    //create a window
    GLFWwindow* window = glfwCreateWindow(width, height, "Hello!", nullptr, NULL);
    glfwMakeContextCurrent(window);

    //send error if glad failed
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    //initialise window 
    glViewport(0, 0, width, height);

    if (huiswerkopdrachten) {
        setupResources();


        /// SETUP MATRIX ///
        /*
        glUseProgram(myProgram);

        int worldLoc = glGetUniformLocation(myProgram, "world");
        int viewLoc = glGetUniformLocation(myProgram, "view");
        int projectionLoc = glGetUniformLocation(myProgram, "projection");
        */

        /// END SETUP MATRIX ///


        // enable stuff so depth is rendered properly
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        glDepthFunc(GL_LEQUAL);
    }
    else {
        setupFinalResources();
    }

    while (!glfwWindowShouldClose(window)) 
    {
        //processInput(window);

        t = glfwGetTime();
        float deltaT = float(t) - previousT;
        previousT = t;

        if (huiswerkopdrachten) {
            handleInput(window, deltaT);

            //lightDir = glm::vec3(0.4, -0.5f, 0.4);
            lightDir = glm::vec3(sin(t / 3), -0.5f, cos(t / 3));

            //colour background
            float r = (float)sin(t);
            float g = (float)cos(1.4f * t);
            float b = (float)sin(1.7f * t);

            r = glm::max(r, 0.5f);
            g = glm::max(g, 0.8f * float(sin(t)));
            b = glm::max(b, 0.8f * float(cos(t)));

            view = glm::lookAt(cameraPosition, cameraPosition + cameraForward, cameraUp);
            projection = glm::perspective(glm::radians(65.0f), width / (float)height, 0.1f, 1000.0f);
        }
        else {
            mousePos = getMousePos(window, deltaT);
            //cout << mousePos.x << ", " << mousePos.y << "\n";
        }
        //glClearColor(r, g, b, 1.0f);
        glClearColor(0,0,0,0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

        if (huiswerkopdrachten) {
            drawSky(view, projection);

            renderTerrain(view, projection);


            // MODEL
            //glEnable(GL_BLEND);
            //glBlendFunc(GL_DST_COLOR, GL_ZERO);
            //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            //glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE);

            renderModel(backpack, modelProgram, glm::vec3(119, 8, 120), glm::vec3(glm::radians(1.0) * -30.0f, glm::radians(1.0) * 230.0f, glm::radians(1.0) * -10.0f), 1, view, projection);
            renderModel(hut, hutProgram, glm::vec3(120, 8, 140), glm::vec3(0, glm::radians(1.0) * -45.0f, 0), 0.05, view, projection);

            //glDisable(GL_BLEND);
        }
        else {
            renderCurlImage();
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}

void drawSky(glm::mat4 view, glm::mat4 projection) {
    // SKYBOX
    glCullFace(GL_FRONT);
    glDisable(GL_DEPTH_TEST);

    glUseProgram(skyProgram);

    int skyWorldLoc = glGetUniformLocation(skyProgram, "world");
    int skyViewLoc = glGetUniformLocation(skyProgram, "view");
    int skyProjectionLoc = glGetUniformLocation(skyProgram, "projection");

    int skyCamPos = glGetUniformLocation(skyProgram, "cameraPosition");
    int skyLight = glGetUniformLocation(skyProgram, "lightDirection");

    glm::mat4 world = glm::mat4(1.f);
    //world = glm::rotate(world, glm::radians(float(t)*45.0f), glm::vec3(0.5f, float(tan(t)), 0.5f));
    //world = glm::scale(world, glm::vec3(1, 1, 1));
    world = glm::translate(world, cameraPosition);

    glUniformMatrix4fv(skyWorldLoc, 1, GL_FALSE, glm::value_ptr(world));
    glUniformMatrix4fv(skyViewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(skyProjectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    glUniform3fv(skyCamPos, 1, glm::value_ptr(cameraPosition));
    glUniform3fv(skyLight, 1, glm::value_ptr(lightDir));
    //glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D, diffuseTexID);

    glBindVertexArray(VAO);

    glDrawElements(GL_TRIANGLES, cubeSize, GL_UNSIGNED_INT, 0);
}

void renderTerrain(glm::mat4 view, glm::mat4 projection) {

    // TERRAIN
    glUseProgram(myProgram);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);

    int worldLoc = glGetUniformLocation(myProgram, "world");
    int viewLoc = glGetUniformLocation(myProgram, "view");
    int projectionLoc = glGetUniformLocation(myProgram, "projection");

    int myCamPos = glGetUniformLocation(myProgram, "cameraPosition");
    int myLight = glGetUniformLocation(myProgram, "lightDirection");

    glm::mat4 world = glm::mat4(1.f);
    //world = glm::rotate(world, glm::radians(float(t)*45.0f), glm::vec3(0.5f, float(tan(t)), 0.5f));
    //world = glm::scale(world, glm::vec3(1, 1, 1));
    world = glm::translate(world, glm::vec3(0, 0, 0));

    glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(world));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    glUniform3fv(myCamPos, 1, glm::value_ptr(cameraPosition));
    glUniform3fv(myLight, 1, glm::value_ptr(lightDir));
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, heightmapID);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, heightNormalID);



    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, dirt);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, sand);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, grass);
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, snow);
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, rock);

    glBindVertexArray(plane);

    glDrawElements(GL_TRIANGLES, terrainSize, GL_UNSIGNED_INT, 0);
}

void renderWater(glm::mat4 view, glm::mat4 projection) {

    // WATER
    glUseProgram(waterProgram);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);

    int worldLoc = glGetUniformLocation(waterProgram, "world");
    int viewLoc = glGetUniformLocation(waterProgram, "view");
    int projectionLoc = glGetUniformLocation(waterProgram, "projection");

    int myCamPos = glGetUniformLocation(waterProgram, "cameraPosition");
    int myLight = glGetUniformLocation(waterProgram, "lightDirection");

    glm::mat4 world = glm::mat4(1.f);
    //world = glm::rotate(world, glm::radians(45.0), glm::vec3(0.5f, float(tan(t)), 0.5f));
    //world = glm::scale(world, glm::vec3(1, 1, 1));
    world = glm::translate(world, glm::vec3(0, 20, 0));

    glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(world));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    glUniform3fv(myCamPos, 1, glm::value_ptr(cameraPosition));
    glUniform3fv(myLight, 1, glm::value_ptr(lightDir));

    glBindVertexArray(VAO);

    glDrawElements(GL_LINES, 6, GL_UNSIGNED_INT, 0);
}

void renderModel(Model* model, unsigned int shader, glm::vec3 pos, glm::vec3 rot, float scale, glm::mat4 view, glm::mat4 projection) {
    //shader gebruiken
    glUseProgram(shader);
    
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    
    //world matrix 
    glm::mat4 world = glm::mat4(1);
    //position
    world = glm::translate(world, pos);
    //scale
    world = glm::scale(world, glm::vec3(scale));
    //rotation
    glm::quat q(rot);
    world *= glm::toMat4(q);


    int modelWorldLoc = glGetUniformLocation(shader, "world");
    int modelViewLoc = glGetUniformLocation(shader, "view");
    int modelProjectionLoc = glGetUniformLocation(shader, "projection");

    int modelCamPos = glGetUniformLocation(shader, "cameraPosition");
    int modelLight = glGetUniformLocation(shader, "lightDirection");


    glUniformMatrix4fv(modelWorldLoc, 1, GL_FALSE, glm::value_ptr(world));
    glUniformMatrix4fv(modelViewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(modelProjectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    glUniform3fv(modelCamPos, 1, glm::value_ptr(cameraPosition));
    glUniform3fv(modelLight, 1, glm::value_ptr(lightDir));

    model->Draw(shader);
}

void setupResources() {
    /// SETUP OBJECT ///
    hut = new Model("a-hut-on-chicken-legs/yaga_house.obj");
    stbi_set_flip_vertically_on_load(true);

    backpack = new Model("backpack/backpack.obj");


    // need 24 vertices for normal/uv-mapped Cube
    float vertices[] = {
        // positions            //colors            // tex coords   // normals
        0.5f, -0.5f, -0.5f,     0.0f, 1.0f, 1.0f,   1.f, 0.f,       0.f, -1.f, 0.f,
        0.5f, -0.5f, 0.5f,      0.0f, 1.0f, 1.0f,   1.f, 1.f,       0.f, -1.f, 0.f,
        -0.5f, -0.5f, 0.5f,     0.0f, 1.0f, 1.0f,   0.f, 1.f,       0.f, -1.f, 0.f,
        -0.5f, -0.5f, -.5f,     0.0f, 1.0f, 1.0f,   0.f, 0.f,       0.f, -1.f, 0.f,

        0.5f, 0.5f, -0.5f,      1.0f, 0.0f, 1.0f,   2.f, 0.f,       1.f, 0.f, 0.f,
        0.5f, 0.5f, 0.5f,       1.0f, 0.0f, 1.0f,   2.f, 1.f,       1.f, 0.f, 0.f,

        0.5f, 0.5f, 0.5f,       1.0f, 1.0f, 0.0f,   1.f, 2.f,       0.f, 0.f, 1.f,
        -0.5f, 0.5f, 0.5f,      1.0f, 1.0f, 0.0f,   0.f, 2.f,       0.f, 0.f, 1.f,

        -0.5f, 0.5f, 0.5f,      1.0f, 1.0f, 0.5f,   -1.f, 1.f,      -1.f, 0.f, 0.f,
        -0.5f, 0.5f, -.5f,      1.0f, 1.0f, 0.5f,   -1.f, 0.f,      -1.f, 0.f, 0.f,

        -0.5f, 0.5f, -.5f,      1.0f, 0.5f, 1.0f,   0.f, -1.f,      0.f, 0.f, -1.f,
        0.5f, 0.5f, -0.5f,      1.0f, 0.5f, 1.0f,   1.f, -1.f,      0.f, 0.f, -1.f,

        -0.5f, 0.5f, -.5f,      0.5f, 1.0f, 1.0f,   3.f, 0.f,       0.f, 1.f, 0.f,
        -0.5f, 0.5f, 0.5f,      0.5f, 1.0f, 1.0f,   3.f, 1.f,       0.f, 1.f, 0.f,

        0.5f, -0.5f, 0.5f,      0.7f, 1.0f, 1.0f,   1.f, 1.f,       0.f, 0.f, 1.f,
        -0.5f, -0.5f, 0.5f,     0.7f, 1.0f, 1.0f,   0.f, 1.f,       0.f, 0.f, 1.f,

        -0.5f, -0.5f, 0.5f,     1.0f, 0.7f, 1.0f,   0.f, 1.f,       -1.f, 0.f, 0.f,
        -0.5f, -0.5f, -.5f,     1.0f, 0.7f, 1.0f,   0.f, 0.f,       -1.f, 0.f, 0.f,

        -0.5f, -0.5f, -.5f,     1.0f, 1.0f, 0.7f,   0.f, 0.f,       0.f, 0.f, -1.f,
        0.5f, -0.5f, -0.5f,     1.0f, 1.0f, 0.7f,   1.f, 0.f,       0.f, 0.f, -1.f,

        0.5f, -0.5f, -0.5f,     1.0f, 1.0f, 1.0f,   1.f, 0.f,       1.f, 0.f, 0.f,
        0.5f, -0.5f, 0.5f,      1.0f, 1.0f, 1.0f,   1.f, 1.f,       1.f, 0.f, 0.f,

        0.5f, 0.5f, -0.5f,      0.0f, 0.0f, 0.0f,   2.f, 0.f,       0.f, 1.f, 0.f,
        0.5f, 0.5f, 0.5f,       0.0f, 0.0f, 0.0f,   2.f, 1.f,       0.f, 1.f, 0.f
    };


    unsigned int indexes[] = {  // note that we start from 0!
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
    cubeSize = sizeof(indexes);

    /// END SETUP OBJECT ///


    //generate vertex array and buffer
    glGenVertexArrays(1, &VAO);
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    unsigned int EBO;
    glGenBuffers(1, &EBO);

    //bind vertex array and buffer
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    // 0. copy our vertices array in a buffer for OpenGL to use
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexes), indexes, GL_STATIC_DRAW);

    // 1. then set the vertex attributes pointers

    int stride = 11 * sizeof(float); //Move the size of 11 floats to start of next vertice
    //pos
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);
    //colour
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);
    //uv
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * 6));
    glEnableVertexAttribArray(2);
    //normal
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * 8));
    glEnableVertexAttribArray(3);

    /// SETUP SHADER PROGRAM ///

    //generate plane
    plane = GeneratePlane("Heightmap.png", GL_RGBA, 4, 1.0f, 1.0f, terrainSize, heightmapID);
    heightNormalID = loadTexture("NormalHeightmap.png", GL_RGBA, 4);

    //load files
    unsigned int diffuseTexID = loadTexture("abstractWoodTexture.jpg", GL_RGB, 3);    
    //terrain types
    dirt = loadTexture("dirt.jpg", GL_RGB, 3);
    sand = loadTexture("sand.jpg", GL_RGB, 3);
    grass = loadTexture("grass.png", GL_RGBA, 4);
    snow = loadTexture("snow.jpg", GL_RGB, 3);
    rock = loadTexture("rock.jpg", GL_RGB, 3);

    water = GeneratePlane("water.png", GL_RGBA, 4, 1.0, 1.0, waterSize, water);

    //initialise shaders
    unsigned int vertID, fragID, vertModel, fragModel, vertHut, fragHut, fragWater, vertWater;

    CreateShader("vertexShader.shader", GL_VERTEX_SHADER, vertID);
    CreateShader("fragmentShader.shader", GL_FRAGMENT_SHADER, fragID);

    CreateShader("vertModel.shader", GL_VERTEX_SHADER, vertModel);
    CreateShader("fragModel.shader", GL_FRAGMENT_SHADER, fragModel);

    CreateShader("vertModel.shader", GL_VERTEX_SHADER, vertHut);
    CreateShader("fragModel.shader", GL_FRAGMENT_SHADER, fragHut);

    CreateShader("vertWater.shader", GL_VERTEX_SHADER, vertWater);
    CreateShader("fragWater.shader", GL_FRAGMENT_SHADER, fragWater);

    unsigned int vertSky, fragSky;

    CreateShader("vertexShaderSky.shader", GL_VERTEX_SHADER, vertSky);
    CreateShader("fragmentShaderSky.shader", GL_FRAGMENT_SHADER, fragSky);


    //create program to hold shaders
    myProgram = glCreateProgram();
    glAttachShader(myProgram, vertID);
    glAttachShader(myProgram, fragID);

    glLinkProgram(myProgram);

    glDeleteShader(vertID);
    glDeleteShader(fragID);

    //water
    waterProgram = glCreateProgram();
    glAttachShader(waterProgram, vertWater);
    glAttachShader(waterProgram, fragWater);

    glLinkProgram(waterProgram);

    glDeleteShader(vertWater);
    glDeleteShader(fragWater);

    //sky
    skyProgram = glCreateProgram();
    glAttachShader(skyProgram, vertSky);
    glAttachShader(skyProgram, fragSky);

    glLinkProgram(skyProgram);

    glDeleteShader(vertSky);
    glDeleteShader(fragSky);

    // model
    modelProgram = glCreateProgram();
    glAttachShader(modelProgram, vertModel);
    glAttachShader(modelProgram, fragModel);

    glLinkProgram(modelProgram);

    glDeleteShader(vertModel);
    glDeleteShader(fragModel);

    // hut model
    hutProgram = glCreateProgram();
    glAttachShader(hutProgram, vertHut);
    glAttachShader(hutProgram, fragHut);

    glLinkProgram(hutProgram);

    glDeleteShader(vertHut);
    glDeleteShader(fragHut);

    /// END SETUP SHADER PROGRAM ///

    glUseProgram(myProgram);
    glUniform1i(glGetUniformLocation(myProgram, "heightmap"), 0);
    glUniform1i(glGetUniformLocation(myProgram, "normalMap"), 1);
    glUniform1i(glGetUniformLocation(myProgram, "dirt"), 2);
    glUniform1i(glGetUniformLocation(myProgram, "sand"), 3);
    glUniform1i(glGetUniformLocation(myProgram, "grass"), 4);
    glUniform1i(glGetUniformLocation(myProgram, "snow"), 5);
    glUniform1i(glGetUniformLocation(myProgram, "rock"), 6);
}

void renderCurlImage() {
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);

    glUseProgram(myProgram);

    int curlWorldLoc = glGetUniformLocation(myProgram, "world");
    int curlViewLoc = glGetUniformLocation(myProgram, "view");
    int curlProjectionLoc = glGetUniformLocation(myProgram, "projection");
    int curlMouse = glGetUniformLocation(myProgram, "mouse");
    int curlTime = glGetUniformLocation(myProgram, "time");

    int curlCamPos = glGetUniformLocation(myProgram, "cameraPosition");
    int curlLight = glGetUniformLocation(myProgram, "lightDirection");

    glm::mat4 world = glm::mat4(1.f);
    //world = glm::rotate(world, glm::radians(float(t)*45.0f), glm::vec3(0.5, float(sin(t)), 0.5));
    //world = glm::rotate(world, glm::radians(10.0f), glm::vec3(float(sin(t)), int(t)%360, float(cos(t))));
    
    //world = glm::translate(world, glm::vec3(float(tan(t)*0.3), float(cos(t) * 0.1), float(sin(t)*0.1)+2.5));
    world = glm::translate(world, glm::vec3(250, -400, 250));    
    world = glm::rotate(world, glm::radians(180.0f), glm::vec3(0,1,0));
    world = glm::scale(world, glm::vec3(0.5, 0.5, 0.5));
    //world = glm::rotate(world, glm::radians(20.0f), glm::vec3(sin(t*2), sin(t), cos(t)));


    glUniformMatrix4fv(curlWorldLoc, 1, GL_FALSE, glm::value_ptr(world));
    glUniformMatrix4fv(curlViewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(curlProjectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    glUniform3fv(curlCamPos, 1, glm::value_ptr(cameraPosition));
    glUniform3fv(curlLight, 1, glm::value_ptr(lightDir));

    //get mousepos to scale to size of image
    mousePos.x += 250;
    mousePos.y += 250;
    //mousePos.y = mousePos.y / height;
    glUniform2fv(curlMouse, 1, glm::value_ptr(mousePos));

    glUniform1f(curlTime, float(t));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, image);

    glBindVertexArray(plane);

    glDrawElements(GL_TRIANGLES, indexesSize, GL_UNSIGNED_INT, 0);
}

void setupFinalResources() {
    image = loadTexture("Candle.png", GL_RGBA, 4);

    cameraPosition = glm::vec3(0, 2, 0);

    static float pitch = 90.0f;

    glm::vec3 euler(glm::radians(pitch), 0, 0);
    glm::quat q(euler);
    cameraPosition += q * glm::vec3(1,1,1);

    cameraUp = q * glm::vec3(0, 1, 0);
    cameraForward = q * glm::vec3(0, 0, 1);

    plane = GeneratePlane(1000, 1000, 1.0f, 1.0f, indexesSize);
    /*
    float vertices[] = {
        // positions            //colors            // tex coords   // normals
        0.5f, -0.5f, -0.5f,     0.0f, 1.0f, 1.0f,   1.f, 0.f,       0.f, -1.f, 0.f,
        0.5f, -0.5f, 0.5f,      0.0f, 1.0f, 1.0f,   1.f, 1.f,       0.f, -1.f, 0.f,
        -0.5f, -0.5f, 0.5f,     0.0f, 1.0f, 1.0f,   0.f, 1.f,       0.f, -1.f, 0.f,
        -0.5f, -0.5f, -.5f,     0.0f, 1.0f, 1.0f,   0.f, 0.f,       0.f, -1.f, 0.f ,
        
        0.5f, 0.5f, -0.5f,      1.0f, 0.0f, 1.0f,   2.f, 0.f,       1.f, 0.f, 0.f,
        0.5f, 0.5f, 0.5f,       1.0f, 0.0f, 1.0f,   2.f, 1.f,       1.f, 0.f, 0.f,

        0.5f, 0.5f, 0.5f,       1.0f, 1.0f, 0.0f,   1.f, 2.f,       0.f, 0.f, 1.f,
        -0.5f, 0.5f, 0.5f,      1.0f, 1.0f, 0.0f,   0.f, 2.f,       0.f, 0.f, 1.f,

        -0.5f, 0.5f, 0.5f,      1.0f, 1.0f, 0.5f,   -1.f, 1.f,      -1.f, 0.f, 0.f,
        -0.5f, 0.5f, -.5f,      1.0f, 1.0f, 0.5f,   -1.f, 0.f,      -1.f, 0.f, 0.f,

        -0.5f, 0.5f, -.5f,      1.0f, 0.5f, 1.0f,   0.f, -1.f,      0.f, 0.f, -1.f,
        0.5f, 0.5f, -0.5f,      1.0f, 0.5f, 1.0f,   1.f, -1.f,      0.f, 0.f, -1.f,

        -0.5f, 0.5f, -.5f,      0.5f, 1.0f, 1.0f,   3.f, 0.f,       0.f, 1.f, 0.f,
        -0.5f, 0.5f, 0.5f,      0.5f, 1.0f, 1.0f,   3.f, 1.f,       0.f, 1.f, 0.f,

        0.5f, -0.5f, 0.5f,      0.7f, 1.0f, 1.0f,   1.f, 1.f,       0.f, 0.f, 1.f,
        -0.5f, -0.5f, 0.5f,     0.7f, 1.0f, 1.0f,   0.f, 1.f,       0.f, 0.f, 1.f,

        -0.5f, -0.5f, 0.5f,     1.0f, 0.7f, 1.0f,   0.f, 1.f,       -1.f, 0.f, 0.f,
        -0.5f, -0.5f, -.5f,     1.0f, 0.7f, 1.0f,   0.f, 0.f,       -1.f, 0.f, 0.f,

        -0.5f, -0.5f, -.5f,     1.0f, 1.0f, 0.7f,   0.f, 0.f,       0.f, 0.f, -1.f,
        0.5f, -0.5f, -0.5f,     1.0f, 1.0f, 0.7f,   1.f, 0.f,       0.f, 0.f, -1.f,

        0.5f, -0.5f, -0.5f,     1.0f, 1.0f, 1.0f,   1.f, 0.f,       1.f, 0.f, 0.f,
        0.5f, -0.5f, 0.5f,      1.0f, 1.0f, 1.0f,   1.f, 1.f,       1.f, 0.f, 0.f,

        0.5f, 0.5f, -0.5f,      0.0f, 0.0f, 0.0f,   2.f, 0.f,       0.f, 1.f, 0.f,
        0.5f, 0.5f, 0.5f,       0.0f, 0.0f, 0.0f,   2.f, 1.f,       0.f, 1.f, 0.f
    };

    unsigned int indexes[] = {
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
    indexesSize = sizeof(indexes);

    //generate vertex array and buffer
    glGenVertexArrays(1, &VAO);
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    unsigned int EBO;
    glGenBuffers(1, &EBO);

    //bind vertex array and buffer
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    // 0. copy our vertices array in a buffer for OpenGL to use
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexes), indexes, GL_STATIC_DRAW);

    // 1. then set the vertex attributes pointers

    int stride = 11 * sizeof(float); //Move the size of 11 floats to start of next vertice
    //pos
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);
    //colour
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);
    //uv
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * 6));
    glEnableVertexAttribArray(2);
    //normal
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * 8));
    glEnableVertexAttribArray(3);
    */
    ///shaders
    unsigned int vertCurl, fragCurl;

    CreateShader("vertCurl.shader", GL_VERTEX_SHADER, vertCurl);
    CreateShader("fragCurl.shader", GL_FRAGMENT_SHADER, fragCurl);


    //create program to hold shaders
    myProgram = glCreateProgram();
    glAttachShader(myProgram, vertCurl);
    glAttachShader(myProgram, fragCurl);

    glLinkProgram(myProgram);

    glDeleteShader(vertCurl);
    glDeleteShader(fragCurl);

    glUseProgram(myProgram);
    glUniform1i(glGetUniformLocation(myProgram, "image"), 0);

    view = glm::lookAt(cameraPosition, cameraPosition + cameraForward, cameraUp);
    projection = glm::perspective(glm::radians(65.0f), width / (float)height, 0.1f, 1000.0f);
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
