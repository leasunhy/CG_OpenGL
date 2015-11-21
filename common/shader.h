#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>

class Shader
{
  public:
    // The program ID
    GLuint Program;

    // ctor
    Shader(const GLchar* vertexPath, const GLchar* fragmentPath);

    // use the program
    void Use();
};

#endif

