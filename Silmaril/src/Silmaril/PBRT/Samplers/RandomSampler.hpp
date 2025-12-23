#pragma once

#include "Sampler.hpp"
#include "Silmaril/DSA/PCG32.hpp"

namespace Silmaril {

    class RandomSampler final : public Sampler
    {
    public:
        RandomSampler(u32 spp);
        virtual ~RandomSampler() = default;

        RandomSampler(const RandomSampler& other);

        virtual f32 Get1D() override;
        virtual glm::vec2 Get2D() override;

        virtual void StartPixel(u32 x, u32 y, u32 sample) override;

        virtual std::unique_ptr<Sampler> Clone() const override;

    private:
        PCG32 m_PCG;
    };

}
