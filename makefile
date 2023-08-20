CFLAGS =-std=c17 -O2
TARGET_FILES = render.c vulkanInstance.c  vulkanInstanceHelper.c vulkanInstanceLogicalDevice.c vulkanInstancePhysicalDevice.c vulkanSwapchain.c window.c
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXrandr -lXi
EXTRAFLAGS = -DDEBUG

MainComp: render.c
	gcc $(CFLAGS) $(TARGET_FILES) $(LDFLAGS) $(EXTRAFLAGS)
