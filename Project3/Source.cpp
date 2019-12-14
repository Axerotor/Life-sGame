
#define GLEW_STATIC

#include <gl/glew.h>
#include <gl/GL.h>
#include <GLFW/glfw3.h>

#include "Shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ft2build.h>

//#include <freetype/freetype.h>

//#include <freetype/ftglyph.h>

//#include <freetype/ftoutln.h>
#include <map>
//#include <freetype/fttrigon.h>
#include FT_FREETYPE_H


#include <iostream>



using namespace std;



struct Character {
	GLuint     TextureID; // ID текстуры глифа
	glm::ivec2 Size;      // Размеры глифа
	glm::ivec2 Bearing;   // Смещение верхней левой точки глифа
	GLuint     Advance;   // Горизонтальное смещение до начала следующего глифа
};


std::map<GLchar, Character> Characters;

GLvoid RendererText(Shader &s, string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color, GLuint& VAOt, GLuint& VBOt);
int main(void)
{
	
	/*
	//Точки
	GLfloat vert[] = {
		-1.0f, 1.0f, 
		-0.3f, 1.0f, 
		-0.3f, -1.0f,
		-1.0f, -1.0f 
	};

	//Индексы
	GLuint ind[] = { 0,1,3,1,2,3 };  
	*/
	//Размер окна
	int width = 480;
	int	height = 640;

	//Initialize the library 
	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create a windowed mode window and its OpenGL context 
	GLFWwindow* window = glfwCreateWindow(height, width, "Hello World", nullptr, nullptr);
	
	//Проверка на ошибку создания окна
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	// Make the window's context current 
	glfwMakeContextCurrent(window);
	
	//Инициализация GLEW
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{

		return -1;
	}
	
	//Инициализация FreeType
	FT_Library ft;
	if (FT_Init_FreeType(&ft)) {
		cout << "ERROR::FREETYPE: Could not init FreeType Library" << endl;
	}
	//Загружаем Шрифт. Высота 14, ширина определяется автоматически(0)
	FT_Face face;
	if (FT_New_Face(ft, "fonts/arial.ttf", 0, &face))
		cout << "ERROR::FREETYPE: Failed to load font" << endl;
	FT_Set_Pixel_Sizes(face, 0, 14);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction

	for (GLubyte c = 0; c < 128; c++)
	{
		// Load character glyph
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		// Generate texture
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);
		// Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Now store character for later use
		Character character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		Characters.insert(std::pair<GLchar, Character>(c, character));
		// Characters[c] = character;
	}

	
	
	/*
	//Матрица масштабирования:
	glm::vec4 victor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::mat4 masshtab = glm::mat4::mat()
	masshtab = glm::scale(masshtab, glm::vec3(width, height, 1.0f));
	//Матрица сдвига:
	masshtab = glm::translate(masshtab, glm::vec3(-width / 2, -height / 2, 0.0f));
	*/


	//Viewport
	glViewport(0, 0, width, height);
	/*
	//VBO & VAO & EBO  
	GLuint VBO, VAO, EBO;
	//Создаю VBO & VAO & EBO
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &EBO);
	//Привязываю VBO & VAO & EBO
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//Отсылаю данные в буфер
	glBufferData(GL_ARRAY_BUFFER, sizeof(vert), vert, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ind), ind, GL_STATIC_DRAW);
	//Аттрибут
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	//Отвязываю VAO, чтобы ничего не испортить в посл. коде
	glBindVertexArray(0);
	*/

	//VBO & VAO для текста
	GLuint VBOt, VAOt;
	glGenBuffers(1, &VBOt);
	glGenVertexArrays(1, &VAOt);
	glBindVertexArray(VAOt);
	glBindBuffer(GL_ARRAY_BUFFER, VBOt);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 24, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//Создадим класс Шейдер
	//Shader ourShader("shaders/shader.vs.txt", "shaders/shader.frag.txt");
	Shader ourShadert("shaders/shadert.vs.txt", "shaders/shadert.frag.txt");
	
	
	glClearColor(0.2f, 0.2f, 0.2f, 0.2f);

	

	//Работа с тектом:

	//Создадим шрифт
	
	
	/*
	//Работа с uniform
	GLuint transformLoc = glGetUniformLocation(ourShader.Program, "trans");
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(masshtab));

	GLuint projection = glGetUniformLocation(ourShadert.Program, "projection");
	glUniformMatrix4fv(projection, 1, GL_FALSE, glm::value_ptr(masshtab));
	*/
	
	// Loop until the user closes the window 
	
	while (!glfwWindowShouldClose(window))
	{
		/*
		glClear(GL_COLOR_BUFFER_BIT || GL_DEPTH_BUFFER_BIT);
		ourShader.Use();

		glBindVertexArray(VAO); //Использую заготовленный VAO
		
		//glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(masshtab));
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); //Отрисовываю
		glBindVertexArray(0); //Отвязываю VAO
		*/
		
		ourShadert.Use();

		glBindVertexArray(VAOt);
		glm::vec3 color(0.0f, 1.0f, 1.0f);
		RendererText(ourShadert, "FreeType", 0.5f, 0.5f, 0.5f, color, VAOt, VBOt);
		glBindVertexArray(0);
		
		

		// Swap front and back buffers 
		glfwSwapBuffers(window);

		// Poll for and process events 
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
	
}

GLvoid RendererText(Shader &s, string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color, GLuint& VAOt, GLuint& VBOt) {
	s.Use();
	glUniform3f(glGetUniformLocation(s.Program, "textColor"), color.x, color.y, color.z);
	//glUniformMatrix4f()
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAOt);
	// Iterate through all characters
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = Characters[*c];

		GLfloat xpos = x + ch.Bearing.x * scale;
		GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

		GLfloat w = ch.Size.x * scale;
		GLfloat h = ch.Size.y * scale;
		// Update VBO for each character
		GLfloat vertices[6][4] = {
			{ xpos, ypos + h, 0.0, 0.0 },
			{ xpos, ypos, 0.0, 1.0 },
			{ xpos + w, ypos, 1.0, 1.0 },
			{ xpos, ypos + h, 0.0, 0.0 },
			{ xpos + w, ypos, 1.0, 1.0 },
			{ xpos + w, ypos + h, 1.0, 0.0 }
		};
		// Render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		// Update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, VBOt);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// Render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}
