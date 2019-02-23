#ifndef SHADER
#define SHADER

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>


class ShaderProgram
{
private:
	unsigned int compileShader(unsigned int type, const std::string &file);
public:
	unsigned int id;

	ShaderProgram() = default;
	ShaderProgram(
			const std::string &vertexShaderFile,
			const std::string &fragmentShaderFile);

	void use();

	void set1i(const std::string &name, int value);

	void set1f(const std::string &name, float value);
	void set2fv(const std::string &name, const glm::vec2 &value);
	void set3fv(const std::string &name, const glm::vec3 &value);

	void setMat3fv(const std::string &name, const glm::mat3 &value);
	void setMat4fv(const std::string &name, const glm::mat4 &value);
};

#endif // SHADER
