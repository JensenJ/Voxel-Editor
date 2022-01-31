#include "ShaderLoader.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <fstream>
#include <iostream>
#include <string>

//Load a shader from a file
const char* ShaderLoader::LoadShader(const char* filePath)
{
	std::string shader = "";
	std::string line;
	std::ifstream file;
	file.open(filePath, std::ios::in);
	if (file.is_open())
	{
		while (getline(file, line))
		{
			shader += line + "\n";
		}
		file.close();
	}
	else
	{
		std::cout << "Unable to open file: " << filePath << std::endl;
		return nullptr;
	}

	return shader.c_str();
}

//Create a shader of a type from a specified file path
unsigned int ShaderLoader::CreateShader(const char* filePath, unsigned int shaderType)
{
	const char* shaderSource = LoadShader(filePath);

	unsigned int shader;
	shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &shaderSource, NULL);
	glCompileShader(shader);

	int success;
	char infoLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		std::cout << "Error: Shader " << filePath << "failed to compile \n" << infoLog << std::endl;
	}

	return shader;
}

//Create a shader program using a vertex shader and fragment shader
unsigned int ShaderLoader::CreateShaderProgram(unsigned int vertexShader, unsigned int fragmentShader)
{
	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();

	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	int success;
	char infoLog[512];
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "Error: Shader linking failed \n" << infoLog << std::endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}

//Create a shader program from a vertex and fragment shader file path
unsigned int ShaderLoader::CreateShaderProgram(const char* vertexPath, const char* fragmentPath)
{
	unsigned int vertexShader = CreateShader(vertexPath, GL_VERTEX_SHADER);
	unsigned int fragmentShader = CreateShader(fragmentPath, GL_FRAGMENT_SHADER);

	return CreateShaderProgram(vertexShader, fragmentShader);
}
