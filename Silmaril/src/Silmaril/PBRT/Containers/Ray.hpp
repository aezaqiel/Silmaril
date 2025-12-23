#pragma once

#include <glm/glm.hpp>

namespace Silmaril {

    struct Ray
    {
        glm::vec3 origin;
        glm::vec3 direction;
        glm::vec3 invDirection;
        f32 time { 0.0f };

        Ray()
            : origin(0.0f), direction(0.0f), invDirection(0.0f)
        {
        }

        Ray(const glm::vec3& origin, const glm::vec3& direction, f32 time = 0.0f)
            : origin(origin), direction(direction), invDirection(1.0f / direction), time(time)
        {
        }

        glm::vec3 At(f32 t) const
        {
            return origin + t * direction;
        }
    };

}
