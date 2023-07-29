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
#include "vulkanSwapchain.h"

// the indicies of various queue families
// think of it as a hashmap
typedef struct {
  uint32_opt graphicsQueue;
  uint32_opt presentationQueue;
} QueueFamilyIndices;

const uint8_t QUEUE_COUNT = 2;
const QueueFamilyIndices QUEUE_INDICES_DEFAULT = {{false, 0}};

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

DeviceSwapchainInfo deviceSwapchainCreate(GLFWwindow *window,
                                          VkSurfaceKHR *surface,
                                          VkPhysicalDevice *pDevice,
                                          VkDevice *device) {

  DeviceSwapchainInfo swapInfo = {};

  QueueFamilyIndices indices = findQueueFamilies(pDevice, surface);
  SwapchainDetails swapchainSupport = swapchainGetSupport(*pDevice, *surface);
  VkSurfaceFormatKHR surfaceFormat = swapchainChooseFormat(swapchainSupport);
  VkPresentModeKHR presentMode = swapchainChoosePresentMode(swapchainSupport);
  VkExtent2D extent = swapchainChooseSwapExtent(window, swapchainSupport);

  // NOTE: THIS IS AN ISSUE OF SOME DESCRIPTION, MIN BIGGER THAN MAX
  // THIS COULD CAUSE ISSUES IN THE FUTURE (IE WITH OTHER DEVICES)
  uint32_t imageCount = swapchainSupport.surfCapabilities.minImageCount + 1;
  if (swapchainSupport.surfCapabilities.minImageCount > 0 &&
      imageCount > swapchainSupport.surfCapabilities.maxImageCount &&
      swapchainSupport.surfCapabilities.maxImageCount != 0) {
    imageCount = swapchainSupport.surfCapabilities.maxImageCount;
  }

  VkSwapchainCreateInfoKHR createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  createInfo.surface = *surface;
  createInfo.minImageCount = imageCount;
  createInfo.imageFormat = surfaceFormat.format;
  createInfo.imageColorSpace = surfaceFormat.colorSpace;
  createInfo.imageExtent = extent;
  createInfo.imageArrayLayers = 1;
  createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  if (indices.graphicsQueue.value != indices.presentationQueue.value) {
    createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    createInfo.queueFamilyIndexCount = 2;

    uint32_t queueIndices[] = {indices.graphicsQueue.value,
                               indices.presentationQueue.value};

    createInfo.pQueueFamilyIndices = queueIndices;

  } else {
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0;
    createInfo.pQueueFamilyIndices = NULL;
  }

  createInfo.preTransform = swapchainSupport.surfCapabilities.currentTransform;
  createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  createInfo.presentMode = presentMode;
  createInfo.clipped = VK_TRUE;
  createInfo.oldSwapchain = VK_NULL_HANDLE;

  VkSwapchainKHR swapchain;
  if (vkCreateSwapchainKHR(*device, &createInfo, NULL, &swapchain) !=
      VK_SUCCESS) {
    printf("%s\n", "failed to create swapchain!");
  }

  swapInfo.swapchain = swapchain;
  swapInfo.details = swapchainSupport;
  swapInfo.imageCount = imageCount;
  swapInfo.imageHandles = swapchainGetImageHandles(*device, &swapInfo);

  return swapInfo;
}
