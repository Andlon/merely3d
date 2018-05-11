#version 330 core

in vec3 normal_world;
in vec3 frag_pos_world;
in vec3 frag_pos_local;

uniform vec3 light_color;
uniform vec3 object_color;

// The position of the camera in world coordinates
uniform vec3 view_pos;

// Light direction is direction from light source to fragment (in world coordinates)
uniform vec3 light_dir;

// The transform taking the reference shape (i.e. a unit cube) into
// the actual shape of the object (i.e. a box with certain extents)
uniform mat3 reference_transform;

uniform float pattern_grid_size;

out vec4 FragColor;

void main()
{
    // TODO: Make ambient/specular etc. configurable
    float ambient_strength = 0.15;
    float specular_strength = 0.5;

    // frag_pos_local gives us local coordinates in the reference
    // primitive (i.e. unit cube). We need to transform by the reference
    // transform in order to obtain the actual local coordinates of the
    // logical entity (i.e. a box with certain extents)
    vec3 local_pos = reference_transform * frag_pos_local;

    // Assign the fragment to a grid cell and determine if the grid cell should
    // be patterned
    ivec3 grid_coords = pattern_grid_size > 0.0
        ? ivec3(round(local_pos / pattern_grid_size))
        : ivec3(0);
    bool patterned = (grid_coords[0] + grid_coords[1] + grid_coords[2]) % 2 != 0;

    vec3 base_color = patterned
                    ? 0.9 * object_color
                    : object_color;

    vec3 normal = normalize(normal_world);

    // Ambient
    vec3 ambient = ambient_strength * light_color;

    // Diffuse
    float diff = max(- dot(normal, light_dir), 0.0);
    vec3 diffuse = diff * light_color;

    // Specular
    vec3 view_dir = normalize(frag_pos_world - view_pos);
    vec3 reflect_dir = reflect(light_dir, normal);
    float spec = pow(max(- dot(view_dir, reflect_dir), 0.0), 32);
    vec3 specular = specular_strength * spec * light_color;

    vec3 result = (ambient + diffuse + specular) * base_color;
    FragColor = vec4(result, 1.0);
}
