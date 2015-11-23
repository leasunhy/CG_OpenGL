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

    // use the program
    void Use();

    GLuint Uniform(const char * name);

    void SetUniform(const char * name, const glm::mat4& mat);
    void SetUniform(const char * name, const glm::mat3& mat);
    void SetUniform(const char * name, const glm::vec4& vec);
    void SetUniform(const char * name, const glm::vec3& vec);
};
#endif

