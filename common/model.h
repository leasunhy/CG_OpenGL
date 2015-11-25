#ifndef MODEL_H
#define MODEL_H

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

class Model
{
 public:
  Model(const GLchar * path);
  void Draw(Shader& shader, bool useTexture = true);

 private:
  std::vector<Mesh> meshes;
  std::string directory;
  std::map<std::string, Texture> loaded_textures;

  void loadModel(const std::string& path);
  void processNodes(const aiNode* node, const aiScene* scene);
  void processMesh(const aiMesh* mesh, const aiScene* scene);
  std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type,
                                       const std::string& type_name);
};

#endif

