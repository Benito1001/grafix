#include "polygon.h"
#include "player.h"
#include "entities.h"
#include "utils.h"
#include "physics.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <iostream>

void framebufferSizeCallback(GLFWwindow *window, int width, int height);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window, Player &player);

double contentScale = 2;

const char *vertexShaderSource = R"glsl(
    #version 430 core
    layout (location = 0) in vec2 vertex;
    layout (location = 1) uniform mat3 transform;
    layout (location = 2) uniform mat3 camera;
    void main() {
        gl_Position = vec4((camera*transform*vec3(vertex.xy, 1)).xy, 0, 1);
    }
)glsl";

const char *fragmentShaderSource = R"glsl(
    #version 430 core
    out vec4 FragColor;
    layout (location = 3) uniform vec4 color;
    void main() {
        FragColor = color;
    }
)glsl";

GLuint compileShader(const char *shaderSource, GLenum type) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &shaderSource, NULL);
    glCompileShader(shader);
    glCheck();

    // check for shader compile errors
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }

    return shader;
}

GLFWwindow* glInit() {
    // initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

    // create window
    const double screenScale = 0.5;
    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    const unsigned int SCR_WIDTH = static_cast<unsigned int>(mode->width*screenScale);
    const unsigned int SCR_HEIGHT = static_cast<unsigned int>(mode->height*screenScale);

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "GRAFIX", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    // load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
    }

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glCheck();

    // compile shaders
    GLint vertexShader = compileShader(vertexShaderSource, GL_VERTEX_SHADER);
    GLint fragmentShader = compileShader(fragmentShaderSource, GL_FRAGMENT_SHADER);

    // link shaders
    GLint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glCheck();

    // check for linking errors
    GLint success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glCheck();

    glUseProgram(shaderProgram);
    glCheck();

    glfwSetScrollCallback(window, scrollCallback);

    return window;
}

int main() {
    GLFWwindow* window = glInit();

    std::vector<Polygon> entities;
    Player player = getPlayerAndEntities(entities, contentScale);

    std::vector<Polygon*> entityPs;
    entityPs.push_back(&player);
    for (size_t i = 0; i < entities.size(); i++) {
        entityPs.push_back(&entities[i]);
    }

    int avgCounter = 30;
    int frameCount = 0;
    double dt = 1.0/60;
    double dtGoal = 1e-4;
    double avgdt = dt*avgCounter;
    double time = getTime() - dt;

    // render loop
    while (!glfwWindowShouldClose(window)) {
        // calculate dt and print it
        dt = getTime() - time;
        time = getTime();
        if (frameCount % avgCounter != 0) {
            avgdt += dt;
        } else {
            //rprint("frame time = " << avgdt/avgCounter*1000 << " ms");
            rprint("frame rate = " << 1/(avgdt / avgCounter) << " fps");
            avgdt = 0;
        }
        
        // poll events and process input
        glfwPollEvents();
        processInput(window, player);

        // render
        glClearColor(1, 1, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        glCheck();

        // squish screen so that the aspect ratio is 1:1
        GLfloat sx, sy;
        int windowHeight, windowWidth;
        glfwGetWindowSize(window, &windowWidth, &windowHeight);
        if (windowHeight > windowWidth) {
            sx = 1;
            sy = static_cast<float>(windowWidth)/windowHeight;
        } else {
            sx = static_cast<float>(windowHeight)/windowWidth;
            sy = 1;
        }
        
        // create and upload camera matrix
        GLfloat cameraMatrix[9] = {
            sx/contentScale, 0, 0,
            0, sy/contentScale, 0,
            0, 0, 1
        };
        glUniformMatrix3fv(2, 1, GL_TRUE, cameraMatrix);
        glCheck();
        physicsUpdate(entityPs, dt, dtGoal);
        
        // draw
        for (Polygon* entity: entityPs) {
            entity->draw();
        }

        // swap buffers
        glfwSwapInterval(1);
        glfwSwapBuffers(window);
        frameCount += 1;
    }

    // terminate
    glfwTerminate();
    return 0;
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    double speed = 1.0/16;
    contentScale -= yoffset*contentScale*speed;
}
    

void processInput(GLFWwindow *window, Player &player) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        player.keys[GLFW_KEY_W] = true;
    } else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_RELEASE) {
        player.keys[GLFW_KEY_W] = false;
    }
    
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        player.keys[GLFW_KEY_S] = true;
    } else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_RELEASE) {
        player.keys[GLFW_KEY_S] = false;
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        player.keys[GLFW_KEY_D] = true;
    } else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_RELEASE) {
        player.keys[GLFW_KEY_D] = false;
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        player.keys[GLFW_KEY_A] = true;
    } else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_RELEASE) {
        player.keys[GLFW_KEY_A] = false;
    }
}

// resize window
void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}