#include "Film.hpp"

#include <stb_image_write.h>

#include "PathConfig.inl"

namespace Silmaril {

    namespace {

        constexpr glm::vec3 ACES(glm::vec3 x)
        {
                constexpr f32 a = 2.51f;
                constexpr f32 b = 0.03f;
                constexpr f32 c = 2.43f;
                constexpr f32 d = 0.59f;
                constexpr f32 e = 0.14f;

                return glm::clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0f, 1.0f);
        }

    }

    Film::Film(u32 width, u32 height)
        : m_Width(width), m_Height(height)
    {
        m_Pixels.resize(width * height, glm::vec3(0.0f));
    }

    void Film::SetPixel(u32 x, u32 y, const glm::vec3& color)
    {
        if (x >= m_Width || y >= m_Height) return;
        m_Pixels[x + y * m_Width] = color;
    }

    void Film::AddSample(u32 x, u32 y, const glm::vec3& L)
    {
        if (x >= m_Width || y >= m_Height) return;
        m_Pixels[x + y * m_Width] += L;
    }

    void Film::Write(const std::string& filename) const
    {
        std::vector<u8> image(m_Pixels.size() * 3);

        for (usize i = 0; i < m_Pixels.size(); ++i) {
                glm::vec3 color = ACES(m_Pixels[i]);

                color.r = glm::pow(color.r, 1.0f / 2.2f);
                color.g = glm::pow(color.g, 1.0f / 2.2f);
                color.b = glm::pow(color.b, 1.0f / 2.2f);

                usize index = i * 3;

                image[index + 0] = static_cast<u8>(std::clamp(color.r * 255.0f, 0.0f, 255.0f));
                image[index + 1] = static_cast<u8>(std::clamp(color.g * 255.0f, 0.0f, 255.0f));
                image[index + 2] = static_cast<u8>(std::clamp(color.b * 255.0f, 0.0f, 255.0f));
        }

        auto outDir = std::filesystem::path(PathConfig::OutputDir);
        if (!std::filesystem::exists(outDir)) {
            std::filesystem::create_directory(outDir);
        }

        i32 success = stbi_write_png(
            (outDir / filename).string().c_str(),
            static_cast<i32>(m_Width),
            static_cast<i32>(m_Height),
            static_cast<i32>(3),
            reinterpret_cast<const void*>(image.data()),
            static_cast<i32>(m_Width * 3)
        );

        if (success) {
                std::println("Image saved {}", filename);
        } else {
                std::println(std::cerr, "Failed to save image");
        }
    }

}
