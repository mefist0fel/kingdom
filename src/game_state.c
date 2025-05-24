#include "pd_api.h"
#include "state.h"
#include "assets.h"

#define max(a,b) \
  ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a > _b ? _a : _b; })
#define min(a,b) \
  ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a < _b ? _a : _b; })


#ifdef _WINDLL
__declspec(dllexport)
#endif

extern void CreateMenuState();

typedef struct {
    int x;
    int y;
    int offset_x;
    int offset_y;
    LCDBitmap* grass;
    LCDBitmap* forest;
    LCDBitmap* cursor;
    LCDBitmap* castle;
} GameData;

#define CURSOR_SPEED 4
#define TILE_X 32
#define TILE_Y 32
int rx;
int ry;

static void GameUpdateTyped(GameData* g) {
    PDButtons pushed;
    PDButtons current;
    pd->system->getButtonState(&current, &pushed, NULL);

    if (current & kButtonDown)
        g->y += 1;
    if (current & kButtonUp)
        g->y -= 1;
    if (current & kButtonLeft)
        g->x -= 1;
    if (current & kButtonRight)
        g->x += 1;

    if (current & kButtonDown)
        g->offset_y += CURSOR_SPEED;
    if (current & kButtonUp)
        g->offset_y -= CURSOR_SPEED;
    if (current & kButtonLeft)
        g->offset_x -= CURSOR_SPEED;
    if (current & kButtonRight)
        g->offset_x += CURSOR_SPEED;

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


    for (int y = 0; y < 8; ++y)
    {
        for (int x = 0; x < 14; ++x)
        {
            rx = x + tileOffsetX;
            ry = y + tileOffsetY;
            if (rx < 0 || rx >= mapX)
                continue;
            if (ry < 0 || ry >= mapY)
                continue;

            LCDBitmap* tile = ((rx + ry) % 13 == 0) ? g->forest : g->grass;
            pd->graphics->drawBitmap(tile, -startOffsetX + x * 32, -startOffsetY + y * 32 - (rx % 2) * 16, kBitmapUnflipped);
        }
    }
    
    pd->graphics->drawBitmap(g->castle, 200, 100, kBitmapUnflipped);

    pd->graphics->drawBitmap(g->cursor, g->x, g->y, kBitmapUnflipped);
}

static void GameUpdate(void* ptr) {
    GameUpdateTyped((GameData*)ptr);
}

static void GameExit(void* ptr) {
    GameData* g = (GameData*)ptr;

    pd->graphics->freeBitmap(g->grass);
    pd->graphics->freeBitmap(g->forest);
    pd->graphics->freeBitmap(g->castle);

    pd->graphics->freeBitmap(g->cursor);
    pd->system->realloc(g, 0);
}

void CreateGameState() {
    GameData* g = pd->system->realloc(NULL, sizeof(GameData));
    const char* err;

    g->grass = pd->graphics->loadBitmap(IMG_TILE_GRASS, &err);
    g->forest = pd->graphics->loadBitmap(IMG_TILE_TREE, &err);
    g->castle = pd->graphics->loadBitmap(IMG_CASTLE, &err);

    g->cursor = pd->graphics->loadBitmap(IMG_CURSOR, &err);

    g->x = 200;
    g->y = 100;

    g->offset_x = -100;
    g->offset_y = -50;

    switchState(g, NULL, GameUpdate, GameExit);
}