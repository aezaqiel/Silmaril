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
        struct Config
        {
            std::shared_ptr<Camera> camera;
            std::shared_ptr<Sampler> sampler;
            u32 depth;
            u32 tile;
        };
    public:
        RandomWalkIntegrator(const Config& config);
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

        u32 m_MaxDepth { 1 };
    };

}
