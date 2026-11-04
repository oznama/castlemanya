// #include <math.h>
#include "collisions.h"

bool check_collision_AABB(RectangleCollisioner r1, RectangleCollisioner r2) {
    return (r1.x < r2.x + r2.width) && (r1.x + r1.width > r2.x) &&
           (r1.y < r2.y + r2.height) && (r1.y + r1.height > r2.y);
}

// bool check_collision_circle(CircleCollisioner c1, CircleCollisioner c2) {
//     float dx = c1.x - c2.x;
//     float dy = c1.y - c2.y;
//     float distance = sqrt(dx * dx + dy * dy);
//     return distance < (c1.radio + c2.radio);
// }