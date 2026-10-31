#include <psxapi.h>
#include <psxpad.h>

#include "input.h"

static uint8_t pad_buffer[2][34];
static uint16_t previous_buttons;

void input_init(void) {
	EnterCriticalSection();
	InitPAD(
		pad_buffer[0], sizeof(pad_buffer[0]),
		pad_buffer[1], sizeof(pad_buffer[1])
	);
	StartPAD();
	ChangeClearPAD(0);
	ExitCriticalSection();
}

uint16_t input_read_pressed(void) {
	PADTYPE *pad = (PADTYPE *) pad_buffer[0];
	uint16_t buttons = 0;

	if (
		(pad->stat == 0) &&
		((pad->type == PAD_ID_DIGITAL) ||
		 (pad->type == PAD_ID_ANALOG_STICK) ||
		 (pad->type == PAD_ID_ANALOG))
	) {
		buttons = (uint16_t) ~pad->btn;
	}

	uint16_t pressed_buttons = buttons & (uint16_t) ~previous_buttons;
	previous_buttons = buttons;

	return pressed_buttons;
}
