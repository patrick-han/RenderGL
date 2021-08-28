#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;

void main()
{
    // Ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos); // Get the light ray
    float diff = max(dot(norm, lightDir), 0.0); // Max since dot product could return negative if light is past 90 degrees
    vec3 diffuse = diff * lightColor;

    FragColor = vec4((ambient + diffuse) * objectColor, 1.0);
}  