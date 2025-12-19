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

    bool Sphere::Intersect(const Ray& ray, f32& distance, SurfaceInteraction& intersect) const
    {
        glm::vec3 oc = ray.origin - m_Center;

        f32 a = glm::dot(ray.direction, ray.direction);
        f32 b = 2.0f * glm::dot(oc, ray.direction);
        f32 c = glm::dot(oc, oc) - m_Radius * m_Radius;

        f32 discriminant = b * b - 4.0f * a * c;

        if (discriminant < 0.0f) return false;

        f32 sqrtd = glm::sqrt(discriminant);
        f32 root = (-b - sqrtd) / (2.0f * a);

        if (root <= std::numeric_limits<f32>::epsilon()) {
            root = (-b + sqrtd) / (2.0f * a);
            if (root <= std::numeric_limits<f32>::epsilon()) {
                return false;
            }
        }

        distance = root;
        intersect.t = root;
        intersect.p = ray.At(root);
        intersect.n = (intersect.p - m_Center) / m_Radius;
        intersect.wo = -ray.direction;

        f32 theta = glm::acos(-intersect.n.y);
        f32 phi = glm::atan(intersect.n.z, intersect.n.x) + glm::pi<f32>();
        intersect.uv = glm::vec2(phi / (2.0f * glm::pi<f32>()), theta / glm::pi<f32>());

        return true;
    }

}
