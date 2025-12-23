#include "Sphere.hpp"

#include <glm/gtc/constants.hpp>

namespace Silmaril {

    Sphere::Sphere(const glm::vec3& center, f32 radius)
        : m_Center(center), m_Radius(radius)
    {
    }

    AABB Sphere::GetBound() const
    {
        return AABB(m_Center - glm::vec3(m_Radius), m_Center + glm::vec3(m_Radius));
    }

    bool Sphere::Intersect(const Ray& ray, f32& tHit, f32 tMax) const
    {
        glm::vec3 oc = ray.origin - m_Center;

        f32 a = glm::dot(ray.direction, ray.direction);
        f32 b = 2.0f * glm::dot(oc, ray.direction);
        f32 c = glm::dot(oc, oc) - m_Radius * m_Radius;

        f32 discriminant = b * b - 4.0f * a * c;

        if (discriminant < 0.0f) return false;

        f32 sqrtd = glm::sqrt(discriminant);
        f32 root = (-b - sqrtd) / (2.0f * a);

        if (root <= std::numeric_limits<f32>::epsilon() || root > tMax) {
            root = (-b + sqrtd) / (2.0f * a);
            if (root <= std::numeric_limits<f32>::epsilon() || root > tMax) {
                return false;
            }
        }

        tHit = root;

        return true;
    }

    void Sphere::FillSurfaceInteraction(const Ray& ray, f32 tHit, SurfaceInteraction& intersection) const
    {
        glm::vec3 p = ray.At(tHit);
        glm::vec3 n = (p - m_Center) / m_Radius;

        f32 theta = glm::acos(std::clamp(-n.y, -1.0f, 1.0f));
        f32 phi = std::atan2(n.z, n.x) + glm::pi<f32>();

        glm::vec2 uv(phi / (2.0f * glm::pi<f32>()), theta / glm::pi<f32>());

        f32 zRadius = glm::sqrt(n.x * n.x + n.z * n.z);
        f32 invZRadius = (zRadius == 0.0f) ? 0.0f : 1.0f / zRadius;
        f32 cosPhi = n.x * invZRadius;
        f32 sinPhi = n.z * invZRadius;

        glm::vec3 dpdu = glm::vec3(-m_Radius * n.z, 0.0f, m_Radius * n.x) * 2.0f * glm::pi<f32>();
        glm::vec3 dpdv = glm::vec3(-n.y * cosPhi, zRadius, -n.y * sinPhi) * m_Radius * glm::pi<f32>();

        if (zRadius == 0.0f) {
            if (n.y > 0.0f) {
                dpdu = glm::vec3(m_Radius * 2.0f * glm::pi<f32>(), 0.0f, 0.0f);
                dpdv = glm::vec3(0.0f, 0.0f, m_Radius * glm::pi<f32>());
            } else {
                dpdu = glm::vec3(m_Radius * 2.0f * glm::pi<f32>(), 0.0f, 0.0f);
                dpdv = glm::vec3(0.0f, 0.0f, -m_Radius * glm::pi<f32>());
            }
        }

        glm::vec3 dndu(dpdu / m_Radius);
        glm::vec3 dndv(dpdv / m_Radius);

        glm::vec3 pError = glm::abs(p) * std::numeric_limits<f32>::max() * 5.0f;

        intersection = SurfaceInteraction(p, pError, uv, -ray.direction, dpdu, dpdv, dndu, dndv, ray.time, this);
        intersection.t = tHit;
    }

}
