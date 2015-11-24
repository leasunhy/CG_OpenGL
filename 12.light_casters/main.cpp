#include <cmath>
#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>

#include <GLFW/glfw3.h>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "common/window.h"
#include "common/texture.h"
#include "common/shader.h"
#include "common/camera.h"

const int windowWidth = 1600;
const int windowHeight = 900;

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
    // Positions           // Normals           // Texture Coords
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
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

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                       (void*)(3 * sizeof(GLfloat)));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                        (void*)(6 * sizeof(GLfloat)));
  glEnableVertexAttribArray(2);

  glBindVertexArray(0);

  Shader shaders("shader.vert", "shader.frag");
  Shader lightShaders("lightShader.vert", "lightShader.frag");

  // load diffuse map (texture)
  GLuint diffuse_map_id = load_texture("container2.png");
  // load specular map (texture)
  GLuint specular_map_id = load_texture("container2_specular.png");

  double last_frame = glfwGetTime();
  while (!glfwWindowShouldClose(window)) {
    double current_frame = glfwGetTime();
    double delta_time = current_frame - last_frame;
    last_frame = current_frame;

    glfwPollEvents();

    do_movement(delta_time);

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GLfloat light_pos_angle = glm::radians(60.0f * current_frame);
    glm::vec3 light_pos(1.2f + sin(light_pos_angle), 1.0f, 2.0f + cos(light_pos_angle));
    //glm::vec3 light_dir(-0.2f, -1.0f, -0.3f);

    // draw common container
    shaders.Use();

    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),
                                (GLfloat)windowWidth / (GLfloat)windowHeight,
                                0.01f, 1000.0f);
    shaders.SetUniform("view", view);
    shaders.SetUniform("projection", projection);

    glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
    //lightColor.r = sin(current_frame * 2.0f);
    //lightColor.g = sin(current_frame * 0.7f);
    //lightColor.b = sin(current_frame * 1.3f);
    shaders.SetUniform("ViewPos", camera.Position);
    shaders.SetUniform("light.position", light_pos);
    //shaders.SetUniform("light.direction", light_dir);
    shaders.SetUniform("light.ambient", lightColor * 0.1f);
    shaders.SetUniform("light.diffuse", lightColor * 0.5f);
    shaders.SetUniform("light.specular", 1.0f, 1.0f, 1.0f);
    shaders.SetUniform("light.constant", 1.0f);
    shaders.SetUniform("light.linear", 0.09f);
    shaders.SetUniform("light.quadratic", 0.032f);

    shaders.SetUniform("material.ambient", 1.0f, 0.5f, 0.31f);
    shaders.SetUniform("material.shininess", 32.0f);
    // use diffuse map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuse_map_id);
    shaders.SetUniform("material.diffuse", 0);
    // use specular map
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specular_map_id);
    shaders.SetUniform("material.specular", 1);

    glBindVertexArray(lightVAO);
    for (int i = 0; i < 10; ++i) {
      glm::mat4 model = glm::translate(glm::mat4(), cubePositions[i]);
      model = glm::rotate(model, (GLfloat)glm::radians(20.0f * i),
                          glm::vec3(1.0f, 0.3f, 0.5f));
      glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
      shaders.SetUniform("model", model);
      shaders.SetUniform("normalMatrix", normalMatrix);
      glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    glBindVertexArray(0);

    // draw lamp
    lightShaders.Use();

    glm::mat4 lightModel = glm::scale(glm::translate(glm::mat4(), light_pos), glm::vec3(0.2f));

    lightShaders.SetUniform("view", view);
    lightShaders.SetUniform("projection", projection);
    lightShaders.SetUniform("model", lightModel);
    lightShaders.SetUniform("lightColor", lightColor);

    glBindVertexArray(lightVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    glfwSwapBuffers(window);
  }

  glfwTerminate();
  return 0;
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

