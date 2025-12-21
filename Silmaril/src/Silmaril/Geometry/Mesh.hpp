#pragma once

#include <glm/glm.hpp>

namespace Silmaril {

    struct Mesh
    {
        std::vector<glm::vec3> p;
        std::vector<glm::vec3> n;
        std::vector<glm::vec2> uv;
        std::vector<u32> indices;
        std::vector<i32> materials;

        Mesh() = default;
        Mesh(
            const std::vector<glm::vec3>& positions,
            const std::vector<u32>& indices,
            const std::vector<glm::vec3>& normals = {},
            const std::vector<glm::vec2>& uvs = {},
            const std::vector<i32>& materials = {}
        )
            : p(positions), n(normals), uv(uvs), indices(indices), materials(materials)
        {
        }
    };

}
