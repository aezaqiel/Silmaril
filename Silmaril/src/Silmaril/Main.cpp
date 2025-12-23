#include "Core/Logger.hpp"
#include "Core/Application.hpp"

int main()
{
    Silmaril::Logger::Init();

    constexpr u32 IMAGE_WIDTH = 400;
    constexpr u32 IMAGE_HEIGHT = 300;
    constexpr u32 SAMPLES = 32;
    constexpr u32 DEPTH = 8;

    constexpr u32 TILE_SIZE = 16;

    Silmaril::Application* app = new Silmaril::Application({
        .window = {
            .width = IMAGE_WIDTH,
            .height = IMAGE_HEIGHT,
            .title = "Silmaril"
        },

        .pbrt = {
            .width = IMAGE_WIDTH,
            .height = IMAGE_HEIGHT,
            .samples = SAMPLES,
            .depth = DEPTH,
            .tile = TILE_SIZE,

            .model = "Assets/Sponza/sponza.obj",

            .lookfrom = { 1100.0f, 180.0f, 0.0f },
            .lookat = { 0.0f, 180.0f, 0.0f },
            .up = { 0.0f, 1.0f, 0.0f },
            .fov = 90.0f,

            .output = "Output.png"
        },

        .renderer = {
            .width = IMAGE_WIDTH,
            .height = IMAGE_HEIGHT,
            .tile = TILE_SIZE
        }
    });

    app->Run();
    delete app;

    Silmaril::Logger::Shutdown();
}
