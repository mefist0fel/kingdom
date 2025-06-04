#pragma once

typedef struct {
    int* data;
    int size_x;
    int size_y;
} Grid;


Grid* createGrid(int map_x, int map_y);
Grid* createRandomGrid(int map_x, int map_y);
void destroyGrid(Grid* grid);

int getGridValue(const Grid* grid, int x, int y);
void setGridValue(const Grid* grid, int x, int y, int value);