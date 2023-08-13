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

bool validationSupported();

int vulkanMakeInstance(VkInstance *instance);

int vulkanInit(VkInstance *instance, GLFWwindow *window);

int vulkanCleanup(GLFWwindow *window, VkInstance *instance, VkDevice *lDevice,
                  VkSurfaceKHR *surface, VkSwapchainKHR *swapchain);
