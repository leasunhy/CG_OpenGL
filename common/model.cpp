#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <set>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "common/model.h"
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
