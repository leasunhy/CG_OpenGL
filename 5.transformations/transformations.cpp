#include <cmath>
#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <SOIL/SOIL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "common/shader.h"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GL_TRUE);
}

GLFWwindow * initWindow() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  GLFWwindow * window = glfwCreateWindow(1600, 900, "LearnOpenGL", nullptr, nullptr);
  if (window == nullptr) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return nullptr;
  }
  glfwMakeContextCurrent(window);

  glewExperimental = GL_TRUE;
  if (glewInit() != GLEW_OK) {
    std::cout << "Failed to initialize GLEW" << std::endl;
    return nullptr;
  }

  glViewport(0, 0, 1600, 900);

  return window;
}

int main() {
  GLFWwindow * window = initWindow();
  if (!window)
    return -1;

  Shader shader("shader.vert", "shader.frag");

  // prepare a VAO
  GLuint VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  // prepare an array of vertices
  GLfloat vertices[] = {
    // Positions          // Colors           // Texture Coords
     0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // Top Right
     0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // Bottom Right
    -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // Bottom Left
    -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // Top Left 
  };

  // prepare an array of indices
  GLuint indices[] = {
    0, 1, 3,
    1, 2, 3
  };

  // prepare a VBO(vertex buffer object) to represent the buffer holding the vertices
  GLuint VBO;
  glGenBuffers(1, &VBO);
  // hints the type of VBO
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  // transfer the vertex data from main memory to GPU
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // prepare EBO(element buffer object)
  GLuint EBO;
  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  // hints how opengl shoud interpret the data
  // positions
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
  glEnableVertexAttribArray(2);

  // unbinds the VAO
  glBindVertexArray(0);

  // prepare texture1
  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  int texwidth, texheight;
  unsigned char * teximage = SOIL_load_image("container.jpg", &texwidth, &texheight, 0, SOIL_LOAD_RGB);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texwidth, texheight, 0, GL_RGB, GL_UNSIGNED_BYTE, teximage);
  glGenerateMipmap(GL_TEXTURE_2D);
  SOIL_free_image_data(teximage);
  glBindTexture(GL_TEXTURE_2D, 0);

  // prepare texture2
  GLuint texture2;
  glGenTextures(1, &texture2);
  glBindTexture(GL_TEXTURE_2D, texture2);
  int texwidth2, texheight2;
  unsigned char * teximage2 = SOIL_load_image("awesomeface.png", &texwidth2, &texheight2, 0, SOIL_LOAD_RGB);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texwidth2, texheight2, 0, GL_RGB, GL_UNSIGNED_BYTE, teximage2);
  glGenerateMipmap(GL_TEXTURE_2D);
  SOIL_free_image_data(teximage2);
  glBindTexture(GL_TEXTURE_2D, 0);

  glfwSetKeyCallback(window, key_callback);
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    shader.Use();

    // use textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(glGetUniformLocation(shader.Program, "ourTexture1"), 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);
    glUniform1i(glGetUniformLocation(shader.Program, "ourTexture2"), 1);

    // DRAW!!!!
    glBindVertexArray(VAO);
    //glDrawArrays(GL_TRIANGLES, 0, 3);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); // eliminated because it's stored in VAO

    GLuint transLoc = glGetUniformLocation(shader.Program, "transform");
    // transformation
    glm::mat4 trans;
    trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
    trans = glm::rotate(trans, glm::radians((GLfloat)glfwGetTime() * 50.0f),
                        glm::vec3(0.0f, 0.0f, 1.0f));

    // use tranform
    glUniformMatrix4fv(transLoc, 1, GL_FALSE, glm::value_ptr(trans));

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glm::mat4 trans1;
    trans1 = glm::translate(trans1, glm::vec3(-0.5f, 0.5f, 0.0f));
    GLfloat scaleFactor = sin((GLfloat)glfwGetTime()) + 1;
    trans1 = glm::scale(trans1, glm::vec3(scaleFactor));

    glUniformMatrix4fv(transLoc, 1, GL_FALSE, glm::value_ptr(trans1));
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);

    glfwSwapBuffers(window);
  }

  glfwTerminate();
  return 0;
}

