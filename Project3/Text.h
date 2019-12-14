#pragma once
#define GLEW_STATIC

#include <gl/glew.h>
#include <gl/GL.h>
#include <GLFW/glfw3.h>

#include "Shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ft2build.h>

#include <freetype/freetype.h>

#include <freetype/ftglyph.h>

#include <freetype/ftoutln.h>

#include <freetype/fttrigon.h>
#include FT_FREETYPE_H
#include <vector>

using std::string;


namespace Freetype {
	struct text {
	public:
		float h;		//������
		GLuint * textures;		 // �������������
		GLuint list_base;           // �������� ��������� �� ������ �����������
		
									
		// ������� ������������� ������� ����� �
		// ������� h �� ����� fname
	void init(const char * fname, unsigned int h);

	// ����������� ������� ��������� �� �������
	void clean();
	};

			// ������� ������� ���������� -  ��� ����� ��������
			// ����� � ���� �� ����������� X,Y ��������� Font ft_font.
			// ������� ������� ���� ������ ����� ����� ��������� � ������
	void print(const text &ft_font, float x, float y, const char *fmt, ...);

		

};