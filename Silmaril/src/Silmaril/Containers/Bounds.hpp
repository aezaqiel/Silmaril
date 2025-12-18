#pragma once

namespace Silmaril {

struct Bounds
{
    f32 min;
    f32 max;

    Bounds()
        : min(std::numeric_limits<f32>::infinity()), max(-std::numeric_limits<f32>::infinity())
    {
    }

    Bounds(f32 min, f32 max)
        : min(min), max(max)
    {
    }

    Bounds(const Bounds& a, const Bounds& b)
    {
        min = a.min <= b.min ? a.min : b.min;
        max = a.max >= b.max ? a.max : b.max;
    }

    inline f32 Size() const
    {
        return max - min;
    }

    inline bool Contains(f32 x) const
    {
        return min <= x && x <= max;
    }

    inline bool Surrounds(f32 x) const
    {
        return min < x && x < max;
    }

    inline f32 Clamp(f32 x) const
    {
        return std::clamp(x, min, max);
    }

    Bounds Expand(f32 delta) const
    {
        f32 pad = delta / 2.0f;
        return Bounds(min - pad, max + pad);
    }
};

}
