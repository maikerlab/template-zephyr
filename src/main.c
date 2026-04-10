/**
 * @file main.c
 * @brief Application entry point
 *
 * Demonstrates the HAL abstraction pattern: initializes hardware via hw_init(),
 * registers a button callback that toggles an LED on press, then idles forever.
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include "hal.h"

LOG_MODULE_REGISTER(main, LOG_LEVEL_DBG);

static const hal_iface_t *hw;

static void on_btn_pressed(btn_id_t btn)
{
	LOG_INF("Button %d pressed", btn);
	hw->led_toggle(LED_1);
}

int main(void)
{
	LOG_INF("Hello, World!");

	hw = hw_init();
	if (hw == NULL) {
		LOG_ERR("Hardware initialization failed");
		return -1;
	}

	if (hw->btn_register_cb(BTN_1, on_btn_pressed) < 0) {
		LOG_ERR("Failed to register button callback");
		return -1;
	}

	/* Keep the main thread alive so ISR-driven callbacks continue to work */
	k_sleep(K_FOREVER);
	return 0;
}
