#pragma once

#include "Silmaril/Containers/AABB.hpp"

namespace Silmaril {

    class Primitive;
    class Light;
    
    struct SurfaceInteraction;

    class Scene
    {
    public:
        Scene(std::shared_ptr<Primitive> aggregate, const std::vector<std::shared_ptr<Light>>& lights);
        ~Scene() = default;

        inline const std::vector<std::shared_ptr<Light>>& GetLights() const { return m_Lights; }
        inline const AABB& GetBound() const { return m_GlobalBound; }

        bool Intersect(const Ray& ray, SurfaceInteraction& intersect) const;
        bool IntersectP(const Ray& ray) const;

    private:
        std::shared_ptr<Primitive> m_Aggregate;
        std::vector<std::shared_ptr<Light>> m_Lights;

        AABB m_GlobalBound;
    };

}
