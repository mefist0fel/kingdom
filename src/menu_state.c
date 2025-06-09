#include "pd_api.h"
#include "state.h"
#include "assets.h"

#ifdef _WINDLL
__declspec(dllexport)
#endif

extern void CreateGameState();

typedef struct {
    LCDFont* font;
    int selected;
    const char* items[3];
    LCDBitmap* background;
} MenuData;

static void MenuUpdateTyped(MenuData* m) {
    PDButtons pushed;
    pd->system->getButtonState(NULL, &pushed, NULL);

    if (pushed & kButtonDown && m->selected < 2)
        m->selected += 1;
    if (pushed & kButtonUp && m->selected > 0)
        m->selected -= 1;

    if (pushed & kButtonA && m->selected == 0) {
        CreateGameState();
    }
    if (pushed & kButtonB && m->selected == 0) {
        CreateGameState();
    }

    pd->graphics->clear(kColorWhite);
    pd->graphics->drawBitmap(m->background, 0, 0, kBitmapUnflipped);
    for (int i = 0; i < 3; ++i) {
        int y = 100 + i * 20;
        if (i == m->selected)
            pd->graphics->fillRect(230, y, 16, 16, kColorBlack);
        pd->graphics->drawText(m->items[i], strlen(m->items[i]), kASCIIEncoding, 250, y);
    }
}

static void MenuUpdate(void* ptr) {
    MenuUpdateTyped((MenuData*)ptr);
}

static void MenuExit(void* ptr) {
    MenuData* m = (MenuData*)ptr;
    pd->graphics->freeBitmap(m->background);
    // pd->system->realloc(g->font, 0); // Don't need to unload system font   
    pd->system->realloc(m, 0);
}

void CreateMenuState() {
    MenuData* m = pd->system->realloc(NULL, sizeof(MenuData));

    const char* err;
    m->font = pd->graphics->loadFont(FONT_PATH, &err);
    m->background = pd->graphics->loadBitmap(IMG_MENU_BACK_PATH, &err);

    m->items[0] = "Start Game";
    m->items[1] = "Continue";
    m->items[2] = "Conf";
    m->selected = 0;

    switchState(m, NULL, MenuUpdate, MenuExit);
}