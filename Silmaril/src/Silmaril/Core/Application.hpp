#pragma once

#include "Silmaril/Cameras/Film.hpp"
#include "Silmaril/Cameras/Camera.hpp"
#include "Silmaril/Samplers/Sampler.hpp"
#include "Silmaril/Integrators/Integrator.hpp"
#include "Silmaril/Scene/Scene.hpp"

namespace Silmaril {

    class Application
    {
    public:
        struct Config
        {
            u32 width;
            u32 height;
            u32 samples;
            u32 depth;

            std::string model;

            glm::vec3 lookfrom;
            glm::vec3 lookat;
            glm::vec3 up;
            f32 fov;
        };

    public:
        Application(const Config& config);
        ~Application();

        void Run();

    private:
        void InitializeIntegrator();
        void InitializeScene();

    private:
        Config m_Config;

        std::shared_ptr<Film> m_Film;
        std::shared_ptr<Camera> m_Camera;
        std::shared_ptr<Sampler> m_Sampler;
        std::unique_ptr<Integrator> m_Integrator;

        std::vector<std::shared_ptr<Primitive>> m_Primitives;
        std::vector<std::shared_ptr<Light>> m_Lights;
        std::unique_ptr<Scene> m_Scene;
    };

}
