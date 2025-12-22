#pragma once

#include <glm/glm.hpp>

namespace Silmaril {

    class Film
    {
    public:
        Film(u32 width, u32 height);
        ~Film() = default;

        inline u32 GetWidth() const { return m_Width; }
        inline u32 GetHeight() const { return m_Height; }
        inline std::vector<glm::vec3> GetPixels() const { return m_Pixels; }

        void SetPixel(u32 x, u32 y, const glm::vec3& color);
        void AddSample(u32 x, u32 y, const glm::vec3& L);
        void AccumulateSample(u32 x, u32 y, const glm::vec3& L, u32 sample);

        void Write(const std::string& filename) const;

    private:
        u32 m_Width { 0 };
        u32 m_Height { 0 };

        std::vector<glm::vec3> m_Pixels;
        std::vector<glm::vec3> m_Accumulator;
    };

}
