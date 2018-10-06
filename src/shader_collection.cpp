#include "shader_collection.hpp"

// TODO: Such a generic name seems like it might easily conflict,
// perhaps we should generate this auto-generated file into a
// folder structure like merely3d/configured/
#include <shaders.hpp>

namespace merely3d
{
    void set_current_shader_object_color(ShaderProgram & program, GLint loc, const Color & color)
    {
        auto color_array = color.into_array();
        program.set_vec3_uniform(loc, color_array.data());
    }

    void set_current_shader_model_transform(ShaderProgram & program, GLint loc, const Eigen::Affine3f & model)
    {
        program.set_mat4_uniform(loc, model.data());
    }

    void set_current_shader_view_transform(ShaderProgram & program, GLint loc, const Eigen::Affine3f & view)
    {
        program.set_mat4_uniform(loc, view.data());
    }

    void set_current_shader_projection_transform(ShaderProgram & program, GLint loc, const Eigen::Matrix4f & projection)
    {
        program.set_mat4_uniform(loc, projection.data());
    }

    void MeshShader::set_normal_transform(const Eigen::Matrix3f & transform)
    {
        shader.set_mat3_uniform(normal_transform_loc, transform.data());
    }

    void MeshShader::set_reference_transform(const Eigen::Matrix3f & transform)
    {
        shader.set_mat3_uniform(reference_transform_loc, transform.data());
    }

    void MeshShader::set_light_color(const Color & color)
    {
        const auto color_array = color.into_array();
        shader.set_vec3_uniform(light_color_loc, color_array.data());
    }

    void MeshShader::set_light_direction(const Eigen::Vector3f & direction)
    {
        shader.set_vec3_uniform(light_dir_loc, direction.data());
    }

    void MeshShader::set_camera_position(const Eigen::Vector3f & position)
    {
        shader.set_vec3_uniform(camera_pos_loc, position.data());
    }

    void MeshShader::set_pattern_grid_size(float size)
    {
        if (size < 0)
        {
            throw std::invalid_argument("Pattern grid size must be non-negative.");
        }

        shader.set_float_uniform(pattern_grid_size_loc, size);
    }

    void MeshShader::set_model_transform(const Eigen::Affine3f & model)
    {
        set_current_shader_model_transform(shader, model_loc, model);
    }

    void MeshShader::set_view_transform(const Eigen::Affine3f & view)
    {
        set_current_shader_view_transform(shader, view_loc, view);
    }

    void MeshShader::set_projection_transform(const Eigen::Matrix4f & projection)
    {
        set_current_shader_projection_transform(shader, projection_loc, projection);
    }

    void MeshShader::set_object_color(const Color & color)
    {
        set_current_shader_object_color(shader, object_color_loc, color);
    }

    void MeshShader::use()
    {
        shader.use();
    }

    MeshShader MeshShader::create_in_context()
    {
        const auto fragment_shader = Shader::compile(ShaderType::Fragment, shaders::default_fragment);
        const auto vertex_shader = Shader::compile(ShaderType::Vertex, shaders::default_vertex);
        auto mesh_program = ShaderProgram::create();
        mesh_program.attach(fragment_shader);
        mesh_program.attach(vertex_shader);
        mesh_program.link();

        auto shader = MeshShader(std::move(mesh_program));

        shader.projection_loc = shader.shader.get_uniform_loc("projection");
        shader.model_loc = shader.shader.get_uniform_loc("model");
        shader.view_loc = shader.shader.get_uniform_loc("view");
        shader.object_color_loc = shader.shader.get_uniform_loc("object_color");
        shader.light_color_loc = shader.shader.get_uniform_loc("light_color");
        shader.light_dir_loc = shader.shader.get_uniform_loc("light_dir");
        shader.normal_transform_loc = shader.shader.get_uniform_loc("normal_transform");
        shader.camera_pos_loc = shader.shader.get_uniform_loc("view_pos");
        shader.reference_transform_loc = shader.shader.get_uniform_loc("reference_transform");
        shader.pattern_grid_size_loc = shader.shader.get_uniform_loc("pattern_grid_size");

        return shader;
    }

    void LineShader::set_model_transform(const Eigen::Affine3f & model)
    {
        set_current_shader_model_transform(shader, model_loc, model);
    }

