#pragma once
#include <GLFW/glfw3.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include "vulkanInstanceLogicalDevice.h"
#include "vulkanInstancePhysicalDevice.h"
#include "vulkanSwapchain.h"
#include "window.h"

extern const char VALIDATION_LAYERS_ARR[1][256];
extern const char *const VALIDATION_LAYERS;
extern const uint8_t VALIDATION_LAYER_COUNT;

extern const char DEVICE_EXTENSIONS_ARR[1][256];
extern const char *const DEVICE_EXTENSIONS;
extern const uint8_t DEVICE_EXTENSION_COUNT;

extern const bool ENABLE_VALIDATION;

bool validationSupported();

int vulkanMakeInstance(VkInstance *instance);

int vulkanInit(VkInstance *instance, GLFWwindow *window);

int vulkanCleanup(GLFWwindow *window, VkInstance *instance, VkDevice *lDevice,
                  VkSurfaceKHR *surface, VkSwapchainKHR *swapchain);
