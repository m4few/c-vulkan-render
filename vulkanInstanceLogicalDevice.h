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
                             VkSurfaceKHR *surface) {
  VkDevice device;
  QueueFamilyIndices indices = findQueueFamilies(physicalDevice, surface);

  uint8_t adjustedQueueCount = 0;
  uint32_t uniqueQueueIndices[QUEUE_COUNT];
  for (uint8_t i = 0; i < QUEUE_COUNT; i++) {
    uint32_t currentIndex = (&(indices.graphicsQueue) + 1)->value;

    // check previous to ensure uniqueness
    bool duplicateFound = false;
    for (uint8_t j = 0; j < i; j++) {
      if (uniqueQueueIndices[j] == currentIndex) {
        duplicateFound = true;
        break;
      }
    }

    // if unique count and add to array
    if (!duplicateFound) {
      uniqueQueueIndices[i] = currentIndex;
      adjustedQueueCount++;
    }
  }

  // queue creation info
  VkDeviceQueueCreateInfo creationInfos[adjustedQueueCount];
  for (uint8_t i = 0; i < adjustedQueueCount; i++) {
    VkDeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = uniqueQueueIndices[i];
    queueCreateInfo.queueCount = 1;
    float queuePriority = 1.0f;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    creationInfos[i] = queueCreateInfo;
  }

  // enabled logical device feature info
  VkPhysicalDeviceFeatures deviceFeatures = {};

  // create logical device
  VkDeviceCreateInfo logicalDeviceCreateInfo = {};

  logicalDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  logicalDeviceCreateInfo.pQueueCreateInfos = creationInfos;
  logicalDeviceCreateInfo.queueCreateInfoCount = adjustedQueueCount;
  logicalDeviceCreateInfo.pEnabledFeatures = &deviceFeatures;

  // extensions NOTE: WEIRD CAST HERE, MAYBE COULD CAUSE PROBLEMS LATER
  logicalDeviceCreateInfo.enabledExtensionCount = DEVICE_EXTENSION_COUNT;
  logicalDeviceCreateInfo.ppEnabledExtensionNames = &DEVICE_EXTENSIONS;

  // validation
  if (ENABLE_VALIDATION == true) {
    logicalDeviceCreateInfo.enabledLayerCount =
        1; // TODO: get actual number of validation layers
    logicalDeviceCreateInfo.ppEnabledLayerNames = &VALIDATION_LAYERS;
  } else {
    logicalDeviceCreateInfo.enabledLayerCount = 0;
  }

  if (vkCreateDevice(*physicalDevice, &logicalDeviceCreateInfo, NULL,
                     &device) != VK_SUCCESS) {
    printf("COULD NOT CREATE THE LOGICAL DEVICE!!");
  }

  return device;
}

QueueHandles deviceGetQueueHandles(VkPhysicalDevice *pDevice, VkDevice *lDevice,
                                   VkSurfaceKHR *surface) {
  // make struct and get indices
  QueueHandles handles = {};
  QueueFamilyIndices indices = findQueueFamilies(pDevice, surface);

  // make queues
  VkQueue graphicsQueue;
  vkGetDeviceQueue(*lDevice, indices.graphicsQueue.value, 0, &graphicsQueue);

  VkQueue presentationQueue;
  vkGetDeviceQueue(*lDevice, indices.presentationQueue.value, 0,
                   &presentationQueue);

  // put in struct and return
  handles.graphicsQueue = graphicsQueue;
  handles.presentationQueue = presentationQueue;

  return handles;
}
