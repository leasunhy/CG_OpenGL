#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>
#include <string>

void init_texture_loading();

GLuint load_texture(const std::string& filename, GLenum wrap_s = GL_REPEAT,
                    GLenum wrap_t = GL_REPEAT);

#endif
