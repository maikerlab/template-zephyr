#include <zephyr/ztest.h>
#include "mock_hal.h"

ZTEST_SUITE(smoke, NULL, NULL, NULL, NULL, NULL);

ZTEST(smoke, test_initial)
{
	zassert_true(true, "This test always passes -- replace with real tests");
}
