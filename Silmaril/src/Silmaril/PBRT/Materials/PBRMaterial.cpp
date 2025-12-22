#include "PBRMaterial.hpp"

#include <glm/gtc/constants.hpp>

#include "Silmaril/PBRT/Textures/SolidTexture.hpp"

namespace Silmaril {

    PBRBSDF::PBRBSDF(const glm::vec3& albedo, f32 metallic, f32 roughness, const glm::vec3& normal)
        : m_Albedo(albedo), m_Metallic(std::clamp(metallic, 0.0f, 1.0f)), m_Roughness(std::max(roughness, 0.04f)), m_Normal(normal)
    {
    }

    glm::vec3 PBRBSDF::f(const glm::vec3& wo, const glm::vec3& wi) const
    {
        glm::vec3 N = m_Normal;
        glm::vec3 V = glm::normalize(wo);
        glm::vec3 L = glm::normalize(wi);
        glm::vec3 H = glm::normalize(V + L);

        f32 NdotV = std::max(glm::dot(N, V), 0.0f);
        f32 NdotL = std::max(glm::dot(N, L), 0.0f);
        f32 HdotV = std::max(glm::dot(H, V), 0.0f);

        if (NdotL <= 0.0f || NdotV <= 0.0f) return glm::vec3(0.0f);

        glm::vec3 F0 = glm::vec3(0.04f);
        F0 = glm::mix(F0, m_Albedo, m_Metallic);

        glm::vec3 F = FresnelSchlick(HdotV, F0);
        f32 D = DistributionGGX(N, H, m_Roughness);
        f32 G = GeometrySmith(N, V, L, m_Roughness);

        glm::vec3 numerator = D * F * G;
        f32 denominator = 4.0f * NdotV * NdotL + 0.0001f;
        glm::vec3 specular = numerator / denominator;

        glm::vec3 kS = F;
        glm::vec3 kD = glm::vec3(1.0f) - kS;
        kD *= (1.0f - m_Metallic);

        glm::vec3 diffuse = kD * m_Albedo * glm::one_over_pi<f32>();

        return diffuse + specular;
    }

    std::optional<BSDFSample> PBRBSDF::SampleF(const glm::vec3& wo, const glm::vec2& u) const
    {
        glm::vec3 N = m_Normal;
        glm::vec3 V = glm::normalize(wo);

        f32 a = m_Roughness * m_Roughness;
        f32 phi = 2.0f * glm::pi<f32>() * u.x;
        f32 cosTheta = glm::sqrt((1.0f - u.y) / (1.0f + (a * a - 1.0f) * u.y));
        f32 sinTheta = glm::sqrt(1.0f - cosTheta * cosTheta);

        glm::vec3 Htan(sinTheta * glm::cos(phi), sinTheta * glm::sin(phi), cosTheta);

        glm::vec3 up = (std::abs(N.z) < 0.999f) ? glm::vec3(0.0f, 0.0f, 1.0f) : glm::vec3(1.0f, 0.0f, 0.0f);
        glm::vec3 tangent = glm::normalize(glm::cross(up, N));
        glm::vec3 bitangent = glm::cross(N, tangent);
        glm::vec3 H = glm::normalize(tangent * Htan.x + bitangent * Htan.y + N * Htan.z);

        glm::vec3 L = glm::normalize(2.0f * glm::dot(V, H) * H - V);

        if (glm::dot(N, L) <= 0.0f) return std::nullopt;

        BSDFSample sample;
        sample.wi = L;
        sample.pdf = Pdf(wo, L);
        sample.f = f(wo, L);

        if (sample.pdf <= 0.0f) return std::nullopt;
        return sample;
    }

    f32 PBRBSDF::Pdf(const glm::vec3& wo, const glm::vec3& wi) const
    {
        glm::vec3 N = m_Normal;
        glm::vec3 V = glm::normalize(wo);
        glm::vec3 L = glm::normalize(wi);

        if (glm::dot(N, L) <= 0.0f) return 0.0f;

        glm::vec3 H = glm::normalize(V + L);

        f32 NdotH = std::max(glm::dot(N, H), 0.0f);
        f32 HdotV = std::max(glm::dot(H, V), 0.0f);

        f32 D = DistributionGGX(N, H, m_Roughness);

        return (D * NdotH) / (4.0f * HdotV + 0.0001f);
    }

    f32 PBRBSDF::DistributionGGX(const glm::vec3& N, const glm::vec3& H, f32 roughness) const
    {
        f32 a = roughness * roughness;
        f32 a2 = a * a;
        f32 NdotH = std::max(glm::dot(N, H), 0.0f);
        f32 NdotH2 = NdotH * NdotH;
        f32 denom = (NdotH2 * (a2 - 1.0f) + 1.0f);

        return a2 / (glm::pi<f32>() * denom * denom);
    }

    f32 PBRBSDF::GeometrySchlickGGX(f32 NdotV, f32 roughness) const
    {
        f32 r = (roughness + 1.0f);
        f32 k = (r * r) / 8.0f;

        return NdotV / (NdotV * (1.0f - k) + k);
    }

    f32 PBRBSDF::GeometrySmith(const glm::vec3& N, const glm::vec3& V, const glm::vec3& L, f32 roughness) const
    {
        return GeometrySchlickGGX(std::max(glm::dot(N, V), 0.0f), roughness) * GeometrySchlickGGX(std::max(glm::dot(N, L), 0.0f), roughness);
    }

    glm::vec3 PBRBSDF::FresnelSchlick(f32 cosTheta, const glm::vec3& F0) const
    {
        return F0 + (1.0f - F0) * glm::pow(1.0f - cosTheta, 5.0f);
    }

    PBRMaterial::PBRMaterial(
        const std::shared_ptr<Texture<glm::vec3>>& albedo,
        const std::shared_ptr<Texture<f32>>& metallic,
        const std::shared_ptr<Texture<f32>>& roughness
    )
        : m_Albedo(albedo), m_Metallic(metallic), m_Roughness(roughness)
    {
    }

    PBRMaterial::PBRMaterial(const glm::vec3& albedo, f32 metallic, f32 roughness)
        :   m_Albedo(std::make_shared<SolidTexture<glm::vec3>>(albedo)),
            m_Metallic(std::make_shared<SolidTexture<f32>>(metallic)),
            m_Roughness(std::make_shared<SolidTexture<f32>>(roughness))
    {
    }

    void PBRMaterial::ComputeScatterFn(SurfaceInteraction& intersect) const
    {
        glm::vec3 albedo = m_Albedo->Evaluate(intersect);
        f32 metallic = m_Metallic->Evaluate(intersect);
        f32 roughness = m_Roughness->Evaluate(intersect);

        intersect.bsdf = std::make_shared<PBRBSDF>(
            albedo,
            metallic,
            roughness,
            intersect.shading.n
        );
    }

}
