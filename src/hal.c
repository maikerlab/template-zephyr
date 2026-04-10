/**
 * @file hal.c
 * @brief HAL implementation using Zephyr GPIO drivers
 *
 * Maps the abstract hal_iface_t to real hardware via devicetree aliases
 * (led0, sw0). Manages LED output, button input with edge-triggered
 * interrupts, and callback dispatch.
 */

#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include "hal.h"

LOG_MODULE_REGISTER(hal, LOG_LEVEL_DBG);

/* GPIO specs obtained from devicetree aliases, indexed by led/btn ID */
static const struct gpio_dt_spec leds[] = {
	[LED_1] = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios),
};

static const struct gpio_dt_spec btns[] = {
	[BTN_1] = GPIO_DT_SPEC_GET(DT_ALIAS(sw0), gpios),
};

/* Per-button callback storage and GPIO callback structs */
static btn_callback_t btn_callbacks[BTN_COUNT];
static struct gpio_callback btn_cb_data[BTN_COUNT];

/**
 * @brief GPIO ISR for button presses.
 *
 * Checks each button's pin against the triggered pin mask and dispatches
 * the registered callback, if any.
 */
static void gpio_btn_isr(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	for (int i = 0; i < BTN_COUNT; i++) {
		if ((pins & BIT(btns[i].pin)) && btn_callbacks[i]) {
			btn_callbacks[i]((btn_id_t)i);
		}
	}
}

static int hw_led_toggle(led_id_t led)
{
	if (led >= LED_COUNT) {
		return -EINVAL;
	}
	return gpio_pin_toggle_dt(&leds[led]);
}

static int hw_btn_register_cb(btn_id_t btn, btn_callback_t cb)
{
	if (btn >= BTN_COUNT) {
		return -EINVAL;
	}
	btn_callbacks[btn] = cb;
	return 0;
}

static const hal_iface_t iface = {
	.led_toggle = hw_led_toggle,
	.btn_register_cb = hw_btn_register_cb,
};

const hal_iface_t *hw_init(void)
{
	for (int i = 0; i < LED_COUNT; i++) {
		if (!gpio_is_ready_dt(&leds[i])) {
			LOG_ERR("LED %d GPIO not ready", i);
			return NULL;
		}
		if (gpio_pin_configure_dt(&leds[i], GPIO_OUTPUT_INACTIVE) < 0) {
			LOG_ERR("Failed to configure LED %d", i);
			return NULL;
		}
	}

	for (int i = 0; i < BTN_COUNT; i++) {
		if (!gpio_is_ready_dt(&btns[i])) {
			LOG_ERR("Button %d GPIO not ready", i);
			return NULL;
		}
		if (gpio_pin_configure_dt(&btns[i], GPIO_INPUT) < 0) {
			LOG_ERR("Failed to configure button %d", i);
			return NULL;
		}
		if (gpio_pin_interrupt_configure_dt(&btns[i], GPIO_INT_EDGE_TO_ACTIVE) < 0) {
			LOG_ERR("Failed to configure button %d interrupt", i);
			return NULL;
		}
		gpio_init_callback(&btn_cb_data[i], gpio_btn_isr, BIT(btns[i].pin));
		if (gpio_add_callback(btns[i].port, &btn_cb_data[i]) < 0) {
			LOG_ERR("Failed to add button %d callback", i);
			return NULL;
		}
	}

	LOG_INF("Hardware initialized");
	return &iface;
}
