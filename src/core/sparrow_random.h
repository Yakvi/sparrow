#if !defined(SPARROW_RANDOM_H)
EXTERN_C_START

// API
struct rng_state;
local void InitRandomizer(struct rng_state* State, u32 Seed);
local u32  RandomNext(struct rng_state* State);
local f32  RandomNext01(struct rng_state* State);
local s32  RandomRange(struct rng_state* State, s32 Min, s32 Max);

// Implementations
#define GameRand 1
#define PCG 2
#define GENERATOR PCG

#if GENERATOR == GameRand
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
RandomNext(struct rng_state* State)
{
    State->State[0] = (State->State[0] << 16) + (State->State[0] >> 16);
    State->State[0] += State->State[1];
    State->State[1] += State->State[0];
    return (State->State[0]);
}

// Returns a random float X in the range: 0.0f <= X < 1.0f,
// from the specified GameRand generator.
local f32
RandomNext01(struct rng_state* State)
{
    u32 Value      = RandomNext(State);
    u32 Exponent   = 127;
    u32 Mantissa   = Value >> 9;
    u32 Result_U32 = (Exponent << 23) | Mantissa;
    f32 Result     = *(f32*)(&Result_U32);
    return (Result - 1.0f);
}

local f32
RandomRangeF(struct rng_state* State, f32 Min, f32 Max)
{
    s32 Result = Min;
    s32 Range  = (Max - Min) + 1;
    if (Range > 0) {
        f32 Value = RandomNext01(State) * Range;
        Result    = Min + Value;
    }
    return (Result);
}

// Returns a random integer N in the range:
// min <= N <= max, from the specified GameRand generator.
local s32
RandomRange(struct rng_state* State, s32 Min, s32 Max)
{
    s32 Result = Min;
    s32 Range  = (Max - Min) + 1;
    if (Range > 0) {
        s32 Value = (s32)(RandomNext01(State) * Range);
        Result += Value;
    }
    return Result;
}
#elif GENERATOR == PCG
struct rng_state
{
    u64 State[2];
};

// murmur3 avalanche 64
local u64
RandomizeSeed(u64 Number)
{
    u64 Result = Number;
    Result ^= Result >> 33;
    Result *= 0xff51afd7ed558ccd;
    Result ^= Result >> 33;
    Result *= 0xc4ceb9fe1a85ec53;
    Result ^= Result >> 33;
    ;

    return (Result);
}

local void
InitRandomizer(struct rng_state* State, u32 Seed)
{
    u64 Value = (((u64)Seed) << 1ULL) | 1ULL;
    Value     = RandomizeSeed(Value);

    State->State[0] = 0U;
    State->State[1] = (Value << 1ULL) | 1ULL;

    RandomNext(State);
    State->State[0] += RandomizeSeed(Value);
    RandomNext(State);
}

local u32
RandomNext(struct rng_state* State)
{
    u64 OldState    = State->State[0];
    State->State[0] = OldState * 0x5851f42d4c957f2dULL + State->State[1];
    u32 XorShifted  = (u32)(((OldState >> 18ULL) ^ OldState) >> 27ULL);
    u32 Rotation    = (u32)(OldState >> 59ULL);

    u32 Result = (XorShifted >> Rotation) | (XorShifted << ((-(int)Rotation) & 31));
    return (Result);
}

local f32
RandomNext01(struct rng_state* State)
{
    u32 Value      = RandomNext(State);
    u32 Exponent   = 127;
    u32 Mantissa   = Value >> 9;
    u32 Result_U32 = (Exponent << 23) | Mantissa;
    f32 Result     = *(f32*)(&Result_U32);
    return (Result - 1.0f);
}

local f32
RandomRangeF(struct rng_state* State, f32 Min, f32 Max)
{
    Assert(Min < Max);
    f32 Result = Min;
    f32 Range  = Max - Min;
    f32 Value  = RandomNext01(State) * Range;
    Result     = Min + Value;

    Assert(Result >= Min);
    Assert(Result < Max);
    return (Result);
}

local s32
RandomRange(struct rng_state* State, s32 Min, s32 Max)
{
    s32 Result = Min;
    s32 Range  = (Max - Min) + 1;
    if (Range > 0) {
        s32 Value = (int)(RandomNext01(State) * Range);
        Result    = Min + Value;
    }
    return (Result);
}

#endif

// cleanup

#undef GENERATOR
#undef GameRand
#undef PCG

/*
Based on GameRand functions from rnd.h v1.0 - Pseudo-random number generators for C/C++.
https://github.com/mattiasgustavsson/libs/blob/main/docs/rnd.md

The library includes four different generators: PCG, WELL, GameRand and XorShift. They all have different 
characteristics, and you might want to use them for different things. GameRand is very fast, but does not give a great
distribution or period length. XorShift is the only one returning a 64-bit value. WELL is an improvement of the often
used Mersenne Twister, and has quite a large internal state. PCG is small, fast and has a small state. If you don't
have any specific reason, you may default to using PCG.

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
