#include "RandomWalkIntegrator.hpp"

#include "Silmaril/PBRT/Cameras/Camera.hpp"
#include "Silmaril/PBRT/Samplers/Sampler.hpp"
#include "Silmaril/PBRT/Containers/Interaction.hpp"
#include "Silmaril/PBRT/Scene/Scene.hpp"
#include "Silmaril/PBRT/Materials/Material.hpp"
#include "Silmaril/PBRT/Materials/BSDF.hpp"

#include "Silmaril/Core/Logger.hpp"
#include "Silmaril/Core/JobSystem.hpp"

namespace Silmaril {

    namespace {

        f32 PowerHeuristic(f32 f, f32 g)
        {
            f32 f2 = f * f;
            f32 g2 = g * g;
            return f2 / (f2 + g2);
        }
    
    }

    RandomWalkIntegrator::RandomWalkIntegrator(const Config& config)
        :   Integrator(config.tile),
            m_Config(config)
    {
    }

    void RandomWalkIntegrator::Render(const Scene& scene)
    {
        m_CancelRender = false;

        const auto& camera = m_Config.camera;
        const auto& sampler = m_Config.sampler;

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

        LOG_INFO("Rendering {} tiles ({}x{}) for {} samples", totalTiles, m_TileSize, m_TileSize, sampler->GetSPP());

        for (u32 pass = 1; pass < (sampler->GetSPP() + 1); ++pass) {
            auto start = std::chrono::steady_clock::now();

            JobSystem::Dispatch(static_cast<u32>(tiles.size()), 1, [&](JobDispatchArgs args) {
                if (m_CancelRender) return;

                const Tile& tile = tiles[args.jobIndex];

                RenderTile(tile, scene, pass);
                if (m_RenderCallback) {
                    m_RenderCallback(tile.x, tile.y, tile.w, tile.h);
                }
            });

            JobSystem::Sync();

            if (m_CancelRender) {
                LOG_WARN("In-progress render cancelled");
                return;
            }

            auto end = std::chrono::steady_clock::now();
            std::chrono::duration<f32> duration = end - start;

            LOG_INFO("Pass [{}/{}] | {:.2f} seconds", pass, sampler->GetSPP(), duration.count());
        }

        camera->GetFilm().Write(m_Config.output);
    }

    void RandomWalkIntegrator::RenderTile(const Tile& tile, const Scene& scene, u32 sample)
    {
        const auto& camera = m_Config.camera;
        auto sampler = m_Config.sampler->Clone();

        for (u32 y = tile.y; y < tile.y + tile.h; ++y) {
            for (u32 x = tile.x; x < tile.x + tile.w; ++x) {
                sampler->StartPixel(x, y, sample);

                CameraSample cs;
                cs.pFilm = glm::vec2(x, y) + sampler->Get2D();

                Ray ray = camera->GenerateRay(cs);

                glm::vec3 L = Li(ray, scene, *sampler, 0);

                camera->GetFilm().AccumulateSample(x, y, L, sample);
            }
        }
    }

    glm::vec3 RandomWalkIntegrator::Li(
        const Ray& ray,
        const Scene& scene,
        Sampler& sampler,
        u32 depth,
        glm::vec3 beta,
        f32 prevPdfBSDF,
        bool prevIsDelta
    )
    {
        if (depth >= m_Config.depth) return glm::vec3(0.0f);

        Ray r = ray;
        glm::vec3 L(0.0f);

        SurfaceInteraction intersect;
        if (!scene.Intersect(r, intersect)) {
            // Skybox
            f32 t = 0.5f * (glm::normalize(r.direction).y + 1.0f);
            glm::vec3 sky = glm::mix(glm::vec3(1.0f), glm::vec3(0.5f, 0.7f, 1.0f), t);

            return beta * sky;
        }

        glm::vec3 wo = -r.direction;

        if (intersect.primitive->GetLight()) {
            const Light* light = intersect.primitive->GetLight();
            glm::vec3 Le = light->L(intersect, wo);

            if (depth == 0 || prevIsDelta) {
                L += beta * Le;
            } else {
                Interaction prev;
                prev.p = r.origin;

                f32 lightPdf = light->PdfLi(prev, intersect);
                f32 weight = PowerHeuristic(prevPdfBSDF, lightPdf);

                L += beta * Le * weight;
            }
        }

        const Material* material = intersect.primitive->GetMaterial();
        if (!material) return L;

        material->ComputeScatterFn(intersect);
        if (!intersect.bsdf) return L;

        // Direct Light
        const auto& lights = scene.GetLights();
        if (!lights.empty()) {
            u32 lightIdx = std::min(static_cast<u32>(sampler.Get1D() * lights.size()), static_cast<u32>(lights.size()) - 1);
            const auto& light = lights[lightIdx];

            f32 lightSelectPdf = 1.0f / lights.size();

            auto ls = light->SampleLi(intersect, sampler.Get2D());

            if (ls && ls->pdf > 0.0f && ls->distance > 0.0f && !glm::isinf(ls->pdf)) {
                glm::vec3 wi = ls->wi;
                f32 lightPdf = ls->pdf / lightSelectPdf;

                Ray shadowRay = intersect.SpawnRay(wi);
                SurfaceInteraction shadowIntersect;
                bool hit = scene.Intersect(shadowRay, shadowIntersect);

                if (!hit || shadowIntersect.t > ls->distance - 0.001f) {
                    glm::vec3 f = intersect.bsdf->f(wo, wi);
                    if (glm::length(f) > 0.0f) {
                        f32 bsdfPdf = intersect.bsdf->Pdf(wo, wi);
                        f32 weight = light->IsDelta() ? 1.0f : PowerHeuristic(lightPdf, bsdfPdf);

                        L += beta * f * ls->li * glm::abs(glm::dot(intersect.shading.n, wi)) * weight / lightPdf;
                    }
                }
            }
        }

        // BSDF Sampling
        auto bs = intersect.bsdf->SampleF(wo, sampler.Get2D());
        if (bs && bs->pdf > 0) {
            glm::vec3 f = bs->f * glm::abs(glm::dot(intersect.shading.n, bs->wi));
            if (glm::length(f) == 0.0f) return L;

            glm::vec3 nextBeta = beta * f / bs->pdf;

            f32 pSurvival = 1.0f;
            if (depth > 3) {
                f32 maxBeta = std::max({ nextBeta.r, nextBeta.g, nextBeta.b });
                f32 q = std::max(0.05f, 1.0f - maxBeta);

                if (sampler.Get1D() < q) return L;

                pSurvival = 1.0f - q;
                nextBeta /= pSurvival;
            }

            Ray nextRay = intersect.SpawnRay(bs->wi);
            bool isSpecular = (bs->pdf > 1e5f);

            L += Li(nextRay, scene, sampler, depth + 1, nextBeta, bs->pdf, isSpecular);
        }

        return L;
    }

}
