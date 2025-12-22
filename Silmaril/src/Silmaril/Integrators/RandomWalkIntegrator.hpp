#pragma once

#include <glm/glm.hpp>

#include "Integrator.hpp"

namespace Silmaril {

    class Camera;
    class Sampler;
    struct Ray;

    class RandomWalkIntegrator final : public Integrator
    {
    public:
        RandomWalkIntegrator(const std::shared_ptr<Camera>& camera, const std::shared_ptr<Sampler>& sampler, usize depth);
        ~RandomWalkIntegrator() = default;

        virtual void Render(const Scene& scene) override;

    private:
        struct Tile
        {
            u32 x;
            u32 y;
            u32 w;
            u32 h;
        };

    private:
        void RenderTile(const Tile& tile, const Scene& scene);
        glm::vec3 Li(const Ray& ray, const Scene& scene, Sampler& sampler, usize depth);

    private:
        std::shared_ptr<Camera> m_Camera;
        std::shared_ptr<Sampler> m_Sampler;
        usize m_MaxDepth { 1 };

        inline static constexpr u32 s_TileSize { 16 };
    };

}
