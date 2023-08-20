#pragma once
#include "vulkanInstancePhysicalDevice.h"

bool deviceHasAllQueues(QueueFamilyIndices indices) {
  return indices.graphicsQueue.exists && indices.presentationQueue.exists;
}

bool deviceHasAllExtensions(VkPhysicalDevice *device) {
  uint32_t extensionCount;
  vkEnumerateDeviceExtensionProperties(*device, NULL, &extensionCount, NULL);

  VkExtensionProperties currentDeviceExtensions[extensionCount];
  vkEnumerateDeviceExtensionProperties(*device, NULL, &extensionCount,
                                       currentDeviceExtensions);

  // ensure all required extensions are supported by the candidate device
  // TODO: MAKE THIS SEARCH FASTER
  uint8_t found = 0; // NOTE: THERE IS POTENTIAL FOR ERROR HERE
  for (uint8_t i = 0; i < DEVICE_EXTENSION_COUNT; i++) {
    for (uint32_t j = 0; j < extensionCount; j++) {
      if (strcmp(DEVICE_EXTENSIONS_ARR[i],
                 currentDeviceExtensions[j].extensionName) == 0) {
        found++;
        break;
      }
    }
  }

  // TODO: clean this up
  if (found < DEVICE_EXTENSION_COUNT) {
    return false;
  }
  return true;
}

bool deviceHasUsableSwapchain(VkPhysicalDevice *device, VkSurfaceKHR *surface) {
  SwapchainDetails details = swapchainGetSupport(*device, *surface);
  return details.surfFormatCount > 0 && details.presentModeCount > 0;
}

// find queue families with various bits enabled
QueueFamilyIndices findQueueFamilies(VkPhysicalDevice *device,
                                     VkSurfaceKHR *surface) {
  QueueFamilyIndices indices = QUEUE_INDICES_DEFAULT;

  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(*device, &queueFamilyCount, NULL);

  VkQueueFamilyProperties queueFamilies[queueFamilyCount];
  vkGetPhysicalDeviceQueueFamilyProperties(*device, &queueFamilyCount,
                                           queueFamilies);

  // look for a graphics queue and a presentation queue
  for (uint32_t i = 0; i < queueFamilyCount; i++) {
    // graphics queue
    if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      uint32_optSet(&indices.graphicsQueue, i);
    }

    // presentation queue
    VkBool32 presExists;
    vkGetPhysicalDeviceSurfaceSupportKHR(*device, i, *surface, &presExists);
    if (presExists) {
      uint32_optSet(&indices.presentationQueue, i);
    }

    // make sure all queues exist
    if (deviceHasAllQueues(indices)) {
      break;
    }
  }

  return indices;
}

// a device is usable if it has a graphics queue
bool deviceIsUsable(VkPhysicalDevice *device, VkSurfaceKHR *surface) {
  QueueFamilyIndices indices = findQueueFamilies(device, surface);
  return deviceHasAllQueues(indices) && deviceHasAllExtensions(device) &&
         deviceHasUsableSwapchain(device, surface);
}

// TODO: FAVOUR DISCRETE GPUS OVER INTEGRATED INSTEAD OF JUST CHOOSING THE FIRST
// ONE FOUND DO THIS BY RANKING DEVICE SUITABILITY
VkPhysicalDevice deviceChoose(VkInstance *instance, VkSurfaceKHR *surface) {
  uint32_t deviceCount = 0;
  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
  vkEnumeratePhysicalDevices(*instance, &deviceCount,
                             NULL); // iterate and count

  // make sure a physical device exists
  if (deviceCount == 0) {
    printf("ERR: no GPUs with vulkan support\n");
  }

  VkPhysicalDevice devices[deviceCount];
  vkEnumeratePhysicalDevices(*instance, &deviceCount,
                             devices); // iterate, count and put in devices

  // set the first usable device as the main device
  for (uint32_t i; i < deviceCount; i++) {
    if (deviceIsUsable(&devices[i], surface)) {
      physicalDevice = devices[i];
      break;
    }
  }

  if (physicalDevice == VK_NULL_HANDLE) {
    printf("ERR: no usable devices found\n");
  }

  return physicalDevice;
}