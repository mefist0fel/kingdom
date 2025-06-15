#define MAX_ARMIES 32
#define MAX_REGIONS 16
#define REGION_ARMY_SLOTS 9
#define MAX_REGION_NEIGHBORS 4

typedef struct {
    int x, y;              // координаты центра региона (замка)
    signed char slot_armies[REGION_ARMY_SLOTS]; // индексы армий в слотах (-1 если пусто)
    signed char neighbors[MAX_REGION_NEIGHBORS]; // список соседних регионов (индексы)
} Region;

typedef struct {
    signed char type;
    signed char faction_id;
    signed char region_id;
    signed char target_region_id;  // target region, army is on the way if it's not the same, as region
    int hp;
    int x, y;
    int move_timer;        // ticks to arrival
    signed char slot_index;        // slot_index
} Army;


void UpdateArmies(Army* armies, Region* regions, int delta_ms);
void SendArmy(Army* army, int target_region_id);