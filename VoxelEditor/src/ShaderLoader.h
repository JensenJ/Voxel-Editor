#pragma once

class ShaderLoader
{
public:
	static const char* LoadShader(const char* filePath);
	static unsigned int CreateShader(const char* filePath, unsigned int shaderType);
	static unsigned int CreateShaderProgram(unsigned int vertexShader, unsigned int fragmentShader);
	static unsigned int CreateShaderProgram(const char* vertexPath, const char* fragmentPath);
};

