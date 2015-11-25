#include <iostream>
#include <IL/il.h>
#include <GL/glew.h>
#include "common/texture.h"

void init_texture_loading() {
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  ilInit();
}

GLuint load_texture(const std::string& filename, GLenum target) {
  int width, height;
  std::cout << "loading texture: " << filename << std::endl;
  //unsigned char * data = SOIL_load_image(filename.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
  
  ILuint ihandle;
  ilEnable(IL_ORIGIN_SET);
  ilGenImages(1, &ihandle);
  ilBindImage(ihandle);
  ILboolean loaded = ilLoadImage(filename.c_str());
  if (loaded == IL_FALSE)
    std::cerr << "unable to load texture: " << filename << std::endl;
  width = ilGetInteger(IL_IMAGE_WIDTH);
  height = ilGetInteger(IL_IMAGE_HEIGHT);
  int memory_needed = width * height * 3 * sizeof(unsigned char);
  unsigned char * data = new unsigned char[memory_needed];
  ilCopyPixels(0, 0, 0, width, height, 1, IL_RGB, IL_UNSIGNED_BYTE, data);

  GLuint id;
  glGenTextures(1, &id);
  glBindTexture(target, id);
  glTexImage2D(target, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(target);
  //SOIL_free_image_data(data);

  ilDeleteImages(1, &ihandle);
  delete [] data;
  glBindTexture(target, 0);
  return id;
}


