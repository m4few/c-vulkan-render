CFLAGS =-std=c17 -O2
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXrandr -lXi

MainComp: render.c
	gcc $(CFLAGS) render.c $(LDFLAGS)
