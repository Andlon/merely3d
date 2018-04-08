#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 projection;
uniform mat4 modelview;

void main()
{
    vec4 model_pos = vec4(aPos, 1.0);
    gl_Position = projection * modelview * model_pos;
}
