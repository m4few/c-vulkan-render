#include <GLFW/glfw3.h>
#include <stdio.h>
#include <vulkan/vulkan.h>

#include "vulkanInstance.h"
#include "vulkanInstanceHelper.h"
#include "vulkanInstanceLogicalDevice.h"
#include "vulkanInstancePhysicalDevice.h"
#include "vulkanSwapchain.h"
#include "window.h"

// vulkanInstanceHelper.h
int uint32_optInit(uint32_opt *x) {
  *x = uint32_opt_DEFAULT;
  return EXIT_SUCCESS;
}

int uint32_optSet(uint32_opt *x, uint32_t n) {
  x->value = n;
  x->exists = true;
  return EXIT_SUCCESS;
}

int uint32_optRemove(uint32_opt *x) {
  x->exists = false;
  return EXIT_SUCCESS;
}

bool strEq(const char *str1, const char *str2) {
  bool equal = true;
  uint8_t count = 0;
  while (true) {
    if (str1[count] != str2[count]) {
      equal = false;
      break;
    }
    if (str1[count] == '\0') {
      break;
    }

    count++;
  }

  return equal;
}

int clamp(int x, int min, int max) {
  const int t = x < min ? min : x;
  return t > max ? max : t;
}

int32_t fileGetLength(FILE *fp) {
  fseek(fp, 0L, SEEK_END);
  int32_t size = ftell(fp);
  rewind(fp);

  return size;
}

// vulkanSwapchain.h
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

VkImageView *swapchainCreateImageViews(VkDevice device,
                                       DeviceSwapchainInfo *swapInfo) {
  VkImageView *imageViews =
      (VkImageView *)malloc(sizeof(VkImageView) * swapInfo->imageCount);
  for (int i = 0; i < swapInfo->imageCount; i++) {
    VkImageViewCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.image = swapInfo->imageHandles[i];
    createInfo.format = swapInfo->details.surfFormats->format;

    // NOTE: you can do some cool monochrome stuff here
    createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(device, &createInfo, NULL, &imageViews[i]) !=
        VK_SUCCESS) {
      printf("%s\n", "err: image view could not be created");
    }
  }

  return imageViews;
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
  swapInfo.imageViews = swapchainCreateImageViews(*device, &swapInfo);

  return swapInfo;
}

// vulkanPhysicalDevice.h
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

// vulkanLogicalDevice.h
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

// window.h
void glfwCallback(int err, const char *msg) { printf(" [%d] %s\n", err, msg); }

GLFWwindow *windowInit(uint16_t winX, uint16_t winY) {

  glfwSetErrorCallback(glfwCallback);
  glfwInit();

  if (!glfwInit()) {
    printf("ITS BROKEN AGAIN SEE window.h\n");
  }

  // extra window constnats
  void *MONITOR = NULL;
  char TITLE[] = "Cereal";

  // start glfw and set constants
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  // return gflw window pointer, the last parameter is an opengl junk value
  GLFWwindow *win = glfwCreateWindow(winX, winY, TITLE, MONITOR, NULL);
  return win;
}

VkSurfaceKHR windowCreateSurface(VkInstance *instance, GLFWwindow *window) {
  VkSurfaceKHR surface;
  bool result = glfwCreateWindowSurface(*instance, window, NULL, &surface);
  if (result != VK_SUCCESS) {
    printf("ERR: FAILED TO CREATE SURFACE!!!");
  }

  return surface;
}

int windowExit(GLFWwindow *window) {
  glfwDestroyWindow(window);
  glfwTerminate();
  return EXIT_SUCCESS;
}

// vulkaninstance.h
// check if all the validation layers actually exist
bool validationSupported() {
  // get the number of layers
  uint32_t realLayerCount;
  vkEnumerateInstanceLayerProperties(&realLayerCount, NULL);

  // get a list of supported layers
  VkLayerProperties layers[realLayerCount];
  vkEnumerateInstanceLayerProperties(&realLayerCount, layers);

  // make sure all layers are supported
  for (uint8_t i = 0; i < VALIDATION_LAYER_COUNT; i++) {

    bool found = false;
    for (uint8_t j = 0; j < realLayerCount; j++) {
      if (strcmp(VALIDATION_LAYERS_ARR[i], layers[j].layerName) == 0) {
        found = true;
        break;
      }
    }
    if (found == false) {
      return false;
    }
  }
  return true;
}

