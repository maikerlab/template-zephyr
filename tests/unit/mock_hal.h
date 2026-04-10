#pragma once

#include <errno.h>
#include <string.h>
#include "hal_iface.h"

typedef struct {
	/* btn_register_cb tracking (per button) */
	btn_callback_t registered_cb[BTN_COUNT];
	int btn_register_cb_count;

	/* led_toggle tracking (per LED) */
	int led_toggle_count[LED_COUNT];
	led_id_t led_toggle_last;
} mock_hal_state_t;

/*
 * File-scoped pointer to the active mock state.
 * Safe because ztest runs tests sequentially in a single thread.
 */
static mock_hal_state_t *mock_hal_active_state;

static int mock_led_toggle(led_id_t led)
{
	if (led < 0 || led >= LED_COUNT) {
		return -EINVAL;
	}

	mock_hal_active_state->led_toggle_count[led]++;
	mock_hal_active_state->led_toggle_last = led;
	return 0;
}

static int mock_btn_register_cb(btn_id_t btn, btn_callback_t cb)
{
	if (btn < 0 || btn >= BTN_COUNT) {
		return -EINVAL;
	}
	if (cb == NULL) {
		return -EINVAL;
	}

	mock_hal_active_state->registered_cb[btn] = cb;
	mock_hal_active_state->btn_register_cb_count++;
	return 0;
}

static inline void mock_hal_reset(mock_hal_state_t *state)
{
	memset(state, 0, sizeof(*state));
}

static inline void mock_hal_create(hal_iface_t *iface, mock_hal_state_t *state)
{
	memset(state, 0, sizeof(*state));
	mock_hal_active_state = state;

	iface->led_toggle = mock_led_toggle;
	iface->btn_register_cb = mock_btn_register_cb;
}
