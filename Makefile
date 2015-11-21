CXX := g++
CXXFLAGS := $(CFLAGS) -Wall -std=c++11
LIBS := $(LIBS) -lglfw -lGLEW -lGLU -lGL -lglut
LDFLAGS := $(LDFLAGS) $(LIBS)

.PHONY: clean

clean:
	find -type f -name \*.o -delete

