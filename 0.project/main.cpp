#include <cmath>
#include <iostream>
#include <vector>
#include <random>
#include <functional>

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
#include "common/sprite.h"
#include "common/mesh.h"
#include "common/model.h"
#include "common/texture.h"
#include "common/light.h"

const int windowWidth = 1600;
const int windowHeight = 900;

bool key_pressed[1024];
void key_callback(GLFWwindow * window, int key, int scancode, int action, int mode);
void cursor_callback(GLFWwindow * window, double xpos, double ypos);
void scroll_callback(GLFWwindow * window, double xoffset, double yoffset);

Camera defaultCamera(glm::vec3(0.0f, 0.5f, 2.0f));
Camera camera(defaultCamera);
void do_movement(double delta_time);

bool flash_light_on = false;

std::default_random_engine rand_generator;
std::uniform_real_distribution<float> rand_rotate_dist(0.0f, 360.0f);
std::uniform_real_distribution<float> rand_scale_dist(0.5f, 1.2f);
auto rand_rotate = std::bind(rand_rotate_dist, rand_generator);
auto rand_scale = std::bind(rand_scale_dist, rand_generator);

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
  Shader spriteShaders("data/shaders/spriteShader.vert", "data/shaders/spriteShader.frag");
  Shader starShaders("data/shaders/spriteShader.vert", "data/shaders/stars.frag");

  std::cout << "Loading models..." << std::endl;
  Model dome("data/models/geodesic_dome.obj");
  Model landscape("data/models/landscape.obj");
  //Model nanosuit("../14.model_loading/nanosuit/nanosuit.obj");
  std::cout << "Models loaded!" << std::endl;

  std::cout << "Loading extra textures..." << std::endl;
  GLuint domeColor = load_texture("data/textures/sky.png");
  GLuint domeGlow = load_texture("data/textures/glow.png");

  Sprite sun("data/textures/sun.png");
  Sprite moon("data/textures/moon.png");
  Sprite star("data/textures/star.png");
  // enable blending!
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  std::vector<glm::mat4> starModels(512);
  for (auto& m : starModels) {
    m = glm::rotate(m, glm::radians(rand_rotate()), glm::vec3(1.0f, 0.0f, 0.0f));
    m = glm::rotate(m, glm::radians(rand_rotate()), glm::vec3(0.0f, 1.0f, 0.0f));
    m = glm::rotate(m, glm::radians(rand_rotate()), glm::vec3(0.0f, 0.0f, 1.0f));
    m = glm::translate(m, glm::vec3(5.0f, 0.0f, 0.0f));
    m = glm::rotate(m, glm::radians(rand_rotate()), glm::vec3(1.0f, 0.0f, 0.0f));
    m = glm::rotate(m, glm::radians(rand_rotate()), glm::vec3(0.0f, 1.0f, 0.0f));
  }

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
    shaders.SetUniform("pointLightCount", 0);
    spotLight.SetUniforms(shaders, "spotLight");
    shaders.SetUniform("material.shininess", 16.0f);
    glm::mat4 nmodel = glm::translate(glm::mat4(), glm::vec3(-1.2f, -1.0f, -2.0f));;
    shaders.SetUniform("model", nmodel);

    //nanosuit.Draw(shaders);

    colorShaders.Use();
    colorShaders.SetUniform("view", view);
    colorShaders.SetUniform("projection", projection);
    colorShaders.SetUniform("ViewPos", camera.Position);
    dirLight.SetUniforms(colorShaders, "dirLight");
    pointLight.SetUniforms(colorShaders, "pointLights[0]");
    colorShaders.SetUniform("pointLightCount", 0);
    spotLight.SetUniforms(colorShaders, "spotLight");
    colorShaders.SetUniform("material.shininess", 16.0f);

    // make the dome and landscape pinned
    glm::mat4 pinnedView = glm::lookAt(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f) + camera.Front,
                                       glm::vec3(0.0f, 1.0f, 0.0f));

    float sunAngle = current_frame * -30.0f;
    glm::mat4 sunModel;
    // sunModel = glm::translate(sunModel, glm::vec3(0.0f, 0.0f, -0.5f));
    sunModel = glm::rotate(sunModel, glm::radians(sunAngle), glm::vec3(0.0f, 0.0f, 1.0f));
    sunModel = glm::translate(sunModel, glm::vec3(3.5f, 0.0f, 0.0f));
    //sunModel = glm::scale(sunModel, glm::vec3(1.0f, 1.0f, 1.0f));
    //sunModel = glm::translate(sunModel, glm::vec3(2.6f, 2.6f, 0.0f));
    //sunModel = glm::scale(sunModel, glm::vec3(0.7f, 0.7f, 0.7f));
    //sunModel = glm::rotate(sunModel, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    float moonAngle = sunAngle + 180.0f;
    glm::mat4 moonModel;
    // moonModel = glm::translate(moonModel, glm::vec3(0.0f, 0.0f, -0.5f));
    moonModel = glm::rotate(moonModel, glm::radians(moonAngle), glm::vec3(0.0f, 0.0f, 1.0f));
    moonModel = glm::translate(moonModel, glm::vec3(3.5f, 0.0f, 0.0f));
    //moonModel = glm::scale(moonModel, glm::vec3(1.0f, 1.0f, 1.0f));
    //moonModel = glm::translate(moonModel, glm::vec3(2.6f, 2.6f, 0.0f));
    //moonModel = glm::scale(moonModel, glm::vec3(0.7f, 0.7f, 0.7f));
    //moonModel = glm::rotate(moonModel, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::vec3 sunPos = glm::vec3(sunModel * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

    // stars
    starShaders.Use();
    starShaders.SetUniform("view", view);
    starShaders.SetUniform("projection", projection);
    starShaders.SetUniform("groundBases[0]", 1.0f, 0.0f, 0.0f);
    starShaders.SetUniform("groundBases[1]", 0.0f, 0.0f, 1.0f);
    starShaders.SetUniform("groundUp", 0.0f, 1.0f, 0.0f);
    starShaders.SetUniform("sunPos", sunPos);
    for (const auto& m : starModels) {
      glm::mat4 model = glm::rotate(glm::mat4(), glm::radians(sunAngle), glm::vec3(0.0f, 0.0f, 1.0f)) * m;
      starShaders.SetUniform("model", model);
      star.Draw(starShaders);
    }
    // starShaders.SetUniform("model", starModels[0]);
    // star.Draw(starShaders);

    colorShaders.Use();
    glm::mat4 lmodel;
    lmodel = glm::scale(lmodel, glm::vec3(3.0f, 3.0f, 3.0f));
    lmodel = glm::translate(lmodel, glm::vec3(0.0f, 0.1f, 0.0f));
    lmodel = glm::rotate(lmodel, glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(lmodel)));
    colorShaders.SetUniform("view", view);
    colorShaders.SetUniform("model", lmodel);
    colorShaders.SetUniform("normalMatrix", normalMatrix);
    landscape.Draw(colorShaders, false);

    domeShaders.Use();
    domeShaders.SetUniform("view", view);
    domeShaders.SetUniform("projection", projection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, domeColor);
    domeShaders.SetUniform("domeColor", 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, domeGlow);
    domeShaders.SetUniform("glow", 1);
    glActiveTexture(GL_TEXTURE0);
    glm::mat4 dmodel;
    dmodel = glm::scale(dmodel, glm::vec3(4.0f, 4.0f, 4.0f));
    domeShaders.SetUniform("model", dmodel);
    domeShaders.SetUniform("sunPos", sunPos);
    dome.Draw(domeShaders, false);

    glm::mat4 sunModelView = view * sunModel;
    for (int i = 0; i < 3; ++i)
      for (int j = 0; j < 3; ++j)
        sunModelView[i][j] = (GLfloat)(i == j);
    sunModelView = glm::scale(sunModelView, glm::vec3(0.5f, 0.5f, 0.5f));
    glm::mat4 moonModelView = view * moonModel;
    for (int i = 0; i < 3; ++i)
      for (int j = 0; j < 3; ++j)
        moonModelView[i][j] = (GLfloat)(i == j);
    moonModelView = glm::scale(moonModelView, glm::vec3(0.5f, 0.5f, 0.5f));

    // cheating billboarding to make the sun and moon always facing the camera
    spriteShaders.Use();
    spriteShaders.SetUniform("view", glm::mat4());
    spriteShaders.SetUniform("projection", projection);
    spriteShaders.SetUniform("model", sunModelView);
    sun.Draw(spriteShaders);
    spriteShaders.SetUniform("model", moonModelView);
    moon.Draw(spriteShaders);

    //starShaders.Use();
    //glm::mat4 smodel;
    //smodel = glm::translate(smodel, glm::vec3(0.0f, 1.0f, 0.0f));
    //smodel = glm::scale(smodel, glm::vec3(18.0f, 18.0f, 18.0f));
    //starShaders.SetUniform("groundBases[0]", 1.0f, 0.0f, 0.0f);
    //starShaders.SetUniform("groundBases[1]", 0.0f, 0.0f, 1.0f);
    //starShaders.SetUniform("groundUp", 0.0f, 1.0f, 0.0f);
    //starShaders.SetUniform("model", smodel);
    //starShaders.SetUniform("view", view);
    //starShaders.SetUniform("projection", projection);
    //starShaders.SetUniform("sunPos", sunPos);
    //glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D, domeStar);
    ////starShaders.SetUniform("stars", 0);


    /*
    lightShaders.Use();

    lightShaders.SetUniform("view", pinnedView);
    lightShaders.SetUniform("projection", projection);
    lightShaders.SetUniform("model", sunModel);
    lightShaders.SetUniform("lightColor", lightColor);

    glBindVertexArray(lightVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    */

    // draw lamp
    /*
    lightShaders.Use();

    glm::mat4 lightModel = glm::scale(glm::translate(glm::mat4(), light_pos), glm::vec3(0.2f));

    lightShaders.SetUniform("view", view);
    lightShaders.SetUniform("projection", projection);
    lightShaders.SetUniform("model", lightModel);
    lightShaders.SetUniform("lightColor", lightColor);

    glBindVertexArray(lightVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    */

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
  } else if (key == GLFW_KEY_G && action == GLFW_PRESS) {
    camera = defaultCamera;
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

