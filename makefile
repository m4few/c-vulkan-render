CFLAGS =-std=c17 -O2
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXrandr -lXi
EXTRAFLAGS = -DDEBUG

MainComp: render.c
	gcc $(CFLAGS) render.c $(LDFLAGS) $(EXTRAFLAGS)
