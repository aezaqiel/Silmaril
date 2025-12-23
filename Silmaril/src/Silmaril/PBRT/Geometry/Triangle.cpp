#include "Triangle.hpp"

#include "Mesh.hpp"

namespace Silmaril {

    Triangle::Triangle(const std::shared_ptr<Mesh>& mesh, u32 index)
        : m_Mesh(mesh), m_Index(index * 3)
    {
    }

    AABB Triangle::GetBound() const
    {
        const glm::vec3& p0 = m_Mesh->p[m_Mesh->indices[m_Index + 0]];
        const glm::vec3& p1 = m_Mesh->p[m_Mesh->indices[m_Index + 1]];
        const glm::vec3& p2 = m_Mesh->p[m_Mesh->indices[m_Index + 2]];

        AABB bbox(p0, p1);
        bbox = AABB(bbox, AABB(p2, p2));

        return bbox;
    }

    bool Triangle::Intersect(const Ray& ray, f32& tHit, f32 tMax) const
    {
        const u32 idx0 = m_Mesh->indices[m_Index + 0];
        const u32 idx1 = m_Mesh->indices[m_Index + 1];
        const u32 idx2 = m_Mesh->indices[m_Index + 2];

        const glm::vec3& p0 = m_Mesh->p[idx0];
        const glm::vec3& p1 = m_Mesh->p[idx1];
        const glm::vec3& p2 = m_Mesh->p[idx2];

        glm::vec3 e1 = p1 - p0;
        glm::vec3 e2 = p2 - p0;

        glm::vec3 s1 = glm::cross(ray.direction, e2);
        f32 divisor = glm::dot(s1, e1);

        if (divisor == 0.0f) return false;
        f32 invDivisor = 1.0f / divisor;

        glm::vec3 s = ray.origin - p0;
        f32 b1 = glm::dot(s, s1) * invDivisor;
        if (b1 < 0.0f || b1 > 1.0f) return false;

        glm::vec3 s2 = glm::cross(s, e1);
        f32 b2 = glm::dot(ray.direction, s2) * invDivisor;
        if (b2 < 0.0f || b1 + b2 > 1.0f) return false;

        f32 t = glm::dot(e2, s2) * invDivisor;

        if (t < 0.0f) return false;

        tHit = t;

        return true;
    }

    void Triangle::FillSurfaceInteraction(const Ray& ray, f32 tHit, SurfaceInteraction& intersection) const
    {
        const u32 idx0 = m_Mesh->indices[m_Index + 0];
        const u32 idx1 = m_Mesh->indices[m_Index + 1];
        const u32 idx2 = m_Mesh->indices[m_Index + 2];

        const glm::vec3& p0 = m_Mesh->p[idx0];
        const glm::vec3& p1 = m_Mesh->p[idx1];
        const glm::vec3& p2 = m_Mesh->p[idx2];

        glm::vec3 e1 = p1 - p0;
        glm::vec3 e2 = p2 - p0;

        glm::vec3 s1 = glm::cross(ray.direction, e2);

        f32 divisor = glm::dot(s1, e1);
        f32 invDivisor = 1.0f / divisor;

        glm::vec3 s = ray.origin - p0;
        f32 b1 = glm::dot(s, s1) * invDivisor;

        glm::vec3 s2 = glm::cross(s, e1);
        f32 b2 = glm::dot(ray.direction, s2) * invDivisor;

        f32 b0 = 1.0f - b1 - b2;

        glm::vec3 pHit = b0 * p0 + b1 * p1 + b2 * p2;

        glm::vec3 pAbsSum = glm::abs(b0 * p0) + glm::abs(b1 * p1) + glm::abs(b2 * p2);
        glm::vec3 pError = std::numeric_limits<f32>::epsilon() * 5.0f * pAbsSum;

        glm::vec2 uv0, uv1, uv2;
        if (!m_Mesh->uv.empty()) {
            uv0 = m_Mesh->uv[idx0];
            uv1 = m_Mesh->uv[idx1];
            uv2 = m_Mesh->uv[idx2];
        } else {
            uv0 = glm::vec2(0.0f, 0.0f);
            uv1 = glm::vec2(1.0f, 0.0f);
            uv2 = glm::vec2(0.0f, 1.0f);
        }

        glm::vec2 uvHit = b0 * uv0 + b1 * uv1 + b2 * uv2;

        glm::vec2 duv1 = uv1 - uv0;
        glm::vec2 duv2 = uv2 - uv0;
        glm::vec3 dp1 = p1 - p0;
        glm::vec3 dp2 = p2 - p0;

        f32 determinant = duv1.x * duv2.y - duv1.y * duv2.x;
        bool degenerateUV = std::abs(determinant) < std::numeric_limits<f32>::epsilon();

        glm::vec3 dpdu;
        glm::vec3 dpdv;

        if (!degenerateUV) {
            f32 invDet = 1.0f / determinant;
            dpdu = (duv2.y * dp1 - duv1.y * dp2) * invDet;
            dpdv = (-duv2.x * dp1 + duv1.x * dp2) * invDet;
        } else {
            glm::vec3 ng = glm::normalize(glm::cross(e1, e2));
            if (std::abs(ng.x) > std::abs(ng.z)) {
                dpdu = glm::vec3(-ng.y, ng.x, 0.0f);
            } else {
                dpdu = glm::vec3(0.0f, -ng.z, ng.y);
            }
            dpdu = glm::normalize(dpdu);
            dpdv = glm::cross(ng, dpdu);
        }

        glm::vec3 dndu(0.0f);
        glm::vec3 dndv(0.0f);

        intersection = SurfaceInteraction(pHit, pError, uvHit, -ray.direction, dpdu, dpdv, dndu, dndv, ray.time, this);
        intersection.t = tHit;

        if (!m_Mesh->n.empty()) {
            const glm::vec3& n0 = m_Mesh->n[idx0];
            const glm::vec3& n1 = m_Mesh->n[idx1];
            const glm::vec3& n2 = m_Mesh->n[idx2];

            glm::vec3 shadingNormal = glm::normalize(b0 * n0 + b1 * n1 + b2 * n2);

            glm::vec3 shadingDndu;
            glm::vec3 shadingDndv;

            if (!degenerateUV) {
                glm::vec3 dn1 = n1 - n0;
                glm::vec3 dn2 = n2 - n0;
                f32 invDet = 1.0f / determinant;

                shadingDndu = (duv2.y * dn1 - duv1.y * dn2) * invDet;
                shadingDndv = (-duv2.x * dn1 + duv1.x * dn2) * invDet;
            } else {
                shadingDndu = glm::vec3(0.0f);
                shadingDndv = glm::vec3(0.0f);
            }

            intersection.SetShadingGeometry(dpdu, dpdv, shadingDndu, shadingDndv, false);
            intersection.shading.n = shadingNormal;
        }
    }

    std::vector<std::shared_ptr<Shape>> Triangle::CreateTriangleMesh(const std::shared_ptr<Mesh>& mesh)
    {
        std::vector<std::shared_ptr<Shape>> triangles;
        triangles.reserve(mesh->indices.size() / 3);

        for (usize i = 0; i < mesh->indices.size() / 3; ++i) {
            triangles.push_back(std::make_shared<Triangle>(mesh, i));
        }

        return triangles;
    }

}
