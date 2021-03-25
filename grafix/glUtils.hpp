#pragma once
#include "utils.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window, Player& player);

double contentScale = 2;

const char* vertexShaderSource = R"glsl(
    #version 430 core
    layout (location = 0) in vec2 vertex;
    layout (location = 1) uniform mat3 transform;
    layout (location = 2) uniform mat3 camera;
    void main() {
        gl_Position = vec4((camera*transform*vec3(vertex.xy, 1)).xy, 0, 1);
    }
)glsl";

const char* fragmentShaderSource = R"glsl(
    #version 430 core
    out vec4 FragColor;
    layout (location = 3) uniform vec4 color;
    void main() {
        FragColor = color;
    }
)glsl";

inline GLuint compileShader(const char* shaderSource, GLenum type) {
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

inline GLFWwindow* glInit() {
    // initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

    // create window
    const double screenScale = 0.75;
    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    const unsigned int SCR_WIDTH = static_cast<unsigned int>(mode->width * screenScale);
    const unsigned int SCR_HEIGHT = static_cast<unsigned int>(mode->height * screenScale);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "GRAFIX", NULL, NULL);
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