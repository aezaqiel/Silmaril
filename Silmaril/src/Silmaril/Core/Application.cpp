#include "Application.hpp"

#include "Logger.hpp"

namespace Silmaril {

    Application::Application(const Config& config)
        : m_Config(config)
    {
        m_Window = std::make_unique<Window>(config.width, config.height, "Silmaril");
        m_Window->BindEventCallback(BIND_EVENT_FN(Application::DispatchEvents));

        m_PBRT = std::make_unique<PBRT>(m_Config.pbrt);
    }

    Application::~Application()
    {
    }

    void Application::Run()
    {
        m_PBRTThread = std::jthread([&] {
            LOG_INFO("Render thread spawned");
            m_PBRT->Render();
        });

        while (m_Running) {
            Window::PollEvents();

            if (!m_Minimized) {
            }
        }
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
