#include "RandomWalkIntegrator.hpp"

#include "Silmaril/Cameras/Camera.hpp"
#include "Silmaril/Samplers/Sampler.hpp"

namespace Silmaril {

    RandomWalkIntegrator::RandomWalkIntegrator(const std::shared_ptr<Camera>& camera, const std::shared_ptr<Sampler>& sampler, usize depth)
        : m_Camera(camera), m_Sampler(sampler), m_MaxDepth(depth)
    {
    }

    void RandomWalkIntegrator::Render(const Scene& scene)
    {
    }

}
