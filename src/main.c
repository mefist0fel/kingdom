#include <stdio.h>
#include <stdlib.h>

#include "pd_api.h"
#include "state.h"

#ifdef _WINDLL
__declspec(dllexport)
#endif

extern State* create_menu_state();

PlaydateAPI* pd = NULL;

static State* currentState = NULL;

const char* fontpath = "/System/Fonts/Asheville-Sans-14-Bold.pft";


LCDFont* font;
int selected;
const char* items[2];
LCDBitmap* background;


void switchState(State* newState) {
    // if (currentState && currentState->cleanup)
    //     currentState->cleanup(currentState->data);

    currentState = newState;
    //currentData = currentState->init();
}
void update_menu() {
    PDButtons pushed;
    pd->system->getButtonState(NULL, &pushed, NULL);

    if (pushed & kButtonDown || pushed & kButtonUp)
        selected = (selected + 1) % 2;

    if (pushed & kButtonA && selected == 0) {
        //switchState(game_state());
    }

    pd->graphics->clear(kColorWhite);
    pd->graphics->drawBitmap(background, 0, 0, kBitmapUnflipped);
    for (int i = 0; i < 2; ++i) {
        int y = 100 + i * 20;
        if (i == selected)
            pd->graphics->fillRect(40, y - 2, 160, 16, kColorBlack);
        pd->graphics->drawText(items[i], strlen(items[i]), kASCIIEncoding, 50, y);
    }
}

int update(void* userdata) {

    // if (currentState && currentState->update)
    //     currentState->update(currentData);

    update_menu();
	// Draw FPS
	pd->system->drawFPS(0,0);

    return 1;
}

void initializeGame(PlaydateAPI* playdate) {
    pd = playdate;
    pd->graphics->setBackgroundColor(kColorWhite);
	
    const char* err;
	//LCDFont* font = pd->graphics->loadFont(fontpath, &err);
    font = pd->graphics->loadFont(fontpath, &err);
	if ( font == NULL )
		pd->system->error("%s:%i Couldn't load font %s: %s", __FILE__, __LINE__, fontpath, err);
    pd->graphics->setFont(font);

    items[0] = "Start Game";
    items[1] = "Continue";
    selected = 0;
    
	const char *menu_back_sprite = "images/snowflake1";
    background = pd->graphics->loadBitmap(menu_back_sprite, &err);
 
    // switchState(create_menu_state());
    //State* m = pd->system->realloc(NULL, sizeof(State));
    pd->system->setUpdateCallback(update, NULL);
}

int eventHandler(PlaydateAPI* playdate, PDSystemEvent event, uint32_t arg) {
    if (event == kEventInit) {
        initializeGame(playdate);
    }
    return 0;
}