#include "army.h"

void MoveArmy(Army* a, Region* regions, int delta_ms) {
    if (a->region_id == a->target_region_id)
        return;

    a->move_timer -= delta_ms;
    if (a->move_timer <= 0) {
        // убираем из старого региона
        Region* from = &regions[a->region_id];
        if (a->slot_index >= 0 && a->slot_index < REGION_ARMY_SLOTS)
            from->slot_armies[a->slot_index] = -1;

        // ищем слот в новом регионе
        Region* to = &regions[a->target_region_id];
        // for (int s = 0; s < REGION_ARMY_SLOTS; ++s) {
        //     if (to->slot_armies[s] == -1) {
        //         to->slot_armies[s] = i;
        //         a->slot_index = s;
        //         break;
        //     }
        // }

        a->region_id = a->target_region_id;
    }
}

void UpdateArmies(Army* armies, Region* regions, int delta_ms) {
    for (int i = 0; i < MAX_ARMIES; ++i) {
        Army* a = &armies[i];
        
        if (a->hp <= 0) continue;
 
        MoveArmy(a, regions, delta_ms);
    }
}

void SendArmy(Army* army, int target_region_id) {
    if (army->region_id == target_region_id) return;

    army->target_region_id = target_region_id;
    army->move_timer = 1000;
}