#include "PBRT.hpp"

#include <glm/gtx/string_cast.hpp>

#include "Silmaril/Core/Logger.hpp"

#include "Silmaril/PBRT/Cameras/PerspectiveCamera.hpp"
#include "Silmaril/PBRT/Samplers/RandomSampler.hpp"
#include "Silmaril/PBRT/Integrators/RandomWalkIntegrator.hpp"

#include "Silmaril/PBRT/Loaders/ModelLoader.hpp"
#include "Silmaril/PBRT/Geometry/GeometricPrimitive.hpp"
#include "Silmaril/PBRT/Geometry/Sphere.hpp"
#include "Silmaril/PBRT/Geometry/BVH.hpp"
#include "Silmaril/PBRT/Materials/MatteMaterial.hpp"
#include "Silmaril/PBRT/Lights/PointLight.hpp"

namespace Silmaril {

    PBRT::PBRT(const Config& config)
        : m_Config(config)
    {
        InitializeIntegrator();

        LOG_INFO("Integrator Settings");
        LOG_INFO(" - Resolution: {}x{}", m_Config.width, m_Config.height);
        LOG_INFO(" - samples:    {}", m_Config.samples);
        LOG_INFO(" - depth:      {}", m_Config.depth);

        LOG_INFO("Camera Settings:");
        LOG_INFO(" - Position:   {}", glm::to_string(m_Config.lookfrom));
        LOG_INFO(" - LookAt:     {}", glm::to_string(m_Config.lookat));
        LOG_INFO(" - FOV:        {:.2f}", m_Config.fov);

        InitializeScene();
    }

    PBRT::~PBRT()
    {
    }

    void PBRT::Render()
    {
        if (m_Scene && m_Integrator) {
            auto renderStart = std::chrono::steady_clock::now();
            m_Integrator->Render(*m_Scene);
            auto renderEnd = std::chrono::steady_clock::now();

            std::chrono::duration<f64> timeRender = renderEnd - renderStart;
            LOG_INFO("Total Execution Time (Render + Save): {:.4f} seconds", timeRender.count());
        } else {
            LOG_ERROR("Integrator || Scene not set");
        }
    }

    void PBRT::InitializeIntegrator()
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

        m_Sampler = std::make_shared<RandomSampler>(m_Config.samples);

        m_Integrator = std::make_unique<RandomWalkIntegrator>(m_Camera, m_Sampler, m_Config.depth);
    }

    void PBRT::InitializeScene()
    {
        LOG_INFO("Initializing Scene...");

        std::vector<std::shared_ptr<Primitive>> primitives;
        std::vector<std::shared_ptr<Light>> lights;

        auto model = ModelLoader::LoadOBJ(m_Config.model);
        if (model) {
            auto modelPrimitives = model->CreatePrimitives();
            primitives.insert(primitives.end(), modelPrimitives.begin(), modelPrimitives.end());
        } else {
            LOG_WARN("No model loaded, falling back to sphere");

            auto sphere = std::make_shared<Sphere>(glm::vec3(0.0f, 1.0f, 0.0f), 1.0f);
            primitives.push_back(std::make_shared<GeometricPrimitive>(sphere, std::make_shared<MatteMaterial>(glm::vec3(0.75f))));
        }

        LOG_INFO("Building Aggregate BVH...");
        auto BVHStart = std::chrono::steady_clock::now();
        auto bvh = BVH::Create(primitives);
        auto BVHEnd = std::chrono::steady_clock::now();

        std::chrono::duration<f64> timeBVH = BVHEnd - BVHStart;
        LOG_INFO("BVH built in {:.4f} seconds", timeBVH.count());

        lights.push_back(std::make_shared<PointLight>(glm::vec3(0.0f, 800.0f, 0.0f), glm::vec3(500000.0f)));
        lights.push_back(std::make_shared<PointLight>(glm::vec3( 500.0f, 200.0f,  100.0f), glm::vec3(100000.0f)));
        lights.push_back(std::make_shared<PointLight>(glm::vec3(-500.0f, 200.0f, -100.0f), glm::vec3(100000.0f)));

        LOG_INFO("Total Primitives: {}", primitives.size());
        LOG_INFO("Total Lights: {}", lights.size());

        m_Scene = std::make_unique<Scene>(bvh, lights);
    }

}
