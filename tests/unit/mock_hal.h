#pragma once

#include "hal_iface.h"

static int mock_led_toggle(led_id_t led)
{
	(void)led;
	return 0;
}

static int mock_btn_register_cb(btn_id_t btn, btn_callback_t cb)
{
	(void)btn;
	(void)cb;
	return 0;
}

static inline hal_iface_t mock_hal_create(void)
{
	return (hal_iface_t){
		.led_toggle = mock_led_toggle,
		.btn_register_cb = mock_btn_register_cb,
	};
}
