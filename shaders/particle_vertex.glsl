#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 color;
layout (location = 2) in float radius;

out vec3 frag_pos_world;
out vec3 frag_color;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    frag_pos_world = pos;
    frag_color = color;
    gl_Position = projection * (view * vec4(pos, 1.0));
    gl_PointSize = 10;
}
