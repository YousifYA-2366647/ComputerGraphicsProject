#version 330 core
out vec4 fragColor;

in vec3 vertColor;
in vec3 FragPos;
in vec3 Normal;

// Licht structuur
struct Light {
    vec3 position;
    vec3 color;
    float constant;
    float linear;
    float quadratic;
};

// Maximum aantal lichtbronnen voor nu
#define MAX_LIGHTS 10
uniform Light lights[MAX_LIGHTS];
uniform int numLights;
uniform vec3 viewPos;
//gebruik van Phong reflectiemodel
void main() {
    vec3 objectColor = vertColor;
    float ambient = 0.1;
    float specularStrength = 0.5;
    int shininess = 32;
    
    // Start met ambient licht
    vec3 result = ambient * objectColor;
    
    // bijdrage van elke lichtbron
    for(int i = 0; i < numLights; i++) {
        // Richting naar lichtbron
        vec3 lightDir = normalize(lights[i].position - FragPos);
        
        // Diffuse belichting
        vec3 norm = normalize(Normal);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * lights[i].color;
        
        // Speculaire belichting
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
        vec3 specular = specularStrength * spec * lights[i].color;
        
        // licht verzwakking
        float distance = length(lights[i].position - FragPos);
        float attenuation = 1.0 / (lights[i].constant + lights[i].linear * distance + 
                            lights[i].quadratic * (distance * distance));
        diffuse *= attenuation;
        specular *= attenuation;
        // Voeg bijdrage van deze lichtbron toe
        result += (diffuse + specular) * objectColor;
    }
    fragColor = vec4(result, 1.0);
}