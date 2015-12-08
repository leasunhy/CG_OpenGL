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

Camera defaultCamera(glm::vec3(0.0f, 0.35f, 2.0f));
Camera camera(defaultCamera);
void do_movement(double delta_time);

bool flash_light_on = false;
bool enable_stars = true;

std::default_random_engine rand_generator;
std::uniform_real_distribution<float> rand_rotate_dist(0.0f, 360.0f);
std::uniform_real_distribution<float> rand_scale_dist(0.5f, 1.2f);
auto rand_rotate = std::bind(rand_rotate_dist, rand_generator);
auto rand_scale = std::bind(rand_scale_dist, rand_generator);

//xzh
void RenderFloor(Shader &shader);
void RenderCubes(Shader &shader);
void RenderCube();
GLuint planeVAO;

int main() {
  // msaa
  glfwWindowHint(GLFW_SAMPLES, 4);
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

  //plane
  Shader simpleDepthShader("data/shaders/shadow_mapping_depth.vs", "data/shaders/shadow_mapping_depth.frag");

  Model ourModel("data/models/nanosuit/nanosuit.obj");

  GLfloat planeVertices[] = {
    // Positions          // Normals         // Texture Coords
    2.0f, 0.0f, 2.0f, 0.0f, 1.0f, 0.0f, 2.0f, 0.0f,
    -2.0f, 0.0f, -2.0f, 0.0f, 1.0f, 0.0f, 0.0f, 2.0f,
    -2.0f, 0.0f, 2.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,

    2.0f, 0.0f, 2.0f, 0.0f, 1.0f, 0.0f, 2.0f, 0.0f,
    2.0f, 0.0f, -2.0f, 0.0f, 1.0f, 0.0f, 2.0f, 2.0f,
    -2.0f, 0.0f, -2.0f, 0.0f, 1.0f, 0.0f, 0.0f, 2.0f
  };

  // Setup plane VAO xzhs
  GLuint planeVBO;
  GLuint woodTexture;
  GLuint rockTexture;
  glGenVertexArrays(1, &planeVAO);
  glGenBuffers(1, &planeVBO);
  glBindVertexArray(planeVAO);
  glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
  glBindVertexArray(0);

  // Load textures
  woodTexture = load_texture("data/textures/wood.png");
  rockTexture = load_texture("data/textures/rock.jpg");

  // Configure depth map FBO
  const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
  GLuint depthMapFBO;
  glGenFramebuffers(1, &depthMapFBO);
  // - Create depth texture
  GLuint depthMap;
  glGenTextures(1, &depthMap);
  glBindTexture(GL_TEXTURE_2D, depthMap);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  GLfloat borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

  glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

  //xzhe

  Shader shaders("data/shaders/shader.vert", "data/shaders/shader.frag");
  Shader colorShaders("data/shaders/shaderColorUniform.vert",
                      "data/shaders/shaderColorUniform.frag");
  Shader domeShaders("data/shaders/dome.vert", "data/shaders/dome.frag");
  Shader lightShaders("data/shaders/lightShader.vert", "data/shaders/lightShader.frag");
  Shader spriteShaders("data/shaders/spriteShader.vert", "data/shaders/spriteShader.frag");
  Shader starShaders("data/shaders/spriteShader.vert", "data/shaders/stars.frag");

  std::cout << "Loading models..." << std::endl;
  Model dome("data/models/geodesic_dome.obj");
  Model landscape("data/models/landscape.obj");
  std::cout << "Models loaded!" << std::endl;

  std::cout << "Loading extra textures..." << std::endl;
  GLuint domeColor = load_texture("data/textures/sky.png", true, GL_MIRRORED_REPEAT, GL_MIRRORED_REPEAT);
  GLuint domeGlow = load_texture("data/textures/glow.png", true, GL_MIRRORED_REPEAT, GL_MIRRORED_REPEAT);

  Sprite sun("data/textures/sun.png");
  Sprite moon("data/textures/moon.png");
  Sprite star("data/textures/star.png");

  // enable blending!
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // enable msaa(multisample anti-aliasing)
  glEnable(GL_MULTISAMPLE);

  std::vector<glm::mat4> starModels(256);
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
    glViewport(0, 0, windowWidth, windowHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);
    glm::mat4 view = camera.GetViewMatrix();

    // sun
    float sunAngle = current_frame * 30.0f;
    glm::mat4 sunModel;
    sunModel = glm::rotate(sunModel, glm::radians(sunAngle), glm::vec3(0.0f, 0.0f, 1.0f));
    sunModel = glm::translate(sunModel, glm::vec3(3.5f, 0.0f, 0.0f));
    glm::vec3 sunPos = glm::vec3(sunModel * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

    // moon
    float moonAngle = sunAngle + 180.0f;
    glm::mat4 moonModel;
    moonModel = glm::rotate(moonModel, glm::radians(moonAngle), glm::vec3(0.0f, 0.0f, 1.0f));
    moonModel = glm::translate(moonModel, glm::vec3(3.5f, 0.0f, 0.0f));

    // directional light
    DirLight dirLight(-sunPos, glm::vec3(0.8f, 0.8f, 0.8f));

    // point light
    GLfloat light_pos_angle = glm::radians(60.0f * current_frame);
    glm::vec3 light_pos(1.2f + sin(light_pos_angle), 0.0f, 2.0f + cos(light_pos_angle));
    glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
    lightColor.r = sin(current_frame * 2.0f);
    lightColor.g = sin(current_frame * 0.7f);
    lightColor.b = sin(current_frame * 1.3f);
    PointLight pointLight(light_pos, lightColor * 0.5f);

    // spot light
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

    colorShaders.Use();
    colorShaders.SetUniform("view", view);
    colorShaders.SetUniform("projection", projection);
    colorShaders.SetUniform("ViewPos", camera.Position);
    dirLight.SetUniforms(colorShaders, "dirLight");
    //pointLight.SetUniforms(colorShaders, "pointLights[0]");
    colorShaders.SetUniform("pointLightCount", 0);
    spotLight.SetUniforms(colorShaders, "spotLight");
    colorShaders.SetUniform("material.shininess", 1.8f);

    // make the dome and landscape pinned
    glm::mat4 pinnedView = glm::lookAt(glm::vec3(0.0f, 1.0f, 0.0f),
                                       glm::vec3(0.0f, 1.0f, 0.0f) + camera.Front,
                                       glm::vec3(0.0f, 1.0f, 0.0f));

    if (enable_stars) {
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
    }

    colorShaders.Use();
    glm::mat4 lmodel;
    lmodel = glm::scale(lmodel, glm::vec3(3.0f, 3.0f, 3.0f));
    lmodel = glm::translate(lmodel, glm::vec3(0.0f, 0.1f, 0.0f));
    lmodel = glm::rotate(lmodel, glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(lmodel)));
    colorShaders.SetUniform("view", view);
    colorShaders.SetUniform("model", lmodel);
    colorShaders.SetUniform("normalMatrix", normalMatrix);
    colorShaders.SetUniform("Color", glm::vec4(0.93f, 0.79f, 0.69f, 1.0f));
    landscape.Draw(colorShaders, false);

    domeShaders.Use();
    domeShaders.SetUniform("view", view);
    domeShaders.SetUniform("projection", projection);
    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_2D, domeColor);
    glActiveTexture(GL_TEXTURE8);
    glBindTexture(GL_TEXTURE_2D, domeGlow);
    domeShaders.SetUniform("domeColor", 7);
    domeShaders.SetUniform("glow", 8);
    glm::mat4 dmodel;
    dmodel = glm::scale(dmodel, glm::vec3(4.0f, 4.0f, 4.0f));
    domeShaders.SetUniform("model", dmodel);
    domeShaders.SetUniform("sunPos", sunPos);
    dome.Draw(domeShaders, false);

    // cheating billboarding to make the sun and moon always face the camera
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

    spriteShaders.Use();
    spriteShaders.SetUniform("view", glm::mat4());
    spriteShaders.SetUniform("projection", projection);
    spriteShaders.SetUniform("model", sunModelView);
    sun.Draw(spriteShaders);
    spriteShaders.SetUniform("model", moonModelView);
    moon.Draw(spriteShaders);

    //xzhs
    // Set texture samples
    shaders.Use();
    glActiveTexture(GL_TEXTURE13);
    glBindTexture(GL_TEXTURE_2D, woodTexture);
    glActiveTexture(GL_TEXTURE14);
    glBindTexture(GL_TEXTURE_2D, rockTexture);
    glActiveTexture(GL_TEXTURE15);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    shaders.SetUniform("material.texture_diffuse1", 14);
    shaders.SetUniform("material.texture_specular1", 14);
    shaders.SetUniform("shadowMap", 15);

    // 1. Render depth of scene to texture (from light's perspective)
    // - Get light projection/view matrix.
    glm::mat4 lightProjection, lightView;
    glm::mat4 lightSpaceMatrix;
    GLfloat near_plane = 1.0f, far_plane = 7.5f;

    lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
    lightView = glm::lookAt(sunPos, glm::vec3(0.0f), glm::vec3(1.0));
    lightSpaceMatrix = lightProjection * lightView;
    // - now render scene from light's point of view
    simpleDepthShader.Use();
    simpleDepthShader.SetUniform("lightSpaceMatrix", lightSpaceMatrix);

    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    RenderFloor(simpleDepthShader);
    RenderCubes(simpleDepthShader);

    glm::mat4 nmodel;
    nmodel = glm::translate(nmodel, glm::vec3(0.1f, 0.3f, -0.5f));
    nmodel = glm::rotate(nmodel, glm::radians(70.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    nmodel = glm::scale(nmodel, glm::vec3(0.05f, 0.05f, 0.05f));
    simpleDepthShader.SetUniform("model", nmodel);
    ourModel.Draw(simpleDepthShader);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // 2. Render scene as normal
    glViewport(0, 0, windowWidth, windowHeight);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shaders.Use();
    shaders.SetUniform("projection", projection);
    shaders.SetUniform("view", view);
    shaders.SetUniform("ViewPos", camera.Position);
    // Set light uniforms
    // PointLight sunPointLight(sunPos, glm::vec3(0.02f, 0.02f, 0.02f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.5f, 0.5f, 0.5f));
    // sunPointLight.SetUniforms(shaders, "pointLights[0]");
    // shaders.SetUniform("pointLightCount", 0);
    dirLight.SetUniforms(shaders, "dirLight");
    shaders.SetUniform("pointLightCount", 0);

    shaders.SetUniform("lightSpaceMatrix", lightSpaceMatrix);

    shaders.SetUniform("material.texture_diffuse1", 14);
    shaders.SetUniform("material.texture_specular1", 14);
    shaders.SetUniform("shadowMap", 15);
    RenderFloor(shaders);
    shaders.SetUniform("material.texture_diffuse1", 13);
    shaders.SetUniform("material.texture_specular1", 13);
    RenderCubes(shaders);
    shaders.SetUniform("model", nmodel);
    ourModel.Draw(shaders);
    //xzhe

    glfwSwapBuffers(window);
  }

  glfwTerminate();
  return 0;
}

