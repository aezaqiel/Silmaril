#pragma once

#include <glm/glm.hpp>

namespace Silmaril {

    struct Ray;

    struct CameraSample
    {
        glm::vec2 pFilm;
    };

    class Camera
    {
    public:
        virtual ~Camera() = default;

        virtual Ray GenerateRay(const CameraSample& s) const = 0;
    };

}
