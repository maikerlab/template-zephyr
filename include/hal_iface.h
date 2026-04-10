#pragma once

typedef enum
{
    LED_1,
    /* TODO: Define additional LEDs here */

    /** Keep this at the end of the enum to have a counter of all LEDs */
    LED_COUNT,
} led_id_t;

typedef enum
{
    BTN_1,
    /* TODO: Define additional buttons here */

    /** Keep this at the end of the enum to have a counter of all buttons */
    BTN_COUNT,
} btn_id_t;

/** Is called by HAL if a button was pressed. */
typedef void (*btn_callback_t)(btn_id_t btn);

/**
 * Abstract hardware interface.
 * In production code filled with real drivers, in tests with mocks.
 */
typedef struct
{
    /** Toggles the state of the specified LED. */
    int (*led_toggle)(led_id_t led);
    /** Registers a callback function for button press events. */
    int (*btn_register_cb)(btn_id_t btn, btn_callback_t cb);
    /* TODO: Add additional hardware interface functions here */
} hal_iface_t;
