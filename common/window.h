#ifndef COMMON_H
#define COMMON_H

#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

GLFWwindow* initWindow(int width, int height,
        const char * title = "Learn OpenGL By Leasunhy") {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  GLFWwindow* window = glfwCreateWindow(width, height, title, nullptr, nullptr);
  if (!window) {
    std::cerr << "Failed to create window." << std::endl;
    return nullptr;
  }
  glfwMakeContextCurrent(window);

  glewExperimental = GL_TRUE;
  if (glewInit() != GLEW_OK) {
    std::cerr << "GLEW failed to initialize." << std::endl;
    return nullptr;
  }

  glViewport(0, 0, width, height);
  return window;
}

#endif  // COMMON_H

