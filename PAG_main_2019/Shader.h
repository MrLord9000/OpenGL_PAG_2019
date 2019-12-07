#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
	// Shader program ID
	unsigned int ID;

	// Constructor should automatically read from file and compile the shader
	Shader(const GLchar* vertexShaderPath, const GLchar* fragmentShaderPath, const GLchar* geometryShaderPath = nullptr);

	// Activate/Use/Bind the shader
	void Use();
	// Uniform utilities (just like the functions in Unity)
	void SetBool(const std::string &name, bool value) const;
	void SetInt(const std::string &name, int value) const;
	void SetFloat(const std::string &name, float value) const;
	void SetVec3(const std::string &name, float x, float y, float z) const;
	void SetVec4(const std::string &name, float x, float y, float z, float w) const;
	void SetVec4(const std::string &name, const glm::vec4 &vec) const;
	void SetMat4(const std::string &name, const glm::mat4 &mat) const;
private:
	void readShader(const GLchar* shaderPath, std::string &shaderCode) const;
	unsigned int compileShader(const char* shaderCode, GLenum shaderType);
};

#endif // !SHADER_H


