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
        MeshShader(ShaderProgram && shader)
            : shader(std::move(shader))
        {}

        GLint projection_loc;
        GLint model_loc;
        GLint view_loc;
        GLint normal_transform_loc;
        GLint object_color_loc;
        GLint light_color_loc;
        GLint light_dir_loc;
        GLint camera_pos_loc;
        GLint reference_transform_loc;
        GLint pattern_grid_size_loc;

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
        LineShader(ShaderProgram && shader)
            : shader(std::move(shader))
        {}

        GLint projection_loc;
        GLint model_loc;
        GLint view_loc;
        GLint object_color_loc;

        ShaderProgram shader;
    };

    class ShaderCollection
    {
    public:
        MeshShader & mesh_shader();
        LineShader & line_shader();

        static ShaderCollection create_in_context();

    private:
        ShaderCollection(MeshShader && mesh_shader,
                         LineShader && line_shader)
            : _mesh_shader(std::move(mesh_shader)),
              _line_shader(std::move(line_shader))
        {}

        MeshShader _mesh_shader;
        LineShader _line_shader;
    };


}
