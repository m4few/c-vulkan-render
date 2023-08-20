#pragma once

#include <vulkan/vulkan_core.h>
#define GLFW_INLCUDE_VULKAN
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

void glfwCallback(int err, const char *msg);

GLFWwindow *windowInit(uint16_t winX, uint16_t winY);

VkSurfaceKHR windowCreateSurface(VkInstance *instance, GLFWwindow *window);

int windowExit(GLFWwindow *window);
