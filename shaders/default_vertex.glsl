#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 normal;

uniform mat4 projection;
uniform mat4 modelview;
uniform mat3 normal_transform;

void main()
{
    vec4 model_pos = vec4(aPos, 1.0);
    normal = normalize(normal_transform * aNormal);
    gl_Position = projection * modelview * model_pos;
}
