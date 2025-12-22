#include "GeometricPrimitive.hpp"

#include "Shape.hpp"

#include "Silmaril/PBRT/Materials/Material.hpp"
#include "Silmaril/PBRT/Containers/AABB.hpp"

namespace Silmaril {

    GeometricPrimitive::GeometricPrimitive(const std::shared_ptr<Shape>& shape, const std::shared_ptr<Material>& material)
        : m_Shape(shape), m_Material(material)
    {
    }

    AABB GeometricPrimitive::GetBound() const
    {
        if (m_Shape) return m_Shape->GetBound();
        return AABB();
    }

    bool GeometricPrimitive::Intersect(const Ray& ray, SurfaceInteraction& intersect) const
    {
        if (!m_Shape) return false;

        f32 t;
        if (!m_Shape->Intersect(ray, t, intersect)) {
            return false;
        }

        intersect.primitive = this;
        return true;
    }

}
