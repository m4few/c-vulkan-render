#pragma once
#include <stdlib.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include "vulkanInstance.h"
#include "vulkanInstancePhysicalDevice.h"

VkDevice deviceCreateLogical(VkPhysicalDevice physicalDevice) {
  VkDevice device;
  QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

  // queue creation info
  VkDeviceQueueCreateInfo queueCreateInfo = {};
  queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queueCreateInfo.queueFamilyIndex = indices.graphicsQueue.value;
  queueCreateInfo.queueCount = 1;

  float queuePriority = 1.0f;
  queueCreateInfo.pQueuePriorities = &queuePriority;

  // enabled logical device feature info
  VkPhysicalDeviceFeatures deviceFeatures = {};

  // create logical device
  VkDeviceCreateInfo logicalDeviceCreateInfo = {};

  logicalDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  logicalDeviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
  logicalDeviceCreateInfo.queueCreateInfoCount = 1;
  logicalDeviceCreateInfo.pEnabledFeatures = &deviceFeatures;
  logicalDeviceCreateInfo.enabledExtensionCount = 0;

  // validation
  if (ENABLE_VALIDATION == true) {
    logicalDeviceCreateInfo.enabledLayerCount =
        1; // TODO: get actual number of validation layers
    logicalDeviceCreateInfo.ppEnabledLayerNames = &VALIDATION_LAYERS;
  } else {
    logicalDeviceCreateInfo.enabledLayerCount = 0;
  }

  if (vkCreateDevice(physicalDevice, &logicalDeviceCreateInfo, NULL, &device) !=
      VK_SUCCESS) {
    printf("COULD NOT CREATE THE LOGICAL DEVICE!!");
  }

  return device;
}
