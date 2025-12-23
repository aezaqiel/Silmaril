#include "PCG32.hpp"

namespace Silmaril {

    void PCG32::Seed(u64 initstate, u64 initseq)
    {
        state = 0u;
        inc = (initseq << 1u) | 1u;
        Next();
        state += initstate;
        Next();
    }

    u32 PCG32::Next()
    {
        u64 oldstate = state;
        state = oldstate * 6364136223846793005ULL + inc;
        u32 xorshifted = static_cast<u32>(((oldstate >> 18u) ^ oldstate) >> 27u);
        u32 rot = static_cast<u32>(oldstate >> 59u);
        return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
    }

    f32 PCG32::Float32()
    {
        return (Next() >> 8) * 0x1p-24f;
    }

}
