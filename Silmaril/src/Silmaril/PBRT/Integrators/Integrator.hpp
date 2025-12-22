#pragma once

namespace Silmaril {

    class Scene;

    class Integrator
    {
    public:
        virtual ~Integrator() = default;

        virtual void Render(const Scene& scene) = 0;
    };

}
