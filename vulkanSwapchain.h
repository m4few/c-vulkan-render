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
} SwapchainDetails;

typedef struct {
  VkSwapchainKHR swapchain;
  SwapchainDetails details;
  uint32_t imageCount;
  VkImage *imageHandles;
} DeviceSwapchainInfo;

SwapchainDetails swapchainGetSupport(VkPhysicalDevice device,
                                     VkSurfaceKHR surface) {
  SwapchainDetails details;

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

VkSurfaceFormatKHR swapchainChooseFormat(SwapchainDetails details) {
  for (uint32_t i = 0; i < details.surfFormatCount; i++) {
    VkSurfaceFormatKHR surfaceFormat = details.surfFormats[i];

    if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
        surfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {

      return surfaceFormat;
    }
  }
  return details.surfFormats[0];
}

VkPresentModeKHR swapchainChoosePresentMode(SwapchainDetails details) {
  for (uint32_t i = 0; i < details.presentModeCount; i++) {
    VkPresentModeKHR mode = details.presentModes[i];

    if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
      return mode;
    }
  }

  return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D swapchainChooseSwapExtent(GLFWwindow *window,
                                     SwapchainDetails details) {
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

VkImage *swapchainGetImageHandles(VkDevice device,
                                  DeviceSwapchainInfo *swapInfo) {

  vkGetSwapchainImagesKHR(device, swapInfo->swapchain, &(swapInfo->imageCount),
                          NULL);

  VkImage *images = (VkImage *)malloc(sizeof(VkImage) * swapInfo->imageCount);

  vkGetSwapchainImagesKHR(device, swapInfo->swapchain, &(swapInfo->imageCount),
                          images);

  return images;
}
