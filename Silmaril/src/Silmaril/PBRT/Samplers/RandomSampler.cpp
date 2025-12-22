#include "RandomSampler.hpp"

namespace Silmaril {

    RandomSampler::RandomSampler(u32 spp)
        : Sampler(spp), m_Gen(std::random_device{}())
    {
    }

    RandomSampler::RandomSampler(const RandomSampler& other)
        : Sampler(other.m_MaxSPP), m_Gen(std::random_device{}())
    {
    }

    f32 RandomSampler::Get1D()
    {
        return m_Dist(m_Gen);
    }

    glm::vec2 RandomSampler::Get2D()
    {
        return glm::vec2(m_Dist(m_Gen), m_Dist(m_Gen));
    }

    std::unique_ptr<Sampler> RandomSampler::Clone() const
    {
        return std::make_unique<RandomSampler>(*this);
    }

}
