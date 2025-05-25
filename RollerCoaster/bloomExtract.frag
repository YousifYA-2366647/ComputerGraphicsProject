#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform float threshold;

void main() {
    vec3 color = texture(screenTexture, TexCoords).rgb;
    
    float brightness = max(max(color.r, color.g), color.b);
    
    float contribution = max(0.0, brightness - threshold);
    contribution = pow(contribution * 2.0, 1.5);
    
    FragColor = vec4(color * contribution, 1.0);
}