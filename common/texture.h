#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>
#include <SOIL/SOIL.h>
#include <iostream>
#include <string>

GLuint load_texture(const char * filename, GLenum target = GL_TEXTURE_2D) {
  int width, height;
  unsigned char * data = SOIL_load_image(filename, &width, &height, 0, SOIL_LOAD_RGB);
  std::cout << filename << ": " << SOIL_last_result() << std::endl;
  GLuint id;
  glGenTextures(1, &id);
  glBindTexture(target, id);
  glTexImage2D(target, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(target);
  SOIL_free_image_data(data);
  glBindTexture(target, 0);
  return id;
}
GLint TextureFromFile(const char* path, std::string directory)
{
  //Generate texture ID and load texture data
  std::string filename = std::string(path);
  filename = directory + '/' + filename;
  GLuint textureID;
  glGenTextures(1, &textureID);
  int width,height;
  unsigned char* image = SOIL_load_image(filename.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
  // Assign texture to ID
  glBindTexture(GL_TEXTURE_2D, textureID);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
  glGenerateMipmap(GL_TEXTURE_2D);

  // Parameters
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);
  SOIL_free_image_data(image);
  return textureID;
}

#endif
