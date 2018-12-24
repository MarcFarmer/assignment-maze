GL_LIBS = `pkg-config --static --libs glfw3` -lGLEW 
EXT = 
CPPFLAGS = `pkg-config --cflags glfw3`

CC = g++
EXE = maze
OBJS = maze-viewer.o Maze.o GameManager.o Cube.o Sphere.o Shader.o Viewer.o

.PHONY: all clean



all: $(EXE)

$(EXE): $(OBJS)
	$(CC) -o $(EXE) $(OBJS) $(GL_LIBS)

maze-viewer.o: maze-viewer.cpp InputState.h
	$(CC) $(CPPFLAGS) -c maze-viewer.cpp

Shader.o: Shader.cpp Shader.hpp
	$(CC) $(CPPFLAGS) -c Shader.cpp

GameManager.o: GameManager.cpp GameManager.h
	$(CC) $(CPPFLAGS) -c GameManager.cpp

Viewer.o: Viewer.h Viewer.cpp InputState.h
	$(CC) $(CPPFLAGS) -c Viewer.cpp

Maze.o: Maze.h Maze.cpp
	$(CC) $(CPPFLAGS) -c Maze.cpp

Cube.o: Cube.h Cube.cpp
	$(CC) $(CPPFLAGS) -c Cube.cpp

Sphere.o: Sphere.hpp Sphere.cpp
	$(CC) $(CPPFLAGS) -c Sphere.cpp

clean:
	rm -f *.o $(EXE)$(EXT)
