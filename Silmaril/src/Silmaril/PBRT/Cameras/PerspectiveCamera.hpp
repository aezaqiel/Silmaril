#pragma once

#include "Camera.hpp"

namespace Silmaril {

    class PerspectiveCamera final : public Camera
    {
    public:
        PerspectiveCamera(const std::shared_ptr<Film>& film, const glm::vec3& position, const glm::vec3& lookat, const glm::vec3& up, f32 fov);
        virtual ~PerspectiveCamera() = default;

        virtual Ray GenerateRay(const CameraSample& s) const override;

    private:
        glm::vec3 m_Position;
        glm::vec3 m_Forward;
        glm::vec3 m_Right;
        glm::vec3 m_Up;

        glm::vec3 u, v, w;

        f32 m_Fov;
        f32 m_AspectRatio;
        f32 m_TanHalfFov;
    };

}
