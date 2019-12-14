#include "Shader.h"

Shader::Shader(const GLchar * vertexPath, const GLchar * fragmentPath)
{
	string vertexCode = "";
	string fragmentCode = "";
	ifstream vShaderFile;
	ifstream fShaderFile;

	string line;

	try {
		//Открываем файл
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);

		//Считываем данные в потоки
		//stringstream vShaderStream, fShaderStream;
		while (getline(vShaderFile, line))
		{
			vertexCode = vertexCode + line;
		}
		while (getline(fShaderFile, line))
		{ 
			fragmentCode = fragmentCode + line;
		}
		//vShaderStream << vShaderFile.rdbuf();
		//fShaderStream << fShaderFile.rdbuf();

		//Закрываем файл
		vShaderFile.close();
		fShaderFile.close();

		//Преобразовываем потоки в массив GLchar
		//vertexCode = vShaderStream.str();
		//fragmentCode = fShaderStream.str();
	}
	catch (...) {
		cout << "Error READ FILE!" << endl;
	}

	//Копируем код
	const GLchar* vShaderCode = vertexCode.c_str();
	const GLchar* fShaderCode = fragmentCode.c_str();

	//Сборка шейдера
	GLuint vertex, fragment;
	GLint success;
	GLchar infoLog[512];

	//Вершинный шейдер
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, nullptr);
	glCompileShader(vertex);

	//Проверка на ошибки. Если они есть - вывод их в cout
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex, 512, nullptr, infoLog);
		cout << "ERROR SHADER VERTEX COMPILATION FAILED\n" << infoLog << endl;
	}


	//Для Фрагментного шейдера
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, nullptr);
	glCompileShader(fragment);

	//Проверка на ошибки. Если они есть - вывод их в cout
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragment, 512, nullptr, infoLog);
		cout << "ERROR SHADER FRAGMENT COMPILATION FAILED\n" << infoLog << endl;
	}

	//Шейдерная программа
	this->Program = glCreateProgram();
	glAttachShader(this->Program, vertex);
	glAttachShader(this->Program, fragment);
	glLinkProgram(this->Program);

	//Проверка на ошибки. Если они есть - вывод их в cout
	glGetProgramiv(this->Program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(this->Program, 512, nullptr, infoLog);
		cout << infoLog << endl;
	}

	// Удаляем шейдеры, поскольку они уже в программе и нам больше не нужны.
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}


void Shader::Use()
{
	glUseProgram(this->Program);
}
