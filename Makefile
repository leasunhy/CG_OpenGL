CXX := g++
CXXFLAGS := $(CFLAGS) -Wall -std=c++11 -I.
LIBS := $(LIBS) -lglfw -lGLEW -lGLU -lGL -lglut -lSOIL
OBJS := common/shader.o
LDFLAGS := $(LDFLAGS) $(LIBS) $(OBJS)

.PHONY: clean

%.e : %.cpp $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $< -o $@

clean:
	find -type f -name \*.o -delete

