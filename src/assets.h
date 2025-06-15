#pragma once

#define FONT_PATH "/System/Fonts/Asheville-Sans-14-Bold.pft"
#define IMG_MENU_BACK_PATH "images/menu_back"

#define IMG_CURSOR "images/cursor"

#define IMG_TILE_GRASS "images/empty"
#define IMG_TILE_TREE "images/trees"

#define IMG_OUTPOST "images/s_outpost"
#define IMG_CASTLE "images/s_castle"
#define IMG_CITADEL "images/s_citadel"
#define IMG_FARM "images/s_farm"
#define IMG_VILLAGE "images/s_village"
#define IMG_BARRACKS "images/s_barracks"
#define IMG_EMPTY_SLOT "images/s_empty_slot"

#define IMG_INFANTRY "images/a_infantry"
#define IMG_ARCHERS "images/a_archers"
#define IMG_CAVALRY "images/a_cavalry"


#if !defined(MIN)
#define MIN(a, b) (((a)<(b))?(a):(b))
#endif

#if !defined(MAX)
#define MAX(a, b) (((a)>(b))?(a):(b))
#endif
