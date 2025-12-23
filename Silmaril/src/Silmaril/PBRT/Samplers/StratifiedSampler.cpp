#include "StratifiedSampler.hpp"

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

        u32 Permute(u32 i, u32 n, u32 scramble)
        {
            return (i + scramble) % n;
        }

    }

    StratifiedSampler::StratifiedSampler(u32 spp, bool jitter)
        : Sampler(spp), m_Jitter(jitter)
    {
        m_XSamples = static_cast<u32>(std::round(std::sqrt(spp)));
        m_YSamples = static_cast<u32>(std::round(static_cast<f32>(spp) / m_XSamples));
        
        m_MaxSPP = m_XSamples * m_YSamples;
        
        m_PCG.Seed(0x853c49e6748fea9bULL, 0xda3e39cb94b95bdbULL);
    }

    StratifiedSampler::StratifiedSampler(const StratifiedSampler& other)
        : Sampler(other.m_MaxSPP),
          m_XSamples(other.m_XSamples),
          m_YSamples(other.m_YSamples),
          m_Jitter(other.m_Jitter),
          m_PCG(other.m_PCG)
    {
    }

    void StratifiedSampler::StartPixel(u32 x, u32 y, u32 sample)
    {
        Sampler::StartPixel(x, y, sample);

        m_CurrentDimension1D = 0;
        m_CurrentDimension2D = 0;

        u64 seed = Hash(x, y, sample);
        m_PCG.Seed(seed, 1442695040888963407ULL);
    }

    f32 StratifiedSampler::Get1D()
    {
        u64 hash = Hash(m_CurrentSampleIndex, m_CurrentDimension1D, 0x12345678);
        u32 stratum = Permute(m_CurrentSampleIndex, m_MaxSPP, static_cast<u32>(hash));

        m_CurrentDimension1D++;

        f32 delta = m_Jitter ? m_PCG.Float32() : 0.5f;
        return (stratum + delta) / static_cast<f32>(m_MaxSPP);
    }

    glm::vec2 StratifiedSampler::Get2D()
    {
        u32 stratum = m_CurrentSampleIndex;

        if (m_CurrentDimension2D > 0) {
            u64 hash = Hash(m_CurrentSampleIndex, m_CurrentDimension2D, 0x87654321);
            stratum = Permute(m_CurrentSampleIndex, m_MaxSPP, static_cast<u32>(hash));
        }

        m_CurrentDimension2D++;

        u32 xStrata = stratum % m_XSamples;
        u32 yStrata = stratum / m_XSamples;

        f32 dx = m_Jitter ? m_PCG.Float32() : 0.5f;
        f32 dy = m_Jitter ? m_PCG.Float32() : 0.5f;

        return glm::vec2(
            (xStrata + dx) / static_cast<f32>(m_XSamples),
            (yStrata + dy) / static_cast<f32>(m_YSamples)
        );
    }

    std::unique_ptr<Sampler> StratifiedSampler::Clone() const
    {
        return std::make_unique<StratifiedSampler>(*this);
    }

}
