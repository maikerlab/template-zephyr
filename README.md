# Zephyr Template

Zephyr RTOS starter template for the nRF7002 DK (nRF5340 + nRF7002 Wi-Fi). Uses a HAL
abstraction layer with function pointers for hardware access, enabling mock-based unit
testing via ztest on `native_sim` without any hardware dependencies.

## Prerequisites

- [nRF Connect SDK (NCS)](https://docs.nordicsemi.com/bundle/ncs-latest/page/nrf/index.html) v3.2.3
- West meta-tool (included with NCS)
- J-Link drivers (bundled with [nRF Command Line Tools](https://www.nordicsemi.com/Products/Development-tools/nrf-command-line-tools))

## Project Structure

```
├── CMakeLists.txt               # Root build configuration
├── prj.conf                     # Application Kconfig
├── boards/                      # Board-specific .conf and .overlay files
├── include/
│   └── hal_iface.h              # Public HAL interface (function pointers)
├── src/
│   ├── main.c                   # Application entry point
│   └── hal/
│       ├── hw_init.h            # Private -- hardware init header
│       └── hw_init.c            # Private -- hardware init implementation
└── tests/
    └── unit/
        ├── CMakeLists.txt       # Test build configuration
        ├── prj.conf             # Test Kconfig (enables ztest)
        ├── testcase.yaml        # Twister test metadata
        ├── mock_hal.h           # Mock HAL for unit tests
        └── src/
            └── smoke.c          # Smoke test suite
```

**Layout rules:**

- Public headers go in `include/`, private headers stay in `src/`
- Board-specific Kconfig: `boards/<board>.conf`, devicetree overlays: `boards/<board>.overlay`
- Sources are listed explicitly in `CMakeLists.txt` (never use `GLOB`)

## Getting Started

```bash
# Source the Zephyr environment
source /opt/nordic/ncs/v3.2.3/zephyr/zephyr-env.sh

# Build
west build -b nrf7002dk/nrf5340/cpuapp -p auto

# Flash
west flash

# Serial monitor (replace port with your device)
tio /dev/<port> -b 115200
```

## Architecture -- Writing Application Code

The project uses a HAL abstraction layer (`hal_iface_t`) to decouple application logic
from hardware drivers. This is a struct of function pointers defined in
`include/hal_iface.h`.

### How it works

1. **`include/hal_iface.h`** defines the interface -- a struct of function pointers
   representing hardware capabilities (e.g., `led_toggle`, `btn_register_cb`).

2. **`src/hal/hw_init.c`** implements each function pointer using real Zephyr drivers,
   wires them into a static `hal_iface_t` instance, and returns it from `hw_init()`.

3. **`src/main.c`** calls `hw_init()` to obtain the interface and uses it through the
   function pointers. Application code never calls drivers directly.

### Extending the interface

To add a new hardware capability (e.g., reading a sensor):

1. Add a function pointer to `hal_iface_t` in `include/hal_iface.h`:

```c
typedef struct {
	int (*led_toggle)(led_id_t led);
	int (*btn_register_cb)(btn_id_t btn, btn_callback_t cb);
	int (*sensor_read)(int *value);  /* new */
} hal_iface_t;
```

2. Implement and wire it in `src/hal/hw_init.c`:

```c
static int hw_sensor_read(int *value)
{
	/* Real driver call here */
	return 0;
}

static const hal_iface_t iface = {
	.led_toggle = hw_led_toggle,
	.btn_register_cb = hw_btn_register_cb,
	.sensor_read = hw_sensor_read,
};
```

3. Use it in application code:

```c
int val;
hw_ifc->sensor_read(&val);
```

4. Add any new `.c` files to `target_sources(app PRIVATE ...)` in the root
   `CMakeLists.txt`.

## Writing Tests

Tests use the ztest framework and construct their own `hal_iface_t` with mock function
pointers. Tests never link `hw_init.c`, so they have no hardware dependencies.

### Steps to add a test

1. **Add a mock** -- when you add a new function pointer to `hal_iface_t`, add a
   corresponding stub in `tests/unit/mock_hal.h` and wire it into `mock_hal_create()`:

```c
static int mock_sensor_read(int *value)
{
	(void)value;
	return 0;
}

static inline hal_iface_t mock_hal_create(void)
{
	return (hal_iface_t){
		.led_toggle = mock_led_toggle,
		.btn_register_cb = mock_btn_register_cb,
		.sensor_read = mock_sensor_read,
	};
}
```

2. **Create a test file** -- add a new `.c` file in `tests/unit/src/`:

```c
#include <zephyr/ztest.h>
#include "mock_hal.h"

ZTEST_SUITE(my_feature, NULL, NULL, NULL, NULL, NULL);

ZTEST(my_feature, test_something)
{
	hal_iface_t hal = mock_hal_create();
	int val;
	zassert_equal(hal.sensor_read(&val), 0);
}
```

3. **Register the source** -- add the file to `target_sources(app PRIVATE ...)` in
   `tests/unit/CMakeLists.txt`:

```cmake
target_sources(app PRIVATE
    src/smoke.c
    src/my_feature.c
)
```

## Running Tests

### On host with native_sim (no hardware needed)

```bash
# Run all unit tests
west twister -T tests/unit -p native_sim

# Verbose output
west twister -T tests/unit -p native_sim -v
```

### On-device on the nRF7002 DK

```bash
# Build tests for the board
west build -b nrf7002dk/nrf5340/cpuapp -p always tests/unit

# Flash and observe output on serial (115200 baud)
west flash
```

Test results appear on the serial console. Example output:

```
*** Using Zephyr OS v4.2.99 ***
Running TESTSUITE smoke
===================================================================
START - test_initial
 PASS - test_initial in 0.001 seconds
===================================================================
TESTSUITE smoke succeeded.
------ TESTSUITE SUMMARY START ------
SUITE PASS -  100.00% [smoke]: pass = 1, fail = 0, skip = 0, total = 1
------ TESTSUITE SUMMARY END ------
===================================================================
PROJECT EXECUTION SUCCESSFUL
```

## Code Style

Follows [Zephyr coding guidelines](https://docs.zephyrproject.org/latest/contribute/coding_guidelines/index.html):

- Indentation: tabs (not spaces)
- Brace style: K&R (opening brace on same line)
- Naming: `snake_case` for variables/functions, `UPPER_CASE` for macros, `_t` suffix for typedefs
- Logging: use `LOG_INF`, `LOG_ERR`, etc. instead of `printk`
- HAL functions return `int` (0 on success, negative errno on failure)
