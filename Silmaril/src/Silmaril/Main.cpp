#include <glm/glm.hpp>

#include "Cameras/Film.hpp"
#include "Cameras/PerspectiveCamera.hpp"
#include "Integrators/RandomWalkIntegrator.hpp"
#include "Scene/Scene.hpp"

#include "Geometry/GeometricPrimitive.hpp"
#include "Geometry/Sphere.hpp"
#include "Geometry/Triangle.hpp"
#include "Geometry/BVH.hpp"

#include "Materials/MatteMaterial.hpp"
#include "Lights/PointLight.hpp"
#include "Samplers/RandomSampler.hpp"

#include "Loaders/MeshLoader.hpp"

int main()
{
    using namespace Silmaril;

    u32 width = 400;
    u32 height = 300;
    u32 samples = 32;
    u32 depth = 8;

    std::vector<std::shared_ptr<Primitive>> primitives;
    std::vector<std::shared_ptr<Light>> lights;

    auto redMat = std::make_shared<MatteMaterial>(glm::vec3(0.8f, 0.2f, 0.2f));
    auto floorMat = std::make_shared<MatteMaterial>(glm::vec3(0.8f));

    auto mesh = MeshLoader::LoadOBJ("Assets/suzanne.obj");
    if (mesh) {
        auto triangles = Triangle::CreateTriangleMesh(mesh);
        for (const auto& triangle : triangles) {
            primitives.push_back(std::make_shared<GeometricPrimitive>(triangle, redMat));
        }
    } else {
        std::println(std::cerr, "Mesh not found, falling back to sphere");

        auto sphere = std::make_shared<Sphere>(glm::vec3(0.0f, 1.0f, 0.0f), 1.0f);
        primitives.push_back(std::make_shared<GeometricPrimitive>(sphere, redMat));
    }

    auto floorSphere = std::make_shared<Sphere>(glm::vec3(0.0f, -1010.0f, 0.0f), 1000.0f);
    primitives.push_back(std::make_shared<GeometricPrimitive>(floorSphere, floorMat));

    std::println("Building BVH");
    auto bvh = BVH::Create(primitives);

    lights.push_back(std::make_shared<PointLight>(glm::vec3(5.0f, 10.0f, 5.0f), glm::vec3(800.0f)));
    lights.push_back(std::make_shared<PointLight>(glm::vec3(-5.0f, 5.0f, -5.0f), glm::vec3(200.0f)));

    Scene scene(bvh, lights);

    auto film = std::make_shared<Film>(width, height);

    glm::vec3 lookfrom(0.0f, 1.0f, 5.0f);
    glm::vec3 lookat(0.0f, 0.0f, 0.0f);
    glm::vec3 up(0.0f, 1.0f, 0.0f);
    auto camera = std::make_shared<PerspectiveCamera>(film, lookfrom, lookat, up, 60.0f);

    auto sampler = std::make_shared<RandomSampler>(samples);

    std::println("Starting render");
    RandomWalkIntegrator integrator(camera, sampler, depth);
    integrator.Render(scene);
}
