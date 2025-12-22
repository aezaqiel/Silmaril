#pragma once

namespace Silmaril {

    class Scene;

    class Integrator
    {
    public:
        using OnRenderCallback = std::function<void(u32, u32, u32, u32)>;

    public:
        Integrator(u32 tile);
        virtual ~Integrator() = default;

        virtual void Render(const Scene& scene) = 0;

        inline void SetRenderCallback(OnRenderCallback callback)
        {
            m_RenderCallback = callback;
        }

    protected:
        u32 m_TileSize { 0 };
        OnRenderCallback m_RenderCallback { nullptr };
    };

}
