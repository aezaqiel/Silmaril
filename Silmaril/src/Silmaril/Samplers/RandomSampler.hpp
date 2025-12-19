#pragma once

#include "Sampler.hpp"

namespace Silmaril {

    class RandomSampler final : public Sampler
    {
    public:
        RandomSampler(u32 spp);
        virtual ~RandomSampler() = default;

        RandomSampler(const RandomSampler& other);

        virtual f32 Get1D() override;
        virtual glm::vec2 Get2D() override;

        virtual std::unique_ptr<Sampler> Clone() const override;

    private:
        std::mt19937 m_Gen;
        std::uniform_real_distribution<f32> m_Dist { 0.0f, 1.0f };
    };

}
