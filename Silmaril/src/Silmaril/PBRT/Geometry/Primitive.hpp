#pragma once

#include "Silmaril/PBRT/Containers/Ray.hpp"
#include "Silmaril/PBRT/Containers/Interaction.hpp"
#include "Silmaril/PBRT/Containers/AABB.hpp"

#include "Silmaril/PBRT/Materials/Material.hpp"
#include "Silmaril/PBRT/Lights/Light.hpp"

namespace Silmaril {

    class Primitive
    {
    public:
        virtual ~Primitive() = default;

        virtual bool Intersect(const Ray& ray, HitInteraction& hit) const = 0;
        virtual void FillSurfaceInteraction(const Ray& ray, const HitInteraction& hit, SurfaceInteraction& intersection) const = 0;

        virtual AABB GetBound() const = 0;

        virtual const Material* GetMaterial() const = 0;
        virtual const Light* GetLight() const = 0;
    };

}
