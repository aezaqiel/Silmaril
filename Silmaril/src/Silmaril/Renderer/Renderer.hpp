#pragma once

#include <glm/glm.hpp>

namespace Silmaril {

    class Renderer
    {
    public:
        struct Config
        {
            u32 width;
            u32 height;
            u32 tile;
        };

    public:
        Renderer(const Config& config);
        ~Renderer();

        Renderer(const Renderer&) = delete;
        Renderer& operator=(const Renderer&) = delete;

        void Update(const std::vector<glm::vec3>& pixels, u32 x, u32 y, u32 w, u32 h);
        void Draw();
    
    private:
        struct SyncObj
        {
            void* fence;
            usize offset;
            usize size;
        };

    private:
        void CompileShaders();

    private:
        Config m_Config;

        u32 m_Texture { 0 };
        u32 m_VAO { 0 };
        u32 m_PBO { 0 };
        u32 m_ShaderProgram { 0 };

        void* m_PBOMappedPtr { nullptr };
        usize m_PBOOffset { 0 };
        usize m_PBOSize { 0 };

        std::queue<SyncObj> m_Fences;

        inline static constexpr usize s_MaxTilesInFlight { 128 };
        inline static constexpr usize s_BytesPerPixel { sizeof(glm::vec3) };
    };

}