    void LineShader::set_view_transform(const Eigen::Affine3f & view)
    {
        set_current_shader_view_transform(shader, view_loc, view);
    }

    void LineShader::set_projection_transform(const Eigen::Matrix4f & projection)
    {
        set_current_shader_projection_transform(shader, projection_loc, projection);
    }

    void LineShader::set_object_color(const Color & color)
    {
        set_current_shader_object_color(shader, object_color_loc, color);
    }

    void LineShader::use()
    {
        shader.use();
    }

    LineShader LineShader::create_in_context()
    {
        const auto basic_fragment_shader = Shader::compile(ShaderType::Fragment, shaders::basic_fragment);
        const auto basic_vertex_shader = Shader::compile(ShaderType::Vertex, shaders::basic_vertex);
        auto line_program = ShaderProgram::create();
        line_program.attach(basic_fragment_shader);
        line_program.attach(basic_vertex_shader);
        line_program.link();

        auto shader = LineShader(std::move(line_program));

        shader.projection_loc = shader.shader.get_uniform_loc("projection");
        shader.model_loc = shader.shader.get_uniform_loc("model");
        shader.view_loc = shader.shader.get_uniform_loc("view");
        shader.object_color_loc = shader.shader.get_uniform_loc("object_color");

        return shader;
    }


    void ParticleShader::set_view_transform(const Eigen::Affine3f &view)
    {
        set_current_shader_view_transform(shader, view_loc, view);
    }

    void ParticleShader::set_projection_transform(const Eigen::Matrix4f &projection)
    {
        set_current_shader_projection_transform(shader, projection_loc, projection);
        const Eigen::Matrix4f inv_projection = projection.inverse();
        shader.set_mat4_uniform(inv_projection_loc, inv_projection.data());
    }

    void ParticleShader::set_viewport_dimensions(float width, float height)
    {
        shader.set_float_uniform(viewport_width_loc, width);
        shader.set_float_uniform(viewport_height_loc, height);
    }

    void ParticleShader::set_near_plane_dist(float dist)
    {
        shader.set_float_uniform(near_plane_dist_loc, dist);
    }

    void ParticleShader::set_light_color(const Color & color)
    {
        const auto color_array = color.into_array();
        shader.set_vec3_uniform(light_color_loc, color_array.data());
    }

    void ParticleShader::set_light_eye_direction(const Eigen::Vector3f & direction)
    {
        shader.set_vec3_uniform(light_eye_dir_loc, direction.data());
    }

    void ParticleShader::use()
    {
        shader.use();
    }

    ParticleShader ParticleShader::create_in_context() {

        const auto particle_fragment_shader = Shader::compile(ShaderType::Fragment, shaders::particle_fragment);
        const auto particle__vertex_shader = Shader::compile(ShaderType::Vertex, shaders::particle_vertex);
        const auto particle_geometry_shader = Shader::compile(ShaderType::Geometry, shaders::particle_geometry);
        auto line_program = ShaderProgram::create();
        line_program.attach(particle_fragment_shader);
        line_program.attach(particle__vertex_shader);
        line_program.attach(particle_geometry_shader);
        line_program.link();

        auto shader = ParticleShader(std::move(line_program));

        shader.projection_loc = shader.shader.get_uniform_loc("projection");
        shader.inv_projection_loc = shader.shader.get_uniform_loc("inv_projection");
        shader.view_loc = shader.shader.get_uniform_loc("view");
        shader.viewport_width_loc = shader.shader.get_uniform_loc("viewport_width");
        shader.viewport_height_loc = shader.shader.get_uniform_loc("viewport_height");
        shader.near_plane_dist_loc = shader.shader.get_uniform_loc("near_plane_dist");
        shader.light_color_loc = shader.shader.get_uniform_loc("light_color");
        shader.light_eye_dir_loc = shader.shader.get_uniform_loc("light_dir_eye");

        return shader;
    }

    MeshShader & ShaderCollection::mesh_shader()
    {
        return _mesh_shader;
    }

    LineShader & ShaderCollection::line_shader()
    {
        return _line_shader;
    }

    ParticleShader &ShaderCollection::particle_shader() {
        return _particle_shader;
    }

    ShaderCollection ShaderCollection::create_in_context()
    {
        return { MeshShader::create_in_context(),
                 LineShader::create_in_context(),
                 ParticleShader::create_in_context() };
    }
}
