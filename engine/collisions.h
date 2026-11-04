#include <stdbool.h>

typedef struct {
    float x, y;
    float width, height;
} RectangleCollisioner;

// typedef struct {
//     float x, y;
//     float radio;
// } CircleCollisioner;

bool check_collision_AABB(RectangleCollisioner r1, RectangleCollisioner r2);
// bool check_collision_circle(CircleCollisioner c1, CircleCollisioner c2);