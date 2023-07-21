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
