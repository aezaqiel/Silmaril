#pragma once

#include "Shape.hpp"
#include "Primitive.hpp"

namespace Silmaril {

    class GeometricPrimitive : public Primitive
    {
    public:
        GeometricPrimitive(const std::shared_ptr<Shape>& shape, const std::shared_ptr<Material>& material);
        ~GeometricPrimitive() = default;

        virtual AABB GetBound() const override;
        inline virtual const Material* GetMaterial() const override { return m_Material.get(); }

        virtual bool Intersect(const Ray& ray, HitInteraction& hit) const override;
        virtual void FillSurfaceInteraction(const Ray& ray, const HitInteraction& hit, SurfaceInteraction& intersection) const override;

    private:
        std::shared_ptr<Shape> m_Shape;
        std::shared_ptr<Material> m_Material;
    };

}
