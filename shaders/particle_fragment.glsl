#version 330 core

in vec3 frag_pos_world;
in vec3 frag_color;

// TODO: Take inverse projection directly to avoid computing inverse in shader
uniform mat4 projection;
uniform float viewport_width;
uniform float viewport_height;
uniform float near_plane_dist;

out vec4 FragColor;

/// Computes the direction vector (in camera/eye space) of the ray
/// associated with the current fragment
vec3 compute_ray_direction()
{
    float ndc_x = 2.0 * (gl_FragCoord.x - viewport_width / 2.0) / viewport_width;
    float ndc_y = - 2.0 * (gl_FragCoord.y - viewport_height / 2.0) / viewport_height;
    vec4 ndc_point = vec4(ndc_x, ndc_y, -1.0, 1.0);
    vec4 view_point = inverse(projection) * near_plane_dist * ndc_point;
    return vec3(view_point);
}

void main()
{
    vec3 ray = compute_ray_direction();

    FragColor = vec4(frag_color, 1.0);
}
