#pragma once

#include <glm/glm.hpp>

namespace Silmaril {

    class Sampler
    {
    public:
        virtual ~Sampler() = default;

        virtual void StartPixel(u32 x, u32 y) = 0;
        virtual f32 Get1D() = 0;
        virtual glm::vec2 Get2D() = 0;

        virtual std::unique_ptr<Sampler> Clone() const = 0;
    };

}
