#pragma once

#include <Eigen/Dense>

#include <merely3d/color.hpp>

#include "shader.hpp"

// TODO: Remove this, can we somehow forward-declare a typedef?
typedef int GLint;

namespace merely3d
{
    class MeshShader
    {
    public:
        void set_model_transform(const Eigen::Affine3f & model);
        void set_view_transform(const Eigen::Affine3f & view);
        void set_projection_transform(const Eigen::Matrix4f & projection);
        void set_normal_transform(const Eigen::Matrix3f & transform);
        void set_reference_transform(const Eigen::Matrix3f & transform);
        void set_object_color(const Color & color);
        void set_light_color(const Color & color);
        void set_light_direction(const Eigen::Vector3f & direction);
        void set_camera_position(const Eigen::Vector3f & position);
        void set_pattern_grid_size(float size);

        void use();

        static MeshShader create_in_context();

    private:
        explicit MeshShader(ShaderProgram && shader)
            : shader(std::move(shader))
        {}

        GLint projection_loc = 0;
        GLint model_loc = 0;
        GLint view_loc = 0;
        GLint normal_transform_loc = 0;
        GLint object_color_loc = 0;
        GLint light_color_loc = 0;
        GLint light_dir_loc = 0;
        GLint camera_pos_loc = 0;
        GLint reference_transform_loc = 0;
        GLint pattern_grid_size_loc = 0;

        ShaderProgram shader;
    };

    class LineShader
    {
    public:
        void set_model_transform(const Eigen::Affine3f & model);
        void set_view_transform(const Eigen::Affine3f & view);
        void set_projection_transform(const Eigen::Matrix4f & projection);
        void set_object_color(const Color & color);

        void use();

        static LineShader create_in_context();

    private:
        explicit LineShader(ShaderProgram && shader)
            : shader(std::move(shader))
        {}

        GLint projection_loc = 0;
        GLint model_loc = 0;
        GLint view_loc = 0;
        GLint object_color_loc = 0;

        ShaderProgram shader;
    };

    class ParticleShader
    {
    public:
        void set_view_transform(const Eigen::Affine3f & view);
        void set_projection_transform(const Eigen::Matrix4f & projection);
        void set_viewport_dimensions(float width, float height);
        void set_near_plane_dist(float dist);
        void set_light_color(const Color & color);
        void set_light_eye_direction(const Eigen::Vector3f & direction);

        void use();

        static ParticleShader create_in_context();

    private:
        explicit ParticleShader(ShaderProgram && shader)
            : shader(std::move(shader))
        {}

        GLint projection_loc = 0;
        GLint inv_projection_loc = 0;
        GLint view_loc = 0;
        GLint viewport_width_loc = 0;
        GLint viewport_height_loc = 0;
        GLint near_plane_dist_loc = 0;
        GLint light_color_loc = 0;
        GLint light_eye_dir_loc = 0;

        ShaderProgram shader;
    };

    class ShaderCollection
    {
    public:
        MeshShader &     mesh_shader();
        LineShader &     line_shader();
        ParticleShader & particle_shader();

        static ShaderCollection create_in_context();

    private:
        ShaderCollection(MeshShader && mesh_shader,
                         LineShader && line_shader,
                         ParticleShader && particle_shader)
            : _mesh_shader(std::move(mesh_shader)),
              _line_shader(std::move(line_shader)),
              _particle_shader(std::move(particle_shader))
        {}

        MeshShader      _mesh_shader;
        LineShader      _line_shader;
        ParticleShader  _particle_shader;
    };


}
