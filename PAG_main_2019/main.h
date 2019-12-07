#pragma once

#include "Shader.h"
#include "Camera.h"

int initEverything(GLFWwindow** window);

void processInput(GLFWwindow * window, Camera &camera);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void framebuffer_size_callback(GLFWwindow * window, int width, int height);