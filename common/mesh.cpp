#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <set>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "common/mesh.h"
#include "common/texture.h"

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices,
           const std::vector<Texture>& textures)
  : vertices(vertices), indices(indices), textures(textures) {
  glGenVertexArrays(1, &this->VAO);
  glGenBuffers(1, &this->VBO);
  glGenBuffers(1, &this->EBO);

  glBindVertexArray(this->VAO);

  glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
  glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex),
               this->vertices.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint),
               this->indices.data(), GL_STATIC_DRAW);

  // vertex positions
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (GLvoid*)offsetof(Vertex, Position));
  glEnableVertexAttribArray(0);

  // vertex normals
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (GLvoid*)offsetof(Vertex, Normal));
  glEnableVertexAttribArray(1);

  // vertex texture coordinates
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (GLvoid*)offsetof(Vertex, TexCoords));
  glEnableVertexAttribArray(2);

  // vertex colors
  glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (GLvoid*)offsetof(Vertex, Colors));
  glEnableVertexAttribArray(3);

  glBindVertexArray(0);
}

void Mesh::Draw(Shader& shader, bool useTexture) {
  GLuint diffuse_count = 0;
  GLuint specular_count = 0;
  if (useTexture) {
    for (size_t i = 0; i < this->textures.size(); ++i) {
      // GL_TEXTUREi = GL_TEXTURE0 + i
      glActiveTexture(GL_TEXTURE0 + i);
      const std::string& name = this->textures[i].type;
      std::stringstream ss;
      ss << "material." << name;
      if (name == "texture_diffuse")
        ss << ++diffuse_count;
      else if (name == "texture_specular")
        ss << ++specular_count;
      shader.SetUniform(ss.str(), (int)i);
      glBindTexture(GL_TEXTURE_2D, this->textures[i].id);
    }
    glActiveTexture(GL_TEXTURE0);
  }

  // draw!
  shader.Use();
  glBindVertexArray(this->VAO);
  glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

