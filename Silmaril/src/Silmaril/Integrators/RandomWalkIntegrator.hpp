#pragma once

#include "Integrator.hpp"

namespace Silmaril {

    class Camera;
    class Sampler;

    class RandomWalkIntegrator final : public Integrator
    {
    public:
        RandomWalkIntegrator(const std::shared_ptr<Camera>& camera, const std::shared_ptr<Sampler>& sampler, usize depth);
        ~RandomWalkIntegrator() = default;

        virtual void Render(const Scene& scene) override;

    private:
        std::shared_ptr<Camera> m_Camera;
        std::shared_ptr<Sampler> m_Sampler;
        usize m_MaxDepth { 1 };
    };

}
