#pragma once
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>

// NOTE: INITIALISE ALL STRUCTS TO SET ALL VALUES TO 0, THIS PREVENTS SEGFAULTS
int vulkanInit(VkInstance instance){
  // this stuff is all optional, don't worry about it
  VkApplicationInfo vkInfo = {};
 
  vkInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  vkInfo.pApplicationName = "Cereal render";
  vkInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  vkInfo.pEngineName = "No Engine";
  vkInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  vkInfo.apiVersion = VK_API_VERSION_1_0;
 
  // this stuff is not optional
  VkInstanceCreateInfo vkCreateInfo = {};

  uint32_t GLFW_EXT_COUNT = 0;
  int ENABLED_LAYERS = 0;
  const char** GLFW_EXTENSIONS = glfwGetRequiredInstanceExtensions(&GLFW_EXT_COUNT);

  vkCreateInfo.enabledExtensionCount = GLFW_EXT_COUNT;
  vkCreateInfo.ppEnabledExtensionNames = GLFW_EXTENSIONS; 
  vkCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  vkCreateInfo.pApplicationInfo = &vkInfo;

  // this creates the instance
  // (info, custom callback, ptr to new instance)
  VkInstance* instPtr = &instance;
  VkResult result = vkCreateInstance(&vkCreateInfo, NULL, instPtr);

  if(result != VK_SUCCESS) {
    printf("ERR: vulkanInit failed",NULL,2); 
  }
  
  return EXIT_SUCCESS;

}
