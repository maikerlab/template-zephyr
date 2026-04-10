/**
 * @file mock_hal.h
 * @brief Mock HAL implementation for unit tests
 *
 * Provides stub implementations of hal_iface_t that record every call
 * (counts, arguments) into a mock_hal_state_t struct. Tests inspect
 * that state to verify application behaviour without real hardware.
 *
 * Usage:
 * @code
 *     mock_hal_state_t state;
 *     hal_iface_t hal;
 *     mock_hal_create(&hal, &state);
 *
 *     hal.led_toggle(LED_1);
 *     zassert_equal(state.led_toggle_count[LED_1], 1);
 * @endcode
 */

#pragma once

#include <errno.h>
#include <string.h>
#include "hal_iface.h"

/** Tracks calls made through the mock HAL functions. */
typedef struct {
	/** Registered button callbacks, indexed by btn_id_t. */
	btn_callback_t registered_cb[BTN_COUNT];
	/** Total number of btn_register_cb calls. */
	int btn_register_cb_count;

	/** Number of led_toggle calls per LED. */
	int led_toggle_count[LED_COUNT];
	/** Last LED ID passed to led_toggle. */
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

/**
 * @brief Reset all tracked state to zero.
 * @param state Mock state to clear.
 */
static inline void mock_hal_reset(mock_hal_state_t *state)
{
	memset(state, 0, sizeof(*state));
}

/**
 * @brief Populate a hal_iface_t with mock functions and bind it to @p state.
 *
 * Also zeroes @p state. Call this in a test fixture's "before" function so
 * each test starts with a clean mock.
 *
 * @param iface HAL interface to populate.
 * @param state Mock state that will record subsequent calls.
 */
static inline void mock_hal_create(hal_iface_t *iface, mock_hal_state_t *state)
{
	memset(state, 0, sizeof(*state));
	mock_hal_active_state = state;

	iface->led_toggle = mock_led_toggle;
	iface->btn_register_cb = mock_btn_register_cb;
}
