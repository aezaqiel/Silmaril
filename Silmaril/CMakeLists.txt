add_subdirectory(thirdparty/glm)
add_subdirectory(thirdparty/tinyobjloader)
add_subdirectory(thirdparty/spdlog)
add_subdirectory(thirdparty/glfw)
add_subdirectory(thirdparty/glad)

add_executable(Silmaril
    src/Silmaril/Main.cpp
    src/Silmaril/Types.hpp

    src/Silmaril/Core/Application.hpp
    src/Silmaril/Core/Application.cpp
    src/Silmaril/Core/Logger.hpp
    src/Silmaril/Core/Logger.cpp
    src/Silmaril/Core/KeyCodes.hpp
    src/Silmaril/Core/Events.hpp
    src/Silmaril/Core/Window.hpp
    src/Silmaril/Core/Window.cpp
    src/Silmaril/Core/JobSystem.hpp
    src/Silmaril/Core/JobSystem.cpp

    src/Silmaril/DSA/PCG32.hpp
    src/Silmaril/DSA/PCG32.cpp

    src/Silmaril/PBRT/Cameras/Camera.hpp
    src/Silmaril/PBRT/Cameras/Camera.cpp
    src/Silmaril/PBRT/Cameras/Film.hpp
    src/Silmaril/PBRT/Cameras/Film.cpp
    src/Silmaril/PBRT/Cameras/PerspectiveCamera.hpp
    src/Silmaril/PBRT/Cameras/PerspectiveCamera.cpp

    src/Silmaril/PBRT/Containers/Ray.hpp
    src/Silmaril/PBRT/Containers/Bounds.hpp
    src/Silmaril/PBRT/Containers/AABB.hpp
    src/Silmaril/PBRT/Containers/AABB.cpp
    src/Silmaril/PBRT/Containers/Interaction.hpp

    src/Silmaril/PBRT/Geometry/Shape.hpp
    src/Silmaril/PBRT/Geometry/Primitive.hpp
    src/Silmaril/PBRT/Geometry/GeometricPrimitive.hpp
    src/Silmaril/PBRT/Geometry/GeometricPrimitive.cpp
    src/Silmaril/PBRT/Geometry/Sphere.hpp
    src/Silmaril/PBRT/Geometry/Sphere.cpp
    src/Silmaril/PBRT/Geometry/Triangle.hpp
    src/Silmaril/PBRT/Geometry/Triangle.cpp
    src/Silmaril/PBRT/Geometry/Mesh.hpp
    src/Silmaril/PBRT/Geometry/Model.hpp
    src/Silmaril/PBRT/Geometry/Model.cpp
    src/Silmaril/PBRT/Geometry/BVH.hpp
    src/Silmaril/PBRT/Geometry/BVH.cpp

    src/Silmaril/PBRT/Integrators/Integrator.hpp
    src/Silmaril/PBRT/Integrators/Integrator.cpp
    src/Silmaril/PBRT/Integrators/RandomWalkIntegrator.hpp
    src/Silmaril/PBRT/Integrators/RandomWalkIntegrator.cpp

    src/Silmaril/PBRT/Lights/Light.hpp
    src/Silmaril/PBRT/Lights/PointLight.hpp
    src/Silmaril/PBRT/Lights/PointLight.cpp

    src/Silmaril/PBRT/Loaders/ModelLoader.hpp
    src/Silmaril/PBRT/Loaders/ModelLoader.cpp

    src/Silmaril/PBRT/Materials/BSDF.hpp
    src/Silmaril/PBRT/Materials/Material.hpp
    src/Silmaril/PBRT/Materials/MatteMaterial.hpp
    src/Silmaril/PBRT/Materials/MatteMaterial.cpp
    src/Silmaril/PBRT/Materials/PBRMaterial.hpp
    src/Silmaril/PBRT/Materials/PBRMaterial.cpp

    src/Silmaril/PBRT/Samplers/Sampler.hpp
    src/Silmaril/PBRT/Samplers/Sampler.cpp
    src/Silmaril/PBRT/Samplers/RandomSampler.hpp
    src/Silmaril/PBRT/Samplers/RandomSampler.cpp

    src/Silmaril/PBRT/Scene/Scene.hpp

    src/Silmaril/PBRT/Textures/Texture.hpp
    src/Silmaril/PBRT/Textures/SolidTexture.hpp
    src/Silmaril/PBRT/Textures/ImageTexture.hpp
    src/Silmaril/PBRT/Textures/ImageTexture.cpp

    src/Silmaril/PBRT/PBRT.hpp
    src/Silmaril/PBRT/PBRT.cpp

    src/Silmaril/Renderer/Renderer.hpp
    src/Silmaril/Renderer/Renderer.cpp

    src/Platform/STBImpl.cpp
)

target_include_directories(Silmaril
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/stb
    ${CMAKE_CURRENT_BINARY_DIR}/generated
)

target_link_libraries(Silmaril
PRIVATE
    glm
    tinyobjloader
    spdlog
    glfw
    glad
)

target_compile_definitions(Silmaril
PRIVATE
    NOMINMAX
    GLM_ENABLE_EXPERIMENTAL
    GLFW_INCLUDE_NONE
)

target_precompile_headers(Silmaril
PRIVATE
    src/Silmaril/PCH.hpp
)

if (WIN32)
    target_compile_definitions(Silmaril
    PRIVATE
        GLFW_EXPOSE_NATIVE_WIN32
    )
endif()

file(TO_CMAKE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/render OUTPUT_FOLDER)
file(TO_CMAKE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/res RES_FOLDER)
file(TO_CMAKE_PATH ${CMAKE_CURRENT_BINARY_DIR}/log LOG_FOLDER)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/PathConfig.inl
    ${CMAKE_CURRENT_BINARY_DIR}/generated/PathConfig.inl
    @ONLY
)
