#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;

out vec3 vertColor;
out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    FragPos = vec3(model * vec4(position, 1.0));
    
    // Bereken normale vector 
    // gebruiken we de genormaliseerde positie
    Normal = normalize(mat3(transpose(inverse(model))) * normalize(position));
    gl_Position = projection * view * model * vec4(position, 1.0);
    vertColor = color;
}