#pragma once

#include "Silmaril/PBRT/Containers/Interaction.hpp"

namespace Silmaril {

    template <typename T>
    class Texture
    {
    public:
        virtual ~Texture() = default;

        virtual T Evaluate(const SurfaceInteraction& interaction) const = 0;
    };

}
