#include "pd_api.h"
#include "state.h"
#include "assets.h"

#ifdef _WINDLL
__declspec(dllexport)
#endif

extern void CreateMenuState();

typedef struct {
    int x;
    int y;
    LCDBitmap* grass;
    LCDBitmap* forest;
    LCDBitmap* cursor;
    LCDBitmap* castle;
} GameData;

#define CURSOR_SPEED 4
static void GameUpdateTyped(GameData* g) {
    PDButtons pushed;
    PDButtons current;
    pd->system->getButtonState(&current, &pushed, NULL);

    if (current & kButtonDown)
        g->y += CURSOR_SPEED;
    if (current & kButtonUp)
        g->y -= CURSOR_SPEED;
        
    if (current & kButtonLeft)
        g->x -= CURSOR_SPEED;
    if (current & kButtonRight)
        g->x += CURSOR_SPEED;

    if (pushed & kButtonA) {
        CreateMenuState();
    }

    pd->graphics->clear(kColorWhite);
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 14; ++x) {
            LCDBitmap* tile = ((x + y) % 7 == 0) ? g->forest : g->grass;
            pd->graphics->drawBitmap(tile, x * 32, y * 32 - (x % 2) * 16, kBitmapUnflipped);
        }
    }
    

    pd->graphics->drawBitmap(g->castle, 100, 100, kBitmapUnflipped);

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

    switchState(g, NULL, GameUpdate, GameExit);
}