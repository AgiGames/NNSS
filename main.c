#include "raylib.h"
#include "grid/grid.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#define WINDOW_SIZE 750
#define GRID_SLICES 300
#define ACCUMULATOR_PROB 0.025f

int main() {
    srand(time(NULL));

    InitWindow(WINDOW_SIZE, WINDOW_SIZE, "Filamenta");
    init_grid(WINDOW_SIZE, GRID_SLICES);
    scatter_accumulators(ACCUMULATOR_PROB);

    bool show_grid = true;
    bool color_accumulators = true;
    bool show_connections = true;
    bool show_iterations = true;
    bool show_accumulator_count = true;

    size_t iteration_count = 0;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        if (IsKeyPressed(KEY_R)) {
            free_grid();
            init_grid(WINDOW_SIZE, GRID_SLICES);
            scatter_accumulators(ACCUMULATOR_PROB);
            iteration_count = 0;
        }
        if (IsKeyPressed(KEY_G)) {
            show_grid = !show_grid;
        }
        if (IsKeyPressed(KEY_A)) {
            color_accumulators = !color_accumulators;
        }
        if (IsKeyPressed(KEY_C)) {
            show_connections = !show_connections;
        }
        if (IsKeyPressed(KEY_T)) {
            show_iterations = !show_iterations;
        }
        if (IsKeyPressed(KEY_Y)) {
            show_accumulator_count = !show_accumulator_count;
        }
        if (IsKeyDown(KEY_LEFT_SHIFT) && IsKeyPressed(KEY_I)) {
            iteration_count += do_n_iterations();
        }
        else if (IsKeyPressed(KEY_I)) {
            expunge_gaussian();
            create_accumulators();
            connect_accumulators();
            iteration_count++;
        }  
        
        // debug key listeners...
        // if (IsKeyPressed(KEY_L)) {
        //     create_accumulators();
        // }
        if (IsKeyPressed(KEY_E)) {
            expunge_gaussian();
        }

        if (show_grid) {
            color_grid(color_accumulators);
        }
        if (show_connections) {
            draw_accumulator_connections();
        }
        
        if (show_iterations) {
            const char* iteration_text = TextFormat("Iteration: %zu", iteration_count);
            DrawText(iteration_text, 10, WINDOW_SIZE - 60, 20, WHITE);
        }
        
        if (show_accumulator_count) {
            if (show_iterations) {
                // Draw bottom left if iterations are showing to keep them grouped
                const char* num_accumulators_text = TextFormat("Accumulators: %zu", get_num_accumulators());
                DrawText(num_accumulators_text, 10, WINDOW_SIZE - 30, 20, GREEN);
            } else {
                // Draw large top left if iterations aren't showing
                const char* num_accumulators_text = TextFormat("%zu", get_num_accumulators());
                DrawText(num_accumulators_text, 10, 10, 50, GREEN);
            }
        }

        EndDrawing();
    }

    free_grid();
    CloseWindow();
    return 0;
}
