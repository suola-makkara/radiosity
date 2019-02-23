#include <iostream>
#include <fstream>
#include <sstream>

#include "shader.hpp"


unsigned int ShaderProgram::compileShader(unsigned int type,
		const std::string &file)
{
	std::ifstream stream(file);
	std::stringstream ss;

	ss << stream.rdbuf();

	std::string shaderSource = ss.str();
	const char *shaderSourcePtr = shaderSource.c_str();

	unsigned int shader;

	shader = glCreateShader(type);

	glShaderSource(shader, 1, &shaderSourcePtr, NULL);

	glCompileShader(shader);

	int compiled;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if (!compiled)
	{
		char infoLog[512];
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		std::cout << "Failed to compile shader: " << infoLog;
	}

	return shader;
}

ShaderProgram::ShaderProgram(
		const std::string &vertexShaderFile,
		const std::string &fragmentShaderFile)
{
	unsigned int vertexShader;
	unsigned int fragmentShader;

	vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderFile);
	fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderFile);

	id = glCreateProgram();

	glAttachShader(id, vertexShader);
	glAttachShader(id, fragmentShader);
	glLinkProgram(id);

	int linked;
	glGetProgramiv(id, GL_LINK_STATUS, &linked);
	if (!linked)
	{
		char infoLog[512];
		glGetProgramInfoLog(id, 512, NULL, infoLog);
		std::cout << "Failed to link shader program:\n" << infoLog;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void ShaderProgram::use()
{
	glUseProgram(id);
}


void ShaderProgram::set1i(const std::string &name, int value)
{
	glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

void ShaderProgram::set1f(const std::string &name, float value)
{
	glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}
void ShaderProgram::set2fv(const std::string &name, const glm::vec2 &value)
{
	glUniform2fv(glGetUniformLocation(id, name.c_str()), 1, glm::value_ptr(value));
}
void ShaderProgram::set3fv(const std::string &name, const glm::vec3 &value)
{
	glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, glm::value_ptr(value));
}

void ShaderProgram::setMat3fv(const std::string &name, const glm::mat3 &value)
{
	glUniformMatrix3fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}
void ShaderProgram::setMat4fv(const std::string &name, const glm::mat4 &value)
{
	glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}
