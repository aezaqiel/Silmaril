#include "Application.hpp"

#include "Logger.hpp"

namespace Silmaril {

    Application::Application(const Config& config)
        : m_Config(config)
    {
        m_Window = std::make_unique<Window>(config.width, config.height, "Silmaril");
        m_Window->BindEventCallback(BIND_EVENT_FN(Application::DispatchEvents));

        m_Renderer = std::make_unique<Renderer>(config.renderer);

        m_PBRT = std::make_unique<PBRT>(config.pbrt);
        m_PBRT->SetTileRenderCallback([&](u32 x, u32 y, u32 w, u32 h) {
            std::scoped_lock<std::mutex> lock(m_RenderQueueMutex);
            m_RenderQueue.emplace_back(x, y, w, h);
        });
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

            {
                std::scoped_lock<std::mutex> lock(m_RenderQueueMutex);
                while (!m_RenderQueue.empty()) {
                    const auto& tile = m_RenderQueue.front();
                    const auto& pixels = m_PBRT->GetFilm()->GetPixels();

                    m_Renderer->Update(pixels, tile.x, tile.y, tile.w, tile.h);

                    m_RenderQueue.pop_front();
                }
            }

            if (!m_Minimized) {
                m_Renderer->Draw();

                m_Window->SwapBuffers();
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
