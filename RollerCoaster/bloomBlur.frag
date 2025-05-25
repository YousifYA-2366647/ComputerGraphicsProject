#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D image;
uniform int horizontal;
uniform float blurRadius;

float weight[9] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216, 0.003, 0.0005, 0.00005, 0.000005);

void main() {
    vec2 tex_offset = 1.0 / textureSize(image, 0);
    vec3 result = texture(image, TexCoords).rgb * weight[0];
    
    for(int i = 1; i < 9; ++i) {
        float offset = float(i) * blurRadius;
        if(horizontal == 1) {
            result += texture(image, TexCoords + vec2(tex_offset.x * offset, 0.0)).rgb * weight[i];
            result += texture(image, TexCoords - vec2(tex_offset.x * offset, 0.0)).rgb * weight[i];
        } else {
            result += texture(image, TexCoords + vec2(0.0, tex_offset.y * offset)).rgb * weight[i];
            result += texture(image, TexCoords - vec2(0.0, tex_offset.y * offset)).rgb * weight[i];
        }
    }
    FragColor = vec4(result, 1.0);
}