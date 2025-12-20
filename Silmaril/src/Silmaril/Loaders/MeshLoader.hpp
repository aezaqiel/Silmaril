#pragma once

#include "Silmaril/Geometry/Triangle.hpp"

namespace Silmaril {

    class MeshLoader
    {
    public:
        static std::shared_ptr<Mesh> LoadOBJ(const std::string& filename);
    };

}
