#pragma once

#include "Shape.hpp"

namespace Silmaril {

    class Sphere final : public Shape
    {
    public:
        Sphere(const glm::vec3& center, f32 radius);
        virtual ~Sphere() = default;

        virtual AABB GetBound() const override;
        virtual f32 Area() const override;

        virtual bool Intersect(const Ray& ray, f32& tHit, f32 tMax) const override;
        virtual void FillSurfaceInteraction(const Ray& ray, f32 tHit, SurfaceInteraction& intersection) const override;

        virtual Interaction Sample(const glm::vec2& u, f32& pdf) const override;
        virtual Interaction Sample(const Interaction& ref, const glm::vec2& u, f32& pdf) const override;

    private:
        glm::vec3 m_Center;
        f32 m_Radius;
    };

}
