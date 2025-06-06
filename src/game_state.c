#include "pd_api.h"
#include "state.h"
#include "grid.h"
#include "assets.h"
#include "building_data.h"

#ifdef _WINDLL
__declspec(dllexport)
#endif

extern void CreateMenuState();

#define MAX_SLOTS 16

#define CANVAS_X 400
#define CANVAS_Y 240
#define TILE_X 64
#define TILE_Y 32

#define NUM_TILES_X ((CANVAS_X / TILE_X) + 1)
#define NUM_TILES_Y ((CANVAS_Y / TILE_Y) + 1)

#define CURSOR_X 32
#define CURSOR_Y 32
#define CURSOR_STEP_X 32
#define CURSOR_STEP_Y 32
#define CURSOR_OFFSET_X -16
#define CURSOR_OFFSET_Y -16

#define MAX_FRAME_MS 36 // 36 ms max frame delta - to support 30 FPS
#define CURSOR_INITIAL_DELAY_MS 180  // first delay on press arrow keys
#define CURSOR_REPEAT_DELAY_MS 80 // repeated delay on press arrow keys

unsigned int prev_time = 0;

typedef struct {
    Grid* grid;
    BuildingSlot slots[MAX_SLOTS];
    FactionData factions[6]; // 0 - neutral / 1 - player / 2-5 - enemies

    // Map offset
    int offset_x;
    int offset_y;
    // Cursor position
    int x;
    int y;
    int input_delay_timer;
    // Slot context menu
    int selected_slot_index;
    int selected_building_index;

    LCDFont* font;

    LCDBitmap* grass;
    LCDBitmap* forest;
    LCDBitmap* cursor;
    LCDBitmap* castle;
    LCDBitmap* building_sprites[N_BUILDING_TYPES];
} GameData;


static void DrawTerrain(GameData* g, int tileOffsetX, int tileOffsetY, int startOffsetX, int startOffsetY) {
    for (int y = 0; y < NUM_TILES_X; ++y) {
        for (int x = 0; x < NUM_TILES_Y; ++x) {
            int rx = x + tileOffsetX;
            int ry = y + tileOffsetY;
            if (rx < 0 || rx >= g->grid->size_x || ry < 0 || ry >= g->grid->size_y) continue;

            LCDBitmap* tile = (g->grid->data[ry * g->grid->size_x + rx] == 0) ? g->forest : g->grass;
            pd->graphics->drawBitmap(tile, -startOffsetX + x * TILE_X, -startOffsetY + y * TILE_Y, kBitmapUnflipped);
        }
    }
    pd->graphics->drawRect(0 - startOffsetX, 0 - startOffsetY, CANVAS_X, CANVAS_Y, kColorBlack);
}

static void DrawBuildings(GameData* g, int tileOffsetX, int tileOffsetY, int startOffsetX, int startOffsetY) {
    for (int i = 0; i < MAX_SLOTS; ++i) {
        BuildingSlot* slot = &g->slots[i];
        if (slot->building_type == BUILDING_NONE)
            break;

        int sx = slot->x - tileOffsetX;
        int sy = slot->y - tileOffsetY;
        if (sx < 0 || sx >= NUM_TILES_X || sy < 0 || sy >= NUM_TILES_Y) continue;

        LCDBitmap* sprite = g->building_sprites[slot->building_type];
        if (sprite) {
            pd->graphics->drawBitmap(
                sprite,
                -startOffsetX + sx * TILE_X,
                -startOffsetY + sy * TILE_Y - 32, // fix hardcoded offset for buildings tiles
                kBitmapUnflipped
            );
        }
    }
}

static void DrawResources(GameData* g) {
    FactionData* player = &g->factions[1];
    char* buffer = NULL;
    pd->system->formatString(&buffer,
        "G %d (+%d) M %d (+%d) F (%d)",
        player->resources[RESOURCE_GOLD],
        player->resources[RESOURCE_GOLD_INCOME],
        player->resources[RESOURCE_MANA],
        player->resources[RESOURCE_MANA_INCOME],
        player->resources[RESOURCE_FOOD]);

    pd->graphics->setFont(g->font);
    pd->graphics->drawText(buffer, strlen(buffer), kASCIIEncoding, 5, 5);
    pd->system->realloc((void*)buffer, 0); // Free buff
}

