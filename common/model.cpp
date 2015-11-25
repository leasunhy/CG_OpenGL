#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <set>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "common/mesh.h"
#include "common/model.h"
#include "common/texture.h"

Model::Model(const GLchar * path) {
  this->loadModel(path);
}

void Model::Draw(Shader& shader, bool useTexture) {
  for (auto& m : this->meshes)
    m.Draw(shader, useTexture);
}

void Model::loadModel(const std::string& path) {
  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

  if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
    std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
    return;
  }

  this->directory = path.substr(0, path.find_last_of('/'));

  this->processNodes(scene->mRootNode, scene);
}

void Model::processNodes(const aiNode* node, const aiScene* scene) {
  std::cout << "Processing nodes..." << std::endl;
  for (GLuint i = 0; i < node->mNumMeshes; ++i) {
    const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
    this->processMesh(mesh, scene);
  }
  for (GLuint i = 0; i < node->mNumChildren; ++i)
    this->processNodes(node->mChildren[i], scene);
}

void Model::processMesh(const aiMesh* mesh, const aiScene* scene) {
  std::vector<Vertex> vertices;
  std::vector<GLuint> indices;
  std::vector<Texture> textures;

  for (GLuint i = 0; i < mesh->mNumVertices; ++i) {
    Vertex vertex;
    vertex.Position.x = mesh->mVertices[i].x;
    vertex.Position.y = mesh->mVertices[i].y;
    vertex.Position.z = mesh->mVertices[i].z;

    vertex.Normal.x = mesh->mNormals[i].x;
    vertex.Normal.y = mesh->mNormals[i].y;
    vertex.Normal.z = mesh->mNormals[i].z;

    // if model contains texcoords
    if (mesh->HasTextureCoords(0)) {
      vertex.TexCoords.x = mesh->mTextureCoords[0][i].x;
      vertex.TexCoords.y = mesh->mTextureCoords[0][i].y;
    } else {
      vertex.TexCoords.x = 0.0f;
      vertex.TexCoords.y = 0.0f;
    }

    if (mesh->mColors && mesh->mColors[0]) {
      vertex.Colors.r = mesh->mColors[0][i].r;
      vertex.Colors.g = mesh->mColors[0][i].g;
      vertex.Colors.b = mesh->mColors[0][i].b;
      vertex.Colors.a = mesh->mColors[0][i].a;
    } else {
      vertex.Colors.r = 1.0f;
      vertex.Colors.g = 1.0f;
      vertex.Colors.b = 1.0f;
      vertex.Colors.a = 1.0f;
    }

    vertices.push_back(vertex);
  }

  for (GLuint i = 0; i < mesh->mNumFaces; ++i) {
    const aiFace& face = mesh->mFaces[i];
    for (GLuint j = 0; j < face.mNumIndices; ++j)
      indices.push_back(face.mIndices[j]);
  }

  aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
  std::vector<Texture> diffuseMaps =
    this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
  textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
  std::vector<Texture> specularMaps =
    this->loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
  textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

  this->meshes.emplace_back(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat,
                                                 aiTextureType type,
                                                 const std::string& type_name) {
  std::vector<Texture> textures;
  for (GLuint i = 0; i < mat->GetTextureCount(type); ++i) {
    aiString str;
    mat->GetTexture(type, i, &str);
    auto iter = loaded_textures.find(str.C_Str());
    if (iter != loaded_textures.end()) {
      textures.push_back(iter->second);
      continue;
    }
    Texture texture;
    texture.id = load_texture(this->directory + '/' + str.C_Str());
    texture.type = type_name;
    textures.push_back(texture);
    loaded_textures[str.C_Str()] = texture;
  }
  return textures;
}
