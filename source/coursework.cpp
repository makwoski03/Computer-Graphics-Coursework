#include <iostream>
#include <cmath>

// OpenGL
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Project Headers
#include <common/shader.hpp>
#include <common/texture.hpp>
#include <common/maths.hpp>
#include <common/camera.hpp>
#include <common/model.hpp>
#include <common/light.hpp>

// Function prototypes
void keyboardInput(GLFWwindow* window);

// Floor Vertex Data
float floorVertices[] = {
    // positions         // tex coords
    -5.0f, 0.0f,  5.0f,   0.0f, 1.0f,  // Bottom-left
     5.0f, 0.0f,  5.0f,   1.0f, 1.0f,  // Bottom-right
     5.0f, 0.0f, -5.0f,   1.0f, 0.0f,  // Top-right
    -5.0f, 0.0f, -5.0f,   0.0f, 0.0f   // Top-left
};

unsigned int floorIndices[] = {
    0, 1, 2,
    2, 3, 0
};

// Wall Vertex Data
float wallVertices[] = {
    // positions       // texture coords
    -5.0f, 0.0f, -5.0f,  0.0f, 0.0f, // Bottom-left
     5.0f, 0.0f, -5.0f,  1.0f, 0.0f, // Bottom-right
     5.0f, 5.0f, -5.0f,  1.0f, 1.0f, // Top-right
    -5.0f, 5.0f, -5.0f,  0.0f, 1.0f  // Top-left
};

unsigned int wallIndices[] = {
    0, 1, 2,
    2, 3, 0
};


int main()
{
    // =========================================================================
    // Window creation - you shouldn't need to change this code
    // -------------------------------------------------------------------------
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }


    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1024, 768, "Computer Graphics Coursework", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to open GLFW window." << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // Enable Depth Testing
    glEnable(GL_DEPTH_TEST);

    // Load Shaders
    Shader shader("vertexShader.glsl", "fragmentShader.glsl");

    // Load Floor Texture
    unsigned int floorTexture = loadTexture("assets/floor/floor_diffuse.jpg");

    //Load the wall
    unsigned int wallTexture = loadTexture("assets/wall/wall_texture.jpg");


    // Generate VAO, VBO, EBO
    unsigned int floorVAO, floorVBO, floorEBO;
    glGenVertexArrays(1, &floorVAO);
    glGenBuffers(1, &floorVBO);
    glGenBuffers(1, &floorEBO);

    glBindVertexArray(floorVAO);

    glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), floorVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floorEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(floorIndices), floorIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    unsigned int wallVAO, wallVBO, wallEBO;
    glGenVertexArrays(1, &wallVAO);
    glGenBuffers(1, &wallVBO);
    glGenBuffers(1, &wallEBO);

    glBindVertexArray(wallVAO);

    glBindBuffer(GL_ARRAY_BUFFER, wallVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(wallVertices), wallVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, wallEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(wallIndices), wallIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);


    // Render loop
    while (!glfwWindowShouldClose(window)) {
        // Input handling
        keyboardInput(window);

        // Clear the screen
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Activate Shader
        shader.use();

        // Projection and View Matrices
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1024.0f / 768.0f, 0.1f, 100.0f);
        shader.setMat4("projection", glm::value_ptr(projection));

        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, -10.0f));
        shader.setMat4("view", glm::value_ptr(view));

        // Floor Model Matrix
        glm::mat4 model = glm::mat4(1.0f);
        shader.setMat4("model", glm::value_ptr(model));

        // Bind Floor Texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, floorTexture);
        shader.setInt("texture1", 0);

        // Draw Floor
        glBindVertexArray(floorVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();

        // Render Wall
        shader.use();
        glm::mat4 wallModel = glm::mat4(1.0f);
        wallModel = glm::translate(wallModel, glm::vec3(0.0f, 0.0f, -5.0f)); // Adjust position as needed
        shader.setMat4("model", glm::value_ptr(wallModel));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, wallTexture);
        shader.setInt("texture1", 0);

        glBindVertexArray(wallVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

    }

    // Clean up
    glDeleteVertexArrays(1, &floorVAO);
    glDeleteBuffers(1, &floorVBO);
    glDeleteBuffers(1, &floorEBO);

    glDeleteVertexArrays(1, &wallVAO);
    glDeleteBuffers(1, &wallVBO);
    glDeleteBuffers(1, &wallEBO);

    glfwTerminate();
    return 0;
}



void keyboardInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
