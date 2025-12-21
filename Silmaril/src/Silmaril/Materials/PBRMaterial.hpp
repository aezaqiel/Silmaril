#pragma once

#include "Material.hpp"
#include "BSDF.hpp"

#include "Silmaril/Textures/Texture.hpp"

namespace Silmaril {

    class PBRBSDF final : public BSDF
    {
    public:
        PBRBSDF(const glm::vec3& albedo, f32 metallic, f32 roughness, const glm::vec3& normal);
        virtual ~PBRBSDF() = default;

        virtual glm::vec3 f(const glm::vec3& wo, const glm::vec3& wi) const override;
        virtual std::optional<BSDFSample> SampleF(const glm::vec3& wo, const glm::vec2& u) const override;
        virtual f32 Pdf(const glm::vec3& wo, const glm::vec3& wi) const override;

    private:
        f32 DistributionGGX(const glm::vec3& N, const glm::vec3& H, f32 roughness) const;
        f32 GeometrySchlickGGX(f32 NdotV, f32 roughness) const;
        f32 GeometrySmith(const glm::vec3& N, const glm::vec3& V, const glm::vec3& L, f32 roughness) const;
        glm::vec3 FresnelSchlick(f32 cosTheta, const glm::vec3& F0) const;

    private:
        glm::vec3 m_Albedo;
        f32 m_Metallic;
        f32 m_Roughness;
        glm::vec3 m_Normal;
    };

    class PBRMaterial final : public Material
    {
    public:
        PBRMaterial(
            const std::shared_ptr<Texture<glm::vec3>>& albedo,
            const std::shared_ptr<Texture<f32>>& metallic,
            const std::shared_ptr<Texture<f32>>& roughness
        );
        PBRMaterial(const glm::vec3& albedo, f32 metallic, f32 roughness);

        virtual ~PBRMaterial() = default;

        virtual void ComputeScatterFn(SurfaceInteraction& intersect) const override;

    private:
        std::shared_ptr<Texture<glm::vec3>> m_Albedo;
        std::shared_ptr<Texture<f32>> m_Metallic;
        std::shared_ptr<Texture<f32>> m_Roughness;
    };

}
