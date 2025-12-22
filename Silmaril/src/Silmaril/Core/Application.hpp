#pragma once

#include "Events.hpp"
#include "Window.hpp"

#include "Silmaril/Renderer/Renderer.hpp"

#include "Silmaril/PBRT/PBRT.hpp"

namespace Silmaril {

    class Application
    {
    public:
        struct Config
        {
            u32 width;
            u32 height;
            std::string title;

            PBRT::Config pbrt;
            Renderer::Config renderer;
        };

    public:
        Application(const Config& config);
        ~Application();

        void Run();

    private:
        void DispatchEvents(const Event& event);

    private:
        Config m_Config;

        bool m_Running { true };
        bool m_Minimized { false };

        std::unique_ptr<Window> m_Window;
        std::unique_ptr<Renderer> m_Renderer;

        std::unique_ptr<PBRT> m_PBRT;
        std::jthread m_PBRTThread;

        std::deque<PBRTRenderEvent> m_RenderQueue;
        std::mutex m_RenderQueueMutex;
    };

}
