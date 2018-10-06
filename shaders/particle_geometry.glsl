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

vec3 orthogonal_to_view_vector(vec3 v)
{
    // We assume here that v.z != 0!
    return vec3(v.y, -v.x, 0);
}

void main() {
    // Input point is the center of the sphere in view space
    vec3 c = vec3(gl_in[0].gl_Position);

    float r = vs_in[0].sphere_radius;

    // TODO: Need to assert positive radius elsewhere in the C++ code
    if (r < -c.z)
    {
        // Determine the part of the sphere which is at all visible
        // from the current vantage point. This can be determined by
        // fitting a cone starting at the view center (origin in view space)
        // such that it just exactly encloses the sphere, in which case
        // the intersection of the cone and the sphere is a circle
        // of radius w, situated at a distance d along the vector
        // pointing to the sphere's center. We next construct
        // a billboard which is just big enough to fit this disk inside.
        float g = r / c.z;
        float w = r * sqrt(1 - g * g);
        float d = -c.z * w * w / (r * r);

        vec3 billboard_center = d * normalize(c);

        // Construct unit vectors p and q which span the plane which is orthogonal
        // to the vector pointing to x
        vec3 p = normalize(orthogonal_to_view_vector(c));
        vec3 q = normalize(cross(c, p));

        // TODO: Currently we just make the billboard much bigger than necessary,
        // so that we know that it's big enough. Need to adapt this so that
        // we create one that is only just big enough (to avoid having to discard
        // too many fragments)
        vec3 offsets[4];
        offsets[0] = w * (- p - q);
        offsets[1] = w * (- p + q);
        offsets[2] = w * (+ p - q);
        offsets[3] = w * (+ p + q);

        for (int i = 0; i < 4; ++i)
        {
            gl_Position = projection * vec4(billboard_center + offsets[i], 1.0);
            vs_out.frag_color = vs_in[0].sphere_color;
            vs_out.sphere_radius = vs_in[0].sphere_radius;
            vs_out.sphere_pos_view = c;
            EmitVertex();
        }
    }

    EndPrimitive();
}
