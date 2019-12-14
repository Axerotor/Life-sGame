#include "Shader.h"

Shader::Shader(const GLchar * vertexPath, const GLchar * fragmentPath)
{
	string vertexCode = "";
	string fragmentCode = "";
	ifstream vShaderFile;
	ifstream fShaderFile;

	string line;

	try {
		//��������� ����
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);

		//��������� ������ � ������
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

		//��������� ����
		vShaderFile.close();
		fShaderFile.close();

		//��������������� ������ � ������ GLchar
		//vertexCode = vShaderStream.str();
		//fragmentCode = fShaderStream.str();
	}
	catch (...) {
		cout << "Error READ FILE!" << endl;
	}

	//�������� ���
	const GLchar* vShaderCode = vertexCode.c_str();
	const GLchar* fShaderCode = fragmentCode.c_str();

	//������ �������
	GLuint vertex, fragment;
	GLint success;
	GLchar infoLog[512];

	//��������� ������
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, nullptr);
	glCompileShader(vertex);

	//�������� �� ������. ���� ��� ���� - ����� �� � cout
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex, 512, nullptr, infoLog);
		cout << "ERROR SHADER VERTEX COMPILATION FAILED\n" << infoLog << endl;
	}


	//��� ������������ �������
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, nullptr);
	glCompileShader(fragment);

	//�������� �� ������. ���� ��� ���� - ����� �� � cout
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragment, 512, nullptr, infoLog);
		cout << "ERROR SHADER FRAGMENT COMPILATION FAILED\n" << infoLog << endl;
	}

	//��������� ���������
	this->Program = glCreateProgram();
	glAttachShader(this->Program, vertex);
	glAttachShader(this->Program, fragment);
	glLinkProgram(this->Program);

	//�������� �� ������. ���� ��� ���� - ����� �� � cout
	glGetProgramiv(this->Program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(this->Program, 512, nullptr, infoLog);
		cout << infoLog << endl;
	}

	// ������� �������, ��������� ��� ��� � ��������� � ��� ������ �� �����.
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}


void Shader::Use()
{
	glUseProgram(this->Program);
}
