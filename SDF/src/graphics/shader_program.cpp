/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent of
DigiPen Institute of Technology is prohibited.
File Name:	shader_program.cpp
Purpose: Program/Shader Interface
Author: Gabriel Ma�eru - gabriel.m
- End Header --------------------------------------------------------*/

#include "shader_program.h"
#include <gl/gl3w.h>
#include "gl_error.h"
#include <fstream>
#include <sstream>
#include <iostream>

shader_program::shader_program(const std::string & vtx, const std::string & frag)
{
	const char * path = "../resources/shaders/";
	paths[0] = path + vtx;
	paths[1] = path + frag;
	if (create_handle())
		compile_program();
}
shader_program::~shader_program()
{
	if (m_handle > 0)
		GL_CALL(glDeleteProgram(m_handle));
}

bool shader_program::is_valid()const
{
	return m_handle > 0 && m_linked;
}

void shader_program::recompile()
{
	try {
		if (m_handle > 0)
			GL_CALL(glDeleteProgram(m_handle));
		m_handle = 0;
		m_linked = false;


		if (create_handle())
			compile_program();
	}
	catch (const std::string & log) { std::cout << log; }
}

void shader_program::use() const
{
	if (is_valid())
		GL_CALL(glUseProgram(m_handle));
}

void shader_program::set_uniform(const char * name, bool val) const
{
	set_uniform(name, val ? 1 : 0);
}

void shader_program::set_uniform(const char * name, int val) const
{
	int loc = uniform_location(name);
	if (loc >= 0)
		GL_CALL(glUniform1i(loc, val));
}

void shader_program::set_uniform(const char * name, float val) const
{
	int loc = uniform_location(name);
	if (loc >= 0)
		GL_CALL(glUniform1f(loc, val));
}

void shader_program::set_uniform(const char * name, const vec2 & v) const
{
	int loc = uniform_location(name);
	if (loc >= 0)
		GL_CALL(glUniform2f(loc, v.x, v.y));
}

void shader_program::set_uniform(const char * name, const vec3 & v) const
{
	int loc = uniform_location(name);
	if (loc >= 0)
		GL_CALL(glUniform3f(loc, v.x, v.y, v.z));
}

void shader_program::set_uniform(const char * name, const vec4 & v) const
{
	int loc = uniform_location(name);
	if (loc >= 0)
		GL_CALL(glUniform4f(loc, v.x, v.y, v.z, v.w));
}

void shader_program::set_uniform(const char * name, const mat3 & m) const
{
	int loc = uniform_location(name);
	if (loc >= 0)
		GL_CALL(glUniformMatrix3fv(loc, 1, GL_FALSE, &m[0][0]));
}

void shader_program::set_uniform(const char * name, const mat4 & m) const
{
	int loc = uniform_location(name);
	if (loc >= 0)
		GL_CALL(glUniformMatrix4fv(loc, 1, GL_FALSE, &m[0][0]));
}

void shader_program::set_uniform_sampler(const int & val) const
{
	if (val >= 0)
		GL_CALL(glUniform1i(val, val));
}

void shader_program::set_uniform_subroutine(unsigned int shader_type, const std::string & value)
{
	auto it_loc = m_subroutine_values.find(value);
	if (it_loc == m_subroutine_values.end())
	{
		unsigned int new_loc = GL_CALL(glGetSubroutineIndex(m_handle, shader_type, value.c_str()));
		m_subroutine_values[value] = new_loc;
		it_loc = m_subroutine_values.find(value);
	}
	GL_CALL(glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &it_loc->second));
}

bool shader_program::create_handle()
{
	if (m_handle <= 0)
	{
		m_handle = GL_CALL(glCreateProgram());
		if (m_handle == 0)
		{
			std::cout << "Unable to create shader program." << std::endl;
			return false;
		}
	}
	return true;
}

void shader_program::compile_program()
{
	if (compile_shader(paths[0], e_shader_type::VERTEX))
		if (compile_shader(paths[1], e_shader_type::FRAGMENT))
			link();

	if (!is_valid())
		throw std::string("Compile Error");
}

bool shader_program::compile_shader(const std::string & filename, const e_shader_type & type)
{
	// Open file
	std::ifstream code_file(filename.c_str(), std::ios::in);
	if (!code_file.is_open())
	{
		std::cout << "File not found." << std::endl;
		return false;
	}

	// Dump code
	std::ostringstream tmp_code;
	while (code_file.good())
	{
		int c = code_file.get();
		if (!code_file.eof())
			tmp_code << static_cast<char>(c);
	}
	code_file.close();
	std::string str_code{ tmp_code.str() };
	const char * code = str_code.c_str();

	// Create Handle
	GLuint shader = 0;
	switch (type)
	{
	case e_shader_type::VERTEX:
		shader = GL_CALL(glCreateShader(GL_VERTEX_SHADER));
		break;
	case e_shader_type::TESS_CONTROL:
		shader = GL_CALL(glCreateShader(GL_TESS_CONTROL_SHADER));
		break;
	case e_shader_type::TESS_EVAL:
		shader = GL_CALL(glCreateShader(GL_TESS_EVALUATION_SHADER));
		break;
	case e_shader_type::GEOMETRY:
		shader = GL_CALL(glCreateShader(GL_GEOMETRY_SHADER));
		break;
	case e_shader_type::FRAGMENT:
		shader = GL_CALL(glCreateShader(GL_FRAGMENT_SHADER));
		break;
	default:
		return false;
	}
	GL_CALL(glShaderSource(shader, 1, &code, NULL));
	GL_CALL(glCompileShader(shader));

	// Check errors
	int result;
	GL_CALL(glGetShaderiv(shader, GL_COMPILE_STATUS, &result));
	if (GL_FALSE == result)
	{
		int length{ 0 };
		GL_CALL(glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length));
		if (length > 0)
		{
			char * c_log = new char[length];
			int written = 0;
			GL_CALL(glGetShaderInfoLog(shader, length, &written, c_log));
			std::cout << c_log << std::endl;
			delete[] c_log;
		}

		return false;
	}

	// Attach
	GL_CALL(glAttachShader(m_handle, shader));
	GL_CALL(glDeleteShader(shader));
	
	return true;
}
void shader_program::link()
{
	if (m_linked || m_handle <= 0)
		return;

	// Link
	GL_CALL(glLinkProgram(m_handle));

	// Check Errors
	int status = 0;
	GL_CALL(glGetProgramiv(m_handle, GL_LINK_STATUS, &status));
	if (GL_FALSE == status)
	{
		int length = 0;
		GL_CALL(glGetProgramiv(m_handle, GL_INFO_LOG_LENGTH, &length));
		if (length > 0)
		{
			char * c_log = new char[length];
			int written = 0;
			GL_CALL(glGetProgramInfoLog(m_handle, length, &written, c_log));
			std::cout << c_log << std::endl;
			delete[] c_log;
		}
	}
	else
		m_linked = true;
}

int shader_program::uniform_location(const char * name) const
{
	auto it = m_uniform_location_map.find(name);
	if (it != m_uniform_location_map.end())
		return it->second;

	int tmp_handle = GL_CALL(glGetUniformLocation(m_handle, name));
	if (tmp_handle >= 0)
		m_uniform_location_map[name] = tmp_handle;
	else
		std::cout << "Uniform: " << name << " not found." << std::endl;
	return tmp_handle;
}