// NOTE: INITIALISE ALL STRUCTS TO SET ALL VALUES TO 0, THIS PREVENTS SEGFAULTS
int vulkanMakeInstance(VkInstance *instance) {
  // check for validation layer support
  // this stuff is all optional, don't worry about it
  VkApplicationInfo vkInfo = {};

  vkInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  vkInfo.pApplicationName = "Cereal render";
  vkInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  vkInfo.pEngineName = "No Engine";
  vkInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  vkInfo.apiVersion = VK_API_VERSION_1_0;

  // this stuff is not optional
  VkInstanceCreateInfo vkCreateInfo = {};

  uint32_t GLFW_EXT_COUNT = 0;
  int ENABLED_LAYERS = 0;
  const char **GLFW_EXTENSIONS =
      glfwGetRequiredInstanceExtensions(&GLFW_EXT_COUNT);

  vkCreateInfo.enabledExtensionCount = GLFW_EXT_COUNT;
  vkCreateInfo.ppEnabledExtensionNames = GLFW_EXTENSIONS;
  vkCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  vkCreateInfo.pApplicationInfo = &vkInfo;

  // add validation information to vkCreateInfo, or don't
  if (ENABLE_VALIDATION == true) {
    if (!validationSupported()) {
      printf("WARNING: VALIDATION LAYERS NOT SUPPORTED, SKIPPING\n");

    } else {
      printf("NOTE: VALIDATION LAYERS ARE ON\n");
      uint8_t realLayerCount;
      vkCreateInfo.enabledLayerCount = VALIDATION_LAYER_COUNT;
      vkCreateInfo.ppEnabledLayerNames = &VALIDATION_LAYERS;
    }
  }
  // this creates the instance
  // (info, custom callback, ptr to new instance)
  VkResult result = vkCreateInstance(&vkCreateInfo, NULL, instance);

  if (result != VK_SUCCESS) {
    printf("ERR: vulkanInit failed", NULL, 2);
  }

  return EXIT_SUCCESS;
}

int vulkanInit(VkInstance *instance, GLFWwindow *window) {
  vulkanMakeInstance(instance);
  // TODO: abstract this in to one function call (eg vulkanMakeDevice??)

  // make surface
  VkSurfaceKHR surface = windowCreateSurface(instance, window);

  // get real device and make logical device
  VkPhysicalDevice physicalDevice = deviceChoose(instance, &surface);
  VkDevice logicalDevice = deviceCreateLogical(&physicalDevice, &surface);

  // get graphics queue handle
  QueueHandles graphicsQueue =
      deviceGetQueueHandles(&physicalDevice, &logicalDevice, &surface);

  // make swapchain
  DeviceSwapchainInfo swapchainInfo =
      deviceSwapchainCreate(window, &surface, &physicalDevice, &logicalDevice);

  // NOTE: this is a repeat of a call in deviceSwapchainCreate(), it could be
  // cut out if swapchainCreate passes a struct with other info
  //  it could even have the image handles in the struct, making this section
  //  completely uneccessary

  return EXIT_SUCCESS;
}

int vulkanCleanup(GLFWwindow *window, VkInstance *instance, VkDevice *lDevice,
                  VkSurfaceKHR *surface, VkSwapchainKHR *swapchain) {
  vkDestroySwapchainKHR(*lDevice, *swapchain, NULL);
  vkDestroySurfaceKHR(*instance, *surface, NULL);
  vkDestroyDevice(*lDevice, NULL);
  vkDestroyInstance(*instance, NULL);
  windowExit(window);
  return EXIT_SUCCESS;
}

int main() {
  // start window
  glfwInit();
  GLFWwindow *RENDER_WINDOW = windowInit(400, 400);

  // start vulkan instance
  VkInstance instance;
  vulkanInit(&instance, RENDER_WINDOW);

  // while the exit flag is down, wait
  while (!glfwWindowShouldClose(RENDER_WINDOW)) {
    glfwPollEvents();
  }

  return windowExit(RENDER_WINDOW);
}
