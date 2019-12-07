#pragma once

#include "Shader.h"
#include "Camera.h"

int initEverything(GLFWwindow** window);

void processInput(GLFWwindow * window, Camera &camera);
void framebuffer_size_callback(GLFWwindow * window, int width, int height);