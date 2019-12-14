#pragma once
#ifndef _SHADER_H_
#define _SHADER_H_

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>

using namespace std;

class Shader {

public:
	// ������������� ���������
	GLuint Program;

	// ����������� ��������� � �������� ������
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath);

	// ������������� ���������
	void Use();

};







#endif