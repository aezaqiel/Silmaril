#include "GeometricPrimitive.hpp"

#include "Shape.hpp"

#include "Silmaril/PBRT/Materials/Material.hpp"
#include "Silmaril/PBRT/Containers/AABB.hpp"

namespace Silmaril {

    GeometricPrimitive::GeometricPrimitive(const std::shared_ptr<Shape>& shape, const std::shared_ptr<Material>& material, const std::shared_ptr<Light>& light)
        : m_Shape(shape), m_Material(material), m_Light(light)
    {
    }

    AABB GeometricPrimitive::GetBound() const
    {
        if (m_Shape) return m_Shape->GetBound();
        return AABB();
    }

    bool GeometricPrimitive::Intersect(const Ray& ray, HitInteraction& hit) const
    {
        if (!m_Shape) return false;

        f32 t;
        if (!m_Shape->Intersect(ray, t, hit.t)) {
            return false;
        }

        hit.t = t;
        hit.primitive = this;

        return true;
    }

    void GeometricPrimitive::FillSurfaceInteraction(const Ray& ray, const HitInteraction& hit, SurfaceInteraction& intersection) const
    {
        if (m_Shape) {
            m_Shape->FillSurfaceInteraction(ray, hit.t, intersection);
            intersection.primitive = this;
        }
    }

}
