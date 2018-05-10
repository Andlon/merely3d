#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 normal_world;
out vec3 frag_pos_world;
out vec3 frag_pos_local;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat3 normal_transform;

void main()
{
    vec4 world_pos = model * vec4(aPos, 1.0);
    normal_world = normalize(normal_transform * aNormal);
    frag_pos_world = vec3(world_pos);
    frag_pos_local = aPos;
    gl_Position = projection * (view * world_pos);
}
