#include "ImageTexture.hpp"

#include <stb_image.h>

#include "Silmaril/Core/Logger.hpp"

#include <PathConfig.inl>

namespace Silmaril {

    namespace {

        std::filesystem::path s_ResPath(PathConfig::ResDir);

        glm::vec3 SampleBilinear(const std::vector<u8>& data, i32 width, i32 height, i32 channels, glm::vec2 uv)
        {
            if (data.empty()) return glm::vec3(1.0f, 0.0f, 1.0f);

            uv = uv - glm::floor(uv);
            uv.y = 1.0f - uv.y;

            f32 u = uv.x * width - 0.5f;
            f32 v = uv.y * height - 0.5f;

            i32 x0 = static_cast<i32>(glm::floor(u));
            i32 y0 = static_cast<i32>(glm::floor(v));
            i32 x1 = x0 + 1;
            i32 y1 = y0 + 1;

            f32 uRatio = u - x0;
            f32 vRatio = v - y0;
            f32 uOpp = 1.0f - uRatio;
            f32 vOpp = 1.0f - vRatio;

            auto GetPixel = [&](i32 x, i32 y) -> glm::vec3 {
                x = std::clamp(x, 0, width - 1);
                y = std::clamp(y, 0, height - 1);

                usize idx = (x + y * width) * channels;
                f32 r = data[idx + 0] / 255.0f;
                f32 g = (channels > 1) ? data[idx + 1] / 255.0f : r;
                f32 b = (channels > 2) ? data[idx + 2] / 255.0f : ((channels > 1) ? 0.0f : r);

                return glm::vec3(r, g, b);
            };

            return
                (GetPixel(x0, y0) * uOpp + GetPixel(x1, y0) * uRatio) * vOpp
                + (GetPixel(x0, y1) * uOpp + GetPixel(x1, y1) * uRatio) * vRatio;
        }
    
    }

    ImageTexture::ImageTexture(const std::string& filename, bool sRGB)
    {
        std::string path = (s_ResPath / filename).string();

        u8* data = stbi_load(path.c_str(), &m_Width, &m_Height, &m_Channels, 3);
        if (data) {
            m_Channels = 3;
            m_Data.resize(m_Width * m_Height * m_Channels);

            if (sRGB) {
                for (i32 i = 0; i < m_Width * m_Height * m_Channels; ++i) {
                    f32 val = data[i] / 255.0f;
                    val = glm::pow(val, 2.2f);
                    m_Data[i] = static_cast<u8>(val * 255.0f);
                }
            } else {
                std::memcpy(m_Data.data(), data, m_Data.size());
            }

            stbi_image_free(data);
            LOG_INFO("Loaded texture: {} ({}, {})", filename, m_Width, m_Height);
        } else {
            LOG_ERROR("Failed to load texture: {}", filename);

            m_Width = 1;
            m_Height = 1;
            m_Channels = 3;
            m_Data = { 255, 0, 255 };
        }
    }

    glm::vec3 ImageTexture::Evaluate(const SurfaceInteraction& interaction) const
    {
        return SampleBilinear(m_Data, m_Width, m_Height, m_Channels, interaction.uv);
    }

    ScalarImageTexture::ScalarImageTexture(const std::string& filename)
    {
        std::string path = (s_ResPath / filename).string();

        u8* data = stbi_load(path.c_str(), &m_Width, &m_Height, &m_Channels, 1);
        if (data) {
            m_Channels = 1;
            m_Data.resize(m_Width * m_Height);
            std::memcpy(m_Data.data(), data, m_Data.size());

            stbi_image_free(data);
            LOG_INFO("Loaded scalar texture: {}, ({}, {})", filename, m_Width, m_Height);
        } else {
            LOG_ERROR("Failed to load scalar texture: {}", filename);

            m_Width = 1;
            m_Height = 1;
            m_Channels = 1;
            m_Data = { 255 };
        }
    }

    f32 ScalarImageTexture::Evaluate(const SurfaceInteraction& interaction) const
    {
        glm::vec3 value = SampleBilinear(m_Data, m_Width, m_Height, m_Channels, interaction.uv);
        return value.r;
    }

}
