/**
 * @file smoke.c
 * @brief Smoke tests for the HAL interface using mocks
 *
 * Demonstrates how to test application code through the mock HAL
 * without any hardware dependencies. Run on native_sim via twister.
 */

#include <zephyr/ztest.h>
#include "mock_hal.h"

static hal_iface_t hal;
static mock_hal_state_t mock;

static void before(void *fixture)
{
	ARG_UNUSED(fixture);
	mock_hal_create(&hal, &mock);
}

ZTEST_SUITE(smoke, NULL, NULL, before, NULL, NULL);

/* -- LED tests ----------------------------------------------------------- */

ZTEST(smoke, test_led_toggle)
{
	zassert_equal(hal.led_toggle(LED_1), 0);
	zassert_equal(mock.led_toggle_count[LED_1], 1);

	hal.led_toggle(LED_1);
	zassert_equal(mock.led_toggle_count[LED_1], 2);
}

ZTEST(smoke, test_led_toggle_invalid)
{
	int ret = hal.led_toggle(LED_COUNT);

	zassert_equal(ret, -EINVAL);
}

/* -- Button tests -------------------------------------------------------- */

static void dummy_btn_cb(btn_id_t btn)
{
	ARG_UNUSED(btn);
}

ZTEST(smoke, test_btn_register_cb)
{
	int ret = hal.btn_register_cb(BTN_1, dummy_btn_cb);

	zassert_equal(ret, 0);
	zassert_equal(mock.btn_register_cb_count, 1);
	zassert_equal(mock.registered_cb[BTN_1], dummy_btn_cb);
}

ZTEST(smoke, test_btn_register_cb_null)
{
	int ret = hal.btn_register_cb(BTN_1, NULL);

	zassert_equal(ret, -EINVAL);
}
