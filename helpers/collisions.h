#include <stdbool.h>

typedef struct {
    float x, y;
    float width, height;
} RectangleCollisioner;

bool check_collision_AABB(RectangleCollisioner r1, RectangleCollisioner r2);