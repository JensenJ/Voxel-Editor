#pragma once
#include <string>
#include <glm/glm.hpp>

class Shader {
public:
	Shader(unsigned int ID);

	void Use();
	void Delete();

	void SetBool(const std::string &name, bool value) const;
	void SetInt(const std::string& name, int value) const;
	void SetFloat(const std::string& name, float value) const;
	void SetMat4(const std::string& name, const glm::mat4& value) const;

	unsigned int GetShaderID();

private:
	unsigned int programID;
};

class ShaderLoader
{
public:
	static std::string LoadShader(const char* filePath);
	static unsigned int CreateShader(const char* filePath, unsigned int shaderType);
	static Shader CreateShaderProgram(unsigned int vertexShader, unsigned int fragmentShader, bool& outSuccess);
	static Shader CreateShaderProgram(const char* vertexPath, const char* fragmentPath, bool& outSuccess);
};
