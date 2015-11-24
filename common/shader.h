#ifndef SHADER_H
#define SHADER_H


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

    GLint Uniform(const char * name);

    void SetUniform(const char * name, const glm::mat4& mat);
    void SetUniform(const char * name, const glm::mat3& mat);
    void SetUniform(const char * name, const glm::vec4& vec);
    void SetUniform(const char * name, const glm::vec3& vec);
    void SetUniform(const char * name, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
    void SetUniform(const char * name, GLfloat x, GLfloat y, GLfloat z);
    void SetUniform(const char * name, GLfloat x);
    void SetUniform(const char * name, int x);
};
#endif

