#include <stddef.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>

#include "../helper/helper.h"
#include "raylib.h"
#include "grid.h"
#include "../globals/globals.h"

size_t window_size_g = 750;
size_t slices_g = 50;
float spacing_g = 15.0f;
size_t num_accumulators = 0;
size_t stddev = 4;

float *grid_values = NULL;
bool *is_accumulator = NULL;
Vector2DA accumulators = {0};
PointPairDA connections = {0};
Vector2 ref_point = {0};
bool changed = false;

void init_grid(size_t window_size, size_t slices) {
    num_accumulators = 0;
    window_size_g = window_size;
    slices_g = slices;
    spacing_g = (float) window_size / slices;
    grid_values = (float*) calloc(slices * slices, sizeof(float));
    is_accumulator = (bool*) calloc(slices * slices, sizeof(bool));
    accumulators.count = 0;
    connections.count = 0;
    changed = false;
}

void scatter_accumulators(float accumulator_prob) {
    for (size_t i = 0; i < slices_g; ++i) {
        for (size_t j = 0; j < slices_g; ++j) {
            float random_number = (float)rand() / RAND_MAX;
            if (random_number > accumulator_prob) continue;

            GRID_ACCESS(is_accumulator, i, j) = true;
            DA_APPEND(accumulators, ((Vector2){(j * spacing_g) + (spacing_g / 2.0f), (i * spacing_g) + (spacing_g / 2.0f)}));
            num_accumulators += 1;
        }
    }
}

void free_grid() {
    if (grid_values != NULL) {
        free(grid_values);
        grid_values = NULL;
    }
    if (is_accumulator != NULL) {
        free(is_accumulator);
        is_accumulator = NULL;
    }
}

void expunge_gaussian() {
    float* grid_values_copy = (float*) calloc(slices_g * slices_g, sizeof(float));
    COPY_ARR(grid_values_copy, grid_values, slices_g * slices_g);
    
    float* temp_array = (float*) calloc(slices_g * slices_g, sizeof(float));
    size_t halflen = 3 * stddev;
    float max_intensity = FLT_MIN;

    // first pass
    for (size_t i = 0; i < slices_g; ++i) {
        for (size_t j = 0; j < slices_g; ++j) {
            size_t l = FIND_MAX(0, (int) j - (int) halflen);
            size_t r = FIND_MIN(slices_g - 1, j + halflen);
            for (size_t k = l; k <= r; ++k) {
                GRID_ACCESS(temp_array, i, j) +=
                    GRID_ACCESS(is_accumulator, i, k) * gaussian1d(
                                                            (float) k,
                                                            (float) j,
                                                            stddev
                                                        );
            }
        }
    }

    for (size_t i = 0; i < slices_g; ++i) {
        for (size_t j = 0; j < slices_g; ++j) {
            size_t l = FIND_MAX(0, (int) i - (int) halflen);
            size_t r = FIND_MIN(slices_g - 1, i + halflen);
            for (size_t k = l; k <= r; ++k) {
                GRID_ACCESS(grid_values, i, j) +=
                    GRID_ACCESS(temp_array, k, j) * gaussian1d(
                                                            (float) k,
                                                            (float) i,
                                                            stddev
                                                        );
            }
            max_intensity = fmax(max_intensity, GRID_ACCESS(grid_values, i, j));
        }
    }

    free(temp_array);

    for (size_t i = 0; i < slices_g; ++i) {
        for (size_t j = 0; j < slices_g; ++j) {
            GRID_ACCESS(grid_values, i, j) /= max_intensity;
            if (!float_equal(GRID_ACCESS(grid_values, i, j), GRID_ACCESS(grid_values_copy, i, j))) {
                changed = true;
            }
        }
    }
    
    free(grid_values_copy);
}

bool is_maxima(size_t i, size_t j) {
    bool up_good = 0, down_good = 0, left_good = 0, right_good = 0;
    float cur_value = GRID_ACCESS(grid_values, i, j);
    if (float_equal(cur_value, 0.0f)) return false;

    up_good =
        (i == 0) ||
        (GRID_ACCESS(grid_values, i - 1, j) < cur_value) ||
        (float_equal(cur_value, GRID_ACCESS(grid_values, i - 1, j)));

    down_good =
        (i + 1 >= slices_g) ||
        (GRID_ACCESS(grid_values, i + 1, j) < cur_value) ||
        (float_equal(cur_value, GRID_ACCESS(grid_values, i + 1, j)));

    left_good =
        (j == 0) ||
        (GRID_ACCESS(grid_values, i, j - 1) < cur_value) ||
        (float_equal(cur_value, GRID_ACCESS(grid_values, i, j - 1)));

    right_good =
        (j + 1 >= slices_g) ||
        (GRID_ACCESS(grid_values, i, j + 1) < cur_value) ||
        (float_equal(cur_value, GRID_ACCESS(grid_values, i, j + 1)));

    return up_good && down_good && left_good && right_good;
}

void create_accumulators() {
    bool *new_is_accumulator = (bool *)calloc(slices_g * slices_g, sizeof(bool));

    for (size_t i = 0; i < slices_g; ++i) {
        for (size_t j = 0; j < slices_g; ++j) {
            GRID_ACCESS(new_is_accumulator, i, j) =
                is_maxima(i, j) || GRID_ACCESS(is_accumulator, i, j);

            if (is_maxima(i, j) && !GRID_ACCESS(is_accumulator, i, j)) {
                num_accumulators++;
                changed = true;
                DA_APPEND(accumulators, ((Vector2){(j * spacing_g) + (spacing_g / 2.0f), (i * spacing_g) + (spacing_g / 2.0f)}));
            }
        }
    }

    free(is_accumulator);
    is_accumulator = new_is_accumulator;
}

