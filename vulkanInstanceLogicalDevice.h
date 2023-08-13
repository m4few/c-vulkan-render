#pragma once
#include <stdlib.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include "vulkanInstance.h"
#include "vulkanInstancePhysicalDevice.h"

// NOTE: THERES A HACK HERE TO ACCESS THESE STRUCTS AS ARRAYS
//  IF ANYTHING WEIRD HAPPENS, ITS PROBABLY THAT
typedef struct {
  VkQueue graphicsQueue;
  VkQueue presentationQueue;
} QueueHandles;

VkDevice deviceCreateLogical(VkPhysicalDevice *physicalDevice,
                             VkSurfaceKHR *surface);

QueueHandles deviceGetQueueHandles(VkPhysicalDevice *pDevice, VkDevice *lDevice,
                                   VkSurfaceKHR *surface);
