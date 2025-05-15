#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;

out vec3 vertColor;

uniform vec3 color1;
uniform vec3 color2;
uniform vec3 color3;

void main() {
	gl_Position = vec4(position, 1.0);
	if (position.x > 0.0 && position.y < 0.0)
        vertColor = color3;
    else if (position.x < 0.0 && position.y < 0.0)
        vertColor = color2;
    else
        vertColor = color1;
}