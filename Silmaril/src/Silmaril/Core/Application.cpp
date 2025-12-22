#include "Application.hpp"

#include <glm/gtx/string_cast.hpp>

#include "Silmaril/Cameras/PerspectiveCamera.hpp"
#include "Silmaril/Samplers/RandomSampler.hpp"
#include "Silmaril/Integrators/RandomWalkIntegrator.hpp"

#include "Silmaril/Loaders/ModelLoader.hpp"
#include "Silmaril/Geometry/GeometricPrimitive.hpp"
#include "Silmaril/Geometry/Sphere.hpp"
#include "Silmaril/Geometry/BVH.hpp"
#include "Silmaril/Materials/MatteMaterial.hpp"
#include "Silmaril/Lights/PointLight.hpp"

namespace Silmaril {

    Application::Application(const Config& config)
        : m_Config(config)
    {
        InitializeIntegrator();

        std::println("Integrator Settings");
        std::println(" - Resolution: {}x{}", m_Config.width, m_Config.height);
        std::println(" - samples:    {}", m_Config.samples);
        std::println(" - depth:      {}", m_Config.depth);

        std::println("Camera Settings:");
        std::println(" - Position:   {}", glm::to_string(m_Config.lookfrom));
        std::println(" - LookAt:     {}", glm::to_string(m_Config.lookat));
        std::println(" - FOV:        {:.2f}", m_Config.fov);

        InitializeScene();
    }

    Application::~Application()
    {
    }

    void Application::Run()
    {
        auto renderStart = std::chrono::steady_clock::now();
        m_Integrator->Render(*m_Scene);
        auto renderEnd = std::chrono::steady_clock::now();

        std::chrono::duration<f64> timeRender = renderEnd - renderStart;
        std::println("Total Execution Time (Render + Save): {:.4f} seconds", timeRender.count());
    }

    void Application::InitializeIntegrator()
    {
        std::println("Initializing Integrator...");

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

    void Application::InitializeScene()
    {
        std::println("Initializing Scene...");

        std::vector<std::shared_ptr<Primitive>> primitives;
        std::vector<std::shared_ptr<Light>> lights;

        auto model = ModelLoader::LoadOBJ("Assets/Sponza/sponza.obj");
        if (model) {
            auto modelPrimitives = model->CreatePrimitives();
            primitives.insert(primitives.end(), modelPrimitives.begin(), modelPrimitives.end());
        } else {
            std::println(std::cerr, "Model not loaded, falling back to sphere");

            auto sphere = std::make_shared<Sphere>(glm::vec3(0.0f, 1.0f, 0.0f), 1.0f);
            primitives.push_back(std::make_shared<GeometricPrimitive>(sphere, std::make_shared<MatteMaterial>(glm::vec3(0.75f))));
        }

        std::println("Building aggregate BVH...");
        auto BVHStart = std::chrono::steady_clock::now();
        auto bvh = BVH::Create(primitives);
        auto BVHEnd = std::chrono::steady_clock::now();

        std::chrono::duration<f64> timeBVH = BVHEnd - BVHStart;
        std::println("BVH built in {:.4f} seconds", timeBVH.count());

        lights.push_back(std::make_shared<PointLight>(glm::vec3(0.0f, 800.0f, 0.0f), glm::vec3(500000.0f)));
        lights.push_back(std::make_shared<PointLight>(glm::vec3( 500.0f, 200.0f,  100.0f), glm::vec3(100000.0f)));
        lights.push_back(std::make_shared<PointLight>(glm::vec3(-500.0f, 200.0f, -100.0f), glm::vec3(100000.0f)));

        std::println("Total Primitives: {}", primitives.size());
        std::println("Total Lights: {}", lights.size());

        m_Scene = std::make_unique<Scene>(bvh, lights);
    }

}
