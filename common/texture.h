#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>
#include <SOIL/SOIL.h>

GLuint load_texture(const char * filename, GLenum target = GL_TEXTURE_2D) {
  int width, height;
  unsigned char * data = SOIL_load_image(filename, &width, &height, 0, SOIL_LOAD_RGB);
  GLuint id;
  glGenTextures(1, &id);
  glBindTexture(target, id);
  glTexImage2D(target, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(target);
  SOIL_free_image_data(data);
  glBindTexture(target, 0);
  return id;
}


#endif
