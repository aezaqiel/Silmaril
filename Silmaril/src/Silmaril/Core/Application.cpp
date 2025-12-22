#include "Application.hpp"

#include <glm/gtx/string_cast.hpp>

#include "Logger.hpp"

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
        m_Window = std::make_unique<Window>(config.width, config.height, "Silmaril");
        m_Window->BindEventCallback(BIND_EVENT_FN(Application::DispatchEvents));

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

    Application::~Application()
    {
    }

    void Application::Run()
    {
        std::jthread render = std::jthread([&] {
            LOG_INFO("Render Thread Spawned");

            auto renderStart = std::chrono::steady_clock::now();
            m_Integrator->Render(*m_Scene);
            auto renderEnd = std::chrono::steady_clock::now();

            std::chrono::duration<f64> timeRender = renderEnd - renderStart;
            LOG_INFO("Total Execution Time (Render + Save): {:.4f} seconds", timeRender.count());
        });

        while (m_Running) {
            Window::PollEvents();

            if (!m_Minimized) {
            }
        }
    }

    void Application::InitializeIntegrator()
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

    void Application::InitializeScene()
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

    void Application::DispatchEvents(const Event& event)
    {
        EventDispatcher dispatcher(event);

        dispatcher.Dispatch<WindowClosedEvent>([&](const WindowClosedEvent& e) -> bool {
            m_Running = false;
            return true;
        });

        dispatcher.Dispatch<WindowMinimizeEvent>([&](const WindowMinimizeEvent& e) -> bool {
            m_Minimized = e.minimized;
            return false;
        });
    }

}
