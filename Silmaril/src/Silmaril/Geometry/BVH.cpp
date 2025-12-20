#include "BVH.hpp"

namespace Silmaril {

    namespace {

        glm::vec3 GetCentroid(const AABB& bbox)
        {
            return glm::vec3(
                (bbox.x.min + bbox.x.max) * 0.5f,
                (bbox.y.min + bbox.y.max) * 0.5f,
                (bbox.z.min + bbox.z.max) * 0.5f
            );
        }
    
    }

    BVH::BVH(const std::shared_ptr<Primitive>& left, const std::shared_ptr<Primitive>& right)
        : m_Left(left), m_Right(right)
    {
        AABB lBox = m_Left ? m_Left->GetBound() : AABB();
        AABB rBox = m_Right ? m_Right->GetBound() : AABB();
        m_Bound = AABB(lBox, rBox);
    }

    bool BVH::Intersect(const Ray& ray, SurfaceInteraction& intersection) const
    {
        if (!m_Bound.Hit(ray, Bounds(0.0001f, std::numeric_limits<f32>::max()))) {
            return false;
        }

        bool lHit = false;
        SurfaceInteraction lIntersect;

        if (m_Left) {
            lHit = m_Left->Intersect(ray, lIntersect);
        }

        f32 rTMax = lHit ? lIntersect.t : std::numeric_limits<f32>::max();
        bool rHit = false;
        SurfaceInteraction rIntersect;

        if (m_Right) {
            if (m_Right->GetBound().Hit(ray, Bounds(0.0001f, rTMax))) {
                rHit = m_Right->Intersect(ray, rIntersect);
            }
        }

        if (lHit && rHit) {
            if (lIntersect.t < rIntersect.t) {
                intersection = lIntersect;
            } else {
                intersection = rIntersect;
            }

            return true;
        } else if (lHit) {
            intersection = lIntersect;
            return true;
        } else if (rHit) {
            intersection = rIntersect;
            return true;
        }

        return false;
    }

    std::shared_ptr<Primitive> BVH::Create(std::vector<std::shared_ptr<Primitive>> primitives)
    {
        if (primitives.empty()) return nullptr;
        if (primitives.size() == 1) return primitives[0];

        AABB centroidBounds;
        for (const auto& p : primitives) {
            glm::vec3 c = GetCentroid(p->GetBound());
            centroidBounds = AABB(centroidBounds, AABB(c, c));
        }

        i32 axis = 0;
        f32 maxExtent = centroidBounds.x.Size();
        if (centroidBounds.y.Size() > maxExtent) {
            axis = 1;
            maxExtent = centroidBounds.y.Size();
        }
        if (centroidBounds.z.Size() > maxExtent) {
            axis = 2;
        }

        usize mid = primitives.size() / 2;
        std::nth_element(primitives.begin(), primitives.begin() + mid, primitives.end(),
            [&](const auto& a, const auto& b) -> bool {
                glm::vec3 ca = GetCentroid(a->GetBound());
                glm::vec3 cb = GetCentroid(b->GetBound());
                return ca[axis] < cb[axis];
            }
        );

        std::vector<std::shared_ptr<Primitive>> lPrims(primitives.begin(), primitives.begin() + mid);
        std::vector<std::shared_ptr<Primitive>> rPrims(primitives.begin() + mid, primitives.end());

        return std::make_shared<BVH>(Create(lPrims), Create(rPrims));
    }

}
