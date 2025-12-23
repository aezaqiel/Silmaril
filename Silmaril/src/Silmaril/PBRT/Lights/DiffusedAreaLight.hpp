#pragma once

#include "Light.hpp"
#include "Silmaril/PBRT/Geometry/Shape.hpp"

namespace Silmaril {

    class DiffuseAreaLight final : public Light
    {
    public:
        DiffuseAreaLight(const std::shared_ptr<Shape>& shape, const glm::vec3& le);
        virtual ~DiffuseAreaLight() = default;

        virtual std::optional<LightSample> SampleLi(const Interaction& ref, const glm::vec2& u) const override;
        virtual glm::vec3 L(const Interaction& interaction, const glm::vec3& w) const override;

    private:
        std::shared_ptr<Shape> m_Shape;
        glm::vec3 m_LEmit;
    };

}
