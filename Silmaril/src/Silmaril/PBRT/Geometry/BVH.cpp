#include "BVH.hpp"

#include "Silmaril/Core/Logger.hpp"

namespace Silmaril {

    namespace {

        struct BVHBuildNode
        {
            AABB bounds;
            BVHBuildNode* children[2];
            u32 splitAxis;
            u32 firstPrimOffset;
            u32 nPrimitives;

            void InitLeaf(u32 first, u32 n, const AABB& b)
            {
                firstPrimOffset = first;
                nPrimitives = n;
                bounds = b;
                children[0] = nullptr;
                children[1] = nullptr;
            }

            void InitInterior(u32 axis, BVHBuildNode* c0, BVHBuildNode* c1)
            {
                children[0] = c0;
                children[1] = c1;
                bounds = AABB(c0->bounds, c1->bounds);
                splitAxis = axis;
                nPrimitives = 0;
            }
        };

        glm::vec3 GetCentroid(const AABB& bbox)
        {
            return glm::vec3(
                (bbox.x.min + bbox.x.max) * 0.5f,
                (bbox.y.min + bbox.y.max) * 0.5f,
                (bbox.z.min + bbox.z.max) * 0.5f
            );
        }

        BVHBuildNode* BuildBVHRecursive(
            std::vector<std::shared_ptr<Primitive>>& primitives,
            u32 start, u32 end,
            u32& totalNodes,
            u32& totalLeaves,
            u32 depth, u32& maxDepth
        )
        {
            maxDepth = std::max(maxDepth, depth);
            BVHBuildNode* node = new BVHBuildNode();
            totalNodes++;

            AABB centroidBounds;
            AABB bbox;
            for (u32 i = start; i < end; ++i) {
                AABB pb = primitives[i]->GetBound();
                glm::vec3 c = GetCentroid(pb);
                centroidBounds = AABB(centroidBounds, AABB(c, c));
                bbox = AABB(bbox, pb);
            }

            u32 nPrimitives = end - start;
            if (nPrimitives == 1) {
                node->InitLeaf(start, nPrimitives, bbox);
                totalLeaves++;
                return node;
            }

            AABB nodeBounds = bbox;

            i32 axis = 0;
            f32 maxExtent = centroidBounds.x.Size();
            if (centroidBounds.y.Size() > maxExtent) {
                axis = 1;
                maxExtent = centroidBounds.y.Size();
            }
            if (centroidBounds.z.Size() > maxExtent) {
                axis = 2;
            }

            u32 mid = (start + end) / 2;
            std::nth_element(primitives.begin() + start, primitives.begin() + mid, primitives.begin() + end,
                [axis](const std::shared_ptr<Primitive>& a, const std::shared_ptr<Primitive>& b) -> bool {
                    glm::vec3 ca = GetCentroid(a->GetBound());
                    glm::vec3 cb = GetCentroid(b->GetBound());
                    return ca[axis] < cb[axis];
                }
            );

            node->InitInterior(axis,
                BuildBVHRecursive(primitives, start, mid, totalNodes, totalLeaves, depth + 1, maxDepth),
                BuildBVHRecursive(primitives, mid, end, totalNodes, totalLeaves, depth + 1, maxDepth)
            );

            return node;
        }

        u32 FlattenBVHTree(BVHBuildNode* node, std::vector<LinearBVHNode>& nodes)
        {
            u32 offset = static_cast<u32>(nodes.size());
            nodes.push_back({});
            LinearBVHNode& linearNode = nodes[offset];

            linearNode.bounds = node->bounds;
            linearNode.nPrimitives = static_cast<u16>(node->nPrimitives);
            linearNode.pad = 0;

            if (node->nPrimitives > 0) {
                linearNode.primitivesOffset = node->firstPrimOffset;
                linearNode.axis = 0;
            } else {
                linearNode.axis = static_cast<u8>(node->splitAxis);
                FlattenBVHTree(node->children[0], nodes);
                linearNode.secondChildOffset = FlattenBVHTree(node->children[1], nodes);
            }

            return offset;
        }

        void DeleteBVHBuildNode(BVHBuildNode* node)
        {
            if (node) {
                if (node->children[0]) DeleteBVHBuildNode(node->children[0]);
                if (node->children[1]) DeleteBVHBuildNode(node->children[1]);
                delete node;
            }
        }
    
    }

    BVH::BVH(std::vector<std::shared_ptr<Primitive>>&& primitives, std::vector<LinearBVHNode>&& nodes)
        : m_Primitives(std::move(primitives)), m_Nodes(std::move(nodes))
    {
    }

    bool BVH::Intersect(const Ray& ray, HitInteraction& hit) const
    {
        if (m_Nodes.empty()) return false;

        bool hitAnything = false;

        glm::vec3 invDir = 1.0f / ray.direction;
        u32 dirIsNeg[3] = { invDir.x < 0, invDir.y < 0, invDir.z < 0 };

        u32 toVisitOffset = 0;
        u32 currentNodeIndex = 0;
        u32 nodesToVisit[64];

        while (true) {
            const LinearBVHNode& node = m_Nodes[currentNodeIndex];

            if (node.bounds.Hit(ray, Bounds(0.0001f, hit.t))) {
                if (node.nPrimitives > 0) {
                    for (u32 i = 0; i < node.nPrimitives; ++i) {
                        if (m_Primitives[node.primitivesOffset + i]->Intersect(ray, hit)) {
                            hitAnything = true;
                        }
                    }
                    if (toVisitOffset == 0) break;
                    currentNodeIndex = nodesToVisit[--toVisitOffset];
                } else {
                    if (dirIsNeg[node.axis]) {
                        nodesToVisit[toVisitOffset++] = currentNodeIndex + 1;
                        currentNodeIndex = node.secondChildOffset;
                    } else {
                        nodesToVisit[toVisitOffset++] = node.secondChildOffset;
                        currentNodeIndex = currentNodeIndex + 1;
                    }
                }
            } else {
                if (toVisitOffset == 0) break;
                currentNodeIndex = nodesToVisit[--toVisitOffset];
            }
        }

        return hitAnything;
    }

    void BVH::FillSurfaceInteraction(const Ray& ray, const HitInteraction& hit, SurfaceInteraction& intersection) const
    {
        LOG_ERROR("FillInteraction called on BVH node. This should not happen if HitRecord points to leaf primitives.");
    }

    std::shared_ptr<Primitive> BVH::Create(std::vector<std::shared_ptr<Primitive>>&& primitives)
    {
        if (primitives.empty()) return nullptr;

        u32 maxDepth = 0;
        u32 totalNodes = 0;
        u32 totalLeaves = 0;

        BVHBuildNode* root = BuildBVHRecursive(primitives, 0, static_cast<u32>(primitives.size()), totalNodes, totalLeaves, 0, maxDepth);

        std::vector<LinearBVHNode> linearNodes;
        linearNodes.reserve(totalNodes);
        FlattenBVHTree(root, linearNodes);

        DeleteBVHBuildNode(root);

        LOG_INFO("BVH Construction Metrics");
        LOG_INFO(" - Total Primitives: {}", primitives.size());
        LOG_INFO(" - Internal Nodes: {}", totalNodes);
        LOG_INFO(" - Leaf Nodes: {}", totalLeaves);
        LOG_INFO(" - Max Tree Depth: {}", maxDepth);

        return std::make_shared<BVH>(std::move(primitives), std::move(linearNodes));
    }

}
