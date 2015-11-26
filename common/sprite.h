#ifndef SPRITE_H
#define SPRITE_H

#include <string>
#include <GL/glew.h>
#include "common/mesh.h"

class Sprite
{
 public:
  Sprite(const std::string& filename);
  void Draw(Shader& shader);
 protected:
  Texture sprite;
  GLuint VAO, VBO;
};

#endif

