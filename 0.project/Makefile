CXX := g++
CXXFLAGS := $(CFLAGS) -Wall -std=c++11 -I..  -I/usr/include/assimp
LIBS := $(LIBS) -lglfw -lGLEW -lGLU -lGL -lglut -lassimp -lSOIL
OBJS := ../common/shader.o ../common/camera.o ../common/model.o\
		../common/mesh.o ../common/light.o ../common/texture.o\
		../common/sprite.o
LDFLAGS := $(LDFLAGS) $(LIBS) $(OBJS)

EXECUTABLE := main.e

.PHONY: clean all run

run: all
	./$(EXECUTABLE)

all: $(EXECUTABLE) $(OBJS)

%.e : %.cpp $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $< -o $@

clean:
	-find -type f -name \*.o -delete
	-rm $(EXECUTABLE) $(OBJS)

