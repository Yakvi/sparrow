#if !defined(SPARROW_RANDOM_H)
EXTERN_C_START

struct rng_state
{
    u32 State[2];
};

// Initialize a GameRand generator with the specified seed.
// The generator is not valid until it's been seeded.
local void
InitRandomizer(struct rng_state* State, u32 Seed)
{
    // Seeding (murmur3_avalanche32)
    u32 Value = (Seed << 1U) | 1U;
    Value ^= Value >> 16;
    Value *= 0x85ebca6b;
    Value ^= Value >> 13;
    Value *= 0xc2b2ae35;
    Value ^= Value >> 16;

    // Initializing state
    State->State[0] = Value;
    State->State[1] = Value ^ 0x49616e42U;
}

// Returns a random number N in the range: 0 <= N <= 0xffffffff,
// from the specified GameRand generator.
local u32
NextRandom(struct rng_state* State)
{
    State->State[0] = (State->State[0] << 16) + (State->State[0] >> 16);
    State->State[0] += State->State[1];
    State->State[1] += State->State[0];
    return (State->State[0]);
}

// Returns a random float X in the range: 0.0f <= X < 1.0f,
// from the specified GameRand generator.
local f32
NextRandom01(struct rng_state* State)
{
    u32 Value      = NextRandom(State);
    u32 Exponent   = 127;
    u32 Mantissa   = Value >> 9;
    u32 Result_U32 = (Exponent << 23) | Mantissa;
    f32 Result     = *(f32*)(&Result_U32);
    return (Result - 1.0f);
}

// Returns a random integer N in the range:
// min <= N <= max, from the specified GameRand generator.
local s32
RandomRange(struct rng_state* State, s32 Min, s32 Max)
{
    s32 Result = Min;
    s32 Range  = (Max - Min) + 1;
    if (Range > 0) {
        s32 Value = (s32)(NextRandom01(State) * Range);
        Result += Value;
    }
    return Result;
}

/*
Based on GameRand functions from rnd.h v1.0 - Pseudo-random number generators for C/C++.

The original library included four different generators: PCG, WELL, GameRand and XorShift. They all have different 
characteristics, and you might want to use them for different things. GameRand is very fast, but does not give a great
distribution or period length. XorShift is the only one returning a 64-bit Value. WELL is an improvement of the often
used Mersenne Twister, and has quite a large internal State. PCG is small, fast and has a small State. For our purposes, 
extreme randomness is not essential, so we went for speed with GameRand.

Based on the random number generator by Ian C. Bullard:
http://www.redditmirror.cc/cache/websites/mjolnirstudios.com_7yjlc/mjolnirstudios.com/IanBullard/files/79ffbca75a75720f066d491e9ea935a0-10.html
GameRand is a random number generator based off an "Image of the Day" posted by Stephan Schaem. More information here:
http://www.flipcode.com/archives/07-15-2002.shtml

Adapted code by Mattias Gustavsson, 2016, Public Domain
Additional Contributors
  Jonatan Hedborg: unsigned s32 to normalized f32 conversion
*/

EXTERN_C_END
#define SPARROW_RANDOM_H
#endif
