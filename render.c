#include <GLFW/glfw3.h>
#include "window.h"
#include <stdio.h>

int main(){
  glfwInit();

  // createwindow
  GLFWwindow* RENDER_WINDOW = windowInit(400,400);

  //while the exit flag is down, wait
  while(!glfwWindowShouldClose(RENDER_WINDOW)){
    glfwPollEvents();
  }


  return windowExit(RENDER_WINDOW);
}

