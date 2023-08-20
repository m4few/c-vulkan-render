#include <GLFW/glfw3.h>
#include <stdio.h>
#include <vulkan/vulkan.h>

#include "vulkanInstance.h"
#include "window.h"

int main() {
  // start window
  glfwInit();
  GLFWwindow *RENDER_WINDOW = windowInit(400, 400);

  // start vulkan instance
  VkInstance instance;
  vulkanInit(&instance, RENDER_WINDOW);

  // while the exit flag is down, wait
  while (!glfwWindowShouldClose(RENDER_WINDOW)) {
    glfwPollEvents();
  }

  return windowExit(RENDER_WINDOW);
}
