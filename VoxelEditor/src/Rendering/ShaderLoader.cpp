#include "ShaderLoader.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <fstream>
#include <iostream>
#include <string>

//Load a shader from a file
std::string ShaderLoader::LoadShader(const char* filePath)
{
	std::string shader = "";
	std::string line;
	std::ifstream file;

	try
	{
		file.open(filePath, std::ios::in);
		if (file.is_open())
		{
			while (getline(file, line))
			{
				shader.append(line + "\n");
			}
			file.close();
		}
		else
		{
			std::cout << "Unable to open file: " << filePath << std::endl;
			return shader;
		}
	}
	catch (const std::ifstream::failure& e)
	{
		std::cout << "Unable to open file: " << &e << std::endl;
		return shader;
	}

	return shader;
}

//Create a shader of a type from a specified file path
unsigned int ShaderLoader::CreateShader(const char* filePath, unsigned int shaderType)
{
	std::string shaderSource = LoadShader(filePath);
	const char* shaderString = shaderSource.c_str();

	unsigned int shader;
	shader = glCreateShader(shaderType);

	glShaderSource(shader, 1, &shaderString, NULL);
	glCompileShader(shader);

	int success;
	char infoLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		std::cout << "Error: Shader " << filePath << " failed to compile \n" << infoLog << std::endl;
	}

	return shader;
}

//Create a shader program using a vertex shader and fragment shader
Shader ShaderLoader::CreateShaderProgram(unsigned int vertexShader, unsigned int fragmentShader, bool& outSuccess)
{
	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();

	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	outSuccess = true;
	int success;
	char infoLog[512];
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "Error: Shader linking failed \n" << infoLog << std::endl;
		outSuccess = false;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return Shader(shaderProgram);
}

//Create a shader program from a vertex and fragment shader file path
Shader ShaderLoader::CreateShaderProgram(const char* vertexPath, const char* fragmentPath, bool& outSuccess)
{
	unsigned int vertexShader = CreateShader(vertexPath, GL_VERTEX_SHADER);
	unsigned int fragmentShader = CreateShader(fragmentPath, GL_FRAGMENT_SHADER);

	return CreateShaderProgram(vertexShader, fragmentShader, outSuccess);
}

//SHADER CLASS

Shader::Shader(unsigned int ID)
{
	programID = ID;
}

void Shader::Use()
{
	glUseProgram(programID);
}

void Shader::Delete()
{
	glDeleteProgram(programID);
}

void Shader::SetBool(const std::string& name, bool value) const
{
	glUniform1i(glGetUniformLocation(programID, name.c_str()), (int)value);
}

void Shader::SetInt(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(programID, name.c_str()), value);
}

void Shader::SetFloat(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(programID, name.c_str()), value);
}

void Shader::SetMat4(const std::string& name, const glm::mat4& value) const
{
	glUniformMatrix4fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, &value[0][0]);
}

unsigned int Shader::GetShaderID()
{
	return programID;
}
