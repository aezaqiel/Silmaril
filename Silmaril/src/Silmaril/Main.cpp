#include "Core/Logger.hpp"
#include "Core/Application.hpp"

int main()
{
    Silmaril::Logger::Init();

    Silmaril::Application* app = new Silmaril::Application({
        .width = 400,
        .height = 300,
        .samples = 16,
        .depth = 4,

        .model = "Assets/Sponza/sponza.obj",

        .lookfrom = { 1100.0f, 180.0f, 0.0f },
        .lookat = { 0.0f, 180.0f, 0.0f },
        .up = { 0.0f, 1.0f, 0.0f },
        .fov = 90.0f
    });

    app->Run();
    delete app;

    Silmaril::Logger::Shutdown();
}
