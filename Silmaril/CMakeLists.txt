add_subdirectory(thirdparty/glm)
add_subdirectory(thirdparty/tinyobjloader)
add_subdirectory(thirdparty/spdlog)

add_executable(Silmaril
    src/Silmaril/Main.cpp
    src/Silmaril/Types.hpp

    src/Silmaril/Cameras/Camera.hpp
    src/Silmaril/Cameras/Camera.cpp
    src/Silmaril/Cameras/Film.hpp
    src/Silmaril/Cameras/Film.cpp
    src/Silmaril/Cameras/PerspectiveCamera.hpp
    src/Silmaril/Cameras/PerspectiveCamera.cpp

    src/Silmaril/Containers/Ray.hpp
    src/Silmaril/Containers/Bounds.hpp
    src/Silmaril/Containers/AABB.hpp
    src/Silmaril/Containers/AABB.cpp
    src/Silmaril/Containers/Interaction.hpp

    src/Silmaril/Core/Application.hpp
    src/Silmaril/Core/Application.cpp
    src/Silmaril/Core/Logger.hpp
    src/Silmaril/Core/Logger.cpp

    src/Silmaril/Geometry/Shape.hpp
    src/Silmaril/Geometry/Primitive.hpp
    src/Silmaril/Geometry/GeometricPrimitive.hpp
    src/Silmaril/Geometry/GeometricPrimitive.cpp
    src/Silmaril/Geometry/Sphere.hpp
    src/Silmaril/Geometry/Sphere.cpp
    src/Silmaril/Geometry/Triangle.hpp
    src/Silmaril/Geometry/Triangle.cpp
    src/Silmaril/Geometry/Mesh.hpp
    src/Silmaril/Geometry/Model.hpp
    src/Silmaril/Geometry/Model.cpp
    src/Silmaril/Geometry/BVH.hpp
    src/Silmaril/Geometry/BVH.cpp

    src/Silmaril/Integrators/Integrator.hpp
    src/Silmaril/Integrators/RandomWalkIntegrator.hpp
    src/Silmaril/Integrators/RandomWalkIntegrator.cpp

    src/Silmaril/Lights/Light.hpp
    src/Silmaril/Lights/PointLight.hpp
    src/Silmaril/Lights/PointLight.cpp

    src/Silmaril/Loaders/ModelLoader.hpp
    src/Silmaril/Loaders/ModelLoader.cpp

    src/Silmaril/Materials/BSDF.hpp
    src/Silmaril/Materials/Material.hpp
    src/Silmaril/Materials/MatteMaterial.hpp
    src/Silmaril/Materials/MatteMaterial.cpp
    src/Silmaril/Materials/PBRMaterial.hpp
    src/Silmaril/Materials/PBRMaterial.cpp

    src/Silmaril/Samplers/Sampler.hpp
    src/Silmaril/Samplers/Sampler.cpp
    src/Silmaril/Samplers/RandomSampler.hpp
    src/Silmaril/Samplers/RandomSampler.cpp

    src/Silmaril/Scene/Scene.hpp

    src/Silmaril/Textures/Texture.hpp
    src/Silmaril/Textures/SolidTexture.hpp
    src/Silmaril/Textures/ImageTexture.hpp
    src/Silmaril/Textures/ImageTexture.cpp

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

file(TO_CMAKE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/render OUTPUT_FOLDER)
file(TO_CMAKE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/res RES_FOLDER)
file(TO_CMAKE_PATH ${CMAKE_CURRENT_BINARY_DIR}/log LOG_FOLDER)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/PathConfig.inl
    ${CMAKE_CURRENT_BINARY_DIR}/generated/PathConfig.inl
    @ONLY
)
