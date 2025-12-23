#pragma once

namespace Silmaril {

    struct PCG32
    {
        u64 state;
        u64 inc;

        void Seed(u64 initstate, u64 initseq);
        u32 Next();

        f32 Float32();

        PCG32() = default;

        PCG32(const PCG32& other)
            : state(other.state), inc(other.inc)
        {
        }

        PCG32& operator=(const PCG32& other)
        {
            state = other.state;
            inc = other.inc;
            return *this;
        }

        PCG32(PCG32&& other)
            : state(other.state), inc(other.inc)
        {
        }

        PCG32& operator=(PCG32&& other)
        {
            state = other.state;
            inc = other.inc;
            return *this;
        }
    };

}
