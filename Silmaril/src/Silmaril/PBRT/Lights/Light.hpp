#pragma once

#include <glm/glm.hpp>

#include "Silmaril/PBRT/Containers/Interaction.hpp"

namespace Silmaril {

    struct LightSample
    {
        glm::vec3 li;
        glm::vec3 wi;
        f32 pdf;
        f32 distance;
    };

    class Light
    {
    public:
        virtual ~Light() = default;

        virtual std::optional<LightSample> SampleLi(const Interaction& ref, const glm::vec2& u) const = 0;
    };

}
