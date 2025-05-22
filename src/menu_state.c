#include "pd_api.h"
#include "state.h"

#ifdef _WINDLL
__declspec(dllexport)
#endif

extern void CreateGameState();

typedef struct {
    LCDFont* font;
    int selected;
    const char* items[2];
    LCDBitmap* background;
} MenuData;

static void MenuUpdateTyped(MenuData* m) {
    PDButtons pushed;
    pd->system->getButtonState(NULL, &pushed, NULL);

    if (pushed & kButtonDown || pushed & kButtonUp)
        m->selected = (m->selected + 1) % 2;

    if (pushed & kButtonA && m->selected == 0) {
        CreateGameState();
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

static char* fontpath = "/System/Fonts/Asheville-Sans-14-Bold.pft";
const char *menu_back_sprite = "images/snowflake1";

static void MenuUpdate(void* ptr) {
    MenuUpdateTyped((MenuData*)ptr);
}

static void MenuExit(void* ptr) {
    MenuData* m = (MenuData*)ptr;
    pd->graphics->freeBitmap(m->background);
    pd->system->realloc(m, 0);
}

void CreateMenuState() {
    MenuData* m = pd->system->realloc(NULL, sizeof(MenuData));

    const char* err;
    m->font = pd->graphics->loadFont(fontpath, &err);
    m->background = pd->graphics->loadBitmap(menu_back_sprite, &err);

    m->items[0] = "Start Game";
    m->items[1] = "Continue";
    m->selected = 0;

    switchState(m, NULL, MenuUpdate, MenuExit);
}