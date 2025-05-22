#include "pd_api.h"
#include "state.h"

#ifdef _WINDLL
__declspec(dllexport)
#endif

extern PlaydateAPI* pd;

typedef struct {
    LCDBitmap* grass;
    LCDBitmap* forest;
} TileData;

void* game_init() {
    const char* err;
    TileData* t = pd->system->realloc(NULL, sizeof(TileData));

	const char *path1 = "images/snowflake2";
	const char *path2 = "images/snowflake3";

    t->grass = pd->graphics->loadBitmap(path1, &err);
    t->forest = pd->graphics->loadBitmap(path2, &err );
    return t;
}

void game_update(TileData* t) {
    pd->graphics->clear(kColorWhite);
    for (int y = 0; y < 5; ++y)
        for (int x = 0; x < 6; ++x) {
            LCDBitmap* tile = ((x + y) % 2 == 0) ? t->grass : t->forest;
            pd->graphics->drawBitmap(tile, x * 32, y * 32, kBitmapUnflipped);
        }
}

void game_cleanup(TileData* t) {
    pd->graphics->freeBitmap(t->grass);
    pd->graphics->freeBitmap(t->forest);
    pd->system->realloc(t, 0);
}

//DEFINE_STATE(game, TileData, game_init, game_update, game_cleanup);