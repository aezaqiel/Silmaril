#include "DiffusedAreaLight.hpp"

namespace Silmaril {

    DiffuseAreaLight::DiffuseAreaLight(const std::shared_ptr<Shape>& shape, const glm::vec3& le)
        : m_Shape(shape), m_LEmit(le)
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

    glm::vec3 DiffuseAreaLight::L(const Interaction& interaction, const glm::vec3& w) const
    {
        if (glm::dot(interaction.n, w) > 0.0f) {
            return m_LEmit;
        }

        return glm::vec3(0.0f);
    }

}
