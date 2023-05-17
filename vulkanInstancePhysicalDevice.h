#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <vulkan/vulkan.h>

#include "vulkanInstanceHelper.h"

// the indicies of various queue families
// think of it as a hashmap
typedef struct {
  uint32_opt graphicsQueue;
} QueueFamilyIndices;

bool hasAllQueues(QueueFamilyIndices indices) {
  bool hasQs = true;
  if (indices.graphicsQueue.exists == false) {
    hasQs = false;
  }

  return hasQs;
}

const QueueFamilyIndices QUEUE_INDICES_DEFAULT = {{false, 0}};

// find queue families with various bits enabled
QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
  QueueFamilyIndices indices = QUEUE_INDICES_DEFAULT;

  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, NULL);
  VkQueueFamilyProperties queueFamilies[queueFamilyCount];
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
                                           queueFamilies);

  // look for a graphics queue
  for (uint32_t i = 0; i < queueFamilyCount; i++) {
    if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      indices.graphicsQueue.value = i;
      indices.graphicsQueue.exists = true;
    }

    if (hasAllQueues(indices)) {
      break;
    }
  }

  return indices;
}

// a device is usable if it has a graphics queue
bool deviceIsUsable(VkPhysicalDevice device) {
  QueueFamilyIndices indices = findQueueFamilies(device);
  return hasAllQueues(indices); // see the todo
}

// TODO: FAVOUR DISCRETE GPUS OVER INTEGRATED INSTEAD OF JUST CHOOSING THE FIRST
// ONE FOUND DO THIS BY RANKING DEVICE SUITABILITY
VkPhysicalDevice deviceChoose(VkInstance instance) {

  uint32_t deviceCount = 0;
  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
  vkEnumeratePhysicalDevices(instance, &deviceCount, NULL); // iterate and count

  // make sure a physical device exists
  if (deviceCount == 0) {
    printf("ERR: no GPUs with vulkan support\n");
  }

  VkPhysicalDevice devices[deviceCount];
  vkEnumeratePhysicalDevices(instance, &deviceCount,
                             devices); // iterate, count and put in devices

  // set the first usable device as the main device
  for (uint32_t i; i < deviceCount; i++) {
    if (deviceIsUsable(devices[i])) {
      physicalDevice = devices[i];
      break;
    }
  }

  if (physicalDevice == VK_NULL_HANDLE) {
    printf("ERR: no usable devices found\n");
  }

  return physicalDevice;
}
