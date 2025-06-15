#include "pd_api.h"

#define DRAW_TYPE_NONE     0
#define DRAW_TYPE_BUILDING 1
#define DRAW_TYPE_ARMY     2

typedef struct {
    signed char handle;
    signed char sprite_type;        // DRAW_TYPE_BUILDING or DRAW_TYPE_ARMY
    signed char faction_id;
    LCDBitmap* sprite;
    int x, y;
    int offset_x, offset_y;    
} DrawEntry;

void DrawAllEntities(DrawEntry* draw_entries, int count);