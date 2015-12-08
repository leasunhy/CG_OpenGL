#include <iostream>
#include <algorithm>
#include <GL/glew.h>
#include "common/texture.h"

#ifdef USE_IL
#include <IL/il.h>
#else
#include <SOIL/SOIL.h>
#endif

void init_texture_loading() {
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#ifdef USE_IL
  ilInit();
#endif
}

void flip_image(unsigned char * data, int width, int height, int channels) {
  int row_width = width * channels;
  for (int row = 0; row < height / 2; ++row) {
    int rrow = height - row - 1;
    for (int i = 0; i < row_width; ++i)
      std::swap(data[row * row_width + i], data[rrow * row_width + i]);
  }
}

GLuint load_texture(const std::string& filename, bool flipY, GLenum wrap_s, GLenum wrap_t) {
  int width, height, channels;
  std::cout << "loading texture: " << filename << std::endl;

#ifndef USE_IL
  unsigned char * data = SOIL_load_image(filename.c_str(), &width, &height, &channels, SOIL_LOAD_RGBA);
#else
  ILuint ihandle;
  ilEnable(IL_ORIGIN_SET);
  ilGenImages(1, &ihandle);
  ilBindImage(ihandle);
  ILboolean loaded = ilLoadImage(filename.c_str());
  if (loaded == IL_FALSE)
    std::cerr << "unable to load texture: " << filename << std::endl;
  width = ilGetInteger(IL_IMAGE_WIDTH);
  height = ilGetInteger(IL_IMAGE_HEIGHT);
  int memory_needed = width * height * 4 * sizeof(unsigned char);
  unsigned char * data = new unsigned char[memory_needed];
  ilCopyPixels(0, 0, 0, width, height, 1, IL_RGBA, IL_UNSIGNED_BYTE, data);
#endif

  if (flipY)
    flip_image(data, width, height, 4);  // # of channels set to 4

  GLuint id;
  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_2D, id);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

  // sets how opengl handles out-of-range texcoords
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t);
  glGenerateMipmap(GL_TEXTURE_2D);

#ifndef USE_IL
  SOIL_free_image_data(data);
#else
  ilDeleteImages(1, &ihandle);
  delete [] data;
#endif

  glBindTexture(GL_TEXTURE_2D, 0);
  return id;
}

