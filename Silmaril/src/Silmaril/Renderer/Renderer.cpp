#include "Renderer.hpp"

#include <glad/gl.h>

#include <PathConfig.inl>

#include "Silmaril/Core/Logger.hpp"

namespace Silmaril {

    namespace {

        std::filesystem::path s_ResPath(PathConfig::ResDir);

        std::string ReadShader(const std::string& filename)
        {
            std::ifstream file(s_ResPath / filename);

            if (!file.is_open()) {
                LOG_ERROR("Failed to open shader: {}", filename);
                return "";
            }

            std::stringstream ss;
            ss << file.rdbuf();

            return ss.str();
        }
    
    }

    Renderer::Renderer(const Config& config)
        : m_Config(config)
    {
        glCreateVertexArrays(1, &m_VAO);

        glCreateTextures(GL_TEXTURE_2D, 1, &m_Texture);
        glTextureParameteri(m_Texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(m_Texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureStorage2D(m_Texture, 1, GL_RGB32F, config.width, config.height);

        usize maxTileSize = config.tile * config.tile * s_BytesPerPixel;
        m_PBOSize = maxTileSize * s_MaxTilesInFlight;

        glCreateBuffers(1, &m_PBO);
        GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
        glNamedBufferStorage(m_PBO, m_PBOSize, nullptr, flags);
        m_PBOMappedPtr = glMapNamedBufferRange(m_PBO, 0, m_PBOSize, flags);

        CompileShaders();
    }

    Renderer::~Renderer()
    {
        while (!m_Fences.empty()) {
            glDeleteSync(static_cast<GLsync>(m_Fences.front().fence));
            m_Fences.pop();
        }

        glDeleteProgram(m_ShaderProgram);
        glDeleteBuffers(1, &m_PBO);
        glDeleteTextures(1, &m_Texture);
        glDeleteVertexArrays(1, &m_VAO);
    }

    void Renderer::Update(const std::vector<glm::vec3>& pixels, u32 x, u32 y, u32 w, u32 h)
    {
        usize dataSize = w * h * s_BytesPerPixel;

        if (dataSize > m_PBOSize) {
            LOG_ERROR("Update size exceeds PBO size!");
            return; 
        }

        if (m_PBOOffset + dataSize > m_PBOSize) {
            m_PBOOffset = 0;
        }

        while (!m_Fences.empty()) {
            auto& sync = m_Fences.front();
            bool overlaps = (m_PBOOffset < sync.offset + sync.size) && (m_PBOOffset + dataSize > sync.offset);

            if (overlaps) {
                GLenum result = glClientWaitSync(static_cast<GLsync>(sync.fence), GL_SYNC_FLUSH_COMMANDS_BIT, std::numeric_limits<u64>::max());
                glDeleteSync(static_cast<GLsync>(sync.fence));
                m_Fences.pop();
            } else {
                GLenum result = glClientWaitSync(static_cast<GLsync>(sync.fence), 0, 0);
                if (result == GL_ALREADY_SIGNALED || result == GL_CONDITION_SATISFIED) {
                    glDeleteSync(static_cast<GLsync>(sync.fence));
                    m_Fences.pop();
                } else {
                    break;
                }
            }
        }

        u8* dst = static_cast<u8*>(m_PBOMappedPtr) + m_PBOOffset;
        for (u32 row = 0; row < h; ++row) {
            const glm::vec3& srcRow = pixels[(y + row) * m_Config.width + x];
            std::memcpy(dst + (row * w * s_BytesPerPixel), &srcRow, w * s_BytesPerPixel);
        }

        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_PBO);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

        glTextureSubImage2D(m_Texture, 0, x, y, w, h, GL_RGB, GL_FLOAT, reinterpret_cast<void*>(m_PBOOffset));

        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

        GLsync fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
        m_Fences.push({ fence, m_PBOOffset, dataSize });

        m_PBOOffset += dataSize;
    }

    void Renderer::Draw()
    {
        glUseProgram(m_ShaderProgram);
        glBindTextureUnit(0, m_Texture);
        glBindVertexArray(m_VAO);

        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    void Renderer::CompileShaders()
    {
        std::string vsSrc = ReadShader("Shaders/Viewport.vert.glsl");
        if (vsSrc.empty()) {
            LOG_ERROR("Vertex shader empty or not found!");
            return;
        }

        const char* vsStr = vsSrc.c_str();

        u32 vs = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vs, 1, &vsStr, nullptr);
        glCompileShader(vs);

        std::string fsSrc = ReadShader("Shaders/Viewport.frag.glsl");
        if (fsSrc.empty()) {
            LOG_ERROR("Fragment shader empty or not found!");
            return;
        }

        const char* fsStr = fsSrc.c_str();

        u32 fs = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fs, 1, &fsStr, nullptr);
        glCompileShader(fs);

        m_ShaderProgram = glCreateProgram();
        glAttachShader(m_ShaderProgram, vs);
        glAttachShader(m_ShaderProgram, fs);
        glLinkProgram(m_ShaderProgram);

        glDeleteShader(fs);
        glDeleteShader(vs);
    }

}
