#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>
//#include <SOIL/SOIL.h>
#include <IL/il.h>
#include <iostream>
#include <string>

void init_texture_loading() {
  ilInit();
}

GLuint load_texture(const std::string& filename, GLenum target = GL_TEXTURE_2D) {
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

#endif
