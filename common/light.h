#ifndef LIGHT_H
#define LIGHT_H

#include <string>
#include <glm/glm.hpp>
#include "common/shader.h"

class Light {
 public:
  virtual ~Light();
  virtual void SetUniforms(Shader& shader, const std::string& name) = 0;
};

class DirLight : public Light {
 public:
  DirLight(glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);
  DirLight(glm::vec3 direction, glm::vec3 diffuse = glm::vec3(0.5f, 0.5f, 0.5f));
  virtual void SetUniforms(Shader& shader, const std::string& name);
 private:
  glm::vec3 direction;
  glm::vec3 ambient;
  glm::vec3 diffuse;
  glm::vec3 specular;
};

class PointLight : public Light {
 public:
  PointLight(glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular,
             float constant = 1.0f, float linear = 0.09f, float quadratic = 0.032f);
  PointLight(glm::vec3 position, glm::vec3 diffuse,
             float constant = 1.0f, float linear = 0.09f, float quadratic = 0.032f);
  virtual void SetUniforms(Shader& shader, const std::string& name);
 private:
  glm::vec3 position;
  glm::vec3 ambient;
  glm::vec3 diffuse;
  glm::vec3 specular;
  float constant;
  float linear;
  float quadratic;
};

class SpotLight : public Light {
 public:
  SpotLight(glm::vec3 position, glm::vec3 direction, glm::vec3 ambient,
            glm::vec3 diffuse, glm::vec3 specular,
            float cutoff = glm::cos(glm::radians(12.5f)),
            float outerCutoff = glm::cos(glm::radians(17.5f)));
  SpotLight(glm::vec3 position, glm::vec3 direction,
            glm::vec3 specular, float cutoff = glm::cos(glm::radians(12.5f)),
            float outerCutoff = glm::cos(glm::radians(17.5f)));
  virtual void SetUniforms(Shader& shader, const std::string& name);
 private:
  glm::vec3 position;
  glm::vec3 direction;
  glm::vec3 ambient;
  glm::vec3 diffuse;
  glm::vec3 specular;
  float cutoff;
  float outerCutoff;
};

#endif
