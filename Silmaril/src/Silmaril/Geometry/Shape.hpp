#pragma once

#include "Silmaril/Containers/Interaction.hpp"
#include "Silmaril/Containers/AABB.hpp"
#include "Silmaril/Containers/Ray.hpp"

namespace Silmaril {

    class Shape
    {
    public:
        virtual ~Shape() = default;

        virtual AABB GetBound() const = 0;
        virtual bool Intersect(const Ray& ray, f32& distance, SurfaceInteraction& intersect) const = 0;

        inline virtual bool IntersectP(const Ray& ray) const
        {
            f32 t;
            SurfaceInteraction intersect;

            return Intersect(ray, t, intersect);
        }

    private:
    };

}
