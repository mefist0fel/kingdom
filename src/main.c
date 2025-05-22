#include <stdio.h>
#include <stdlib.h>

#include "pd_api.h"
#include "state.h"

#ifdef _WINDLL
__declspec(dllexport)
#endif

extern void CreateMenuState();

PlaydateAPI* pd = NULL;
static State currentState;

const char* fontpath = "/System/Fonts/Asheville-Sans-14-Bold.pft";

void switchState(
    void* data,
    void (*onEnter)(void* data),
    void (*onUpdate)(void* data),
    void (*onExit)(void* data))
{
    if (currentState.exit)
        currentState.exit(currentState.data);

    currentState.data = data;
    currentState.update = onUpdate;
    currentState.exit = onExit;

    if (onEnter)
        onEnter(data);
}

int update(void* userdata) {
    if (currentState.update)
        currentState.update(currentState.data);

	pd->system->drawFPS(0,0);

    return 1;
}

void initializeGame(PlaydateAPI* playdate) {
    pd = playdate;
    pd->graphics->setBackgroundColor(kColorWhite);
    const char* err;
    pd->graphics->setFont(pd->graphics->loadFont(fontpath, &err));
    CreateMenuState();
    pd->system->setUpdateCallback(update, NULL);
}

int eventHandler(PlaydateAPI* playdate, PDSystemEvent event, uint32_t arg) {
    if (event == kEventInit) {
        initializeGame(playdate);
    }
    return 0;
}