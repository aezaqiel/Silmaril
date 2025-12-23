#include "DiffusedAreaLight.hpp"

#include <glm/gtx/norm.hpp>

namespace Silmaril {

    DiffuseAreaLight::DiffuseAreaLight(const std::shared_ptr<Shape>& shape, const glm::vec3& le)
        : Light(LightFlags::Area), m_Shape(shape), m_LEmit(le)
    {
    }

    std::optional<LightSample> DiffuseAreaLight::SampleLi(const Interaction& ref, const glm::vec2& u) const
    {
        f32 pdf = 0.0f;
        Interaction pShape = m_Shape->Sample(ref, u, pdf);

        if (pdf == 0.0f || glm::length(pShape.p - ref.p) == 0.0f) return std::nullopt;

        LightSample ls;
        ls.wi = pShape.p - ref.p;
        ls.distance = glm::length(ls.wi);
        ls.wi = glm::normalize(ls.wi);

        f32 cosTheta = glm::abs(glm::dot(pShape.n, -ls.wi));
        if (cosTheta == 0.0f) return std::nullopt;

        ls.pdf = pdf * (ls.distance * ls.distance) / cosTheta;

        if (glm::dot(pShape.n, -ls.wi) > 0.0f) {
            ls.li = m_LEmit;
        } else {
            ls.li = glm::vec3(0.0f);
        }

        return ls;
    }

    f32 DiffuseAreaLight::PdfLi(const Interaction& ref, const glm::vec3& wi) const
    {
        Ray ray = ref.SpawnRay(wi);
        f32 tHit;

        if (!m_Shape->Intersect(ray, tHit, std::numeric_limits<f32>::max())) {
            return 0.0f;
        }

        SurfaceInteraction intersection;
        m_Shape->FillSurfaceInteraction(ray, tHit, intersection);

        return PdfLi(ref, intersection);
    }

    f32 DiffuseAreaLight::PdfLi(const Interaction& ref, const Interaction& lightHit) const
    {
        f32 dist2 = glm::distance2(ref.p, lightHit.p);
        glm::vec3 wi = glm::normalize(lightHit.p - ref.p);
        f32 cosTheta = glm::abs(glm::dot(lightHit.n, -wi));
        f32 area = m_Shape->Area();

        if (cosTheta < 1e-4 || area <= 0.0f) return 0.0f;

        return dist2 / (area * cosTheta);
    }

    glm::vec3 DiffuseAreaLight::L(const Interaction& interaction, const glm::vec3& w) const
    {
        if (glm::dot(interaction.n, w) > 0.0f) {
            return m_LEmit;
        }

        return glm::vec3(0.0f);
    }

}
