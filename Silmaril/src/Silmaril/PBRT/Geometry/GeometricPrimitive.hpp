#pragma once

#include "Shape.hpp"
#include "Primitive.hpp"

namespace Silmaril {

    class GeometricPrimitive : public Primitive
    {
    public:
        GeometricPrimitive(const std::shared_ptr<Shape>& shape, const std::shared_ptr<Material>& material);
        ~GeometricPrimitive() = default;

        inline virtual const Material* GetMaterial() const override { return m_Material.get(); }

        virtual AABB GetBound() const override;
        virtual bool Intersect(const Ray& ray, SurfaceInteraction& intersect) const override;

    private:
        std::shared_ptr<Shape> m_Shape;
        std::shared_ptr<Material> m_Material;
    };

}
