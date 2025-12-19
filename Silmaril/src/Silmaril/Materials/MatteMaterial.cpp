#include "MatteMaterial.hpp"

#include <glm/gtc/constants.hpp>

namespace Silmaril {

    SimpleDiffuseBSDF::SimpleDiffuseBSDF(const glm::vec3& color, const glm::vec3& normal)
        : m_Color(color), m_Normal(normal)
    {
    }

    glm::vec3 SimpleDiffuseBSDF::f(const glm::vec3& wo, const glm::vec3& wi) const
    {
        return m_Color * glm::one_over_pi<f32>();
    }

    std::optional<BSDFSample> SimpleDiffuseBSDF::SampleF(const glm::vec3& wo, const glm::vec2& u) const
    {
        glm::vec3 w = m_Normal;
        glm::vec3 a = (glm::abs(w.x) > 0.9f) ? glm::vec3(0.0f, 1.0f, 0.0f) : glm::vec3(1.0f, 0.0f, 0.0f);
        glm::vec3 v = glm::normalize(glm::cross(w, a));
        glm::vec3 uVec = glm::cross(w, v);

        f32 r1 = u.x;
        f32 r2 = u.y;
        f32 z = glm::sqrt(1.0f - r2);
        f32 phi = 2.0f * glm::pi<f32>() * r1;
        f32 x = glm::cos(phi) * glm::sqrt(r2);
        f32 y = glm::sin(phi) * glm::sqrt(r2);

        glm::vec3 wi = uVec * x + v * y + w * z;
        wi = glm::normalize(wi);

        BSDFSample sample;
        sample.wi = wi;
        sample.pdf = z * glm::one_over_pi<f32>();
        sample.f = m_Color * glm::one_over_pi<f32>();

        return sample;
    }

    f32 SimpleDiffuseBSDF::Pdf(const glm::vec3& wo, const glm::vec3& wi) const
    {
        f32 cosTheta = glm::dot(m_Normal, wi);
        if (cosTheta <= 0.0f) return 0.0f;
        return cosTheta * glm::one_over_pi<f32>();
    }

    MatteMaterial::MatteMaterial(const glm::vec3& color)
        : m_Color(color)
    {
    }

    void MatteMaterial::ComputeScatterFn(SurfaceInteraction& intersect) const
    {
        intersect.bsdf = std::make_shared<SimpleDiffuseBSDF>(m_Color, intersect.n);
    }

}
