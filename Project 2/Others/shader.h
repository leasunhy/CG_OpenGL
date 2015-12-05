#pragma once
#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

#include <GL/glew.h>; // ����glew��ȡ���е�OpenGL��Ҫheaders

class Shader
{
public:
	// ����ID
	GLuint Program;
	// ��������ȡ������Shader
	Shader(const GLchar * vertexSourcePath, const GLchar * fragmentSourcePath);
	// ʹ��Program
	void Use();
};

#endif