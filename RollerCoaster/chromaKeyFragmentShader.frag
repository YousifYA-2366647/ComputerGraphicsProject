#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D overlayTexture;

void main()
{
    vec2 flippedCoords = vec2(TexCoords.x, 1.0 - TexCoords.y);
    vec4 texColor = texture(overlayTexture, flippedCoords);
    
    if (texColor.g > 0.5 &&                
        texColor.g > texColor.r * 1.5 &&     
        texColor.g > texColor.b * 1.5) {     
        discard;  
    }
    
    float Y = 0.299 * texColor.r + 0.587 * texColor.g + 0.114 * texColor.b;
    float Cb = 0.564 * (texColor.b - Y);
    float Cr = 0.713 * (texColor.r - Y);
    
    if (Cb < 0.0 && Cr < -0.1 &&     
        Y > 0.1) {                   
        discard;
    }
    FragColor = texColor;
}