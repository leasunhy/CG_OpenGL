#include <iostream>
#include "common/light.h"

static std::ostream& operator<<(std::ostream& out, const glm::vec3& a) {
  out << a.x << " " << a.y << " " << a.z;
  return out;
}

Light::~Light() {}

DirLight::DirLight(glm::vec3 direction, glm::vec3 ambient,
                   glm::vec3 diffuse, glm::vec3 specular)
  : direction(direction), ambient(ambient), diffuse(diffuse), specular(specular) {}

DirLight::DirLight(glm::vec3 direction, glm::vec3 diffuse)
: direction(direction), ambient(diffuse * 0.2f), diffuse(diffuse),
    specular(1.0f, 1.0f, 1.0f) {}


PointLight::PointLight(glm::vec3 position, glm::vec3 ambient,
                       glm::vec3 diffuse, glm::vec3 specular, float constant,
                       float linear, float quadratic)
  : position(position), ambient(ambient), diffuse(diffuse), specular(specular),
    constant(constant), linear(linear), quadratic(quadratic) {}

PointLight::PointLight(glm::vec3 position, glm::vec3 diffuse,
                       float constant, float linear, float quadratic)
  : position(position), ambient(diffuse * 0.2f), diffuse(diffuse),
    specular(1.0f, 1.0f, 1.0f), constant(constant), linear(linear), quadratic(quadratic) {}



SpotLight::SpotLight(glm::vec3 position, glm::vec3 direction, glm::vec3 ambient,
          glm::vec3 diffuse, glm::vec3 specular, float cutoff,
          float outerCutoff)
  : position(position), direction(direction), ambient(ambient), diffuse(diffuse),
    specular(specular), cutoff(cutoff), outerCutoff(outerCutoff) {}

SpotLight::SpotLight(glm::vec3 position, glm::vec3 direction,
                     glm::vec3 specular, float cutoff,
                     float outerCutoff)
  : position(position), direction(direction), ambient(specular * 0.1f),
    diffuse(specular * 0.5f), specular(specular), cutoff(cutoff), outerCutoff(outerCutoff) {
  std::cout << "position: " << position << std::endl;
  std::cout << "direction: " << direction << std::endl;
  std::cout << "ambient: " << ambient << std::endl;
  std::cout << "diffuse: " << diffuse << std::endl;
  std::cout << "specular: " << specular << std::endl;
  std::cout << "cutoff: " << cutoff << std::endl;
  std::cout << "outer: " << outerCutoff << std::endl;
}



void DirLight::SetUniforms(Shader& shader, const std::string& name) {
  shader.SetUniform(name + ".direction", direction);
  shader.SetUniform(name + ".ambient", ambient);
  shader.SetUniform(name + ".diffuse", diffuse);
  shader.SetUniform(name + ".specular", specular);
}

void PointLight::SetUniforms(Shader& shader, const std::string& name) {
  shader.SetUniform(name + ".position", position);
  shader.SetUniform(name + ".ambient", ambient);
  shader.SetUniform(name + ".diffuse", diffuse);
  shader.SetUniform(name + ".specular", specular);
  shader.SetUniform(name + ".constant", constant);
  shader.SetUniform(name + ".linear", linear);
  shader.SetUniform(name + ".quadratic", quadratic);
}

void SpotLight::SetUniforms(Shader& shader, const std::string& name) {
  shader.SetUniform(name + ".ambient", ambient);
  shader.SetUniform(name + ".diffuse", diffuse);
  shader.SetUniform(name + ".specular", specular);
  shader.SetUniform(name + ".position", position);
  shader.SetUniform(name + ".direction", direction);
  shader.SetUniform(name + ".cutoff", cutoff);
  shader.SetUniform(name + ".outerCutoff", outerCutoff);
}
