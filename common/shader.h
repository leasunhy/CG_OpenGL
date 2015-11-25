#ifndef SHADER_H
#define SHADER_H


#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>

class Shader
{
  public:
    // The program ID
    GLuint Program;

    // ctor
    Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
    ~Shader();

    // use the program
    void Use();

    GLint Uniform(const std::string& name);

    void SetUniform(const std::string& name, const glm::mat4& mat);
    void SetUniform(const std::string& name, const glm::mat3& mat);
    void SetUniform(const std::string& name, const glm::vec4& vec);
    void SetUniform(const std::string& name, const glm::vec3& vec);
    void SetUniform(const std::string& name, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
    void SetUniform(const std::string& name, GLfloat x, GLfloat y, GLfloat z);
    void SetUniform(const std::string& name, GLfloat x);
    void SetUniform(const std::string& name, int x);
};
#endif

