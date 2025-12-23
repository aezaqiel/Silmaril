#pragma once

#include "Primitive.hpp"

namespace Silmaril {

    struct LinearBVHNode
    {
        AABB bounds;
        union {
            u32 primitivesOffset;
            u32 secondChildOffset;
        };
        u16 nPrimitives;
        u8 axis;
        u8 pad;
    };

    class BVH final : public Primitive
    {
    public:
        BVH(const std::shared_ptr<Primitive>& left, const std::shared_ptr<Primitive>& right);
        BVH(std::vector<std::shared_ptr<Primitive>>&& primitives, std::vector<LinearBVHNode>&& nodes);
        virtual ~BVH() = default;

        inline virtual AABB GetBound() const override { return m_Nodes.empty() ? AABB() : m_Nodes[0].bounds; }
        inline virtual const Material* GetMaterial() const override { return nullptr; }

        virtual bool Intersect(const Ray& ray, HitInteraction& hit) const override;
        virtual void FillSurfaceInteraction(const Ray& ray, const HitInteraction& hit, SurfaceInteraction& intersection) const override;

        static std::shared_ptr<Primitive> Create(std::vector<std::shared_ptr<Primitive>>&& primitives);

    private:
        std::vector<std::shared_ptr<Primitive>> m_Primitives;
        std::vector<LinearBVHNode> m_Nodes;
    };

}
