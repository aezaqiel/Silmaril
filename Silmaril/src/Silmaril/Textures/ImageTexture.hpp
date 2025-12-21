#pragma once

#include <glm/glm.hpp>

#include "Texture.hpp"

namespace Silmaril {

    class ImageTexture final : public Texture<glm::vec3>
    {
    public:
        ImageTexture(const std::string& filename, bool sRGB = true);
        virtual ~ImageTexture() = default;

        virtual glm::vec3 Evaluate(const SurfaceInteraction& interaction) const override;

    private:
        std::vector<u8> m_Data;
        i32 m_Width { 0 };
        i32 m_Height { 0 };
        i32 m_Channels { 0 };
    };

    class ScalarImageTexture final : public Texture<f32>
    {
    public:
        ScalarImageTexture(const std::string& filename);
        virtual ~ScalarImageTexture() = default;

        virtual f32 Evaluate(const SurfaceInteraction& interaction) const override;

    private:
        std::vector<u8> m_Data;
        i32 m_Width { 0 };
        i32 m_Height { 0 };
        i32 m_Channels { 0 };
    };

}
