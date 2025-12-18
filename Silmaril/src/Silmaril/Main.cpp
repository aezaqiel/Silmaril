#include "Core/Application.hpp"

int main()
{
    Silmaril::Application* app = new Silmaril::Application();
    app->Run();
    delete app;
}
