#include "renderers.hpp"
#include "mesh_util.hpp"
#include "gl_errors.hpp"

#include <Eigen/Dense>

#include <algorithm>
#include <unordered_set>

using Eigen::Affine3f;
using Eigen::Matrix3f;
using Eigen::Vector3f;
using Eigen::Scaling;
using Eigen::Translation3f;

namespace merely3d
{
    template <typename Shape>
    Affine3f build_model_transform(const Renderable<Shape> & renderable)
    {
        return Translation3f(renderable.position)
               * renderable.orientation
               * Scaling(renderable.scale);
    }

    static void enable_wireframe_rendering(bool enable)
    {
        if (enable)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    }

    /// Renders a set of static meshes that all share the same GlTriangleMesh. This is useful if the same 3D model
    /// is being rendered many times, but with different transforms or materials.
    ///
    /// NB! Assumes that the uniforms not specific
    /// to the individual renderable are all correctly set.
    void render_static_meshes(std::vector<Renderable<StaticMesh>> & renderables,
                              GlTriangleMesh & gl_mesh,
                              ShaderCollection & shaders)
    {
        gl_mesh.bind();
        auto & line_shader = shaders.line_shader();
        auto & mesh_shader = shaders.mesh_shader();

        auto draw_primitive = [&] (const Renderable<StaticMesh> & renderable)
        {
            const auto ref_transform = Eigen::Matrix3f::Identity();
            const auto model = build_model_transform(renderable);

            if (renderable.material.wireframe)
            {
                line_shader.set_model_transform(model);
                line_shader.set_object_color(renderable.material.color);
                glDrawElements(GL_TRIANGLES, gl_mesh.index_count(), GL_UNSIGNED_INT, 0);
            }
            else
            {
                const auto normal_transform = Matrix3f(model.linear().inverse().transpose());
                mesh_shader.set_model_transform(model);
                mesh_shader.set_normal_transform(normal_transform);
                mesh_shader.set_object_color(renderable.material.color);
                mesh_shader.set_reference_transform(ref_transform);
                mesh_shader.set_pattern_grid_size(std::max(0.0f, renderable.material.pattern_grid_size));
                glDrawElements(GL_TRIANGLES, gl_mesh.index_count(), GL_UNSIGNED_INT, 0);
            }
        };

        // Partition vector so that renderables that are to be rendered as wireframes
        // come first
        auto filled_begin = std::partition(renderables.begin(), renderables.end(),
                                           [] (const Renderable<StaticMesh> & renderable)
                                           {
                                               return renderable.material.wireframe;
                                           });

        // Don't cull faces when rendering wireframes
        glDisable(GL_CULL_FACE);
        line_shader.use();
        enable_wireframe_rendering(true);
        std::for_each(renderables.begin(), filled_begin, draw_primitive);

        // But do cull back faces for everything else
        // (Note: This is especially important for correct rendering of "flat" meshes,
        // in which a given triangle has two faces pointing opposite directions)
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        mesh_shader.use();
        enable_wireframe_rendering(false);
        std::for_each(filled_begin, renderables.end(), draw_primitive);
    }

    /// Render primitives.
    ///
    /// NB! Assumes that the uniforms not specific
    /// to the individual renderable are all correctly set.
    template <typename ReferenceTransform, typename Shape>
    void render_primitives(std::vector<Renderable<Shape>> & renderables,
                           ShaderCollection & shaders,
                           GlPrimitive & primitive,
                           ReferenceTransform && reference_transform)
    {
        primitive.bind();
        auto & line_shader = shaders.line_shader();
        auto & mesh_shader = shaders.mesh_shader();

        auto draw_primitive = [&] (const Renderable<Shape> & renderable)
        {
            const auto ref_transform = reference_transform(renderable.shape);
            const auto model = build_model_transform(renderable) * ref_transform;

            if (renderable.material.wireframe)
            {
                line_shader.set_model_transform(model);
                line_shader.set_object_color(renderable.material.color);
                glDrawArrays(GL_TRIANGLES, 0, primitive.vertex_count());
            }
            else
            {
                const auto normal_transform = Matrix3f(model.linear().inverse().transpose());
                mesh_shader.set_model_transform(model);
                mesh_shader.set_normal_transform(normal_transform);
                mesh_shader.set_object_color(renderable.material.color);
                mesh_shader.set_reference_transform(Matrix3f(ref_transform));
                mesh_shader.set_pattern_grid_size(std::max(0.0f, renderable.material.pattern_grid_size));
                glDrawArrays(GL_TRIANGLES, 0, primitive.vertex_count());
            }
        };

        // Partition vector so that renderables that are to be rendered as wireframes
        // come first
        auto filled_begin = std::partition(renderables.begin(), renderables.end(),
            [] (const Renderable<Shape> & renderable)
            {
                return renderable.material.wireframe;
            });

        // Don't cull faces when rendering wireframes
        glDisable(GL_CULL_FACE);
        line_shader.use();
        enable_wireframe_rendering(true);
        std::for_each(renderables.begin(), filled_begin, draw_primitive);

        // But do cull back faces for everything else
        // (Note: this is absolutely necessary for rectangles)
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        mesh_shader.use();
        enable_wireframe_rendering(false);
        std::for_each(filled_begin, renderables.end(), draw_primitive);
    }