void color_grid(bool color_accumulators) {
    for (size_t i = 0; i < slices_g; ++i) {
        for (size_t j = 0; j < slices_g; ++j) {
            float x = j * spacing_g;
            float y = i * spacing_g;

            DrawRectangle(x, y, spacing_g, spacing_g, heatmap_cmap(GRID_ACCESS(grid_values, i, j)));
            if (GRID_ACCESS(is_accumulator, i, j) && color_accumulators) {
                DrawCircle(x + (spacing_g / 2.0f), y + (spacing_g / 2.0f), (spacing_g * 0.3f), WHITE);
            }
        }
    }
}

void draw_accumulator_connections() {
    for (size_t i = 0; i < connections.count; ++i) {
        PointPair pp = connections.items[i];
        // DrawText("BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB", 10, 10, 20, WHITE);
        DrawLineV(pp.p1, pp.p2, WHITE);
    }
}

void connect_accumulators() {
    connections.count = 0;
    
    // 1. Grid-based spatial partitioning
    // We'll use a local grid to bucket points. Bucket size reflects the Gaussian reach.
    float bucket_size = spacing_g * stddev * 2.0f;
    size_t grid_w = ceilf((float)window_size_g / bucket_size);
    size_t grid_h = ceilf((float)window_size_g / bucket_size);
    
    // Use an array of dynamic arrays (index buckets)
    typedef struct {
        size_t *indices;
        size_t count;
        size_t capacity;
    } Bucket;

    Bucket *grid = calloc(grid_w * grid_h, sizeof(Bucket));

    // 2. Assign points to buckets
    for (size_t i = 0; i < accumulators.count; ++i) {
        size_t gx = fmaxf(0, fminf(grid_w - 1, floorf(accumulators.items[i].x / bucket_size)));
        size_t gy = fmaxf(0, fminf(grid_h - 1, floorf(accumulators.items[i].y / bucket_size)));
        Bucket *b = &grid[gy * grid_w + gx];
        if (b->count >= b->capacity) {
            b->capacity = b->capacity == 0 ? 8 : b->capacity * 2;
            b->indices = realloc(b->indices, b->capacity * sizeof(size_t));
        }
        b->indices[b->count++] = i;
    }

    // 3. For each point, search only neighboring buckets and themselves
    for (size_t i = 0; i < accumulators.count; ++i) {
        ref_point = accumulators.items[i];
        size_t gx = fmaxf(0, fminf(grid_w - 1, floorf(ref_point.x / bucket_size)));
        size_t gy = fmaxf(0, fminf(grid_h - 1, floorf(ref_point.y / bucket_size)));

        size_t row = (size_t)((ref_point.y - (spacing_g / 2.0f)) / spacing_g);
        size_t col = (size_t)((ref_point.x - (spacing_g / 2.0f)) / spacing_g);
        float point_intensity = GRID_ACCESS(grid_values, row, col);

        // Maximum filaments proportional to density
        size_t max_filaments = floor(point_intensity * 10); // Heuristic scale
        if (max_filaments < 2) max_filaments = 2;

        // Collect candidates from 3x3 buckets (local neighborhood)
        Vector2 *candidates = malloc(128 * sizeof(Vector2)); // Temporary small buffer
        size_t candidate_count = 0;
        size_t candidate_cap = 128;

        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                int nx = (int)gx + dx;
                int ny = (int)gy + dy;
                if (nx < 0 || nx >= (int)grid_w || ny < 0 || ny >= (int)grid_h) continue;

                Bucket *b = &grid[ny * grid_w + nx];
                for (size_t j = 0; j < b->count; ++j) {
                    size_t point_idx = b->indices[j];
                    if (point_idx == i) continue; // Skip self

                    if (candidate_count >= candidate_cap) {
                        candidate_cap *= 2;
                        candidates = realloc(candidates, candidate_cap * sizeof(Vector2));
                    }
                    candidates[candidate_count++] = accumulators.items[point_idx];
                }
            }
        }

        if (candidate_count > 0) {
            size_t num_connect = fmin(candidate_count, max_filaments);
            qselect(candidates, num_connect, candidate_count, sizeof(Vector2), points_compar);

            for (size_t j = 0; j < num_connect; ++j) {
                Vector2 closest = candidates[j];
                size_t j_row = (size_t)((closest.y - (spacing_g / 2.0f)) / spacing_g);
                size_t j_col = (size_t)((closest.x - (spacing_g / 2.0f)) / spacing_g);

                if (gaussian2d(j_col, j_row, col, row, stddev) < 0.1f) continue;
                DA_APPEND(connections, ((PointPair) {ref_point, closest}));
            }
        }
        free(candidates);
    }

    // Cleanup
    for (size_t i = 0; i < grid_w * grid_h; ++i) {
        if (grid[i].indices) free(grid[i].indices);
    }
    free(grid);
}

size_t do_n_iterations() {
    size_t iters = 0;
    while (true) {
        changed = false;
        expunge_gaussian();
        create_accumulators();
        connect_accumulators();
        iters++;
        if (changed == false) break;
    }
    return iters;
}

size_t get_num_accumulators() {
    return num_accumulators;
}
