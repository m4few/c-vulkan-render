#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include "vulkanInstance.h"
#include "vulkanInstanceHelper.h"

// the indicies of various queue families
// think of it as a hashmap
typedef struct {
  uint32_opt graphicsQueue;
  uint32_opt presentationQueue;
} QueueFamilyIndices;

extern const uint8_t QUEUE_COUNT;
extern const QueueFamilyIndices QUEUE_INDICES_DEFAULT;

bool deviceHasAllQueues(QueueFamilyIndices indices);

bool deviceHasAllExtensions(VkPhysicalDevice *device);

bool deviceHasUsableSwapchain(VkPhysicalDevice *device, VkSurfaceKHR *surface);

// find queue families with various bits enabled
QueueFamilyIndices findQueueFamilies(VkPhysicalDevice *device,
                                     VkSurfaceKHR *surface);

// a device is usable if it has a graphics queue
bool deviceIsUsable(VkPhysicalDevice *device, VkSurfaceKHR *surface);

VkPhysicalDevice deviceChoose(VkInstance *instance, VkSurfaceKHR *surface);
