#ifndef MESH_H
#define MESH_H

#include <string>
#include <vector>
#include <map>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "common/shader.h"

struct Vertex
{
  glm::vec3 Position;
  glm::vec3 Normal;
  glm::vec3 TexCoords;
  glm::vec4 Colors;
};

struct Texture
{
  GLuint id;
  std::string type;
};

class Mesh
{
 public:
  std::vector<Vertex> vertices;
  std::vector<GLuint> indices;
  std::vector<Texture> textures;

  Mesh(const std::vector<Vertex>& vertices,
       const std::vector<GLuint>& indices,
       const std::vector<Texture>& textures);

  void Draw(Shader& shader, bool useTexture = true);

 private:
  GLuint VAO, VBO, EBO;
};


#endif
