#pragma once
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "vulkanInstanceHelper.h"

const char* const VALIDATION_LAYERS = {"VK_LAYER_KHRONOS_validation"};
uint8_t VALIDATION_LAYER_COUNT = 1;

#ifdef DEBUG
  bool ENABLE_VALIDATION = true;
#else
  bool ENABLE_VALIDATION = false;
#endif // DEBUG


// check if all the validation layers actually exist
bool validationSupported(){
  // get the number of layers
  uint32_t realLayerCount;
  vkEnumerateInstanceLayerProperties(&realLayerCount, NULL);
  
  // get a list of supported layers
  VkLayerProperties layers[realLayerCount];
  vkEnumerateInstanceLayerProperties(&realLayerCount, layers);
  

  // make sure all layers are supported 
  for(uint8_t i = 0; i < VALIDATION_LAYER_COUNT; i++){
    
    bool found = false;
    for(uint8_t j = 0; j < realLayerCount; j++){
      
      if(strEq(&VALIDATION_LAYERS[i], layers[j].layerName)){
        found = true;
        break;
      }
    }

    if(found == false){
      return false;
    }
  }

  return true;
}


// NOTE: INITIALISE ALL STRUCTS TO SET ALL VALUES TO 0, THIS PREVENTS SEGFAULTS
int vulkanInit(VkInstance instance){
  // check for validation layer support
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

  // add validation information to vkCreateInfo, or don't
  if(ENABLE_VALIDATION = true){
    if(!validationSupported()){
      printf("WARNING: VALIDATION LAYERS NOT SUPPORTED, SKIPPING");
    
    }else{
      printf("NOTE: VALIDATION LAYERS ARE ON");
      uint8_t realLayerCount;
      vkCreateInfo.enabledLayerCount = VALIDATION_LAYER_COUNT;
      vkCreateInfo.ppEnabledLayerNames = &VALIDATION_LAYERS;
    }
  }
  
  // this creates the instance
  // (info, custom callback, ptr to new instance)
  VkInstance* instPtr = &instance;
  VkResult result = vkCreateInstance(&vkCreateInfo, NULL, instPtr);

  if(result != VK_SUCCESS) {
    printf("ERR: vulkanInit failed",NULL,2); 
  }
  
  return EXIT_SUCCESS;

}


int vulkanCleanup(GLFWwindow* window, VkInstance instance) {
    vkDestroyInstance(instance, NULL);
    windowExit(window); 
    return EXIT_SUCCESS;
}
