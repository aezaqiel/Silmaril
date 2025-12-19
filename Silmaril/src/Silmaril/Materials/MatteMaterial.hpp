#pragma once

#include "Material.hpp"
#include "BSDF.hpp"

namespace Silmaril {

    class SimpleDiffuseBSDF final : public BSDF
    {
    public:
        SimpleDiffuseBSDF(const glm::vec3& color, const glm::vec3& normal);
        virtual ~SimpleDiffuseBSDF() = default;

        virtual glm::vec3 f(const glm::vec3& wo, const glm::vec3& wi) const override;
        virtual std::optional<BSDFSample> SampleF(const glm::vec3& wo, const glm::vec2& u) const override;
        virtual f32 Pdf(const glm::vec3& wo, const glm::vec3& wi) const override;

    private:
        glm::vec3 m_Color;
        glm::vec3 m_Normal;
    };

    class MatteMaterial final : public Material
    {
    public:
        MatteMaterial(const glm::vec3& color);
        virtual ~MatteMaterial() = default;

        virtual void ComputeScatterFn(SurfaceInteraction& intersect) const override;

    private:
        glm::vec3 m_Color;
    };

}
