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
        f32 time;
        glm::vec3 pError;
        glm::vec3 wo;
        glm::vec3 n;

        Interaction()
            : time(0.0f), pError(0.0f), n(0.0f, 1.0f, 0.0f), wo(0.0f), p(0.0f)
        {
        }

        Interaction(const glm::vec3& p, const glm::vec3& n, const glm::vec3& pError, const glm::vec3& wo, f32 time)
            : p(p), time(time), pError(pError), wo(wo), n(n)
        {
        }

        virtual ~Interaction() = default;

        inline virtual bool IsSurfaceInteraction() const { return false; }

        inline Ray SpawnRay(const glm::vec3& d) const
        {
            constexpr f32 eps = 0.0001f;

            glm::vec3 offset((glm::dot(d, n) > 0.0f) ? n : -n);
            glm::vec3 origin(p + offset * eps);

            return Ray(origin, d, time);
        }

        inline Ray SpawnRayTo(const Interaction& target) const
        {
            glm::vec3 origin(SpawnRay(target.p - p).origin);
            glm::vec3 d(target.p - origin);

            return Ray(origin, d, time);
        }
    };

    struct Shading
    {
        glm::vec3 n;

        glm::vec3 dpdu;
        glm::vec3 dpdv;

        glm::vec3 dndu;
        glm::vec3 dndv;
    };

    struct SurfaceInteraction final : public Interaction
    {
        glm::vec2 uv;

        glm::vec3 dpdu;
        glm::vec3 dpdv;

        glm::vec3 dndu;
        glm::vec3 dndv;

        Shading shading;

        const Shape* shape { nullptr };
        const Primitive* primitive { nullptr };

        std::shared_ptr<BSDF> bsdf { nullptr };

        f32 t { 0.0f };

        SurfaceInteraction() = default;
        virtual ~SurfaceInteraction() = default;

        SurfaceInteraction(
            const glm::vec3& p, const glm::vec3& pError,
            const glm::vec2& uv,
            const glm::vec3& wo,
            const glm::vec3& dpdu, const glm::vec3& dpdv,
            const glm::vec3& dndu, const glm::vec3& dndv,
            f32 time,
            const Shape* shape
        )   :   Interaction(p, glm::normalize(glm::cross(dpdu, dpdv)), pError, wo, time),
                uv(uv), dpdu(dpdu), dpdv(dpdv), dndu(dndu), dndv(dndv), shape(shape)
        {
            shading.n = n;
            shading.dpdu = dpdu;
            shading.dpdv = dpdv;
            shading.dndu = dndu;
            shading.dndv = dndv;
        }

        inline virtual bool IsSurfaceInteraction() const { return true; }

        void SetShadingGeometry(
            const glm::vec3& dpdu, const glm::vec3& dpdv,
            const glm::vec3& dndu, const glm::vec3& dndv,
            bool orientationIsAuthoritative
        )
        {
            shading.n = glm::normalize(glm::cross(dpdu, dpdv));

            if (orientationIsAuthoritative) {
                n = FaceForward(n, shading.n);
            } else {
                shading.n = FaceForward(shading.n, n);
            }

            shading.dpdu = dpdu;
            shading.dpdv = dpdv;
            shading.dndu = dndu;
            shading.dndv = dndv;
        }

    private:
        inline static glm::vec3 FaceForward(const glm::vec3& n, const glm::vec3& v)
        {
            return (glm::dot(n, v) < 0.0f) ? -n : n;
        }
    };

}
