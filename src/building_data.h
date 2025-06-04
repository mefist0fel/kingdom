#pragma once

#define MAX_RESOURCE_CHANGES 4
#define MAX_TRANSFORMS 4

// Resource types
typedef enum {
    RESOURCE_NONE = 0,
    RESOURCE_GOLD = 1,
    RESOURCE_GOLD_INCOME = 2,
    RESOURCE_FOOD = 3,
    RESOURCE_MANA = 4,
    RESOURCE_MANA_INCOME = 5,
    RESOURCE_TECH_CAVALRY = 6,
    RESOURCE_UNIT_LIMIT = 7,
    N_RESOURCES = 8
} ResourceType;

// Building types
typedef enum {
    BUILDING_NONE = 0,
    BUILDING_OUTPOST = 1,
    BUILDING_CASTLE = 2,
    BUILDING_CITADEL = 3,
    BUILDING_EMPTY = 4,
    BUILDING_FARM = 5,
    BUILDING_VILLAGE = 6,
    BUILDING_TEMPLE = 7,
    BUILDING_BARRACKS = 8,
    BUILDING_GOLD_VEIN = 9,
    BUILDING_MINE = 10,
    BUILDING_CRYSTAL_VEIN = 11,
    BUILDING_ALCHEMIST = 12,
    N_BUILDING_TYPES = 13
} BuildingType;

// ===== faction and slot =====
#define FACTION_NEUTRAL 0
#define FACTION_PLAYER 1
#define FACTION_ENEMY 2
#define FACTION_E_2 3
#define FACTION_E_3 4
#define FACTION_E_4 5

typedef struct {
    signed char id;
    int resources[N_RESOURCES];
} FactionData;

typedef struct {
    signed char region_id;
    signed char faction_id;
    signed char building_type; // BuildingType
    short timer;
    int x, y, w, h; // position for UI/hitbox
} BuildingSlot;

// ===== building core =====
typedef struct {
    signed char type;   // ResourceType
    char amount;
} ResourceChange;

typedef struct {
    signed char id; // BuildingType
    const char* name;
    ResourceChange cost[MAX_RESOURCE_CHANGES];
    ResourceChange static_change[MAX_RESOURCE_CHANGES];
    ResourceChange income[MAX_RESOURCE_CHANGES];
    signed char transforms[MAX_TRANSFORMS]; // Possible transforms (FSM)
    short build_time; // In ticks
} BuildingData;

extern const BuildingData building_data[N_BUILDING_TYPES];
