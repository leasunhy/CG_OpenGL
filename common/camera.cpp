#include <glm/gtc/matrix_transform.hpp>

#include "common/camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 up, GLfloat yaw, GLfloat pitch)
  : Position(position), Front(glm::vec3(0.0f, 0.0f, -1.0f)),
    WorldUp(up), Yaw(yaw), Pitch(pitch), MovementSpeed(5.0f),
    MouseSensitivity(0.25f), Zoom(45.0f) {
  this->Update();
}

glm::mat4 Camera::GetViewMatrix() { return this->LookAt; }

void Camera::DoKeyboard(CameraMovement direction, GLfloat deltaTime) {
  GLfloat velocity = this->MovementSpeed * deltaTime;
  if (direction == FORWARD)
    this->Position += this->Front * velocity;
  if (direction == BACKWARD)
    this->Position -= this->Front * velocity;
  if (direction == LEFT)
    this->Position -= this->Right * velocity;
  if (direction == RIGHT)
    this->Position += this->Right * velocity;
  this->Update();
}

void Camera::DoMouseCursor(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch) {
  xoffset *= this->MouseSensitivity;
  yoffset *= this->MouseSensitivity;

  this->Yaw   += xoffset;
  this->Pitch += yoffset;

  // Make sure that when pitch is out of bounds, screen doesn't get flipped
  if (constrainPitch) {
    if (this->Pitch > 89.0f)
      this->Pitch = 89.0f;
    if (this->Pitch < -89.0f)
      this->Pitch = -89.0f;
  }

  while (this->Yaw > 360.0f)
    this->Yaw -= 360.0f;
  while (this->Yaw < -360.0f)
    this->Yaw += 360.0f;

  // Update Front, Right and Up Vectors using the updated Eular angles
  this->Update();
}

void Camera::DoMouseScroll(GLfloat yoffset) {
  if (this->Zoom >= 1.0f && this->Zoom <= 45.0f)
    this->Zoom -= yoffset;
  if (this->Zoom <= 1.0f)
    this->Zoom = 1.0f;
  if (this->Zoom >= 45.0f)
    this->Zoom = 45.0f;
}

void Camera::Update() {
  glm::vec3 front;
  front.x = cos(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
  front.y = sin(glm::radians(this->Pitch));
  front.z = sin(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
  this->Front = glm::normalize(front);
  this->Right = glm::normalize(glm::cross(this->Front, this->WorldUp));
  this->Up    = glm::normalize(glm::cross(this->Right, this->Front));

  this->LookAt = glm::lookAt(this->Position, this->Position + this->Front, this->WorldUp);
}
