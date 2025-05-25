#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform float offset; // Determines the strength of the gray scale, between 1 and 0

void main()
{
    vec3 color = texture(screenTexture, TexCoords).rgb;

    float gray = dot(color, vec3(0.299, 0.587, 0.114));
    vec3 grayscale = vec3(gray);

    vec3 result = mix(color, grayscale, offset);

    FragColor = vec4(result, 1.0);
}
