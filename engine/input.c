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

void input_read(uint16_t *held_buttons, uint16_t *pressed_buttons) {
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

	*held_buttons = buttons;
	*pressed_buttons = buttons & (uint16_t) ~previous_buttons;
	previous_buttons = buttons;
}