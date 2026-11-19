#define DOOR_W_BASE 13
#define DOOR_H 81
#define DOOR_SPRITES 10

void door_init(void);
void door_render_instance(int32_t x, int32_t y, int32_t cam_x, int32_t cam_y, uint8_t rotateu, uint8_t open, uint8_t *io_frame, uint8_t *io_timer);