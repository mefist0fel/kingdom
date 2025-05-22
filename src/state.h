#pragma once

typedef struct {
    void (*update)(void* data);
    void* data;
} State;

extern void switchState(State*);

// #define DEFINE_STATE(NAME, TYPE, INIT_FN, UPDATE_FN, CLEANUP_FN) \
//     void* INIT_FN(); \
//     void UPDATE_FN(TYPE*); \
//     void CLEANUP_FN(TYPE*); \
//     static void* NAME##_init_wrapper() { return INIT_FN(); } \
//     static void NAME##_update_wrapper(void* data) { UPDATE_FN((TYPE*)data); } \
//     static void NAME##_cleanup_wrapper(void* data) { CLEANUP_FN((TYPE*)data); } \
//     State NAME##_state = { NAME##_init_wrapper, NAME##_update_wrapper, NAME##_cleanup_wrapper }