#pragma once

#include "Silmaril/PBRT/Containers/Interaction.hpp"
#include "Silmaril/PBRT/Containers/AABB.hpp"
#include "Silmaril/PBRT/Containers/Ray.hpp"

namespace Silmaril {

    class Shape
    {
    public:
        virtual ~Shape() = default;

        virtual AABB GetBound() const = 0;
        virtual f32 Area() const = 0;

        virtual bool Intersect(const Ray& ray, f32& tHit, f32 tMax) const = 0;
        virtual void FillSurfaceInteraction(const Ray& ray, f32 tHit, SurfaceInteraction& intersection) const = 0;

        inline virtual bool IntersectP(const Ray& ray) const
        {
            f32 t;
            return Intersect(ray, t, std::numeric_limits<f32>::max());;
        }

        virtual Interaction Sample(const glm::vec2& u, f32& pdf) const = 0;
        virtual Interaction Sample(const Interaction& ref, const glm::vec2& u, f32& pdf) const
        {
            return Sample(u, pdf);
        }

        virtual f32 Pdf(const Interaction& ref, const glm::vec3& wi) const
        {
            return 1.0f / Area();
        }
    };

}
