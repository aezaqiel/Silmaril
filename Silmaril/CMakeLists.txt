add_subdirectory(thirdparty/glm)

add_executable(Silmaril
    src/Silmaril/Main.cpp
    src/Silmaril/Types.hpp

    src/Silmaril/Cameras/Camera.hpp
    src/Silmaril/Cameras/PerspectiveCamera.hpp

    src/Silmaril/Containers/Ray.hpp
    src/Silmaril/Containers/Bounds.hpp
    src/Silmaril/Containers/AABB.hpp
    src/Silmaril/Containers/AABB.cpp
    src/Silmaril/Containers/Interaction.hpp

    src/Silmaril/Core/Application.hpp
    src/Silmaril/Core/Application.cpp

    src/Silmaril/Geometry/Shape.hpp
    src/Silmaril/Geometry/Primitive.hpp

    src/Silmaril/Integrators/Integrator.hpp
    src/Silmaril/Integrators/RandomWalkIntegrator.hpp
    src/Silmaril/Integrators/RandomWalkIntegrator.cpp

    src/Silmaril/Lights/Light.hpp

    src/Silmaril/Materials/BSDF.hpp
    src/Silmaril/Materials/Material.hpp

    src/Silmaril/Samplers/Sampler.hpp

    src/Silmaril/Scene/Scene.hpp
)

target_include_directories(Silmaril
PRIVATE
    src
)

target_link_libraries(Silmaril
PRIVATE
    glm
)

target_compile_definitions(Silmaril
PRIVATE
    NOMINMAX
    GLM_ENABLE_EXPERIMENTAL
)

target_precompile_headers(Silmaril
PRIVATE
    src/Silmaril/PCH.hpp
)
