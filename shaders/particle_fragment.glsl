#version 330 core

in vec3 frag_pos_world;
in vec3 frag_color;

out vec4 FragColor;

void main()
{
    FragColor = vec4(frag_color, 1.0);
}