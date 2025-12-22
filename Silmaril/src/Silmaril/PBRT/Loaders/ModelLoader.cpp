#include "ModelLoader.hpp"

#include <tiny_obj_loader.h>

#include "Silmaril/PBRT/Geometry/Mesh.hpp"

#include "Silmaril/PBRT/Textures/Texture.hpp"
#include "Silmaril/PBRT/Textures/SolidTexture.hpp"
#include "Silmaril/PBRT/Textures/ImageTexture.hpp"

#include "Silmaril/PBRT/Materials/PBRMaterial.hpp"

#include "Silmaril/Core/Logger.hpp"

#include <PathConfig.inl>

namespace Silmaril {

    namespace {

        std::filesystem::path s_ResPath(PathConfig::ResDir);

        std::shared_ptr<Texture<glm::vec3>> LoadColorTexture(const std::string& baseDir, const std::string& texName, const glm::vec3& fallback)
        {
            if (!texName.empty()) {
                std::filesystem::path fullPath = std::filesystem::path(baseDir) / texName;
                return std::make_shared<ImageTexture>(fullPath.string(), true);
            }

            return std::make_shared<SolidTexture<glm::vec3>>(fallback);
        }

        std::shared_ptr<Texture<f32>> LoadScalarTexture(const std::string& baseDir, const std::string& texName, f32 fallback)
        {
            if (!texName.empty()) {
                std::filesystem::path fullPath = std::filesystem::path(baseDir) / texName;
                return std::make_shared<ScalarImageTexture>(fullPath.string());
            }

            return std::make_shared<SolidTexture<f32>>(fallback);
        }
    
    }

    std::shared_ptr<Model> ModelLoader::LoadOBJ(const std::string& filename)
    {
        std::string path = (s_ResPath / filename).string();
        std::string baseDir = std::filesystem::path(filename).parent_path().string();

        tinyobj::ObjReaderConfig config;
        config.mtl_search_path = (s_ResPath / baseDir).string();

        tinyobj::ObjReader reader;

        if (!reader.ParseFromFile(path, config)) {
            if (!reader.Error().empty()) {
                LOG_ERROR("TinyObjReader: {}", reader.Error());
            }
            return nullptr;
        }

        if (!reader.Warning().empty()) {
            LOG_WARN("TinyObjReader: {}", reader.Warning());
        }

        const auto& attrib = reader.GetAttrib();
        const auto& shapes = reader.GetShapes();
        const auto& materials = reader.GetMaterials();

        auto model = std::make_shared<Model>();
        model->materials.reserve(materials.size());

        for (const auto& material : materials) {
            glm::vec3 albedoValue = glm::vec3(material.diffuse[0], material.diffuse[1], material.diffuse[2]);
            auto albedoTex = LoadColorTexture(baseDir, material.diffuse_texname, albedoValue);

            f32 metallicValue = material.metallic;
            auto metallicTex = LoadScalarTexture(baseDir, material.metallic_texname, metallicValue);

            f32 roughnessValue = material.roughness;
            if (roughnessValue == 0.0f && material.shininess > 0.0f) {
                roughnessValue = glm::pow(2.0f / (material.shininess + 2.0f), 0.25f);
            }
            auto roughnessTex = LoadScalarTexture(baseDir, material.roughness_texname, roughnessValue);

            model->materials.push_back(std::make_shared<PBRMaterial>(albedoTex, metallicTex, roughnessTex));
        }

        auto mesh = std::make_shared<Mesh>();
        mesh->p.reserve(attrib.vertices.size() / 3);
        mesh->n.reserve(attrib.normals.size() / 3);
        mesh->uv.reserve(attrib.texcoords.size() / 2);

        for (const auto& shape : shapes) {
            usize offset = 0;

            for (usize f = 0; f < shape.mesh.num_face_vertices.size(); ++f) {
                u8 fv = shape.mesh.num_face_vertices[f];
                if (fv != 3) {
                    offset += fv;
                    continue;
                }

                mesh->materials.push_back(shape.mesh.material_ids[f]);

                for (usize v = 0; v < 3; ++v) {
                    tinyobj::index_t idx = shape.mesh.indices[offset + v];

                    mesh->p.push_back({
                        attrib.vertices[3 * idx.vertex_index + 0],
                        attrib.vertices[3 * idx.vertex_index + 1],
                        attrib.vertices[3 * idx.vertex_index + 2]
                    });

                    if (idx.normal_index >= 0) {
                        mesh->n.push_back({
                            attrib.normals[3 * idx.normal_index + 0],
                            attrib.normals[3 * idx.normal_index + 1],
                            attrib.normals[3 * idx.normal_index + 2]
                        });
                    }

                    if (idx.texcoord_index >= 0) {
                        mesh->uv.push_back({
                            attrib.texcoords[2 * idx.texcoord_index + 0],
                            attrib.texcoords[2 * idx.texcoord_index + 1]
                        });
                    }

                    mesh->indices.push_back(static_cast<u32>(mesh->indices.size()));
                }

                offset += 3;
            }
        }

        if (mesh->n.size() != mesh->p.size()) mesh->n.clear();
        if (mesh->uv.size() != mesh->p.size()) mesh->uv.clear();

        model->mesh = std::move(mesh);

        LOG_INFO("Loaded model: {} ({} vertices, {} shapes, {} materials)", filename, model->mesh->p.size(), shapes.size(), materials.size());

        return model;
    }

}
