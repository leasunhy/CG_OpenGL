#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>
#include <string>

void init_texture_loading();

GLuint load_texture(const std::string& filename, GLenum target = GL_TEXTURE_2D);

#endif
