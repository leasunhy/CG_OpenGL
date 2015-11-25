#include <cmath>
#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>

#include <GLFW/glfw3.h>
#include <IL/il.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "common/window.h"
#include "common/shader.h"
#include "common/camera.h"
#include "common/model.h"
#include "common/texture.h"
#include "common/light.h"

const int windowWidth = 1600;
const int windowHeight = 900;

bool key_pressed[1024];
void key_callback(GLFWwindow * window, int key, int scancode, int action, int mode);
void cursor_callback(GLFWwindow * window, double xpos, double ypos);
void scroll_callback(GLFWwindow * window, double xoffset, double yoffset);

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
void do_movement(double delta_time);

bool flash_light_on = true;

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

  // prepare texture loading library(devil)
  init_texture_loading();

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

  Shader shaders("data/shaders/shader.vert", "data/shaders/shader.frag");
  Shader colorShaders("data/shaders/shaderColor.vert", "data/shaders/shaderColor.frag");
  Shader domeShaders("data/shaders/dome.vert", "data/shaders/dome.frag");
  Shader lightShaders("data/shaders/lightShader.vert", "data/shaders/lightShader.frag");

  std::cout << "Loading models..." << std::endl;
  Model dome("data/models/geodesic_dome.obj");
  Model landscape("data/models/landscape.obj");
  std::cout << "Models loaded!" << std::endl;

  std::cout << "Loading extra textures..." << std::endl;
  GLuint domeColor = load_texture("data/textures/sky.png");
  GLuint domeGlow = load_texture("data/textures/glow.png");

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
    glm::vec3 light_pos(1.2f + sin(light_pos_angle), 0.0f, 2.0f + cos(light_pos_angle));

    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),
                                (GLfloat)windowWidth / (GLfloat)windowHeight,
                                0.01f, 1000.0f);


    glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
    lightColor.r = sin(current_frame * 2.0f);
    lightColor.g = sin(current_frame * 0.7f);
    lightColor.b = sin(current_frame * 1.3f);

    // directional light
    DirLight dirLight(glm::vec3(-0.2f, -1.0f, -0.3f));

    // point light
    PointLight pointLight(light_pos, lightColor * 0.5f);


    SpotLight spotLight(camera.Position, camera.Front,
                        glm::vec3((GLfloat)flash_light_on));

    shaders.Use();
    shaders.SetUniform("view", view);
    shaders.SetUniform("projection", projection);
    shaders.SetUniform("ViewPos", camera.Position);
    dirLight.SetUniforms(shaders, "dirLight");
    pointLight.SetUniforms(shaders, "pointLights[0]");
    shaders.SetUniform("pointLightCount", 1);
    spotLight.SetUniforms(shaders, "spotLight");
    shaders.SetUniform("material.shininess", 16.0f);

    colorShaders.Use();
    colorShaders.SetUniform("view", view);
    colorShaders.SetUniform("projection", projection);
    colorShaders.SetUniform("ViewPos", camera.Position);
    dirLight.SetUniforms(colorShaders, "dirLight");
    pointLight.SetUniforms(colorShaders, "pointLights[0]");
    colorShaders.SetUniform("pointLightCount", 1);
    spotLight.SetUniforms(colorShaders, "spotLight");
    colorShaders.SetUniform("material.shininess", 16.0f);

    // make the dome and landscape pinned
    glm::mat4 pinnedView = glm::lookAt(glm::vec3(0.0f), camera.Front,
                                       glm::vec3(0.0f, 1.0f, 0.0f));

    colorShaders.Use();
    glm::mat4 model;
    model = glm::translate(glm::mat4(), glm::vec3(0.0f, -0.4f, 0.0f));
    model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
    glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
    colorShaders.SetUniform("view", pinnedView);
    colorShaders.SetUniform("model", model);
    colorShaders.SetUniform("normalMatrix", normalMatrix);
    landscape.Draw(colorShaders, false);

    domeShaders.Use();
    domeShaders.SetUniform("view", pinnedView);
    domeShaders.SetUniform("projection", projection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, domeColor);
    domeShaders.SetUniform("domeColor", 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, domeGlow);
    domeShaders.SetUniform("glow", 1);
    glActiveTexture(GL_TEXTURE0);
    glm::mat4 dmodel;
    dmodel = glm::translate(dmodel, glm::vec3(0.0f, -1.0f, 0.0f));
    dmodel = glm::scale(dmodel, glm::vec3(5.0f, 5.0f, 5.0f));
    domeShaders.SetUniform("model", dmodel);
    float sunAngle = glm::radians(current_frame * 30.0f);
    //glm::vec3 sunPos(1.0f * glm::cos(sunAngle), 1.0f * glm::sin(sunAngle), 0.0f);
    glm::mat4 sunModel;
    sunModel = glm::translate(sunModel, glm::vec3(-90.0f, -90.0f, 0.0f));
    sunModel = glm::rotate(sunModel, sunAngle, glm::vec3(0.0f, 0.0f, 1.0f));
    domeShaders.SetUniform("sunPos",
                           glm::vec3(sunModel * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)));
    dome.Draw(domeShaders, false);

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

int display_mode = 0;
int point_size = 1;
void key_callback(GLFWwindow * window, int key, int scancode, int action, int mode) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GL_TRUE);
  } else if (key == GLFW_KEY_TAB && action == GLFW_PRESS) {
    display_mode = (display_mode + 1) % 3;
    glPolygonMode(GL_FRONT_AND_BACK,
            (display_mode == 0) ? GL_FILL : ((display_mode == 1) ? GL_LINE : GL_POINT));
  } else if (key == GLFW_KEY_EQUAL && action == GLFW_PRESS) {
    glPointSize(++point_size);
  } else if (key == GLFW_KEY_MINUS && action == GLFW_PRESS) {
    glPointSize(point_size = (point_size - 1 > 0) ? (point_size - 1) : 1);
  } else if (key == GLFW_KEY_0 && action == GLFW_PRESS) {
    flash_light_on = !flash_light_on;
  } else if (key >= 0 && key < 1024) {
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

