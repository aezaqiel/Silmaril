#pragma once

#include <glm/glm.hpp>

namespace Silmaril {

    struct BSDFSample
    {
        glm::vec3 wi;
        glm::vec3 f;
        f32 pdf;
    };

    class BSDF
    {
    public:
        virtual ~BSDF() = default;

        virtual glm::vec3 f(const glm::vec3& wo, const glm::vec3& wi) const = 0;
        virtual std::optional<BSDFSample> SampleF(const glm::vec3& wo, const glm::vec2& u) const = 0;
        virtual f32 Pdf(const glm::vec3& wo, const glm::vec3& wi) const = 0;
    };

}
