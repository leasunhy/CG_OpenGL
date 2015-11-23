#ifndef CAMERA_H
#define CAMERA_H

#include <GL/glew.h>
#include <glm/glm.hpp>

enum CameraMovement {
  FORWARD,
  BACKWARD,
  LEFT,
  RIGHT
};


class Camera {
 public:
  // camera attributes
  glm::vec3 Position;
  glm::vec3 Front;
  glm::vec3 Up;
  glm::vec3 Right;
  glm::vec3 WorldUp;
  // Euler angles
  GLfloat Yaw;
  GLfloat Pitch;
  // camera options
  GLfloat MovementSpeed;
  GLfloat MouseSensitivity;
  GLfloat Zoom;

  // ctor with vectors
  Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
         glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
         GLfloat yaw = -90.0f, GLfloat pitch = 0.0f);

  // get view matrix
  glm::mat4 GetViewMatrix();

  // process keyboard events
  void DoKeyboard(CameraMovement direction, GLfloat deltaTime);

  // process mouse cousor events
  void DoMouseCursor(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch = true);

  // process mouse scrolling events
  void DoMouseScroll(GLfloat yoffset);

 private:
  glm::mat4 LookAt;

  void Update();
};

#endif
