#include "polygon.hpp"
#include "player.hpp"
#include "entities.hpp"
#include "utils.hpp"
#include "physics.hpp"
#include "glUtils.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <iostream>

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
    double dtGoal = 5e-3;
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

        double repetitions = std::ceil(dt / dtGoal);
        for (size_t i = 0; i < repetitions; i++) {
            double pdt = dt / repetitions;
            physicsUpdate(entityPs, pdt);
        }

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
