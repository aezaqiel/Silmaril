#include "PerspectiveCamera.hpp"

#include "Silmaril/Containers/Ray.hpp"

namespace Silmaril {

    PerspectiveCamera::PerspectiveCamera(const std::shared_ptr<Film>& film, const glm::vec3& position, const glm::vec3& lookat, const glm::vec3& up, f32 fov)
        : Camera(film), m_Position(position), m_Fov(fov)
    {
        m_AspectRatio = static_cast<f32>(film->GetWidth()) / static_cast<f32>(film->GetHeight());

        w = glm::normalize(position - lookat);
        u = glm::normalize(glm::cross(up, w));
        v = glm::cross(w, u);

        m_TanHalfFov = glm::tan(glm::radians(fov) / 2.0f);
    }

    Ray PerspectiveCamera::GenerateRay(const CameraSample& s) const
    {
        f32 px = (2.0f * (s.pFilm.x / static_cast<f32>(GetFilm().GetWidth())) - 1.0f) * m_AspectRatio * m_TanHalfFov;
        f32 py = (1.0f - 2.0f * (s.pFilm.y / static_cast<f32>(GetFilm().GetHeight()))) * m_TanHalfFov;

        glm::vec3 direction = glm::normalize(u * px + v * py - w);

        return Ray(m_Position, direction);
    }

}