    /// Returns the linear transformation that
    /// transforms a reference cube into the provided Box.
    Eigen::AlignedScaling3f box_reference_transform(const Box & box)
    {
        return Scaling(box.extents);
    }

    Eigen::AlignedScaling3f rectangle_reference_transform(const Rectangle & rectangle)
    {
        const auto & extents = rectangle.extents;
        return Scaling(extents.x(), extents.y(), 1.0f);
    }

    Eigen::AlignedScaling3f sphere_reference_transform(const Sphere & sphere)
    {
        const auto r = sphere.radius;
        return Scaling(r, r, r);
    }

    TrianglePrimitiveRenderer TrianglePrimitiveRenderer::build()
    {
        const auto cube_verts = unit_cube_vertices_and_normals();
        auto gl_cube = GlPrimitive::create(cube_verts);

        const auto rect_verts = unit_rectangle_vertices_and_normals();
        auto gl_rect = GlPrimitive::create(rect_verts);

        const auto sphere_verts = unit_sphere_vertices_and_normals();
        auto gl_sphere = GlPrimitive::create(sphere_verts);

        return TrianglePrimitiveRenderer(std::move(gl_cube),
                                         std::move(gl_rect),
                                         std::move(gl_sphere));
    }

    void TrianglePrimitiveRenderer::render(
                ShaderCollection & shaders,
                CommandBuffer & buffer,
                const Camera & camera,
                const Eigen::Matrix4f & projection)
    {
        auto & mesh_shader = shaders.mesh_shader();
        auto & line_shader = shaders.line_shader();

        const Eigen::Affine3f view = camera.transform().inverse();

        // TODO: Make lighting configurable rather than hard-coded
        const auto light_color = Color(1.0, 1.0, 1.0);
        const Eigen::Vector3f light_dir = Eigen::Vector3f(0.9, 1.2, -0.8).normalized();

        // Set up uniforms that are invariant across renderables
        mesh_shader.use();
        mesh_shader.set_light_color(light_color);
        mesh_shader.set_light_direction(light_dir);
        mesh_shader.set_view_transform(view);
        mesh_shader.set_projection_transform(projection);
        mesh_shader.set_camera_position(camera.position());
        line_shader.use();
        line_shader.set_projection_transform(projection);
        line_shader.set_view_transform(view);

        render_primitives(buffer.rectangles(), shaders, gl_rectangle, rectangle_reference_transform);
        render_primitives(buffer.boxes(), shaders, gl_cube, box_reference_transform);
        render_primitives(buffer.spheres(), shaders, gl_sphere, sphere_reference_transform);
    }

    MeshRenderer MeshRenderer::build(const std::shared_ptr<GlGarbagePile> & garbage)
    {
        return MeshRenderer(garbage);
    }

