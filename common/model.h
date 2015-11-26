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

#include "common/mesh.h"
#include "common/shader.h"

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

