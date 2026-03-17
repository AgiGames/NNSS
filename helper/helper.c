#include "helper.h"
#include <stdbool.h>
#include <math.h>
#include <stddef.h>
#include <limits.h>

#include "raylib.h"
#include "raymath.h"
#include "../globals/globals.h"

float gaussian1d(float x, float mean, size_t stddev) {
    float dx = x - mean;
    float exponent = (-0.5f * (dx * dx)) / (float) (stddev * stddev);
    return expf(exponent);
}

float gaussian2d(float x, float y, float mean_x, float mean_y, size_t stddev)
{
    float dx = x - mean_x;
    float dy = y - mean_y;

    float exponent = (-0.5f * (dx*dx + dy*dy)) / (float) (stddev * stddev);
    return expf(exponent);
}

bool float_equal(float a, float b) {
    float diff = fabsf(a - b);
    float largest = fmaxf(fabsf(a), fabsf(b));
    return diff <= largest * 1e-6f;
}

Color color_lerp(Color c1, Color c2, float t) {
    return (Color){
        (unsigned char)(c1.r + (c2.r - c1.r) * t),
        (unsigned char)(c1.g + (c2.g - c1.g) * t),
        (unsigned char)(c1.b + (c2.b - c1.b) * t),
        255
    };
}

Color heatmap_cmap(float intensity) {
    // Define color nodes for the cosmic web gradient
    Color c0 = {5, 5, 15, 255};     // Deep space black/blue
    Color c1 = {30, 10, 60, 255};    // Faint deep purple
    Color c2 = {80, 25, 120, 255};   // Vibrant purple/blue
    Color c3 = {150, 100, 255, 255}; // Bright cyan/purple node
    Color c4 = {255, 255, 255, 255}; // Pure glowing white

    if (intensity <= 0.05f) {
        float t = intensity / 0.05f;
        return color_lerp(c0, c1, t);
    } else if (intensity <= 0.3f) {
        float t = (intensity - 0.05f) / 0.25f;
        return color_lerp(c1, c2, t);
    } else if (intensity <= 0.7f) {
        float t = (intensity - 0.3f) / 0.4f;
        return color_lerp(c2, c3, t);
    } else {
        float t = (intensity - 0.7f) / 0.3f;
        return color_lerp(c3, c4, t);
    }
}

int points_compar(const void *a, const void *b) {
    const Vector2 *pa = a;
    const Vector2 *pb = b;

    float da = Vector2Distance(ref_point, *pa);
    float db = Vector2Distance(ref_point, *pb);

    if (da < db) return -1;
    if (da > db) return 1;
    return 0;
}

void void_swap(void* a, void* b, size_t size) {
    char* cpa = a;
    char* cpb = b;
    for (size_t i = 0; i < size; ++i) {
        char temp = cpa[i];
        cpa[i] = cpb[i];
        cpb[i] = temp;
    }
}

size_t partition(void* base, size_t l, size_t r, size_t element_size, int (*cmp)(const void*, const void*)) {
    void* pivot = MEMB_SELECT(base, r, element_size);
    size_t i = l;
    for (size_t j = l; j <= r - 1; ++j) {
        void* j_element = MEMB_SELECT(base, j, element_size);
        if (cmp(j_element, pivot) <= 0) {
            void* i_element = MEMB_SELECT(base, i, element_size);
            void_swap(i_element, j_element, element_size);
            ++i;
        }
    }
    void* i_element = MEMB_SELECT(base, i, element_size);
    void_swap(i_element, pivot, element_size);
    return i;
}

size_t qselect_recur(void* base, size_t l, size_t r, size_t k, 
    size_t num_elements, size_t element_size, int (*cmp)(const void*, const void*)) {
    if (k > 0 && k <= num_elements) {
        int partition_index = partition(base, l, r, element_size, cmp);

        if (partition_index == k - 1) {
            return partition_index;
        }

        if (partition_index > k - 1) {
            return qselect_recur(base, l, partition_index - 1, k, num_elements, element_size, cmp);
        }

        return qselect_recur(base, partition_index + 1, r, k, num_elements, element_size, cmp);
    }

    return INT_MAX;
}

size_t qselect(void* base, size_t k, size_t num_elements, size_t element_size, int (*cmp)(const void*, const void*)) {
    return qselect_recur(base, 0, num_elements - 1, k, num_elements, element_size, cmp);
}
