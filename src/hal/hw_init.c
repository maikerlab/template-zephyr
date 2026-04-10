#include <zephyr/logging/log.h>
#include "hw_init.h"

LOG_MODULE_REGISTER(hw_init, LOG_LEVEL_DBG);

static int hw_led_toggle(led_id_t led)
{
	/* TODO: Implement real GPIO toggle logic */
	return 0;
}

static int hw_btn_register_cb(btn_id_t btn, btn_callback_t cb)
{
	/* TODO: Implement real GPIO interrupt callback registration */
	return 0;
}

static const hal_iface_t iface = {
	.led_toggle = hw_led_toggle,
	.btn_register_cb = hw_btn_register_cb,
};

const hal_iface_t *hw_init(void)
{
	/* TODO: Add hardware initialization here, return NULL on failure */
	return &iface;
}
