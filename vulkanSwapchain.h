#include "vulkanInstanceHelper.h"
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
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
  VkImageView *imageViews;
} DeviceSwapchainInfo;

SwapchainDetails swapchainGetSupport(VkPhysicalDevice device,
                                     VkSurfaceKHR surface);

VkSurfaceFormatKHR swapchainChooseFormat(SwapchainDetails details);

VkPresentModeKHR swapchainChoosePresentMode(SwapchainDetails details);

VkExtent2D swapchainChooseSwapExtent(GLFWwindow *window,
                                     SwapchainDetails details);

VkImage *swapchainGetImageHandles(VkDevice device,
                                  DeviceSwapchainInfo *swapInfo);

VkImageView *swapchainCreateImageViews(VkDevice device,
                                       DeviceSwapchainInfo *swapInfo);