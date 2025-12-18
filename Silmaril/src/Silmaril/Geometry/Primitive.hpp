#pragma once

namespace Silmaril {

    struct Ray;
    struct SurfaceInteraction;
    struct AABB;

    class Material;
    class Shape;

    class Primitive
    {
    public:
        virtual bool Intersect(const Ray& ray, SurfaceInteraction& intersect) const = 0;
        virtual AABB GlobalBound() const = 0;
        virtual const Material* GetMaterial() const = 0;
    };

    class GeometricPrimitive : public Primitive
    {
    public:
        GeometricPrimitive();
        ~GeometricPrimitive() = default;

        inline virtual const Material* GetMaterial() const override { return m_Material.get(); }

        virtual bool Intersect(const Ray& ray, SurfaceInteraction& intersect) const override;

    private:
        std::shared_ptr<Shape> m_Shape;
        std::shared_ptr<Material> m_Material;
    };

}