static void DrawCursor(GameData* g) {
    pd->graphics->drawBitmap(g->cursor, g->x, g->y, kBitmapUnflipped);
}

static int GetSlotAtCursor(GameData* g) {
    int cx = (g->x + g->offset_x) / TILE_X;
    int cy = (g->y + g->offset_y) / TILE_Y;
    for (int i = 0; i < MAX_SLOTS; ++i) {
        if (g->slots[i].building_type == BUILDING_NONE) break;
        if (g->slots[i].x == cx && g->slots[i].y == cy)
            return i;
    }
    return -1;
}

static int GetCursorDirection(PDButtons button, int* dx, int* dy) {
    *dx = 0;
    *dy = 0;

    if (button & kButtonLeft)  { *dx = -1; return 1; }
    if (button & kButtonRight) { *dx =  1; return 1; }
    if (button & kButtonUp)    { *dy = -1; return 1; }
    if (button & kButtonDown)  { *dy =  1; return 1; }

    return 0;
}

static void HandleCursorMovement(GameData* g, PDButtons current, PDButtons pushed) {
    // Selection
    if (pushed & kButtonA) {
        int index = GetSlotAtCursor(g);
        if (index >= 0) {
            g->selected_slot_index = index;
            g->selected_building_index = 0;
        }
    }

    // process delay
    unsigned int current_time = pd->system->getCurrentTimeMilliseconds();
    unsigned int dt = MIN(current_time - prev_time, MAX_FRAME_MS);
    prev_time = current_time;
    
    g->input_delay_timer -= dt;

    // Moving Cursor
    int dx, dy;
    // On button press
    if (GetCursorDirection(pushed, &dx, &dy)) {
        g->x += CURSOR_STEP_X * dx;
        g->y += CURSOR_STEP_Y * dy;
        g->input_delay_timer = CURSOR_INITIAL_DELAY_MS;
    }
    // On button hold
    if (GetCursorDirection(current, &dx, &dy) && g->input_delay_timer <= 0) {
        g->x += CURSOR_STEP_X * dx;
        g->y += CURSOR_STEP_Y * dy;
        g->input_delay_timer = CURSOR_REPEAT_DELAY_MS;
    }

    // Moving offset (camera)
    if (g->x < 0) {
        g->x += CURSOR_STEP_X;
        g->offset_x -= CURSOR_STEP_X;
    }
    if (g->x > CANVAS_X - CURSOR_X) {
        g->x -= CURSOR_STEP_X;
        g->offset_x += CURSOR_STEP_X;
    }
    if (g->y < 0) {
        g->y += CURSOR_STEP_Y;
        g->offset_y -= CURSOR_STEP_Y;
    }
    if (g->y > CANVAS_Y - CURSOR_Y) {
        g->y -= CURSOR_STEP_Y;
        g->offset_y += CURSOR_STEP_Y;
    }
}

static void ApplyResourceChanges(FactionData* faction, const ResourceChange* changes, int multiplier) {
    for (int i = 0; i < MAX_RESOURCE_CHANGES; ++i) {
        if (changes[i].type == RESOURCE_NONE) break;
        faction->resources[changes[i].type] += changes[i].amount * multiplier;
    }
}

static void ChangeBuildingInSlot(GameData* g, int slot_index, BuildingType new_type) {
    BuildingSlot* slot = &g->slots[slot_index];
    const BuildingData* old_data = &building_data[slot->building_type];
    const BuildingData* new_data = &building_data[new_type];
    FactionData* faction = &g->factions[slot->faction_id];

    ApplyResourceChanges(faction, old_data->static_change, -1);
    ApplyResourceChanges(faction, new_data->cost, -1);
    ApplyResourceChanges(faction, new_data->static_change, 1);

    slot->building_type = new_type;
}

static void DrawBuildMenu(GameData* g) {
    BuildingSlot* slot = &g->slots[g->selected_slot_index];
    const BuildingData* data = &building_data[slot->building_type];

    pd->graphics->setFont(g->font);
    pd->graphics->fillRect(200, 40, 180, 120, kColorWhite);
    pd->graphics->drawRect(200, 40, 180, 120, kColorBlack);

    for (int i = 0; i < MAX_TRANSFORMS; ++i) {
        BuildingType bt = data->transforms[i];
        if (bt == BUILDING_NONE) break;

        int y = 50 + i * 20;
        if (i == g->selected_building_index)
            pd->graphics->fillRect(210, y, 8, 8, kColorBlack);
        pd->graphics->drawText(building_data[bt].name, strlen(building_data[bt].name), kASCIIEncoding, 230, y);
    }
}

