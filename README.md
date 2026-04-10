# Zephyr Application Template

A maintainable, testable starter template for Zephyr RTOS applications. Ships with a
HAL abstraction layer (function-pointer interface), mock-based unit tests on `native_sim`,
and GitHub Actions CI.

The default target board is the **nRF7002 DK** (nRF5340 + nRF7002 Wi-Fi), but the
template is board-agnostic -- see [Adapting to another board](#adapting-to-another-board).

## Prerequisites

- [nRF Connect SDK (NCS)](https://docs.nordicsemi.com/bundle/ncs-latest/page/nrf/index.html) v3.2.3
  (or vanilla Zephyr -- see [west.yml](west.yml) comments)
- West meta-tool (included with NCS)
- J-Link drivers (bundled with [nRF Command Line Tools](https://www.nordicsemi.com/Products/Development-tools/nrf-command-line-tools))

## Project Structure

```
├── CMakeLists.txt               # Root build configuration
├── prj.conf                     # Application Kconfig
├── west.yml                     # West manifest (SDK version)
├── boards/                      # Board-specific .conf and .overlay files
├── include/
│   └── hal_iface.h              # Public HAL interface (function pointers)
├── src/
│   ├── main.c                   # Application entry point
│   ├── hal.h                    # Private -- hardware init header
│   └── hal.c                    # Private -- hardware init implementation
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

- Public headers go in `include/`, private headers stay in `src/`.
- Board-specific Kconfig: `boards/<board>.conf`, devicetree overlays: `boards/<board>.overlay`.
- Sources are listed explicitly in `CMakeLists.txt` (never use `GLOB`).

## Getting Started

```bash
# Source the Zephyr environment (NCS example)
export PATH="/opt/nordic/ncs/toolchains/<hash>/bin:$PATH"
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

2. **`src/hal.c`** implements each function pointer using real Zephyr drivers,
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

2. Implement and wire it in `src/hal.c`:

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
hw->sensor_read(&val);
```

4. Add any new `.c` files to `target_sources(app PRIVATE ...)` in the root
   `CMakeLists.txt`.

## Writing Tests

Tests use the ztest framework and construct their own `hal_iface_t` with mock function
pointers. Tests never link `hal.c`, so they have no hardware dependencies.

### Steps to add a test

1. **Add a mock** -- when you add a new function pointer to `hal_iface_t`, add a
   corresponding stub in `tests/unit/mock_hal.h` and wire it into `mock_hal_create()`:

```c
static int mock_sensor_read(int *value)
{
	(void)value;
	return 0;
}

/* inside mock_hal_create(): */
iface->sensor_read = mock_sensor_read;
```

2. **Create a test file** -- add a new `.c` file in `tests/unit/src/`:

```c
#include <zephyr/ztest.h>
#include "mock_hal.h"

static hal_iface_t hal;
static mock_hal_state_t mock;

static void before(void *fixture)
{
	ARG_UNUSED(fixture);
	mock_hal_create(&hal, &mock);
}

ZTEST_SUITE(my_feature, NULL, NULL, before, NULL, NULL);

ZTEST(my_feature, test_something)
{
	int val;
	zassert_equal(hal.sensor_read(&val), 0);
}
```

3. **Register the source** -- add the file to `target_sources(app PRIVATE ...)` in
   `tests/unit/CMakeLists.txt`.

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
START - test_led_toggle
 PASS - test_led_toggle in 0.001 seconds
START - test_led_toggle_invalid
 PASS - test_led_toggle_invalid in 0.001 seconds
START - test_btn_register_cb
 PASS - test_btn_register_cb in 0.001 seconds
START - test_btn_register_cb_null
 PASS - test_btn_register_cb_null in 0.001 seconds
===================================================================
TESTSUITE smoke succeeded.
------ TESTSUITE SUMMARY START ------
SUITE PASS - 100.00% [smoke]: pass = 4, fail = 0, skip = 0, total = 4
------ TESTSUITE SUMMARY END ------
===================================================================
PROJECT EXECUTION SUCCESSFUL
```

## Adapting to Another Board

This template works with any Zephyr-supported board. To retarget:

1. **Change the build target:**

```bash
west build -b <your_board> -p auto
```

2. **Add board overlays** (if needed) in the `boards/` directory:
   - `boards/<board>.conf` -- Kconfig overrides for your board.
   - `boards/<board>.overlay` -- devicetree customisation (pin mappings, peripherals).

3. **Check devicetree aliases** -- the HAL expects `led0` and `sw0` aliases. Most
   Zephyr boards define these by default. If yours doesn't, add them in your overlay:

```dts
/ {
	aliases {
		led0 = &my_led;
		sw0 = &my_button;
	};
};
```

4. **Update `west.yml`** (if not using NCS) -- see the comments in `west.yml` for how to
   point at vanilla Zephyr instead of the nRF Connect SDK.

## CI

The GitHub Actions workflow (`.github/workflows/ci.yml`) runs on every push and PR to
`main`. It has three jobs:

- **build** -- compiles firmware for the nRF7002 DK and uploads `.hex` / `.bin` artifacts.
- **test** -- runs unit tests on `native_sim` via twister.
- **lint** -- checks code formatting with `clang-format`.

## Troubleshooting

| Problem | Solution |
|---------|----------|
| `GPIO not ready` at runtime | Check that the devicetree aliases (`led0`, `sw0`) exist for your board. Inspect `build/zephyr/zephyr.dts`. |
| Build errors after renaming overlay files | Use `-p always` for a pristine rebuild. |
| `west update` reset my local changes | `west update` checks out pinned revisions. Never edit files under `zephyr/`, `modules/`, `nrf/`. |
| Serial port not found | Check the port path. On macOS use `ls /dev/cu.usb*`, on Linux use `ls /dev/ttyACM*`. |
| Kconfig symbol silently set to `n` | Check unmet dependencies with `west build -t menuconfig`. |

## Code Style

Follows [Zephyr coding guidelines](https://docs.zephyrproject.org/latest/contribute/coding_guidelines/index.html).
A `.clang-format` and `.editorconfig` are included. See [CONTRIBUTING.md](CONTRIBUTING.md)
for details.
