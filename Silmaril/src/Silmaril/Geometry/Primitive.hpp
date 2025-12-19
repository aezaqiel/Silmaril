#pragma once

#include "Silmaril/Containers/Ray.hpp"
#include "Silmaril/Containers/Interaction.hpp"
#include "Silmaril/Containers/AABB.hpp"

#include "Silmaril/Materials/Material.hpp"

namespace Silmaril {

    class Primitive
    {
    public:
        virtual bool Intersect(const Ray& ray, SurfaceInteraction& intersect) const = 0;

        virtual AABB GetBound() const = 0;
        virtual const Material* GetMaterial() const = 0;
    };

}
