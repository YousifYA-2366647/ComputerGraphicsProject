#ifndef VERTEX_H
#define VERTEX_H

#include <glm-1.0.1/glm/glm.hpp>
// Struct to save vertex position, color and textures
struct Vertex {
	glm::vec3 Position;
	glm::vec3 Color;
	glm::vec2 Texture;
};

#endif