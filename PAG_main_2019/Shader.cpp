#include "Shader.h"

// =============================================================================
// ===== Public Member Functions

Shader::Shader(const GLchar* vertexShaderPath, const GLchar* fragmentShaderPath, const GLchar* geometryShaderPath)
{
	std::string vertexCode, fragmentCode, geometryCode;
	readShader(vertexShaderPath, vertexCode);
	readShader(fragmentShaderPath, fragmentCode);

	// Read geomety shader only if specified
	if (geometryShaderPath != nullptr)
	{
		readShader(geometryShaderPath, geometryCode);
	}

	// Compile shaders
	unsigned int vertex, fragment, geometry;
	int success;
	char infoLog[512];

	// Vertex Shader compilation
	vertex = compileShader(vertexCode.c_str(), GL_VERTEX_SHADER);
	// Fragment Shader compilation
	fragment = compileShader(fragmentCode.c_str(), GL_FRAGMENT_SHADER);
	// Geometry Shader compilation
	if (geometryShaderPath != nullptr)
	{
		geometry = compileShader(geometryCode.c_str(), GL_GEOMETRY_SHADER);
	}

	// Shader program creation
	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	if (geometryShaderPath != nullptr)
	{
		glAttachShader(ID, geometry);
	}
	glLinkProgram(ID);

	// Print linking errors
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(ID, 512, nullptr, infoLog);
		std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << "\n";
	}

	// Delete unnecessary shaders
	glDeleteShader(vertex);
	glDeleteShader(fragment);
	if (geometryShaderPath != nullptr)
	{
		glDeleteShader(geometry);
	}
}

void Shader::Use()
{
	glUseProgram(ID);
}

void Shader::SetBool(const std::string &name, bool value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}
void Shader::SetInt(const std::string &name, int value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::SetFloat(const std::string &name, float value) const
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::SetVec3(const std::string &name, float x, float y, float z) const
{
	glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}
void Shader::SetVec3(const std::string& name, const glm::vec3& vec) const
{
	glUniform3f(glGetUniformLocation(ID, name.c_str()), vec.x, vec.y, vec.z);
}
void Shader::SetVec4(const std::string &name, float x, float y, float z, float w) const
{
	glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}
void Shader::SetVec4(const std::string & name, const glm::vec4 & vec) const
{
	glUniform4f(glGetUniformLocation(ID, name.c_str()), vec.x, vec.y, vec.z, vec.w);
}
void Shader::SetMat4(const std::string &name, const glm::mat4 &mat) const
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

// =============================================================================
// ===== Private Member Functions

void Shader::readShader(const GLchar* shaderPath, std::string &shaderCode) const
{
	std::ifstream shaderFile;
	shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit); // Make sure that filestream can throw exceptions
	try
	{
		shaderFile.open(shaderPath);
		std::stringstream shaderStream;
		shaderStream << shaderFile.rdbuf();
		shaderFile.close();
		shaderCode = shaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cerr << "ERROR. Shader file not succesfully read.\n";
	}
}

unsigned int Shader::compileShader(const char* shaderCode, GLenum shaderType)
{
	unsigned int shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &shaderCode, nullptr);
	glCompileShader(shader);

	int success;
	char infoLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader, 512, nullptr, infoLog);
		std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << "\n";
	}

	return shader;
}