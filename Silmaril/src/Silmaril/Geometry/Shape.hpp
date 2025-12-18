#pragma once

#include "Silmaril/Containers/Interaction.hpp"

namespace Silmaril {

    struct AABB;
    struct Ray;

    class Shape
    {
    public:
        virtual ~Shape() = default;

        virtual AABB Bound() const = 0;
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
