#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform float offset; // Determines the strength of the blur

void main()
{
    vec2 tex_offset = vec2(offset);

    float kernel[9] = float[](
        1.0, 2.0, 1.0,
        2.0, 4.0, 2.0,
        1.0, 2.0, 1.0
    );

    vec3 result = vec3(0.0);
    int index = 0;

    for (int y = -1; y <= 1; ++y) {
        for (int x = -1; x <= 1; ++x) {
            vec2 offsetCoord = TexCoords + vec2(x, y) * tex_offset;
            vec3 sample = texture(screenTexture, offsetCoord).rgb;
            result += sample * kernel[index];
            index++;
        }
    }

    result /= 16.0;
    FragColor = vec4(result, 1.0);
}
