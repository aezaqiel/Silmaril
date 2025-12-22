#pragma once

#include "Events.hpp"
#include "Window.hpp"

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

        std::unique_ptr<PBRT> m_PBRT;
        std::jthread m_PBRTThread;
    };

}
