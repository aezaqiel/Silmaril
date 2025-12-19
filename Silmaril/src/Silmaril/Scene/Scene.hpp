#pragma once

#include "Silmaril/Geometry/Primitive.hpp"
#include "Silmaril/Lights/Light.hpp"
#include "Silmaril/Containers/Interaction.hpp"
#include "Silmaril/Containers/AABB.hpp"

namespace Silmaril {

    class Scene
    {
    public:
        Scene(std::shared_ptr<Primitive> aggregate, const std::vector<std::shared_ptr<Light>>& lights)
            : m_Aggregate(aggregate), m_Lights(lights)
        {
        }

        ~Scene() = default;

        inline const std::vector<std::shared_ptr<Light>>& GetLights() const { return m_Lights; }
        inline const AABB& GetBound() const { return m_GlobalBound; }

        inline bool Intersect(const Ray& ray, SurfaceInteraction& intersect) const
        {
            if (!m_Aggregate) return false;
            return m_Aggregate->Intersect(ray, intersect);
        }

        inline bool IntersectP(const Ray& ray) const
        {
            if (!m_Aggregate) return false;

            SurfaceInteraction scratch;
            return m_Aggregate->Intersect(ray, scratch);
        }

    private:
        std::shared_ptr<Primitive> m_Aggregate;
        std::vector<std::shared_ptr<Light>> m_Lights;

        AABB m_GlobalBound;
    };

}
