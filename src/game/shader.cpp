#include <GL/glew.h>
#include <SDL3/SDL.h>
#include <fstream>
#include <sstream>

#include "shader.h"

ShaderProgramSource ParseShader(const std::string &filepath)
{
	std::ifstream stream(filepath);
	enum class ShaderType
	{
		NONE = -1,
		VERTEX = -0,
		FRAGMENT = 1
	};

	std::string line;
	std::stringstream ss[2];
	ShaderType type = ShaderType::NONE;

	while (getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
			{
				type = ShaderType::VERTEX;
			}
			else if (line.find("fragment") != std::string::npos)
			{
				type = ShaderType::FRAGMENT;
			}
		}
		else
		{
			ss[(int)type] << line << "\n";
		}
	}
	return {ss[0].str(), ss[1].str()};
}

unsigned int CompileShader(unsigned int type,
						   const std::string &source)
{
	unsigned int id = glCreateShader(type);
	const char *src = source.c_str();
	glShaderSource(id, 1, &src, nullptr); // read the docs on this.
	glCompileShader(id);
	if (type == GL_VERTEX_SHADER)
	{
		SDL_Log("--- Compiling Vertex Shader ---");
	}
	else
	{
		SDL_Log("--- Compiling Fragment Shader ---");
	}
	SDL_Log("%s", src);
	SDL_Log("-----------------------------");
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);

	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char *message = (char *)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		if (type == GL_VERTEX_SHADER)
		{
			SDL_Log("Failed to compile vertex shader: %s", message);
		}
		else
		{
			SDL_Log("Failed to compile frag shader: %s", message);
		}
		glDeleteShader(id);
		return 0;
	}

	return id;
}

unsigned int CreateShader(const std::string &vertexShader,
						  const std::string &fragmentShader)
{
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);

	int isLinked;
	glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
	if (isLinked == GL_FALSE)
	{
		int maxLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
		char *infoLog = (char *)alloca(
			maxLength * sizeof(char)); // Use alloca for stack allocation
		glGetProgramInfoLog(program, maxLength, &maxLength, infoLog);
		SDL_Log("Failed to link shader program: %s", infoLog);
		glDeleteProgram(program);
		glDeleteShader(vs);
		glDeleteShader(fs);
		return 0;
	}

	glValidateProgram(program);

	int isValid;
	glGetProgramiv(program, GL_VALIDATE_STATUS, &isValid);
	if (isValid == GL_FALSE)
	{
		int maxLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
		char *infoLog = (char *)alloca(maxLength * sizeof(char));
		glGetProgramInfoLog(program, maxLength, &maxLength, infoLog);
		SDL_Log("Shader program validation failed: %s", infoLog);
	}

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}