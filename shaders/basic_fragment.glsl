#version 330 core

uniform vec3 object_color;

out vec4 FragColor;

void main()
{
    FragColor = vec4(object_color, 1.0);
}