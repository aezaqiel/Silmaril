#pragma once

#include "Primitive.hpp"
#include "Mesh.hpp"

#include "Silmaril/Materials/Material.hpp"

namespace Silmaril {

    struct Model
    {
        std::shared_ptr<Mesh> mesh;
        std::vector<std::shared_ptr<Material>> materials;

        std::vector<std::shared_ptr<Primitive>> CreatePrimitives();
    };

}
