#include "vulkanInstanceHelper.h"
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <vulkan/vulkan_core.h>

typedef struct {
  VkSurfaceCapabilitiesKHR surfCapabilities;

  VkSurfaceFormatKHR *surfFormats;
  uint32_t surfFormatCount;

  VkPresentModeKHR *presentModes;
  uint32_t presentModeCount;
} swapchainDetails;

swapchainDetails swapchainGetSupport(VkPhysicalDevice device,
                                     VkSurfaceKHR surface) {
  swapchainDetails details;

  // fill struct
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface,
                                            &details.surfCapabilities);

  vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface,
                                       &details.surfFormatCount, NULL);

  if (details.surfFormatCount != 0) {
    details.surfFormats = (VkSurfaceFormatKHR *)malloc(
        sizeof(VkSurfaceFormatKHR) * details.surfFormatCount);

    vkGetPhysicalDeviceSurfaceFormatsKHR(
        device, surface, &details.surfFormatCount, details.surfFormats);
  }

  vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface,
                                            &details.presentModeCount, NULL);

  if (details.presentModeCount != 0) {
    details.presentModes = (VkPresentModeKHR *)malloc(sizeof(VkPresentModeKHR) *
                                                      details.presentModeCount);

    vkGetPhysicalDeviceSurfacePresentModesKHR(
        device, surface, &details.presentModeCount, details.presentModes);
  }

  return details;
}

VkSurfaceFormatKHR swapchainChooseFormat(swapchainDetails details) {
  for (uint32_t i = 0; i < details.surfFormatCount; i++) {
    VkSurfaceFormatKHR surfaceFormat = details.surfFormats[i];

    if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
        surfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {

      return surfaceFormat;
    }
  }
  return details.surfFormats[0];
}

VkPresentModeKHR swapchainChoosePresentMode(swapchainDetails details) {
  for (uint32_t i = 0; i < details.presentModeCount; i++) {
    VkPresentModeKHR mode = details.presentModes[i];

    if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
      return mode;
    }
  }

  return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D swapchainChooseSwapExtent(GLFWwindow *window,
                                     swapchainDetails details) {
  if (details.surfCapabilities.currentExtent.width != UINT_MAX) {
    return details.surfCapabilities.currentExtent;
  } else {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    VkExtent2D extent = {(uint32_t)width, (uint32_t)height};

    extent.width =
        clamp(extent.width, details.surfCapabilities.minImageExtent.width,
              details.surfCapabilities.maxImageExtent.width);
    extent.height =
        clamp(extent.height, details.surfCapabilities.minImageExtent.height,
              details.surfCapabilities.maxImageExtent.height);

    return extent;
  }
}

VkSwapchainKHR swapchainCreate(GLFWwindow *window, VkSurfaceKHR *surface,
                               VkPhysicalDevice *pDevice, VkDevice *device,
                               QueueFamilyIndices *indices) {

  swapchainDetails swapchainSupport = swapchainGetSupport(*pDevice, *surface);
  VkSurfaceFormatKHR surfaceFormat = swapchainChooseFormat(swapchainSupport);
  VkPresentModeKHR presentMode = swapchainChoosePresentMode(swapchainSupport);
  VkExtent2D extent = swapchainChooseSwapExtent(window, swapchainSupport);

  uint32_t imageCount = swapchainSupport.surfCapabilities.minImageCount + 1;
  if (swapchainSupport.surfCapabilities.minImageCount > 0 &&
      imageCount > swapchainSupport.surfCapabilities.maxImageCount) {
    imageCount = swapchainSupport.surfCapabilities.maxImageCount;
  }

  // INFO: possible segfault here??
  VkSwapchainCreateInfoKHR createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  createInfo.surface = *surface;
  createInfo.minImageCount = imageCount;
  createInfo.imageFormat = surfaceFormat.format;
  createInfo.imageColorSpace = surfaceFormat.colorSpace;
  createInfo.imageExtent = extent;
  createInfo.imageArrayLayers = 1;
  createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  if (indices->graphicsQueue.value != indices->presentationQueue.value) {
    createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    createInfo.queueFamilyIndexCount = 2;

    uint32_t queueIndices[] = {indices->graphicsQueue.value,
                               indices->presentationQueue.value};

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
    printf("%s", "failed to create swap chain!");
  }

  return swapchain;
}
