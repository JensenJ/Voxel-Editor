#pragma once
#include <string>

class Shader {
public:
	Shader(unsigned int ID);

	void Use();
	void Delete();

	void SetBool(const std::string &name, bool value) const;
	void SetInt(const std::string& name, int value) const;
	void SetFloat(const std::string& name, float value) const;

private:
	unsigned int programID;
};

class ShaderLoader
{
public:
	static const char* LoadShader(const char* filePath);
	static unsigned int CreateShader(const char* filePath, unsigned int shaderType);
	static Shader CreateShaderProgram(unsigned int vertexShader, unsigned int fragmentShader, bool& outSuccess);
	static Shader CreateShaderProgram(const char* vertexPath, const char* fragmentPath, bool& outSuccess);
};
