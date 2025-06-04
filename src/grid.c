#include "pd_api.h"
#include "state.h"
#include "grid.h"

static unsigned long int rnd_next = 1;

static unsigned int prandom() {
    const int rand_max = 32767;
    rnd_next = rnd_next * 1103515245 + 12345;
    return (unsigned int)(rnd_next / 65536) % (rand_max + 1);
}

Grid* createGrid(int map_x, int map_y) {
    Grid* grid = pd->system->realloc(NULL, sizeof(Grid));
    grid->data = pd->system->realloc(NULL, sizeof(int) * map_x * map_y);
    grid->size_x = map_x;
    grid->size_y = map_y;
    for (int i = 0; i < map_x * map_y; i++) {
        grid->data[i] = 0;
    }
    return grid;
}

Grid* createRandomGrid(int map_x, int map_y) {
    Grid* grid = pd->system->realloc(NULL, sizeof(Grid));
    grid->size_x = map_x;
    grid->size_y = map_y;
    grid->data = pd->system->realloc(NULL, sizeof(int) * map_x * map_y);

    for (int i = 0; i < map_x * map_y; i++) {
        grid->data[i] = (int)(prandom() % 7);
    }

    return grid;
}

void destroyGrid(Grid* grid) {
    if (!grid) return;
    pd->system->realloc(grid->data, 0);
    pd->system->realloc(grid, 0);
}

int getGridValue(const Grid* grid, int x, int y) {
    if (!grid || x < 0 || y < 0 || x >= grid->size_x || y >= grid->size_y)
        return -1; // error
    return grid->data[y * grid->size_x + x];
}

void setGridValue(const Grid* grid, int x, int y, int value) {
    if (!grid || x < 0 || y < 0 || x >= grid->size_x || y >= grid->size_y)
        return;
    grid->data[y * grid->size_x + x] = value;
}