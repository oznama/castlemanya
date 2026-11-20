#define DOOR_W_BASE 13
#define DOOR_H 81
#define DOOR_SPRITES 10

typedef enum
{
	TRANSITION_NONE = 0,
	TRANSITION_ENTER_DOOR,
	TRANSITION_EXIT_DOOR
} DoorTransitionType;

void door_init(void);
void door_render_instance(int32_t x, int32_t y, int32_t cam_x, int32_t cam_y, uint8_t rotateu, uint8_t open, uint8_t *io_frame, uint8_t *io_timer);