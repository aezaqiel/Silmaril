#pragma once

#include "Primitive.hpp"

namespace Silmaril {

    class BVH final : public Primitive
    {
    public:
        BVH(const std::shared_ptr<Primitive>& left, const std::shared_ptr<Primitive>& right);
        virtual ~BVH() = default;

        inline virtual AABB GetBound() const override { return m_Bound; }
        inline virtual const Material* GetMaterial() const override { return nullptr; }

        virtual bool Intersect(const Ray& ray, HitInteraction& hit) const override;
        virtual void FillSurfaceInteraction(const Ray& ray, const HitInteraction& hit, SurfaceInteraction& intersection) const override;

        static std::shared_ptr<Primitive> Create(std::vector<std::shared_ptr<Primitive>> primitives);

    private:
        std::shared_ptr<Primitive> m_Left;
        std::shared_ptr<Primitive> m_Right;
        AABB m_Bound;
    };

}
