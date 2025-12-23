#pragma once

#include "Shape.hpp"

namespace Silmaril {

    struct Mesh;

    class Triangle final : public Shape
    {
    public:
        Triangle(const std::shared_ptr<Mesh>& mesh, u32 index);
        virtual ~Triangle() = default;

        virtual AABB GetBound() const override;
        virtual f32 Area() const override;

        virtual bool Intersect(const Ray& ray, f32& tHit, f32 tMax) const override;
        virtual void FillSurfaceInteraction(const Ray& ray, f32 tHit, SurfaceInteraction& intersection) const override;

        virtual Interaction Sample(const glm::vec2& u, f32& pdf) const override;

        static std::vector<std::shared_ptr<Shape>> CreateTriangleMesh(const std::shared_ptr<Mesh>& mesh);

    private:
        std::shared_ptr<Mesh> m_Mesh;
        u32 m_Index;
    };

}