static void HandleBuildMenu(GameData* g, PDButtons pushed) {
    BuildingSlot* slot = &g->slots[g->selected_slot_index];
    const BuildingData* data = &building_data[slot->building_type];

    if (pushed & kButtonDown) g->selected_building_index++;
    if (pushed & kButtonUp) g->selected_building_index--;
    if (g->selected_building_index < 0) g->selected_building_index = 0;

    int available = 0;
    for (int i = 0; i < MAX_TRANSFORMS; ++i) {
        if (data->transforms[i] == BUILDING_NONE) break;
        available++;
    }
    if (g->selected_building_index >= available)
        g->selected_building_index = available - 1;

    if (pushed & kButtonA && g->selected_building_index >= 0) {
        BuildingType new_type = data->transforms[g->selected_building_index];
        ChangeBuildingInSlot(g, g->selected_slot_index, new_type);
        g->selected_slot_index = -1;
        g->selected_building_index = 0;
    }

    if (pushed & kButtonB) {
        g->selected_building_index = 0;
        g->selected_slot_index = -1;
    }
}

static void GameUpdateTyped(GameData* g) {
    pd->graphics->clear(kColorWhite);

    int tileOffsetX = g->offset_x / TILE_X;
    int tileOffsetY = g->offset_y / TILE_Y;
    int startOffsetX = g->offset_x - tileOffsetX * TILE_X;
    int startOffsetY = g->offset_y - tileOffsetY * TILE_Y;

    DrawTerrain(g, tileOffsetX, tileOffsetY, startOffsetX, startOffsetY);
    DrawBuildings(g, tileOffsetX, tileOffsetY, startOffsetX, startOffsetY);

    pd->graphics->drawBitmap(g->cursor, g->x, g->y, kBitmapUnflipped);

    // Control
    PDButtons pushed;
    PDButtons current;
    pd->system->getButtonState(&current, &pushed, NULL);

    if (g->selected_slot_index == -1) {
        DrawCursor(g);
        HandleCursorMovement(g, current, pushed);
    } else {
        DrawBuildMenu(g);
        HandleBuildMenu(g, pushed);
    }
    DrawResources(g);
}

static void GameUpdate(void* ptr) {
    GameUpdateTyped((GameData*)ptr);
}

static void GameExit(void* ptr) {
    GameData* g = (GameData*)ptr;

    destroyGrid(g->grid);
    
    pd->graphics->freeBitmap(g->grass);
    pd->graphics->freeBitmap(g->forest);
    pd->graphics->freeBitmap(g->castle);
    
    for (int i = 0; i < N_BUILDING_TYPES; ++i)
        if (g->building_sprites[i]) pd->graphics->freeBitmap(g->building_sprites[i]);

    pd->graphics->freeBitmap(g->cursor);
    pd->system->realloc(g->font, 0);
    pd->system->realloc(g, 0);
    pd->system->removeAllMenuItems();
}

void backToMenuCallback(void* userdata) {
    CreateMenuState();
}

// Sort buildings by view order
static void SortBuildingsByY(BuildingSlot* slots, int max_slots) {
    for (int i = 0; i < max_slots; ++i) {
        if (slots[i].building_type == BUILDING_NONE)
            continue;

        for (int j = 0; j < max_slots; ++j) {
            if (i == j) continue;
            if (slots[j].building_type == BUILDING_NONE)
                continue;

            if (slots[i].y < slots[j].y)
                continue;

            if (slots[i].y > slots[j].y) {
                BuildingSlot tmp = slots[i];
                slots[i] = slots[j];
                slots[j] = tmp;
            }
        }
    }
}

