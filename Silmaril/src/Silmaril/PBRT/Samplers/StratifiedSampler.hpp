#pragma once

#include "Sampler.hpp"
#include "Silmaril/DSA/PCG32.hpp"

namespace Silmaril {

    class StratifiedSampler final : public Sampler
    {
    public:
        StratifiedSampler(u32 spp, bool jitter = true);
        virtual ~StratifiedSampler() = default;

        StratifiedSampler(const StratifiedSampler& other);

        virtual f32 Get1D() override;
        virtual glm::vec2 Get2D() override;

        virtual void StartPixel(u32 x, u32 y, u32 s) override;

        virtual std::unique_ptr<Sampler> Clone() const override;

    private:
        u32 m_XSamples { 0 };
        u32 m_YSamples { 0 };
        bool m_Jitter { true };

        u32 m_CurrentDimension1D { 0 };
        u32 m_CurrentDimension2D { 0 };

        PCG32 m_PCG;
    };

}
