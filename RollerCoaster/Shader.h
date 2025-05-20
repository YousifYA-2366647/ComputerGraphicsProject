#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm-1.0.1/glm/glm.hpp>
#include <glm-1.0.1/glm/gtc/type_ptr.hpp>

#pragma once
class Shader
{
public:
	// Construct a Shader object with the vertex shader and fragment shader
	// Will create a program for the shaders
	Shader(const char* vertexPath, const char* fragmentPath);
	
	// Tell the 'Shader' object to use the shader program
	void use();

	// Value setters for if the shaders need to be updated
	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setVec3(const std::string& name, glm::vec3 value) const;
	void setVec4(const std::string& name, glm::vec4 value) const;
private:
	// The id of the shader program
	unsigned int ID;
};

#endif