#version 330 core
layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

in VertexData
{
    vec3 sphere_color;
    float sphere_radius;
} vs_in[];

out VertexData
{
    vec3 frag_color;
    vec3 sphere_pos_view;
    float sphere_radius;
} vs_out;

uniform mat4 projection;

vec3 orthogonal_vector(vec3 v)
{
    // This is adapted from Peter Abeles'
    // answer in https://stackoverflow.com/a/45824016

    float x_abs = abs(v.x);
    float y_abs = abs(v.y);
    float z_abs = abs(v.z);
    float scale = x_abs + y_abs + z_abs;

    if (scale == 0)
        return vec3(0,0,0);

    vec3 v_scaled = v / scale;

    if (x_abs > y_abs) return vec3(v_scaled.z, 0.0,-v_scaled.x);
    else return vec3(0.0, v_scaled.z,-v_scaled.y);
}

void main() {
    // Input point is the center of the sphere in view space
    vec3 c = vec3(gl_in[0].gl_Position);

    // Construct unit vectors p and q which span the plane which is orthogonal
    // to the vector pointing to x
    vec3 p = normalize(orthogonal_vector(c));
    vec3 q = normalize(cross(c, p));

    float r = vs_in[0].sphere_radius;

    // TODO: Currently we just make the billboard much bigger than necessary,
    // so that we know that it's big enough. Need to adapt this so that
    // we create one that is only just big enough (to avoid having to discard
    // too many fragments)
    vec3 offsets[4];
    offsets[0] = 2.0 * r * (- p - q);
    offsets[1] = 2.0 * r * (- p + q);
    offsets[2] = 2.0 * r * (+ p - q);
    offsets[3] = 2.0 * r * (+ p + q);

    for (int i = 0; i < 4; ++i)
    {
        gl_Position = projection * vec4(c + offsets[i], 1.0);
        vs_out.frag_color = vs_in[0].sphere_color;
        vs_out.sphere_radius = vs_in[0].sphere_radius;
        vs_out.sphere_pos_view = c;
        EmitVertex();
    }

    EndPrimitive();
}
