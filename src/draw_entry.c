#include "draw_entry.h"
#include "army.h"
#include "state.h"

void PrepareDrawList(DrawEntry* draw_entries, int count) {
    for (int i = 0; i < count; ++i) {
        for (int j = i + 1; j < count; ++j) {
            if (draw_entries[i].sprite == NULL || draw_entries[j].sprite == NULL)
                continue;
            if (draw_entries[i].y > draw_entries[j].y) {
                DrawEntry tmp = draw_entries[i];
                draw_entries[i] = draw_entries[j];
                draw_entries[j] = tmp;
            }
        }
    }
}

void DrawAllEntities(DrawEntry* draw_entries, int count) {
    for (int i = 0; i < count; ++i) {
        DrawEntry* e = &draw_entries[i];
        if (e->sprite)
            pd->graphics->drawBitmap(e->sprite, e->x, e->y, kBitmapUnflipped);
    }
}