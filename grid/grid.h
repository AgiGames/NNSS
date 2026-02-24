#ifndef GRID_H
#define GRID_H

#include "raylib.h"
#include <stddef.h>
#define GRID_ACCESS(grid, grid_access_i, grid_access_j) grid[(grid_access_i) * (slices_g) + (grid_access_j)] 

void init_grid(size_t window_size, size_t slices);
void free_grid();
void color_grid(bool color_accumulators);
void scatter_accumulators(float accumulator_prob);
void expunge_gaussian();
void create_accumulators();
size_t get_num_accumulators();
void connect_accumulators();
void draw_accumulator_connections();
void do_n_iterations();

#endif
