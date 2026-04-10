#include <zephyr/logging/log.h>
#include "hal/hw_init.h"

LOG_MODULE_REGISTER(main, LOG_LEVEL_DBG);

static void on_btn_pressed(btn_id_t btn)
{
	LOG_INF("Button %d pressed", btn);
}

int main(void)
{
	LOG_INF("Hello, World!");

	const hal_iface_t *hw_ifc = hw_init();
	if (hw_ifc == NULL) {
		LOG_ERR("Hardware initialization failed");
		return -1;
	}

	hw_ifc->btn_register_cb(BTN_1, on_btn_pressed);
    hw_ifc->led_toggle(LED_1);

	return 0;
}
