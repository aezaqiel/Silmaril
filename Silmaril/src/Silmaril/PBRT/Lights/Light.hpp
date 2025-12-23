#pragma once

#include <glm/glm.hpp>

#include "Silmaril/PBRT/Containers/Interaction.hpp"

namespace Silmaril {

    #define BIT(x) (1 << x)

    enum class LightFlags : u32
    {
        DeltaPosition   = BIT(0),
        DeltaDirection  = BIT(1),
        Area            = BIT(2),
        Infinite        = BIT(3)
    };

    inline bool IsDeltaLight(const LightFlags& flags)
    {
        return  (static_cast<u32>(flags) & static_cast<u32>(LightFlags::DeltaDirection)) ||
                (static_cast<u32>(flags) & static_cast<u32>(LightFlags::DeltaPosition));
    }

    struct LightSample
    {
        glm::vec3 li;
        glm::vec3 wi;
        f32 pdf;
        f32 distance;
    };

    class Light
    {
    public:
        Light(LightFlags flags)
            : m_Flags(flags)
        {
        }

        virtual ~Light() = default;

        virtual std::optional<LightSample> SampleLi(const Interaction& ref, const glm::vec2& u) const = 0;

        virtual f32 PdfLi(const Interaction& ref, const glm::vec3& wi) const
        {
            return 0.0f;
        }

        virtual f32 PdfLi(const Interaction& ref, const Interaction& lightHit) const
        {
            return PdfLi(ref, glm::normalize(lightHit.p - ref.p));
        }

        virtual glm::vec3 L(const Interaction& interaction, const glm::vec3& w) const
        {
            return glm::vec3(0.0f);
        }

        virtual bool IsDelta() const { return IsDeltaLight(m_Flags); }

    private:
        LightFlags m_Flags;
    };

}
