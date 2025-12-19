#pragma once

#include <glm/glm.hpp>

#include "Film.hpp"
#include "Silmaril/Containers/Ray.hpp"

namespace Silmaril {

    struct CameraSample
    {
        glm::vec2 pFilm;
    };

    class Camera
    {
    public:
        Camera(const std::shared_ptr<Film>& film);
        virtual ~Camera() = default;

        inline Film& GetFilm() const { return *m_Film; }

        virtual Ray GenerateRay(const CameraSample& s) const = 0;

    private:
        std::shared_ptr<Film> m_Film;
    };

}
