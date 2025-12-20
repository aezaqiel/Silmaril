#pragma once

#include <glm/glm.hpp>

namespace Silmaril {

    struct Mesh
    {
        std::vector<glm::vec3> p;
        std::vector<glm::vec3> n;
        std::vector<glm::vec2> uv;
        std::vector<u32> indices;

        Mesh(
            const std::vector<glm::vec3>& positions,
            const std::vector<u32>& indices,
            const std::vector<glm::vec3>& normals = {},
            const std::vector<glm::vec2>& uvs = {}
        )
            : p(positions), n(normals), uv(uvs), indices(indices)
        {
        }
    };

}
