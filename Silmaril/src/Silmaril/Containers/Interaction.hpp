#pragma once

#include <glm/glm.hpp>

#include "Ray.hpp"
#include "Silmaril/Materials/BSDF.hpp"

namespace Silmaril {

    class Shape;
    class Primitive;

    struct Interaction
    {
        glm::vec3 p;
        glm::vec3 n;
        glm::vec3 wo;
        f32 t;

        Interaction()
            : t(0.0f) {}

        Interaction(const glm::vec3& p, const glm::vec3& n, const glm::vec3& wo, f32 t)
            : p(p), n(n), wo(wo), t(t) {}

        inline Ray SpawnRay(const glm::vec3& d) const
        {
            return Ray(p + n * std::numeric_limits<f32>::epsilon(), d);
        }
    };

    struct SurfaceInteraction : public Interaction
    {
        const Shape* shape { nullptr };
        const Primitive* primitive { nullptr };

        std::shared_ptr<BSDF> bsdf { nullptr };

        glm::vec2 uv;

        SurfaceInteraction() = default;
        SurfaceInteraction(const glm::vec3& p, const glm::vec3& n, const glm::vec3& wo, f32 t)
            : Interaction(p, n, wo, t) {}
    };

}
