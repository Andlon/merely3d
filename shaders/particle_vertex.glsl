#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 color;
layout (location = 2) in float radius;

out vec3 frag_color;
out vec3 sphere_pos_view;
out float sphere_radius;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    vec4 view_pos = view * vec4(pos, 1.0);
    sphere_pos_view = vec3(view_pos);
    sphere_radius = radius;
    frag_color = color;
    gl_Position = projection * view_pos;
    gl_PointSize = 100;
}
