#pragma once

#include "Cameras/Film.hpp"
#include "Cameras/Camera.hpp"
#include "Samplers/Sampler.hpp"
#include "Integrators/Integrator.hpp"
#include "Scene/Scene.hpp"

namespace Silmaril {

    class PBRT
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
        PBRT(const Config& config);
        ~PBRT();

        void Render();

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
