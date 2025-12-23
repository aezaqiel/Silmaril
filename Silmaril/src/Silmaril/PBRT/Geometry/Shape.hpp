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

        virtual bool Intersect(const Ray& ray, f32& tHit, f32 tMax) const = 0;
        virtual void FillSurfaceInteraction(const Ray& ray, f32 tHit, SurfaceInteraction& intersection) const = 0;

        inline virtual bool IntersectP(const Ray& ray) const
        {
            f32 t;
            return Intersect(ray, t, std::numeric_limits<f32>::max());;
        }

    private:
    };

}