void CreateGameState() {
    GameData* g = pd->system->realloc(NULL, sizeof(GameData));
    const char* err;

    g->grid = createRandomGrid(32, 32);

    g->font = pd->graphics->loadFont(FONT_PATH, &err);

    g->grass = pd->graphics->loadBitmap(IMG_TILE_GRASS, &err);
    g->forest = pd->graphics->loadBitmap(IMG_TILE_TREE, &err);
    g->castle = pd->graphics->loadBitmap(IMG_CASTLE, &err); // TODO kill me

    g->cursor = pd->graphics->loadBitmap(IMG_CURSOR, &err);

    // Load building sprites
    for (int i = 0; i < N_BUILDING_TYPES; ++i) g->building_sprites[i] = NULL;
    g->building_sprites[BUILDING_CASTLE] = pd->graphics->loadBitmap(IMG_CASTLE, &err);
    g->building_sprites[BUILDING_FARM] = pd->graphics->loadBitmap(IMG_FARM, &err);
    g->building_sprites[BUILDING_EMPTY] = pd->graphics->loadBitmap(IMG_EMPTY_SLOT, &err);
    g->building_sprites[BUILDING_VILLAGE] = pd->graphics->loadBitmap(IMG_VILLAGE, &err);
    g->building_sprites[BUILDING_TEMPLE] = pd->graphics->loadBitmap(IMG_CASTLE, &err);
    g->building_sprites[BUILDING_BARRACKS] = pd->graphics->loadBitmap(IMG_BARRACKS, &err);
    g->building_sprites[BUILDING_CITADEL] = pd->graphics->loadBitmap(IMG_CASTLE, &err);
    g->building_sprites[BUILDING_OUTPOST] = pd->graphics->loadBitmap(IMG_CASTLE, &err);
    g->building_sprites[BUILDING_ALCHEMIST] = pd->graphics->loadBitmap(IMG_CASTLE, &err);
    g->building_sprites[BUILDING_MINE] = pd->graphics->loadBitmap(IMG_CASTLE, &err);

    // Initialize slots (zeroed by default)
    g->slots[0] = (BuildingSlot){ .region_id = 0, .faction_id = 1, .building_type = BUILDING_CASTLE, .x = 4, .y = 5 };
    g->slots[1] = (BuildingSlot){ .region_id = 0, .faction_id = 1, .building_type = BUILDING_EMPTY, .x = 6, .y = 5 };
    g->slots[2] = (BuildingSlot){ .region_id = 1, .faction_id = 2, .building_type = BUILDING_CASTLE, .x = 10, .y = 6 };
    g->slots[3] = (BuildingSlot){ .region_id = 1, .faction_id = 2, .building_type = BUILDING_EMPTY, .x = 12, .y = 6 };
    g->slots[4] = (BuildingSlot){ .region_id = 2, .faction_id = FACTION_NEUTRAL, .building_type = BUILDING_OUTPOST, .x = 6, .y = 10 };
    g->slots[5] = (BuildingSlot){ .region_id = 2, .faction_id = FACTION_NEUTRAL, .building_type = BUILDING_VILLAGE, .x = 8, .y = 10 };

    SortBuildingsByY(g->slots, MAX_SLOTS);

    // init factions
    FactionData* player = &g->factions[FACTION_PLAYER];
    player->resources[RESOURCE_GOLD] = 100;
    player->resources[RESOURCE_GOLD_INCOME] = 1;
    player->resources[RESOURCE_MANA] = 0;
    player->resources[RESOURCE_MANA_INCOME] = 0;
    player->resources[RESOURCE_FOOD] = 2;

    // Враг (фракция 2)
    FactionData* enemy = &g->factions[FACTION_ENEMY];
    enemy->resources[RESOURCE_GOLD] = 100;
    enemy->resources[RESOURCE_GOLD_INCOME] = 0;
    enemy->resources[RESOURCE_MANA] = 0;
    enemy->resources[RESOURCE_MANA_INCOME] = 0;
    enemy->resources[RESOURCE_FOOD] = 3;

    g->offset_x = 0;
    g->offset_y = 0;

    g->x = 16;
    g->y = 0;
    g->input_delay_timer = 0;

    g->selected_slot_index = -1; // not selected
    g->selected_building_index = 0;

    pd->system->addMenuItem("Game menu", backToMenuCallback, NULL);
    // pd->system->addCheckmarkMenuItem("Chackbox", 1, backToMenuCallback, NULL);
    // const char *options[] = {"one", "two", "three"};
    // pd->system->addOptionsMenuItem("Options", options, 3, backToMenuCallback, NULL);

    switchState(g, NULL, GameUpdate, GameExit);
}