#version 330 core

in vec3 normal;

uniform vec3 light_color;
uniform vec3 object_color;

// Light direction is direction from light source to fragment
uniform vec3 light_dir;

out vec4 FragColor;

void main()
{
    float ambient_strength = 0.1;
    vec3 ambient = ambient_strength * light_color;

    float diff = max(- dot(normal, light_dir), 0.0);
    vec3 diffuse = diff * light_color;

    vec3 result = (ambient + diffuse) * object_color;
    FragColor = vec4(result, 1.0);
}