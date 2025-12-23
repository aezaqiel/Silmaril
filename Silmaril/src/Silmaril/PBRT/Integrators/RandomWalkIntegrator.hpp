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

            std::string output;
        };
    public:
        RandomWalkIntegrator(const Config& config);
        virtual ~RandomWalkIntegrator() = default;

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
        void RenderTile(const Tile& tile, const Scene& scene, u32 sample);
        glm::vec3 Li(const Ray& ray, const Scene& scene, Sampler& sampler, u32 depth, glm::vec3 beta = glm::vec3(1.0f));

    private:
        Config m_Config;
    };

}
