#include <cmath>
#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "common/shader.h"
#include "common/camera.h"

const int windowWidth = 1600;
const int windowHeight = 900;

GLFWwindow* initWindow(int width, int height);
bool key_pressed[1024];
void key_callback(GLFWwindow * window, int key, int scancode, int action, int mode);
void cursor_callback(GLFWwindow * window, double xpos, double ypos);
void scroll_callback(GLFWwindow * window, double xoffset, double yoffset);

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
void do_movement(double delta_time);

int main() {
  GLFWwindow * window = initWindow(windowWidth, windowHeight);
  if (!window) {
    glfwTerminate();
    return -1;
  }
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetKeyCallback(window, key_callback);
  glfwSetCursorPosCallback(window, cursor_callback);
  glfwSetScrollCallback(window, scroll_callback);
  glEnable(GL_DEPTH_TEST);

  // prepare an array of vertices
  GLfloat vertices[] = {
    // Positions          // normal vectors
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
};
  glm::vec3 cubePositions[] = {
    glm::vec3( 0.0f,  0.0f,  0.0f),
    glm::vec3( 2.0f,  5.0f, -15.0f),
    glm::vec3(-1.5f, -2.2f, -2.5f),
    glm::vec3(-3.8f, -2.0f, -12.3f),
    glm::vec3( 2.4f, -0.4f, -3.5f),
    glm::vec3(-1.7f,  3.0f, -7.5f),
    glm::vec3( 1.3f, -2.0f, -2.5f),
    glm::vec3( 1.5f,  2.0f, -2.5f),
    glm::vec3( 1.5f,  0.2f, -1.5f),
    glm::vec3(-1.3f,  1.0f, -1.5f)
  };

  GLuint lightVAO;
  glGenVertexArrays(1, &lightVAO);
  glBindVertexArray(lightVAO);

  GLuint lightVBO;
  glGenBuffers(1, &lightVBO);
  glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
                       (void*)(3 * sizeof(GLfloat)));
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);

  Shader shaders("shader.vert", "shader.frag");
  Shader lightShaders("lightShader.vert", "lightShader.frag");

  double last_frame = glfwGetTime();
  while (!glfwWindowShouldClose(window)) {
    double current_frame = glfwGetTime();
    double delta_time = current_frame - last_frame;
    last_frame = current_frame;

    glfwPollEvents();

    do_movement(delta_time);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GLfloat light_pos_angle = glm::radians(60.0f * current_frame);
    glm::vec3 light_pos(1.2f + sin(light_pos_angle), 1.0f, 2.0f + cos(light_pos_angle));

    // draw common container
    shaders.Use();

    GLuint normalMatrixLoc = glGetUniformLocation(shaders.Program, "normalMatrix");
    GLuint modelLoc = glGetUniformLocation(shaders.Program, "model");
    GLuint viewLoc = glGetUniformLocation(shaders.Program, "view");
    GLuint projectionLoc = glGetUniformLocation(shaders.Program, "projection");

    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),
                                (GLfloat)windowWidth / (GLfloat)windowHeight,
                                0.01f, 1000.0f);
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    GLuint objectColorLoc = glGetUniformLocation(shaders.Program, "objectColor");
    GLuint lightColorLoc = glGetUniformLocation(shaders.Program, "lightColor");
    glUniform3f(objectColorLoc, 1.0f, 0.5f, 0.31f);
    glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
    GLuint lightPosLoc = glGetUniformLocation(shaders.Program, "lightPos");
    glUniform3f(lightPosLoc, light_pos.x, light_pos.y, light_pos.z);

    glm::mat4 model = glm::translate(glm::mat4(), cubePositions[0]);
    model = glm::rotate(model, (GLfloat)glm::radians(60.0f * current_frame),
                        glm::vec3(1.0f, 1.0f, 1.0f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glm::mat3 normalMatrix = glm::mat3(view *glm::transpose(glm::inverse(model)));
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    glBindVertexArray(lightVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    // draw lamp
    lightShaders.Use();

    model = glm::scale(glm::translate(glm::mat4(), light_pos), glm::vec3(0.2f));

    modelLoc = glGetUniformLocation(lightShaders.Program, "model");
    viewLoc = glGetUniformLocation(lightShaders.Program, "view");
    projectionLoc = glGetUniformLocation(lightShaders.Program, "projection");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glBindVertexArray(lightVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    glfwSwapBuffers(window);
  }

  glfwTerminate();
  return 0;
}

GLFWwindow* initWindow(int width, int height) {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  GLFWwindow* window = glfwCreateWindow(width, height, "Learn Opengl by Leasunhy", nullptr, nullptr);
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

void key_callback(GLFWwindow * window, int key, int scancode, int action, int mode) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GL_TRUE);
  else if (key >= 0 && key < 1024) {
    if (action == GLFW_PRESS)
      key_pressed[key] = true;
    else if (action == GLFW_RELEASE)
      key_pressed[key] = false;
  }
}

void do_movement(double delta_time) {
  if (key_pressed[GLFW_KEY_W])
    camera.DoKeyboard(FORWARD, delta_time);
  if (key_pressed[GLFW_KEY_S])
    camera.DoKeyboard(BACKWARD, delta_time);
  if (key_pressed[GLFW_KEY_A])
    camera.DoKeyboard(LEFT, delta_time);
  if (key_pressed[GLFW_KEY_D])
    camera.DoKeyboard(RIGHT, delta_time);
}

bool mouse_first = true;
double mouse_lastx = 0.0;
double mouse_lasty = 0.0;
void cursor_callback(GLFWwindow * window, double xpos, double ypos) {
  if (mouse_first) {
    mouse_lastx = xpos;
    mouse_lasty = ypos;
    mouse_first = false;
  }
  double xoffset = xpos - mouse_lastx;
  double yoffset = mouse_lasty - ypos;
  mouse_lastx = xpos;
  mouse_lasty = ypos;
  camera.DoMouseCursor(xoffset, yoffset);
}

void scroll_callback(GLFWwindow * window, double xoffset, double yoffset) {
  camera.DoMouseScroll(yoffset);
}
