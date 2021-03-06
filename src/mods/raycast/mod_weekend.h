#if !defined(RAYTRACING_IN_A_WEEKEND_H)

#include "min_crt.h"
#include "core/sparrow_platform.h"
#include "math/sparrow_math.h"
#include "console/console_platform.h"

#include "raycast_ray.h"
#include "raycast_diffuse.h"
#include "raycast_geometry.h"

struct collider
{
    sphere Sphere;
    // TODO: Other collision types in union + type enum?

    hit_record LastHit;

    collider* Prev;
    collider* Next;
};

struct world
{
    camera MainCamera;

    collider* ColliderSentinel;
    collider* FirstAvailableCollider;

    b32 IsInitialized;
};

inline collider*
SpawnCollider(memory* Memory, world* World)
{
    collider* Result = 0;
    if (World->FirstAvailableCollider) {
        Result                        = World->FirstAvailableCollider;
        World->FirstAvailableCollider = World->FirstAvailableCollider->Prev;
    }
    else {
        Result = GetStruct(Memory, collider);
    }
    // prev <-> sent ---> prev <-> coll <-> sent
    Result->Next = World->ColliderSentinel;
    Result->Prev = World->ColliderSentinel->Prev;

    World->ColliderSentinel->Prev = Result;
    Result->Prev->Next            = Result;

    return (Result);
}

inline void
RemoveCollider(world* World, collider* Collider)
{
    World->FirstAvailableCollider = Collider;
    Collider->Prev->Next          = Collider->Next;
    Collider->Next->Prev          = Collider->Prev;
    Collider->Prev                = World->FirstAvailableCollider;
}

#define RAYTRACING_IN_A_WEEKEND_H
#endif