void key_callback(GLFWwindow * window, int key, int /* scancode */, int action, int /* mode */) {
  static int display_mode = 0;
  static int point_size = 1;
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
  } else if (key == GLFW_KEY_H && action == GLFW_PRESS) {
    enable_stars = !enable_stars;
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

void cursor_callback(GLFWwindow * /* window */, double xpos, double ypos) {
  static bool mouse_first = true;
  static double mouse_lastx = 0.0;
  static double mouse_lasty = 0.0;
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

void scroll_callback(GLFWwindow * /* window */, double /* xoffset */, double yoffset) {
  camera.DoMouseScroll(yoffset);
}

void RenderFloor(Shader &shader) {
  // Floor
  glm::mat4 model = glm::mat4();
  model = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.3f, 0.0f));
  model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
  shader.SetUniform("model", model);
  glBindVertexArray(planeVAO);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);
}

//xzhs
void RenderCubes(Shader &shader) {
  // Cubes
  glm::mat4 model = glm::mat4();
  model = glm::mat4();
  model = glm::translate(model, glm::vec3(0.2f, 0.4f, 0.5f));
  model = glm::rotate(model, (GLfloat)glfwGetTime() * 5.0f, glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
  glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
  RenderCube();
  model = glm::mat4();
  model = glm::translate(model, glm::vec3(0.5f, 0.4f, -0.5));
  glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
  RenderCube();
  model = glm::mat4();
  model = glm::translate(model, glm::vec3(0.3f, 0.5f, 0.5f));
  model = glm::rotate(model, 60.0f, glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
  model = glm::scale(model, glm::vec3(0.5));
  glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
  RenderCube();
}

GLfloat vertices[] = {
  // Back face
  -0.05f, -0.05f, -0.05f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // Bottom-left
  0.05f, 0.05f, -0.05f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // top-right
  0.05f, -0.05f, -0.05f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, // bottom-right
  0.05f, 0.05f, -0.05f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,  // top-right
  -0.05f, -0.05f, -0.05f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,  // bottom-left
  -0.05f, 0.05f, -0.05f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,// top-left
  // Front face
  -0.05f, -0.05f, 0.05f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
  0.05f, -0.05f, 0.05f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,  // bottom-right
  0.05f, 0.05f, 0.05f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,  // top-right
  0.05f, 0.05f, 0.05f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top-right
  -0.05f, 0.05f, 0.05f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,  // top-left
  -0.05f, -0.05f, 0.05f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // bottom-left
  // Left face
  -0.05f, 0.05f, 0.05f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
  -0.05f, 0.05f, -0.05f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-left
  -0.05f, -0.05f, -0.05f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  // bottom-left
  -0.05f, -0.05f, -0.05f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
  -0.05f, -0.05f, 0.05f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
  -0.05f, 0.05f, 0.05f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
  // Right face
  0.05f, 0.05f, 0.05f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-left
  0.05f, -0.05f, -0.05f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
  0.05f, 0.05f, -0.05f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-right
  0.05f, -0.05f, -0.05f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  // bottom-right
  0.05f, 0.05f, 0.05f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,  // top-left
  0.05f, -0.05f, 0.05f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-left
  // Bottom face
  -0.05f, -0.05f, -0.05f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
  0.05f, -0.05f, -0.05f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, // top-left
  0.05f, -0.05f, 0.05f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,// bottom-left
  0.05f, -0.05f, 0.05f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom-left
  -0.05f, -0.05f, 0.05f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom-right
  -0.05f, -0.05f, -0.05f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
  // Top face
  -0.05f, 0.05f, -0.05f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,// top-left
  0.05f, 0.05f, 0.05f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
  0.05f, 0.05f, -0.05f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // top-right
  0.05f, 0.05f, 0.05f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
  -0.05f, 0.05f, -0.05f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,// top-left
  -0.05f, 0.05f, 0.05f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f // bottom-left
};

// RenderCube() Renders a 1x1 3D cube in NDC.
void RenderCube() {
  static GLuint cubeVAO = 0;
  static GLuint cubeVBO = 0;
  // Initialize (if necessary)
  if (cubeVAO == 0) {
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    // Fill buffer
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // Link vertex attributes
    glBindVertexArray(cubeVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }
  // Render Cube
  glBindVertexArray(cubeVAO);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  glBindVertexArray(0);
}
//xzhe

