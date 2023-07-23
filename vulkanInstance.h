#pragma once
#include <GLFW/glfw3.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

// NOTE: this seems hacky, its because create structs want a weird ptr type
const char VALIDATION_LAYERS_ARR[1][256] = {"VK_LAYER_KHRONOS_validation"};
const char *const VALIDATION_LAYERS = {VALIDATION_LAYERS_ARR[0]};
const uint8_t VALIDATION_LAYER_COUNT = 1;

const char DEVICE_EXTENSIONS_ARR[1][256] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
const char *const DEVICE_EXTENSIONS = {DEVICE_EXTENSIONS_ARR[0]};
const uint8_t DEVICE_EXTENSION_COUNT = 1;

// TODO: ADD MESSAGE CALLBACK FOR DEBUG INFO
#ifdef DEBUG
const bool ENABLE_VALIDATION = true;
#else
const bool ENABLE_VALIDATION = false;
#endif // DEBUG

#include "vulkanInstance.h"
#include "vulkanInstanceHelper.h"
#include "vulkanInstanceLogicalDevice.h"
#include "vulkanInstancePhysicalDevice.h"
#include "window.h"

// check if all the validation layers actually exist
bool validationSupported() {
  // get the number of layers
  uint32_t realLayerCount;
  vkEnumerateInstanceLayerProperties(&realLayerCount, NULL);

  // get a list of supported layers
  VkLayerProperties layers[realLayerCount];
  vkEnumerateInstanceLayerProperties(&realLayerCount, layers);

  // make sure all layers are supported
  for (uint8_t i = 0; i < VALIDATION_LAYER_COUNT; i++) {

    bool found = false;
    for (uint8_t j = 0; j < realLayerCount; j++) {
      if (strcmp(VALIDATION_LAYERS_ARR[i], layers[j].layerName) == 0) {
        found = true;
        break;
      }
    }
    if (found == false) {
      return false;
    }
  }
  return true;
}

// NOTE: INITIALISE ALL STRUCTS TO SET ALL VALUES TO 0, THIS PREVENTS SEGFAULTS
int vulkanMakeInstance(VkInstance *instance) {
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
  const char **GLFW_EXTENSIONS =
      glfwGetRequiredInstanceExtensions(&GLFW_EXT_COUNT);

  vkCreateInfo.enabledExtensionCount = GLFW_EXT_COUNT;
  vkCreateInfo.ppEnabledExtensionNames = GLFW_EXTENSIONS;
  vkCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  vkCreateInfo.pApplicationInfo = &vkInfo;

  // add validation information to vkCreateInfo, or don't
  if (ENABLE_VALIDATION == true) {
    if (!validationSupported()) {
      printf("WARNING: VALIDATION LAYERS NOT SUPPORTED, SKIPPING\n");

    } else {
      printf("NOTE: VALIDATION LAYERS ARE ON\n");
      uint8_t realLayerCount;
      vkCreateInfo.enabledLayerCount = VALIDATION_LAYER_COUNT;
      vkCreateInfo.ppEnabledLayerNames = &VALIDATION_LAYERS;
    }
  }
  // this creates the instance
  // (info, custom callback, ptr to new instance)
  VkResult result = vkCreateInstance(&vkCreateInfo, NULL, instance);

  if (result != VK_SUCCESS) {
    printf("ERR: vulkanInit failed", NULL, 2);
  }

  return EXIT_SUCCESS;
}

int vulkanInit(VkInstance *instance, GLFWwindow *window) {
  vulkanMakeInstance(instance);
  // TODO: abstract this in to one function call (eg vulkanMakeDevice??)

  // make surface
  VkSurfaceKHR surface = windowCreateSurface(instance, window);

  // get real device and make logical device
  VkPhysicalDevice physicalDevice = deviceChoose(instance, &surface);
  VkDevice logicalDevice = deviceCreateLogical(&physicalDevice, &surface);

  // get graphics queue handle
  QueueHandles graphicsQueue =
      deviceGetQueueHandles(&physicalDevice, &logicalDevice, &surface);

  // make swapchain
  VkSwapchainKHR swapchain =
      deviceSwapchainCreate(window, &surface, &physicalDevice, &logicalDevice);

  return EXIT_SUCCESS;
}

int vulkanCleanup(GLFWwindow *window, VkInstance *instance, VkDevice *lDevice,
                  VkSurfaceKHR *surface, VkSwapchainKHR *swapchain) {
  vkDestroySwapchainKHR(*lDevice, *swapchain, NULL);
  vkDestroySurfaceKHR(*instance, *surface, NULL);
  vkDestroyDevice(*lDevice, NULL);
  vkDestroyInstance(*instance, NULL);
  windowExit(window);
  return EXIT_SUCCESS;
}
