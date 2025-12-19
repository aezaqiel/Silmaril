#include <glm/glm.hpp>

#include "Cameras/Film.hpp"
#include "Cameras/PerspectiveCamera.hpp"
#include "Integrators/RandomWalkIntegrator.hpp"
#include "Scene/Scene.hpp"
#include "Geometry/GeometricPrimitive.hpp"

#include "Geometry/Sphere.hpp"
#include "Materials/MatteMaterial.hpp"
#include "Lights/PointLight.hpp"
#include "Samplers/RandomSampler.hpp"

int main()
{
    using namespace Silmaril;

    u32 width = 1920;
    u32 height = 1080;
    u32 samples = 128;
    u32 depth = 8;

    auto film = std::make_shared<Film>(width, height);

    glm::vec3 lookfrom(0.0f, 0.0f, 3.0f);
    glm::vec3 lookat(0.0f, 0.0f, 0.0f);
    glm::vec3 up(0.0f, 1.0f, 0.0f);

    auto camera = std::make_shared<PerspectiveCamera>(film, lookfrom, lookat, up, 90.0f);

    std::vector<std::shared_ptr<Light>> lights;
    lights.push_back(std::make_shared<PointLight>(glm::vec3(10.0f), glm::vec3(500.0f)));

    auto sphere = std::make_shared<Sphere>(glm::vec3(0.0f, 0.0f, 0.0f), 1.0f);
    auto material = std::make_shared<MatteMaterial>(glm::vec3(0.8f, 0.2f, 0.8f));

    auto primitive = std::make_shared<GeometricPrimitive>(sphere, material);

    Scene scene(primitive, lights);

    auto sampler = std::make_shared<RandomSampler>(samples);

    RandomWalkIntegrator integrator(camera, sampler, depth);
    integrator.Render(scene);
}
