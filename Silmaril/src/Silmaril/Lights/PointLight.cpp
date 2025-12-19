#include "PointLight.hpp"

namespace Silmaril {

    PointLight::PointLight(const glm::vec3& p, const glm::vec3& intensity)
        : m_Pos(p), m_Intensity(intensity)
    {
    }

    std::optional<LightSample> PointLight::SampleLi(const Interaction& ref, const glm::vec2& u) const
    {
        LightSample ls;
        ls.wi = glm::normalize(m_Pos - ref.p);
        ls.distance = glm::distance(m_Pos, ref.p);
        ls.pdf = 1.0f;

        ls.li = m_Intensity / (ls.distance * ls.distance);

        return ls;
    }

}
