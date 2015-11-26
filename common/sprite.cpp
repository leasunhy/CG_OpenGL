#include <GL/glew.h>

#include "common/texture.h"

#include "common/sprite.h"

Sprite::Sprite(const std::string& filename)
    : sprite(load_texture(filename)) {
  GLfloat vertices[] = { 
      // Pos      // Tex
      -1.0f, 1.0f, 0.0f, 1.0f,
      1.0f, -1.0f, 1.0f, 0.0f,
      -1.0f, -1.0f, 0.0f, 0.0f, 
  
      -1.0f, 1.0f, 0.0f, 1.0f,
      1.0f, 1.0f, 1.0f, 1.0f,
      1.0f, -1.0f, 1.0f, 0.0f
  };
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat),
          (void*)(2 * sizeof(GLfloat)));
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);
}

void Sprite::Draw(Shader& shader) {
  shader.Use();
  sprite.Bind(GL_TEXTURE0);
  shader.SetUniform("sprite", 0);
  glBindVertexArray(this->VAO);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);
}

