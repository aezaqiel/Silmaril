#pragma once

#include "Silmaril/Geometry/Model.hpp"

namespace Silmaril {

    class ModelLoader
    {
    public:
        static std::shared_ptr<Model> LoadOBJ(const std::string& filename);
    };

}
