#include "pd_api.h"
#include "state.h"

#ifdef _WINDLL
__declspec(dllexport)
#endif

extern State* game_state();
extern PlaydateAPI* pd;

typedef struct {
    LCDFont* font;
    int selected;
    const char* items[2];
    LCDBitmap* background;
} MenuData;

void* menu_init() {
    MenuData* m = pd->system->realloc(NULL, sizeof(MenuData));
    const char* err;
    const char* fontpath = "/System/Fonts/Asheville-Sans-14-Bold.pft";
    m->font = pd->graphics->loadFont(fontpath, &err);
    m->items[0] = "Start Game";
    m->items[1] = "Continue";
    m->selected = 0;

	const char *menu_back_sprite = "images/snowflake1";
    m->background = pd->graphics->loadBitmap(menu_back_sprite, &err);
    return m;
}

void menu_update(MenuData* m) {
    PDButtons pushed;
    pd->system->getButtonState(NULL, &pushed, NULL);

    if (pushed & kButtonDown || pushed & kButtonUp)
        m->selected = (m->selected + 1) % 2;

    if (pushed & kButtonA && m->selected == 0) {
    //    switchState(game_state());
    }

    pd->graphics->clear(kColorWhite);
    pd->graphics->drawBitmap(m->background, 0, 0, kBitmapUnflipped);
    for (int i = 0; i < 2; ++i) {
        int y = 100 + i * 20;
        if (i == m->selected)
            pd->graphics->fillRect(40, y - 2, 160, 16, kColorBlack);
        pd->graphics->drawText(m->items[i], strlen(m->items[i]), kASCIIEncoding, 50, y);
    }
}

void menu_cleanup(MenuData* m) {
    pd->graphics->freeBitmap(m->background);
    pd->system->realloc(m, 0);
}

// DEFINE_STATE(menu, MenuData, menu_init, menu_update, menu_cleanup);


// void create_menu_state() {
//    Stat MenuState = {
//     .init = menu_init,
//     .update = menu_update,
//     .cleanup = menu_cleanup
//    }
// }

    // static void NAME##_update_wrapper(void* data) { UPDATE_FN((MenuData*)data); }
    // static void NAME##_cleanup_wrapper(void* data) { CLEANUP_FN((MenuData*)data); }
    // State NAME##_state = { NAME##_init_wrapper, NAME##_update_wrapper, NAME##_cleanup_wrapper }