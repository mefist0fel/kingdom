#include "pd_api.h"
#include "state.h"
#include "grid.h"
#include "assets.h"

#ifdef _WINDLL
__declspec(dllexport)
#endif

#define max(a,b) \
  ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a > _b ? _a : _b; })
#define min(a,b) \
  ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a < _b ? _a : _b; })

extern void CreateMenuState();

typedef struct {
    Grid* grid;

    int x;
    int y;
    int offset_x;
    int offset_y;
    LCDBitmap* grass;
    LCDBitmap* forest;
    LCDBitmap* cursor;
    LCDBitmap* castle;
} GameData;

#define LCG_A 1664525
#define LCG_C 1013904223
#define LCG_M 0xFFFFFFFF

static unsigned int lcg_next(unsigned int* seed) {
    *seed = (*seed) * LCG_A + LCG_C; // & LCG_M;
    return *seed;
}

Grid* createGrid(int map_x, int map_y) {
    Grid* grid = pd->system->realloc(NULL, sizeof(Grid));
    grid->data = pd->system->realloc(NULL, sizeof(int) * map_x * map_y);
    grid->size_x = map_x;
    grid->size_y = map_y;
    for (int i = 0; i < map_x * map_y; i++) {
        grid->data[i] = 0;
    }
    return grid;
}

Grid* createRandomGrid(int map_x, int map_y) {
    Grid* grid = pd->system->realloc(NULL, sizeof(Grid));
    grid->size_x = map_x;
    grid->size_y = map_y;
    grid->data = pd->system->realloc(NULL, sizeof(int) * map_x * map_y);

    unsigned int seed = 0;
    for (int i = 0; i < map_x * map_y; i++) {
        grid->data[i] = (int)lcg_next(&seed) % 3;
    }

    return grid;
}

int getGridValue(const Grid* grid, int x, int y) {
    if (!grid || x < 0 || y < 0 || x >= grid->size_x || y >= grid->size_y)
        return -1; // error
    return grid->data[y * grid->size_x + x];
}

void setGridValue(const Grid* grid, int x, int y, int value) {
    if (!grid || x < 0 || y < 0 || x >= grid->size_x || y >= grid->size_y)
        return;
    grid->data[y * grid->size_x + x] = value;
}

void destroyGrid(Grid* grid) {
    if (!grid) return;
    pd->system->realloc(grid->data, 0);
    pd->system->realloc(grid, 0);
}



#define CURSOR_SPEED 8
#define TILE_X 32
#define TILE_Y 32

#define CURSOR_X 32
#define CURSOR_Y 32

#define CANVAS_X 400
#define CANVAS_Y 240

int rx;
int ry;

static void GameUpdateTyped(GameData* g) {
    PDButtons pushed;
    PDButtons current;
    pd->system->getButtonState(&current, &pushed, NULL);

    if (current & kButtonLeft)
        g->x -= CURSOR_SPEED;
    if (current & kButtonRight)
        g->x += CURSOR_SPEED;
    if (current & kButtonDown)
        g->y += CURSOR_SPEED;
    if (current & kButtonUp)
        g->y -= CURSOR_SPEED;

    if (g->x < 0) {
        g->x += CURSOR_SPEED;
        g->offset_x -= CURSOR_SPEED;
    }
    if (g->x > CANVAS_X - CURSOR_X) {
        g->x -= CURSOR_SPEED;
        g->offset_x += CURSOR_SPEED;
    }
    if (g->y < 0) {
        g->y += CURSOR_SPEED;
        g->offset_y -= CURSOR_SPEED;
    }
    if (g->y > CANVAS_Y - CURSOR_Y) {
        g->y -= CURSOR_SPEED;
        g->offset_y += CURSOR_SPEED;
    }

    if (pushed & kButtonA) {
        CreateMenuState();
    }

    pd->graphics->clear(kColorWhite);

    int mapX = 32;
    int mapY = 32;

    int tileOffsetX = g->offset_x / TILE_X;
    int tileOffsetY = g->offset_y / TILE_Y;
    int startOffsetX = g->offset_x - tileOffsetX * TILE_X;
    int startOffsetY = g->offset_y - tileOffsetY * TILE_Y;


    for (int y = 0; y < 9; ++y)
    {
        for (int x = 0; x < 14; ++x)
        {
            rx = x + tileOffsetX;
            ry = y + tileOffsetY;
            if (rx < 0 || rx >= g->grid->size_x)
                continue;
            if (ry < 0 || ry >= g->grid->size_y)
                continue;

            LCDBitmap* tile = (getGridValue(g->grid, rx, ry) == 0) ? g->forest : g->grass;
            pd->graphics->drawBitmap(tile, -startOffsetX + x * 32, -startOffsetY + y * 32 - (rx % 2) * 16, kBitmapUnflipped);
        }
    }
    for (int y = 0; y < 9; ++y)
    {
        for (int x = 0; x < 14; ++x)
        {
            rx = x + tileOffsetX;
            ry = y + tileOffsetY;
            if (rx < 0 || rx >= mapX)
                continue;
            if (ry < 0 || ry >= mapY)
                continue;

            if ((rx + 3 + ry) % 19 == 0) {
                pd->graphics->drawBitmap(g->castle, -startOffsetX + x * 32 - 16, -startOffsetY + y * 32 - (rx % 2) * 16 - 32, kBitmapUnflipped);
            }
        }
    }
    
    //pd->graphics->drawBitmap(g->castle, 200, 100, kBitmapUnflipped);

    pd->graphics->drawBitmap(g->cursor, g->x, g->y, kBitmapUnflipped);
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

    pd->graphics->freeBitmap(g->cursor);
    pd->system->realloc(g, 0);
}

void CreateGameState() {
    GameData* g = pd->system->realloc(NULL, sizeof(GameData));
    const char* err;

    g->grid = createRandomGrid(32, 32);
    g->grass = pd->graphics->loadBitmap(IMG_TILE_GRASS, &err);
    g->forest = pd->graphics->loadBitmap(IMG_TILE_TREE, &err);
    g->castle = pd->graphics->loadBitmap(IMG_CASTLE, &err);

    g->cursor = pd->graphics->loadBitmap(IMG_CURSOR, &err);

    g->x = 200 - 16;
    g->y = 120 - 16;

    g->offset_x = 0;
    g->offset_y = 0;

    switchState(g, NULL, GameUpdate, GameExit);
}