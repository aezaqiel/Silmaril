#include "RandomWalkIntegrator.hpp"

#include "Silmaril/Cameras/Camera.hpp"
#include "Silmaril/Samplers/Sampler.hpp"
#include "Silmaril/Containers/Interaction.hpp"
#include "Silmaril/Scene/Scene.hpp"
#include "Silmaril/Materials/Material.hpp"
#include "Silmaril/Materials/BSDF.hpp"

namespace Silmaril {

    RandomWalkIntegrator::RandomWalkIntegrator(const std::shared_ptr<Camera>& camera, const std::shared_ptr<Sampler>& sampler, usize depth)
        : m_Camera(camera), m_Sampler(sampler), m_MaxDepth(depth)
    {
    }

    void RandomWalkIntegrator::Render(const Scene& scene)
    {
        u32 width = m_Camera->GetFilm().GetWidth();
        u32 height = m_Camera->GetFilm().GetHeight();

        std::vector<u32> yCoords(height);
        std::iota(yCoords.begin(), yCoords.end(), 0);

        std::for_each(std::execution::par, yCoords.begin(), yCoords.end(),
            [&](u32 y) {
                auto sampler = m_Sampler->Clone();

                for (u32 x = 0; x < width; ++x) {
                    sampler->StartPixel(x, y);
                    glm::vec3 accumulator(0.0f);

                    while (sampler->StartNextSample()) {
                        CameraSample cs;
                        glm::vec2 offset = sampler->Get2D();
                        cs.pFilm = glm::vec2(x, y) + offset;

                        Ray ray = m_Camera->GenerateRay(cs);

                        accumulator += Li(ray, scene, *sampler, 0);
                    }

                    accumulator /= static_cast<f32>(sampler->GetSPP());
                    m_Camera->GetFilm().SetPixel(x, y, accumulator);
                }
            }
        );

        m_Camera->GetFilm().Write("Output.png");
    }

    glm::vec3 RandomWalkIntegrator::Li(const Ray& ray, const Scene& scene, Sampler& sampler, usize depth)
    {
        if (depth >= m_MaxDepth) return glm::vec3(0.0f);

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
