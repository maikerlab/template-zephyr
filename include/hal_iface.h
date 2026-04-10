/**
 * @file hal_iface.h
 * @brief Public Hardware Abstraction Layer interface
 *
 * Defines the HAL as a struct of function pointers. Production code fills
 * these with real Zephyr drivers (see hal.c); tests inject mocks (see
 * tests/unit/mock_hal.h). Application code only depends on this header,
 * keeping it decoupled from hardware specifics.
 *
 * All HAL functions return 0 on success, negative errno on failure.
 */

#pragma once

/** LED identifiers. */
typedef enum {
	LED_1,
	/* TODO: Define additional LEDs here */

	/** Sentinel -- keep last. Equals the number of LEDs defined above. */
	LED_COUNT,
} led_id_t;

/** Button identifiers. */
typedef enum {
	BTN_1,
	/* TODO: Define additional buttons here */

	/** Sentinel -- keep last. Equals the number of buttons defined above. */
	BTN_COUNT,
} btn_id_t;

/** Callback invoked by the HAL when a button is pressed. */
typedef void (*btn_callback_t)(btn_id_t btn);

/**
 * Abstract hardware interface.
 * In production code filled with real drivers, in tests with mocks.
 */
typedef struct {
	/** Toggle the state of the specified LED. */
	int (*led_toggle)(led_id_t led);

	/** Register a callback for button press events. */
	int (*btn_register_cb)(btn_id_t btn, btn_callback_t cb);

	/* TODO: Add additional hardware interface functions here */
} hal_iface_t;
