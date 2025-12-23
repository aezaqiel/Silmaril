#pragma once

#include <glm/glm.hpp>

namespace Silmaril {

    class Sampler
    {
    public:
        Sampler(u32 spp);
        virtual ~Sampler() = default;

        virtual f32 Get1D() = 0;
        virtual glm::vec2 Get2D() = 0;

        virtual std::unique_ptr<Sampler> Clone() const = 0;

        inline u32 GetSPP() const { return m_MaxSPP; }

        virtual void StartPixel(u32 x, u32 y, u32 sample)
        {
            m_CurrentSampleIndex = sample;
        }

        virtual bool StartNextSample()
        {
            m_CurrentSampleIndex++;
            return m_CurrentSampleIndex <= m_MaxSPP;
        }

    protected:
        u32 m_MaxSPP { 1 };
        u32 m_CurrentSampleIndex { 0 };
    };

}
