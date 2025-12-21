#pragma once

#include "Texture.hpp"

namespace Silmaril {

    template <typename T>
    class SolidTexture final : public Texture<T>
    {
    public:
        SolidTexture(const T& value)
            : m_Value(value)
        {
        }

        virtual ~SolidTexture() = default;

        virtual T Evaluate(const SurfaceInteraction& interaction) const override
        {
            return m_Value;
        }

    private:
        T m_Value;
    };

}
