#include <math.h>

float gaussian2d_1std(float x, float y, float mean_x, float mean_y)
{
    float dx = x - mean_x;
    float dy = y - mean_y;

    float exponent = -0.5f * (dx*dx + dy*dy);
    return expf(exponent);
}
