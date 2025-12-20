#include "MeshLoader.hpp"

#include <tiny_obj_loader.h>

#include "Silmaril/Geometry/Mesh.hpp"

#include <PathConfig.inl>

namespace Silmaril {

    namespace {

        std::filesystem::path s_ResPath(PathConfig::ResDir);
    
    }

    std::shared_ptr<Mesh> MeshLoader::LoadOBJ(const std::string& filename)
    {
        std::string path = (s_ResPath / filename).string();

        tinyobj::ObjReader reader;

        if (!reader.ParseFromFile(path)) {
            if (!reader.Error().empty()) {
                std::println(std::cerr, "TinyObjReader: {}", reader.Error());
            }
            return nullptr;
        }

        if (!reader.Warning().empty()) {
            std::println(std::clog, "TinyObjReader: {}", reader.Warning());
        }

        const auto& attrib = reader.GetAttrib();
        const auto& shapes = reader.GetShapes();

        // TODO: Materials

        std::vector<glm::vec3> p;
        std::vector<glm::vec3> n;
        std::vector<glm::vec2> uv;
        std::vector<u32> indices;

        p.reserve(attrib.vertices.size() / 3);
        n.reserve(attrib.normals.size() / 3);
        uv.reserve(attrib.texcoords.size() / 2);

        for (const auto& shape : shapes) {
            for (const auto& index : shape.mesh.indices) {
                p.push_back({
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]
                });

                if (index.normal_index >= 0) {
                    n.push_back({
                        attrib.normals[3 * index.normal_index + 0],
                        attrib.normals[3 * index.normal_index + 1],
                        attrib.normals[3 * index.normal_index + 2]
                    });
                }

                if (index.texcoord_index >= 0) {
                    uv.push_back({
                        attrib.texcoords[2 * index.texcoord_index + 0],
                        attrib.texcoords[2 * index.texcoord_index + 1]
                    });
                }

                indices.push_back(static_cast<u32>(indices.size()));
            }
        }

        if (n.size() != p.size()) n.clear();
        if (uv.size() != p.size()) uv.clear();

        std::println("Loaded mesh: {} ({} vertices, {} shapes)", filename, p.size(), shapes.size());

        return std::make_shared<Mesh>(p, indices, n, uv);
    }

}
