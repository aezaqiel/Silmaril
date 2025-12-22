#pragma once

#include "Silmaril/PBRT/Containers/Interaction.hpp"

namespace Silmaril {

    class Material
    {
    public:
        virtual ~Material() = default;

        virtual void ComputeScatterFn(SurfaceInteraction& intersect) const = 0;
    };

}

