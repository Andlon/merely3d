#pragma once

#include <merely3d/material.hpp>
#include <merely3d/primitives.hpp>
#include <merely3d/renderable.hpp>
#include <merely3d/mesh.hpp>

#include <Eigen/Dense>

#include <type_traits>
#include <vector>

namespace merely3d
{
    class CommandBuffer
    {
    public:
        void clear();

        template <typename Shape>
        void push_renderable(const Renderable<Shape> & renderable);

        void push_line(const Line & line);

        void push_particle(const Particle & particle);

        const std::vector<Renderable<Rectangle>> &  rectangles() const;
        const std::vector<Renderable<Box>> &        boxes() const;
        const std::vector<Renderable<Sphere>> &     spheres() const;
        const std::vector<Renderable<StaticMesh>> & meshes() const;
        const std::vector<Line> &                   lines() const;
        const std::vector<float> &                  particle_data() const;

        std::vector<Renderable<Rectangle>> &  rectangles();
        std::vector<Renderable<Box>> &        boxes();
        std::vector<Renderable<Sphere>> &     spheres();
        std::vector<Renderable<StaticMesh>> & meshes();
        std::vector<Line> &                   lines();
        std::vector<float> &                  particle_data();

    private:
        std::vector<Renderable<Rectangle>>  _rectangles;
        std::vector<Renderable<Box>>        _boxes;
        std::vector<Renderable<Sphere>>     _spheres;
        std::vector<Renderable<StaticMesh>> _meshes;
        std::vector<Line>                   _lines;
        std::vector<float>                  _particle_data;
    };

    inline void CommandBuffer::clear()
    {
        _rectangles.clear();
        _boxes.clear();
        _spheres.clear();
        _meshes.clear();
        _lines.clear();
        _particle_data.clear();
    }

    inline const std::vector<Renderable<Rectangle>> & CommandBuffer::rectangles() const
    {
        return _rectangles;
    }

    inline const std::vector<Renderable<Box>> & CommandBuffer::boxes() const
    {
        return _boxes;
    }

    inline const std::vector<Renderable<Sphere>> & CommandBuffer::spheres() const
    {
        return _spheres;
    }

    inline const std::vector<Renderable<StaticMesh>> & CommandBuffer::meshes() const
    {
        return _meshes;
    }

    inline const std::vector<Line> & CommandBuffer::lines() const {
        return _lines;
    }

    inline const std::vector<float> & CommandBuffer::particle_data() const
    {
        return _particle_data;
    }

    inline std::vector<Renderable<Rectangle>> & CommandBuffer::rectangles()
    {
        return _rectangles;
    }

    inline std::vector<Renderable<Box>> & CommandBuffer::boxes()
    {
        return _boxes;
    }

    inline std::vector<Renderable<Sphere>> & CommandBuffer::spheres()
    {
        return _spheres;
    }

    inline std::vector<Renderable<StaticMesh>> & CommandBuffer::meshes()
    {
        return _meshes;
    }

    inline std::vector<Line> & CommandBuffer::lines()
    {
        return _lines;
    }

    inline std::vector<float> & CommandBuffer::particle_data()
    {
        return _particle_data;
    }

    template<typename Shape>
    inline void CommandBuffer::push_renderable(const Renderable <Shape> & renderable)
    {
        static_assert(!std::is_same<Shape, Shape>::value, "Shape not supported");
    }

    template <>
    inline void CommandBuffer::push_renderable(const Renderable<Box> & renderable)
    {
        _boxes.push_back(renderable);
    }

    template <>
    inline void CommandBuffer::push_renderable(const Renderable<Rectangle> & renderable)
    {
        _rectangles.push_back(renderable);
    }

    template <>
    inline void CommandBuffer::push_renderable(const Renderable<Sphere> &renderable)
    {
        _spheres.push_back(renderable);
    }

    template <>
    inline void CommandBuffer::push_renderable(const merely3d::Renderable<StaticMesh> &renderable)
    {
        _meshes.push_back(renderable);
    }

    inline void CommandBuffer::push_line(const Line &line)
    {
        _lines.push_back(line);
    }

    inline void CommandBuffer::push_particle(const Particle & particle)
    {
        const auto & p = particle;
        const auto offset = _particle_data.size();
        _particle_data.resize(offset + 7);
        _particle_data[offset + 0] = p.position.x();
        _particle_data[offset + 1] = p.position.y();
        _particle_data[offset + 2] = p.position.z();
        _particle_data[offset + 3] = p.color.r();
        _particle_data[offset + 4] = p.color.g();
        _particle_data[offset + 5] = p.color.b();
        _particle_data[offset + 6] = p.radius;
    }
}

