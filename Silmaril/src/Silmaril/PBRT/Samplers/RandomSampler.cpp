#include "RandomSampler.hpp"

namespace Silmaril {

    namespace {

        u64 Hash(u32 x, u32 y, u32 s)
        {
            uint64_t z = (static_cast<uint64_t>(x) << 32) ^ (static_cast<uint64_t>(y) << 16) ^ s;
            z ^= z >> 30;
            z *= 0xbf58476d1ce4e5b9ULL;
            z ^= z >> 27;
            z *= 0x94d049bb133111ebULL;
            z ^= z >> 31;
            return z;
        }
    
    }

    RandomSampler::RandomSampler(u32 spp)
        : Sampler(spp)
    {
        m_PCG.Seed(0x853c49e6748fea9bULL, 0xda3e39cb94b95bdbULL);
    }

    RandomSampler::RandomSampler(const RandomSampler& other)
        : Sampler(other.m_MaxSPP), m_PCG(other.m_PCG)
    {
    }

    f32 RandomSampler::Get1D()
    {
        return m_PCG.Float32();
    }

    glm::vec2 RandomSampler::Get2D()
    {
        return glm::vec2(m_PCG.Float32(), m_PCG.Float32());
    }

    void RandomSampler::StartPixel(u32 x, u32 y, u32 sample)
    {
        Sampler::StartPixel(x, y, sample);

        u64 seed = Hash(x, y, sample);
        m_PCG.Seed(seed, 1442695040888963407ULL);
    }

    std::unique_ptr<Sampler> RandomSampler::Clone() const
    {
        return std::make_unique<RandomSampler>(*this);
    }

}
