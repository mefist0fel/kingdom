#pragma once

typedef struct {
    void* data;
    void (*update)(void* data);
    void (*exit)(void* data);
} State;

extern void switchState(
    void* data,
    void (*onEnter)(void* data),
    void (*onUpdate)(void* data),
    void (*onExit)(void* data));

extern struct PlaydateAPI* pd;