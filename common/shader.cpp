#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

Shader::Shader(const GLchar* vertexPath, const GLchar* fragmentPath) {
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    // ensures ifstream objects throw exceptions
    vShaderFile.exceptions(std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::badbit);
    try {
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    } catch (std::ifstream::failure e) {
        std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
    }
    vShaderFile.close();
    fShaderFile.close();

    const char* vertexShaderSource = vertexCode.c_str();
    const char* fragmentShaderSource = fragmentCode.c_str();

    GLint success;
    GLchar infoLog[512];

    // prepare vertex shader
    GLuint vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // prepare fragment shader
    GLuint fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // prepare shader program
    this->Program = glCreateProgram();
    glAttachShader(this->Program, vertexShader);
    glAttachShader(this->Program, fragmentShader);
    glLinkProgram(this->Program);

    // delete shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

}

Shader::~Shader() {
    glDeleteProgram(this->Program);
}

void Shader::Use() {
    glUseProgram(this->Program);
}

GLuint Shader::Uniform(const char *name) {
    return glGetUniformLocation(this->Program, name);
}

void Shader::SetUniform(const char * name, const glm::mat4& mat) {
    GLuint loc = this->Uniform(name);
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::SetUniform(const char * name, const glm::mat3& mat) {
  GLuint loc = this->Uniform(name);
  glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::SetUniform(const char * name, const glm::vec3& vec) {
  GLuint loc = this->Uniform(name);
  glUniform3f(loc, vec.x, vec.y, vec.z);
}

void Shader::SetUniform(const char * name, const glm::vec4& vec) {
  GLuint loc = this->Uniform(name);
  glUniform4f(loc, vec.x, vec.y, vec.z, vec.w);
}

void Shader::SetUniform(const char * name, GLfloat x, GLfloat y, GLfloat z, GLfloat w) {
  GLuint loc = this->Uniform(name);
  glUniform4f(loc, x, y, z, w);
}

void Shader::SetUniform(const char * name, GLfloat x, GLfloat y, GLfloat z) {
  GLuint loc = this->Uniform(name);
  glUniform3f(loc, x, y, z);
}

void Shader::SetUniform(const char * name, GLfloat x) {
  GLuint loc = this->Uniform(name);
  glUniform1f(loc, x);
}