    void MeshRenderer::render(ShaderCollection &shaders,
                              CommandBuffer &buffer,
                              const Camera &camera,
                              const Eigen::Matrix4f &projection)
    {
        // TODO: Merge some of the code here with the code in TrianglePrimitiveRenderer
        auto & mesh_shader = shaders.mesh_shader();
        auto & line_shader = shaders.line_shader();

        const Eigen::Affine3f view = camera.transform().inverse();

        // TODO: Make lighting configurable rather than hard-coded
        const auto light_color = Color(1.0, 1.0, 1.0);
        const Eigen::Vector3f light_dir = Eigen::Vector3f(0.9, 1.2, -0.8).normalized();

        // Set up uniforms that are invariant across renderables
        mesh_shader.use();
        mesh_shader.set_light_color(light_color);
        mesh_shader.set_light_direction(light_dir);
        mesh_shader.set_view_transform(view);
        mesh_shader.set_projection_transform(projection);
        mesh_shader.set_camera_position(camera.position());
        line_shader.use();
        line_shader.set_projection_transform(projection);
        line_shader.set_view_transform(view);

        auto & meshes = buffer.meshes();

        // Make sure that meshes that share the same underlying data are consecutive in the buffer
        std::sort(meshes.begin(), meshes.end(),
                  [] (const Renderable<StaticMesh> & mesh1, const Renderable<StaticMesh> & mesh2)
        {
            const auto ptr1 = mesh1.shape._data.get();
            const auto ptr2 = mesh2.shape._data.get();
            return ptr1 < ptr2;
        });

        std::vector<Renderable<StaticMesh>> consecutive_meshes;

        auto outer_iter = meshes.cbegin();
        auto inner_iter = meshes.cbegin();

        // Keep track of which meshes were actually rendered, so that we may throw out
        // the rest from our cache
        std::unordered_set<detail::UniqueMeshId> rendered_meshes;

        while (outer_iter != meshes.cend())
        {
            const auto outer_id = outer_iter->shape._data->id;
            while (inner_iter != meshes.cend() && inner_iter->shape._data->id == outer_id)
            {
                ++inner_iter;
            }
            consecutive_meshes.assign(outer_iter, inner_iter);

            auto cache_iter = _mesh_cache.find(outer_id);
            if (cache_iter == _mesh_cache.end())
            {
                const auto & mesh_data = *outer_iter->shape._data;
                auto gl_mesh = GlTriangleMesh::create(_garbage, mesh_data.vertices_and_normals, mesh_data.faces);
                _mesh_cache.insert(std::make_pair(outer_id, std::move(gl_mesh)));
            }

            cache_iter = _mesh_cache.find(outer_id);
            auto & gl_mesh = cache_iter->second;

            render_static_meshes(consecutive_meshes, gl_mesh, shaders);
            rendered_meshes.insert(outer_id);

            outer_iter = inner_iter;
        }

        std::vector<detail::UniqueMeshId> meshes_to_remove;
        for (const auto & pair : _mesh_cache)
        {
            const auto & id = pair.first;
            if (rendered_meshes.count(id) == 0)
            {
                meshes_to_remove.push_back(id);
            }
        }

        for (const auto & id : meshes_to_remove)
        {
            _mesh_cache.erase(id);
        }

    }

    void ParticleRenderer::render(ShaderCollection & shaders,
                                  CommandBuffer & buffer,
                                  const Camera & camera,
                                  const Eigen::Matrix4f & projection)
    {
        auto & shader = shaders.particle_shader();

        const Eigen::Affine3f view = camera.transform().inverse();

        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);
        const float viewport_width = static_cast<float>(viewport[2]);
        const float viewport_height = static_cast<float>(viewport[3]);

        // Compute the distance to the near plane by transforming from a point on the near plane in
        // NDC to view space. We have that
        //  n = P * X / N,
        // where N is the distance to the near plane, X = [x, 1] and x are the coordinate in
        // view space, and n represents the coordinates in NDC. Letting Y = X / N,
        // we have that N = 1 / Y_w, where Y = P^-1 * n.
        const float near_plane_dist = 1.0 / (projection.inverse() * Eigen::Vector4f(0.0, 0.0, -1.0, 1.0)).w();

        // TODO: Make lighting configurable rather than hard-coded
        const auto light_color = Color(1.0, 1.0, 1.0);
        const Eigen::Vector3f light_dir_world = Eigen::Vector3f(0.9, 1.2, -0.8).normalized();
        const Eigen::Vector3f light_dir_eye = view.linear() * light_dir_world;

        shader.use();
        shader.set_view_transform(view);
        shader.set_projection_transform(projection);
        shader.set_viewport_dimensions(viewport_width, viewport_height);
        shader.set_near_plane_dist(near_plane_dist);
        shader.set_light_color(light_color);
        shader.set_light_eye_direction(light_dir_eye);

        assert(buffer.particle_data().size() % 7 == 0);
        const auto num_particles = buffer.particle_data().size() / 7;
        _particle_buffer.update_buffer(buffer.particle_data().data(), num_particles);
        _particle_buffer.bind();

        MERELY_CHECK_GL_ERRORS();

        glEnable(GL_PROGRAM_POINT_SIZE);
        // The following line MAY be required on Windows, or in some configurations. On the other hand,
        // this caused an error on my Linux machine. TODO: Remove this once we know whether or not we need it.
        // glEnable(0x8861/*GL_POINT_SPRITE*/); // should be enabled by default in OpenGL 3.3, but isn't
        glPointParameteri(GL_POINT_SPRITE_COORD_ORIGIN, GL_LOWER_LEFT);
        MERELY_CHECK_GL_ERRORS();

        glDrawArrays(GL_POINTS, 0, num_particles);
        MERELY_CHECK_GL_ERRORS();
        _particle_buffer.unbind();
    }

    ParticleRenderer ParticleRenderer::build(const std::shared_ptr<GlGarbagePile> & garbage)
    {
        return ParticleRenderer(GlParticleBuffer::create(garbage));
    }
}
