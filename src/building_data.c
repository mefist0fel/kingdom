#include "building_data.h"

const BuildingData building_data[N_BUILDING_TYPES] = {
    [BUILDING_OUTPOST] = {
        .id = BUILDING_OUTPOST,
        .name = "Outpost",
        .cost = {},
        .static_change = {
            {RESOURCE_FOOD, 1},
            {RESOURCE_GOLD_INCOME, 1}
        },
        .income = {},
        .transforms = {BUILDING_CASTLE},
        .build_time = 0
    },
    [BUILDING_CASTLE] = {
        .id = BUILDING_CASTLE,
        .name = "Castle G 10",
        .cost = {{RESOURCE_GOLD, 10}},
        .static_change = {
            {RESOURCE_FOOD, 1},
            {RESOURCE_GOLD_INCOME, 2}
        },
        .income = {},
        .transforms = {BUILDING_CITADEL},
        .build_time = 0
    },
    [BUILDING_CITADEL] = {
        .id = BUILDING_CITADEL,
        .name = "Citadel G 20",
        .cost = {{RESOURCE_GOLD, 20}},
        .static_change = {
            {RESOURCE_FOOD, 2},
            {RESOURCE_GOLD_INCOME, 2},
            {RESOURCE_TECH_CAVALRY, 1}
        },
        .income = {},
        .transforms = {},
        .build_time = 0
    },
    [BUILDING_EMPTY] = {
        .id = BUILDING_EMPTY,
        .name = "Destroy",
        .cost = {},
        .static_change = {},
        .income = {},
        .transforms = {
            BUILDING_FARM,
            BUILDING_VILLAGE,
            BUILDING_TEMPLE,
            BUILDING_BARRACKS
        },
        .build_time = 0
    },
    [BUILDING_FARM] = {
        .id = BUILDING_FARM,
        .name = "Farm G 10",
        .cost = {{RESOURCE_GOLD, 10}},
        .static_change = {{RESOURCE_FOOD, 1}},
        .income = {},
        .transforms = {BUILDING_EMPTY},
        .build_time = 0
    },
    [BUILDING_VILLAGE] = {
        .id = BUILDING_VILLAGE,
        .name = "Village G 10",
        .cost = {{RESOURCE_GOLD, 10}},
        .static_change = {{RESOURCE_GOLD_INCOME, 1}},
        .income = {},
        .transforms = {BUILDING_EMPTY},
        .build_time = 0
    },
    [BUILDING_TEMPLE] = {
        .id = BUILDING_TEMPLE,
        .name = "Temple G 10",
        .cost = {{RESOURCE_GOLD, 10}},
        .static_change = {{RESOURCE_MANA_INCOME, 1}},
        .income = {},
        .transforms = {BUILDING_EMPTY},
        .build_time = 0
    },
    [BUILDING_BARRACKS] = {
        .id = BUILDING_BARRACKS,
        .name = "Barracks G 15",
        .cost = {{RESOURCE_GOLD, 15}},
        .static_change = {
            {RESOURCE_FOOD, 1},
            {RESOURCE_UNIT_LIMIT, 1}
        },
        .income = {},
        .transforms = {BUILDING_EMPTY},
        .build_time = 0
    },
    [BUILDING_GOLD_VEIN] = {
        .id = BUILDING_GOLD_VEIN,
        .name = "Gold Vein",
        .cost = {},
        .static_change = {},
        .income = {},
        .transforms = {BUILDING_MINE},
        .build_time = 0
    },
    [BUILDING_MINE] = {
        .id = BUILDING_MINE,
        .name = "Mine G 10",
        .cost = {{RESOURCE_GOLD, 10}},
        .static_change = {{RESOURCE_GOLD_INCOME, 1}},
        .income = {},
        .transforms = {BUILDING_GOLD_VEIN},
        .build_time = 0
    },
    [BUILDING_CRYSTAL_VEIN] = {
        .id = BUILDING_CRYSTAL_VEIN,
        .name = "Crystal Vein",
        .cost = {},
        .static_change = {},
        .income = {},
        .transforms = {BUILDING_ALCHEMIST},
        .build_time = 0
    },
    [BUILDING_ALCHEMIST] = {
        .id = BUILDING_ALCHEMIST,
        .name = "Alchemist G 10",
        .cost = {{RESOURCE_GOLD, 10}},
        .static_change = {{RESOURCE_MANA_INCOME, 1}},
        .income = {},
        .transforms = {BUILDING_CRYSTAL_VEIN},
        .build_time = 0
    },
};