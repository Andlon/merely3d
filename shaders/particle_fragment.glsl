#version 330 core

in vec3 frag_color;
in vec3 sphere_pos_view;
in float sphere_radius;

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
    float ndc_y = 2.0 * (gl_FragCoord.y - viewport_height / 2.0) / viewport_height;
    vec4 ndc_point = vec4(ndc_x, ndc_y, -1.0, 1.0);
    vec4 view_point = inverse(projection) * near_plane_dist * ndc_point;

    return vec3(view_point);
}

/// Performs an intersection test between a ray and a sphere,
/// returning the parameter t such that `t * ray_direction`
/// gives the point of intersection if any such point exists.
///
/// Note that if there is no intersection, a negative value of
/// t is returned, which means that one cannot distinguish between
/// the case when there is no intersection and the case in which
/// the ray starts within the sphere. This is simply because we wish
/// to discard the entire sphere if the ray starts from inside of the sphere.
float intersect_ray_sphere(vec3 ray_direction, vec3 sphere_center, float radius)
{
    // d = the direction vector of the ray
    // c = sphere center
    // r = radius * radius
    float dTd = dot(ray_direction, ray_direction);
    float cTc = dot(sphere_center, sphere_center);
    float dTc = dot(ray_direction, sphere_center);
    float r2 = radius * radius;

    // points on the ray satisfy X = t * d for some t >= 0.
    // points on the sphere satisfy || X - c || = r^2
    // points in the intersection satisfy
    // t^2 d^d - 2 t d^c + c^c - r^2 = 0     (*)
    //
    // We seek a non-negative t for which (*) is satisfied. If no such t exists,
    // there is no intersection. In this particular case, we signal this by returning
    // a *negative* t. The solution is given by *real* solutions to the quadratic formula.
    // If only complex solutions exist, there is no intersection.

    // Cancelling terms in the quadratic equation, we have that
    //  t = [ d^T c +- sqrt( (d^T c)^2 - d^T d * (c^T c - r^2) ) ] / (d^T d)
    // we refer to the term in the square root as the discriminant.

    float discriminant = dTc * dTc - dTd * (cTc - r2);

    if (discriminant < 0)
    {
        return -1.0;
    }
    else
    {
        float sqrt_disc = sqrt(discriminant);
        float t1 = (dTc + sqrt_disc) / dTd;
        float t2 = (dTc - sqrt_disc) / dTd;
        return min(t1, t2);
    }
}

void main()
{
    vec3 ray = compute_ray_direction();
    float t = intersect_ray_sphere(ray, sphere_pos_view, sphere_radius);

    if (t >= 0)
    {
        FragColor = vec4(frag_color, 1.0);
    }
    else
    {
        discard;
    }
}
