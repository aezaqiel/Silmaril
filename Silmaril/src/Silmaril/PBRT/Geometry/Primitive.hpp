#pragma once

#include "Silmaril/PBRT/Containers/Ray.hpp"
#include "Silmaril/PBRT/Containers/Interaction.hpp"
#include "Silmaril/PBRT/Containers/AABB.hpp"

#include "Silmaril/PBRT/Materials/Material.hpp"

namespace Silmaril {

    class Primitive
    {
    public:
        virtual bool Intersect(const Ray& ray, SurfaceInteraction& intersect) const = 0;

        virtual AABB GetBound() const = 0;
        virtual const Material* GetMaterial() const = 0;
    };

}
