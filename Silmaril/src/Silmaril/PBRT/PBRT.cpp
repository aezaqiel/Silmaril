#include "PBRT.hpp"

#include <glm/gtx/string_cast.hpp>

#include "Silmaril/Core/Logger.hpp"

#include "Cameras/PerspectiveCamera.hpp"
#include "Samplers/StratifiedSampler.hpp"
#include "Integrators/RandomWalkIntegrator.hpp"

#include "Loaders/ModelLoader.hpp"
#include "Geometry/GeometricPrimitive.hpp"
#include "Geometry/Sphere.hpp"
#include "Geometry/BVH.hpp"
#include "Materials/MatteMaterial.hpp"
#include "Lights/DiffusedAreaLight.hpp"

namespace Silmaril {

    PBRT::PBRT(const Config& config)
        : m_Config(config)
    {
        LOG_INFO("Initializing Integrator...");

        m_Film = std::make_shared<Film>(m_Config.width, m_Config.height);

        m_Camera = std::make_shared<PerspectiveCamera>(
            m_Film,
            m_Config.lookfrom,
            m_Config.lookat,
            m_Config.up,
            m_Config.fov
        );

        m_Sampler = std::make_shared<StratifiedSampler>(m_Config.samples);

        m_Integrator = std::make_unique<RandomWalkIntegrator>(RandomWalkIntegrator::Config{
            m_Camera,
            m_Sampler,
            m_Config.depth,
            m_Config.tile,
            m_Config.output
        });

        LOG_INFO("Integrator Settings");
        LOG_INFO(" - Resolution: {}x{}", m_Config.width, m_Config.height);
        LOG_INFO(" - samples:    {}", m_Config.samples);
        LOG_INFO(" - depth:      {}", m_Config.depth);

        LOG_INFO("Camera Settings:");
        LOG_INFO(" - Position:   {}", glm::to_string(m_Config.lookfrom));
        LOG_INFO(" - LookAt:     {}", glm::to_string(m_Config.lookat));
        LOG_INFO(" - FOV:        {:.2f}", m_Config.fov);
    }

    PBRT::~PBRT()
    {
    }

    void PBRT::LoadScene()
    {
        LOG_INFO("Initializing Scene...");

        std::vector<std::shared_ptr<Primitive>> primitives;

        auto model = ModelLoader::LoadOBJ(m_Config.model);
        if (model) {
            auto modelPrimitives = model->CreatePrimitives();
            primitives.insert(primitives.end(), modelPrimitives.begin(), modelPrimitives.end());
        } else {
            LOG_WARN("No model loaded, falling back to sphere");

            auto sphere = std::make_shared<Sphere>(glm::vec3(0.0f, 1.0f, 0.0f), 1.0f);
            primitives.push_back(std::make_shared<GeometricPrimitive>(sphere, std::make_shared<MatteMaterial>(glm::vec3(0.75f))));
        }

        auto lightMat = std::make_shared<MatteMaterial>(glm::vec3(0.0f));

        auto sphere1 = std::make_shared<Sphere>(glm::vec3(0.0f, 800.0f, 0.0f), 1.0f);
        auto sphere2 = std::make_shared<Sphere>(glm::vec3(500.0f, 200.0f, 100.0f), 1.0f);
        auto sphere3 = std::make_shared<Sphere>(glm::vec3(-500.0f, 200.0f, -100.0f), 1.0f);

        auto light1 = m_Lights.emplace_back(std::make_shared<DiffuseAreaLight>(
            sphere1,
            glm::vec3(500000.0f)
        ));

        auto light2 = m_Lights.emplace_back(std::make_shared<DiffuseAreaLight>(
            sphere2,
            glm::vec3(100000.0f)
        ));

        auto light3 = m_Lights.emplace_back(std::make_shared<DiffuseAreaLight>(
            sphere3,
            glm::vec3(100000.0f)
        ));

        primitives.push_back(std::make_shared<GeometricPrimitive>(sphere1, lightMat, light1));
        primitives.push_back(std::make_shared<GeometricPrimitive>(sphere2, lightMat, light2));
        primitives.push_back(std::make_shared<GeometricPrimitive>(sphere3, lightMat, light3));

        LOG_INFO("Total Primitives: {}", primitives.size());
        LOG_INFO("Total Lights: {}", m_Lights.size());

        LOG_INFO("Building Aggregate BVH...");
        auto BVHStart = std::chrono::steady_clock::now();
        m_AggregatePrimitive = BVH::Create(std::move(primitives));
        auto BVHEnd = std::chrono::steady_clock::now();

        std::chrono::duration<f64> timeBVH = BVHEnd - BVHStart;
        LOG_INFO("BVH built in {:.4f} seconds", timeBVH.count());

        m_Scene = std::make_unique<Scene>(m_AggregatePrimitive, m_Lights);
    }

    void PBRT::Render()
    {
        if (m_Scene && m_Integrator) {
            auto renderStart = std::chrono::steady_clock::now();
            m_Integrator->Render(*m_Scene);
            auto renderEnd = std::chrono::steady_clock::now();

            std::chrono::duration<f64> timeRender = renderEnd - renderStart;
            LOG_INFO("Integrator Execution Time: {:.4f} seconds", timeRender.count());
        } else {
            LOG_ERROR("Integrator || Scene not set");
        }
    }

    void PBRT::SetTileRenderCallback(Integrator::OnRenderCallback callback)
    {
        m_Integrator->SetRenderCallback(callback);
    }

    void PBRT::RequestStop()
    {
        m_Integrator->Cancel();
    }

}
