#include "pd_api.h"
#include "state.h"
#include "assets.h"

#ifdef _WINDLL
__declspec(dllexport)
#endif

extern void CreateMenuState();

typedef struct {
    LCDBitmap* grass;
    LCDBitmap* forest;
} GameData;

static void GameUpdateTyped(GameData* g) {
    PDButtons pushed;
    pd->system->getButtonState(NULL, &pushed, NULL);

    if (pushed & kButtonA) {
        CreateMenuState();
    }

    pd->graphics->clear(kColorWhite);
    for (int y = 0; y < 8; ++y)
        for (int x = 0; x < 14; ++x) {
            LCDBitmap* tile = ((x + y) % 2 == 0) ? g->grass : g->forest;
            pd->graphics->drawBitmap(tile, x * 32, y * 32, kBitmapUnflipped);
        }
}

static void GameUpdate(void* ptr) {
    GameUpdateTyped((GameData*)ptr);
}

static void GameExit(void* ptr) {
    GameData* g = (GameData*)ptr;
    pd->graphics->freeBitmap(g->grass);
    pd->graphics->freeBitmap(g->forest);
    pd->system->realloc(g, 0);
}

void CreateGameState() {
    GameData* g = pd->system->realloc(NULL, sizeof(GameData));
    const char* err;
    g->grass = pd->graphics->loadBitmap(IMG_TILE_GRASS, &err);
    g->forest = pd->graphics->loadBitmap(IMG_TILE_TREE, &err);

    switchState(g, NULL, GameUpdate, GameExit);
}