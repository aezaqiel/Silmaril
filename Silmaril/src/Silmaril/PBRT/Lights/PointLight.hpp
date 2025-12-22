#pragma once

#include "Light.hpp"

namespace Silmaril {

    class PointLight final : public Light
    {
    public:
        PointLight(const glm::vec3& p, const glm::vec3& intensity);
        virtual ~PointLight() = default;

        virtual std::optional<LightSample> SampleLi(const Interaction& ref, const glm::vec2& u) const override;

    private:
        glm::vec3 m_Pos;
        glm::vec3 m_Intensity;
    };

}
