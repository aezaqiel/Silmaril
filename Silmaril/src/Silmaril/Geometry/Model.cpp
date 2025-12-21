#include "Model.hpp"

#include "Triangle.hpp"
#include "GeometricPrimitive.hpp"

#include "Silmaril/Materials/MatteMaterial.hpp"

namespace Silmaril {

    std::vector<std::shared_ptr<Primitive>> Model::CreatePrimitives()
    {
        if (!mesh) return {};

        usize triangles = mesh->indices.size() / 3;

        std::vector<std::shared_ptr<Primitive>> primitives;
        primitives.reserve(triangles);

        auto defaultMaterial = std::make_shared<MatteMaterial>(glm::vec3(0.75f));

        for (u32 i = 0; i < triangles; ++i) {
            auto triangle = std::make_shared<Triangle>(mesh, i);
            std::shared_ptr<Material> material = defaultMaterial;

            if (!mesh->materials.empty()) {
                i32 matIdx = mesh->materials[i];
                if (matIdx >= 0 && matIdx < static_cast<i32>(materials.size())) {
                    material = materials[matIdx];
                }
            } else if (!materials.empty()) {
                material = materials[0];
            }

            primitives.push_back(std::make_shared<GeometricPrimitive>(triangle, material));
        }

        return primitives;
    }

}
