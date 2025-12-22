#include <glm/glm.hpp>

#include "Cameras/Film.hpp"
#include "Cameras/PerspectiveCamera.hpp"
#include "Integrators/RandomWalkIntegrator.hpp"
#include "Scene/Scene.hpp"

#include "Geometry/GeometricPrimitive.hpp"
#include "Geometry/Sphere.hpp"
#include "Geometry/Model.hpp"
#include "Geometry/BVH.hpp"

#include "Materials/MatteMaterial.hpp"
#include "Lights/PointLight.hpp"
#include "Samplers/RandomSampler.hpp"

#include "Loaders/ModelLoader.hpp"

int main()
{
    using namespace Silmaril;

    u32 width = 400;
    u32 height = 300;
    u32 samples = 64;
    u32 depth = 8;

    std::println("Initializing Scene...");

    std::vector<std::shared_ptr<Primitive>> primitives;
    std::vector<std::shared_ptr<Light>> lights;

    auto model = ModelLoader::LoadOBJ("Assets/Sponza/sponza.obj");
    if (model) {
        auto modelPrimitives = model->CreatePrimitives();
        primitives.insert(primitives.end(), modelPrimitives.begin(), modelPrimitives.end());
    } else {
        std::println(std::cerr, "Model not loaded, falling back to sphere");

        auto sphere = std::make_shared<Sphere>(glm::vec3(0.0f, 1.0f, 0.0f), 1.0f);
        primitives.push_back(std::make_shared<GeometricPrimitive>(sphere, std::make_shared<MatteMaterial>(glm::vec3(0.75f))));
    }

    std::println("Total Primitives: {}", primitives.size());

    std::println("Building BVH...");
    auto startBVH = std::chrono::steady_clock::now();

    auto bvh = BVH::Create(primitives);

    auto endBVH = std::chrono::steady_clock::now();
    std::chrono::duration<double> timeBVH = endBVH - startBVH;
    std::println("BVH built in {:.4f} seconds", timeBVH.count());

    lights.push_back(std::make_shared<PointLight>(glm::vec3(0.0f, 800.0f, 0.0f), glm::vec3(500000.0f)));
    lights.push_back(std::make_shared<PointLight>(glm::vec3( 500.0f, 200.0f,  100.0f), glm::vec3(100000.0f)));
    lights.push_back(std::make_shared<PointLight>(glm::vec3(-500.0f, 200.0f, -100.0f), glm::vec3(100000.0f)));

    std::println("Total Lights: {}", lights.size());

    Scene scene(bvh, lights);

    auto film = std::make_shared<Film>(width, height);

    glm::vec3 lookfrom(1100.0f, 180.0f, 0.0f);
    glm::vec3 lookat(0.0f, 180.0f, 0.0f);
    glm::vec3 up(0.0f, 1.0f, 0.0f);
    f32 fov = 75.0f;

    auto camera = std::make_shared<PerspectiveCamera>(film, lookfrom, lookat, up, fov);

    std::println("Camera Settings:");
    std::println(" - Position: ({}, {}, {})", lookfrom.x, lookfrom.y, lookfrom.z);
    std::println(" - LookAt:   ({}, {}, {})", lookat.x, lookat.y, lookat.z);
    std::println(" - FOV:      {}", fov);

    auto sampler = std::make_shared<RandomSampler>(samples);

    std::println("Starting render");
    std::println(" - Resolution: {}x{}", width, height);
    std::println(" - samples:    {}", samples);
    std::println(" - depth:      {}", depth);

    RandomWalkIntegrator integrator(camera, sampler, depth);

    auto startRender = std::chrono::steady_clock::now();
    integrator.Render(scene);
    auto endRender = std::chrono::steady_clock::now();
    std::chrono::duration<double> timeRender = endRender - startRender;

    std::println("Total Execution Time (Render + Save): {:.4f} seconds", timeRender.count());
}
