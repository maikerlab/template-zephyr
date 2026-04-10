**EMBEDDER PROJECT CONTEXT**
<OVERVIEW>
Name = maikerlab_zephyr_template
Target MCU = nRF5340 (dual-core Arm Cortex-M33) + nRF7002 Wi-Fi companion IC
Board = nrf7002dk/nrf5340/cpuapp
Toolchain = nRF Connect SDK (NCS) v3.2.3 / West / Zephyr CMake
Toolchain Path = /opt/nordic/ncs
Debug Interface = jlink (onboard SEGGER J-Link on nRF7002 DK)
RTOS / SDK = Zephyr RTOS via nRF Connect SDK
Project Summary = Generic Zephyr starter template for the nRF7002 DK.
</OVERVIEW>

<COMMANDS>
# --- Build / Compile --------------------------------------------------------
build_command = west build -b nrf7002dk/nrf5340/cpuapp -p auto

# --- Flash ------------------------------------------------------------------
flash_command = west flash

# --- Debug ------------------------------------------------------------------
gdb_server_command = JLinkGDBServer -device nRF5340_xxAA_APP -if SWD -speed 4000 -port 61234
gdb_server_host = localhost
gdb_server_port = 61234
# gdb_client_command =
target_connection = remote

# --- Serial Monitor ----------------------------------------------------------
serial_port = auto
serial_baudrate = 115200
serial_monitor_command = tio {port} -b {baud}
serial_monitor_interactive = true
serial_encoding = ascii
serial_startup_commands = []
</COMMANDS>

# Project Overview

A reusable Zephyr starter template targeting the nRF7002 DK (nRF5340 + nRF7002 Wi-Fi). Uses a HAL abstraction layer (`hal_iface_t` in `include/hal_iface.h`) with function pointers for hardware access, enabling mock-based unit testing via ztest on `native_sim`. Production hardware is initialized through `src/hal/hw_init.c` (private), while tests construct their own `hal_iface_t` with mock implementations.

# Bash Commands

```bash
# Build
west build -b nrf7002dk/nrf5340/cpuapp -p auto

# Build (pristine)
west build -b nrf7002dk/nrf5340/cpuapp -p always

# Flash
west flash

# Menuconfig (interactive Kconfig)
west build -t menuconfig

# Clean build directory
rm -rf build

# Run unit tests (twister)
west twister -T tests/unit -p native_sim

# Run unit tests (verbose)
west twister -T tests/unit -p native_sim -v
```

# Code Style

Follow [Zephyr coding guidelines](https://docs.zephyrproject.org/latest/contribute/coding_guidelines/index.html):
- Indentation: tabs (not spaces)
- Brace style: K&R (opening brace on same line for functions, structs, control flow)
- Use Zephyr logging (`LOG_INF`, `LOG_ERR`, etc.) instead of `printk`
- Use Zephyr kernel APIs (`k_msgq`, `k_thread`, `k_sleep`, etc.)
- Snake_case for variables, functions, types; UPPER_CASE for macros and constants
- Typedef structs with `_t` suffix
- Devicetree overlays in `boards/`, never edit SDK `.dts` files
- Kconfig in `prj.conf`, board-specific overrides in `boards/<board>.conf`
- HAL interface (`hal_iface_t`) uses function pointers for hardware abstraction
- Public headers in `include/`, private implementation headers in `src/`
- `hw_init.h`/`hw_init.c` are private -- tests create their own `hal_iface_t` with mocks
- HAL functions return `int` (0 success, negative errno) per Zephyr convention
- List sources explicitly in CMakeLists.txt, do not use `GLOB_RECURSE`

# Testing

- `tests/` is the root directory for all tests
- `tests/unit/` contains unit tests using ztest on `native_sim`
- Unit test infrastructure:
  - `tests/unit/CMakeLists.txt` -- standalone Zephyr project; includes `../../include` and `../../src`; add test source files to `target_sources(app PRIVATE ...)`
  - `tests/unit/prj.conf` -- enables `CONFIG_ZTEST=y` and `CONFIG_LOG=y`
  - `tests/unit/testcase.yaml` -- twister test metadata; allowed platforms: `native_sim`, `nrf7002dk/nrf5340/cpuapp`; harness: `ztest`
- Tests construct their own `hal_iface_t` with mock function pointers -- never link `hw_init.c` in tests
- Add new test source files explicitly in `tests/unit/CMakeLists.txt`
