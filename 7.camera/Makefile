CXX := g++
CXXFLAGS := $(CFLAGS) -Wall -std=c++11 -I..
LIBS := $(LIBS) -lglfw -lGLEW -lGLU -lGL -lglut -lSOIL
OBJS := ../common/shader.o ../common/camera.o
LDFLAGS := $(LDFLAGS) $(LIBS) $(OBJS)

EXECUTABLE := camera.e

.PHONY: clean all run

run: all
	./$(EXECUTABLE)

all: $(EXECUTABLE) $(OBJS)

%.e : %.cpp $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $< -o $@

clean:
	find -type f -name \*.o -delete
	rm $(EXECUTABLE)

