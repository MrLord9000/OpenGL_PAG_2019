#pragma once

#include "Shader.h"

int initEverything(GLFWwindow** window);

void createMenger(glm::mat4 &model, glm::vec3 cubePositions[], Shader &shaderProgram, int iteration, int iterations);
void createCarpet(std::vector<float>& vertices, std::vector<unsigned int>& indices, int iterations);

void carpetFunction(std::vector<float>& vertices, std::vector<unsigned int>& indices, float width, float height, float x, float y, int iteration, int iterations);

void processInput(GLFWwindow * window);
void framebuffer_size_callback(GLFWwindow * window, int width, int height);

void createQuad2D(float x, float y, float width, float height, std::vector<float> &vertices, std::vector<unsigned int> &indices);
