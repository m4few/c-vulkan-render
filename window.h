#pragma once

#define GLFW_INLCUDE_VULKAN
#include<GLFW/glfw3.h>
#include<stdlib.h>
#include<stdio.h>


void glfwCallback(int err, const char* msg){
  printf(" [%d] %s\n", err,msg);
}


GLFWwindow* windowInit(uint16_t winX, uint16_t winY){
 
  glfwSetErrorCallback(glfwCallback);
  glfwInit();
  

  if(!glfwInit()){
    printf("ITS BROKEN AGAIN SEE window.h\n"); 
  }

  //extra window constnats
  void* MONITOR = NULL;
  char TITLE[] = "Cereal";

  // start glfw and set constants
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  
  //return gflw window pointer, the last parameter is an opengl junk value
  GLFWwindow* win = glfwCreateWindow(winX, winY, TITLE, MONITOR, NULL); 
  return win;
}


int windowExit(GLFWwindow* window){
  glfwDestroyWindow(window);
  glfwTerminate();
  return EXIT_SUCCESS;
}

