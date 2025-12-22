#include "RandomWalkIntegrator.hpp"

#include "Silmaril/PBRT/Cameras/Camera.hpp"
#include "Silmaril/PBRT/Samplers/Sampler.hpp"
#include "Silmaril/PBRT/Containers/Interaction.hpp"
#include "Silmaril/PBRT/Scene/Scene.hpp"
#include "Silmaril/PBRT/Materials/Material.hpp"
#include "Silmaril/PBRT/Materials/BSDF.hpp"

#include "Silmaril/Core/Logger.hpp"

namespace Silmaril {

    RandomWalkIntegrator::RandomWalkIntegrator(const Config& config)
        :   Integrator(config.tile),
            m_Config(config)
    {
    }

    void RandomWalkIntegrator::Render(const Scene& scene)
    {
        m_CancelRender = false;

        const auto& camera = m_Config.camera;

        u32 width = camera->GetFilm().GetWidth();
        u32 height = camera->GetFilm().GetHeight();

        u32 tilesX = (width + m_TileSize - 1) / m_TileSize;
        u32 tilesY = (height + m_TileSize - 1) / m_TileSize;
        u32 totalTiles = tilesX * tilesY;

        std::vector<Tile> tiles;
        tiles.reserve(totalTiles);

        for (u32 y = 0; y < tilesY; ++y) {
            for (u32 x = 0; x < tilesX; ++x) {
                u32 x0 = x * m_TileSize;
                u32 y0 = y * m_TileSize;
                u32 x1 = std::min(x0 + m_TileSize, width);
                u32 y1 = std::min(y0 + m_TileSize, height);
                tiles.push_back({ x0, y0, x1 - x0, y1 - y0 });
            }
        }

        LOG_INFO("Rendering {} tiles ({}x{})", totalTiles, m_TileSize, m_TileSize);

        std::for_each(std::execution::par, tiles.begin(), tiles.end(), [&](const Tile& tile) {
            if (m_CancelRender) return;

            RenderTile(tile, scene);

            if (m_RenderCallback) {
                m_RenderCallback(tile.x, tile.y, tile.w, tile.h);
            }
        });

        if (m_CancelRender) {
            LOG_WARN("In-progress render cancelled");
            return;
        }

        camera->GetFilm().Write(m_Config.output);
    }

    void RandomWalkIntegrator::RenderTile(const Tile& tile, const Scene& scene)
    {
        const auto& camera = m_Config.camera;
        auto sampler = m_Config.sampler->Clone();

        for (u32 y = tile.y; y < tile.y + tile.h; ++y) {
            for (u32 x = tile.x; x < tile.x + tile.w; ++x) {
                sampler->StartPixel(x, y);

                glm::vec3 accumulator(0.0f);
                while (sampler->StartNextSample()) {
                    CameraSample cs;
                    cs.pFilm = glm::vec2(x, y) + sampler->Get2D();

                    Ray ray = camera->GenerateRay(cs);

                    accumulator += Li(ray, scene, *sampler, 0);
                }

                accumulator /= static_cast<f32>(sampler->GetSPP());
                camera->GetFilm().SetPixel(x, y, accumulator);
            }
        }
    }

    glm::vec3 RandomWalkIntegrator::Li(const Ray& ray, const Scene& scene, Sampler& sampler, usize depth)
    {
        if (depth >= m_Config.depth) return glm::vec3(0.0f);

        SurfaceInteraction intersect;
        if (!scene.Intersect(ray, intersect)) {
            glm::vec3 unit_dir = glm::normalize(ray.direction);
            f32 t = 0.5f * (unit_dir.y + 1.0f);
            return (1.0f - t) * glm::vec3(1.0f, 1.0f, 1.0f) + t * glm::vec3(0.5f, 0.7f, 1.0f) * 0.2f;
        }

        const Material* material = intersect.primitive->GetMaterial();
        if (!material) return glm::vec3(0.0f);

        material->ComputeScatterFn(intersect);
        if (!intersect.bsdf) return glm::vec3(0.0f);

        glm::vec3 L(0.0f);
        glm::vec3 wo = -ray.direction;

        for (const auto& light : scene.GetLights()) {
            auto ls = light->SampleLi(intersect, sampler.Get2D());

            if (ls && ls->pdf > 0 && ls->distance > 0) {
                Ray shadowRay = intersect.SpawnRay(ls->wi);
                SurfaceInteraction shadowIntersect;
                bool hit = scene.Intersect(shadowRay, shadowIntersect);

                if (!hit || shadowIntersect.t > ls->distance - 0.0001f) {
                    glm::vec3 f = intersect.bsdf->f(wo, ls->wi);
                    if (glm::length(f) > 0) {
                        L += f * ls->li * glm::abs(glm::dot(intersect.shading.n, ls->wi)) / ls->pdf;
                    }
                }
            }
        }

        auto bs = intersect.bsdf->SampleF(wo, sampler.Get2D());
        if (bs && bs->pdf > 0) {
            Ray nextRay = intersect.SpawnRay(bs->wi);
            glm::vec3 indirect = Li(nextRay, scene, sampler, depth + 1);
            L += bs->f * indirect * glm::abs(glm::dot(intersect.shading.n, bs->wi)) / bs->pdf;
        }

        return L;
    }

}
