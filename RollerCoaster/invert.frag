#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform float offset; // Determines the strength of the inversion, between 1 and 0

void main()
{
    vec3 color = texture(screenTexture, TexCoords).rgb;

    vec3 inverted = vec3(1.0) - color;

    vec3 result = mix(color, inverted, offset);

    FragColor = vec4(result, 1.0);
}
