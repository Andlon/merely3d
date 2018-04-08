#version 330 core

in vec3 normal_world;
in vec3 frag_pos_world;

uniform vec3 light_color;
uniform vec3 object_color;

// The position of the camera in world coordinates
uniform vec3 view_pos;

// Light direction is direction from light source to fragment (in world coordinates)
uniform vec3 light_dir;

out vec4 FragColor;

void main()
{
    // TODO: Make ambient/specular etc. configurable
    float ambient_strength = 0.1;
    float specular_strength = 0.5;

    // Ambient
    vec3 ambient = ambient_strength * light_color;

    // Diffuse
    float diff = max(- dot(normal_world, light_dir), 0.0);
    vec3 diffuse = diff * light_color;

    // Specular
    vec3 view_dir = normalize(frag_pos_world - view_pos);
    vec3 reflect_dir = reflect(light_dir, normal_world);
    float spec = pow(max(- dot(view_dir, reflect_dir), 0.0), 32);
    vec3 specular = specular_strength * spec * light_color;

    vec3 result = (ambient + diffuse + specular) * object_color;
    FragColor = vec4(result, 1.0);
}