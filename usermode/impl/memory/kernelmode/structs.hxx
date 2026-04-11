#pragma once

typedef struct _WNF_STATE_NAME {
    ULONG Data[2];
} WNF_STATE_NAME;

typedef struct _WNF_STATE_NAME* PWNF_STATE_NAME;
typedef const struct _WNF_STATE_NAME* PCWNF_STATE_NAME;

typedef enum {
    WnfDataScopeSystem = 0,
    WnfDataScopeSession = 1,
    WnfDataScopeUser = 2,
    WnfDataScopeProcess = 3,
    WnfDataScopeMachine = 4,
    WnfDataScopePhysicalMachine = 5
} _WNF_DATA_SCOPE;

typedef enum {
    WnfWellKnownStateName = 0,
    WnfPermanentStateName = 1,
    WnfPersistentStateName = 2,
    WnfTemporaryStateName = 3
} _WNF_STATE_NAME_LIFETIME;

typedef struct _WNF_TYPE_ID
{
    GUID TypeId;
} WNF_TYPE_ID, * PWNF_TYPE_ID;

typedef const WNF_TYPE_ID* PCWNF_TYPE_ID;

extern "C"
NTSTATUS
NTAPI
NtCreateWnfStateName(
    _Out_ PWNF_STATE_NAME StateName, // ULONG64, out
    _In_ _WNF_STATE_NAME_LIFETIME NameLifetime, // UINT
    _In_ _WNF_DATA_SCOPE DataScope, // UINT
    _In_ BOOLEAN PersistData, // BOOLEAN
    _In_opt_ PCWNF_TYPE_ID TypeId, // ULONG64
    _In_ ULONG MaximumStateSize, // ULONG
    _In_ PSECURITY_DESCRIPTOR SecurityDescriptor // PSECURITY_DESCRIPTOR
);

extern "C"
NTSTATUS
NTAPI
NtDeleteWnfStateName(
    _In_ PCWNF_STATE_NAME StateName
);

struct entity_determine_in
{
    entity_determine_in() = default;

    entity_determine_in(
        uintptr_t playerstate, uintptr_t pawn, uintptr_t bonearray, uintptr_t mesh,
        uintptr_t rootcomp, uintptr_t entitybounds, uintptr_t velocity, uintptr_t teamindex,
        uintptr_t isdying, uintptr_t isdowned, uintptr_t habenaro, uintptr_t ranked)
        : PlayerState(playerstate), Pawn(pawn), BoneArray(bonearray), Mesh(mesh), RootComponent(rootcomp),
        EntityBounds(entitybounds), Velocity(velocity), TeamIndex(teamindex), isDying(isdying),
        isDowned(isdowned), HabenaroComponent(habenaro), RankedProgress(ranked)
    {
    }
    uintptr_t PlayerState = 0;
    uintptr_t Pawn = 0;
    uintptr_t BoneArray = 0;
    uintptr_t Mesh = 0;
    uintptr_t RootComponent = 0;
    uintptr_t EntityBounds = 0;
    uintptr_t Velocity = 0;
    uintptr_t TeamIndex = 0;
    uintptr_t isDying = 0;
    uintptr_t isDowned = 0;
    uintptr_t HabenaroComponent = 0;
    uintptr_t RankedProgress = 0;
};



struct actor_determine_in
{
    actor_determine_in(
        uintptr_t actor, uintptr_t sim, uintptr_t pickup,
        uintptr_t search, uintptr_t pickupEntry, uintptr_t itemRarity,
        uintptr_t itemNamePtr, uintptr_t rc, uintptr_t rl)
        : AActor(actor), SimulatingTooLongLength(sim), PickupSpawnSource(pickup),
        SearchPtr(search), PickupEntry(pickupEntry), ItemRarity(itemRarity),
        ItemNamePtr(itemNamePtr), RootComponent(rc), RelativeLocation(rl)
    {
    }

    actor_determine_in() {};

    uintptr_t AActor = 0;
    uintptr_t SimulatingTooLongLength = 0;
    uintptr_t PickupSpawnSource = 0;
    uintptr_t SearchPtr = 0;
    uintptr_t PickupEntry = 0;
    uintptr_t ItemRarity = 0;
    uintptr_t ItemNamePtr = 0;
    uintptr_t RootComponent = 0;
    uintptr_t RelativeLocation = 0;
};

struct actor_packet_in
{
    uintptr_t Actor = 0;
    uintptr_t RootComponent = 0;
    uintptr_t RelativeLocation = 0;
};