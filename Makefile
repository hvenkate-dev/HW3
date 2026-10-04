all: viewer

viewer: main.cpp gfx.cpp math3d.cpp gfx.h math3d.h types.h
	g++ -Wall main.cpp gfx.cpp math3d.cpp -o viewer -lX11

viewer-debug: main.cpp gfx.cpp math3d.cpp gfx.h math3d.h types.h
	g++ -Wall -g main.cpp gfx.cpp math3d.cpp -o viewer-debug -lX11

clean:
	rm -f viewer viewer-debug