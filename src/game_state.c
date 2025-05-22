#include "pd_api.h"
#include "state.h"

#ifdef _WINDLL
__declspec(dllexport)
#endif

typedef struct {
    LCDBitmap* grass;
    LCDBitmap* forest;
} GameData;

static void GameUpdateTyped(GameData* g) {
    pd->graphics->clear(kColorWhite);
    for (int y = 0; y < 5; ++y)
        for (int x = 0; x < 6; ++x) {
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

const char *path1 = "images/snowflake2";
const char *path2 = "images/snowflake3";

void CreateGameState() {
    GameData* g = pd->system->realloc(NULL, sizeof(GameData));
    const char* err;
    g->grass = pd->graphics->loadBitmap(path1, &err);
    g->forest = pd->graphics->loadBitmap(path2, &err);

    switchState(g, NULL, GameUpdate, GameExit);
}