#pragma once

#include <glm/glm.hpp>

#include "Bounds.hpp"
#include "Ray.hpp"

namespace Silmaril {

    struct AABB
    {
        Bounds x;
        Bounds y;
        Bounds z;

        AABB() = default;

        AABB(const Bounds& x, const Bounds& y, const Bounds& z)
            : x(x), y(y), z(z)
        {
        }

        AABB(const glm::vec3& a, const glm::vec3& b)
        {
            x = (a.x <= b.x) ? Bounds(a.x, b.x) : Bounds(b.x, a.x);
            y = (a.y <= b.y) ? Bounds(a.y, b.y) : Bounds(b.y, a.y);
            z = (a.z <= b.z) ? Bounds(a.z, b.z) : Bounds(b.z, a.z);
        }

        AABB(const AABB& b0, const AABB& b1)
        {
            x = Bounds(b0.x, b1.x);
            y = Bounds(b0.y, b1.y);
            z = Bounds(b0.z, b1.z);
        }

        inline const Bounds& AxisBounds(usize index) const
        {
            if (index == 1) return y;
            if (index == 2) return z;
            return x;
        }

        bool Hit(const Ray& ray, Bounds clip) const;
    };

}
