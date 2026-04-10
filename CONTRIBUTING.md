# Contributing

## Code style

Follow the [Zephyr coding guidelines](https://docs.zephyrproject.org/latest/contribute/coding_guidelines/index.html).
A `.clang-format` file is included -- run `clang-format -i <file>` before committing,
or enable format-on-save in your editor.

Key points:

- Tabs for indentation, tab width 8
- K&R brace style (opening brace on same line)
- `snake_case` for variables and functions, `UPPER_CASE` for macros, `_t` suffix for typedefs
- Use `LOG_INF` / `LOG_ERR` / `LOG_DBG` instead of `printk`
- HAL functions return `int` (0 success, negative errno on failure)

## Adding a new hardware capability

1. **Interface** -- add a function pointer to `hal_iface_t` in `include/hal_iface.h`.
2. **Implementation** -- implement the function using Zephyr drivers and wire it into the
   static `iface` struct in `src/hal.c`.
3. **Mock** -- add a matching stub in `tests/unit/mock_hal.h` and wire it into
   `mock_hal_create()`.
4. **Test** -- write a test in `tests/unit/src/` that exercises the new function through
   the mock. Add the test file to `tests/unit/CMakeLists.txt`.

## Adding tests

- Create a new `.c` file in `tests/unit/src/`.
- Add it to `target_sources(app PRIVATE ...)` in `tests/unit/CMakeLists.txt`.
- Use `ZTEST_SUITE` and `ZTEST` macros from `<zephyr/ztest.h>`.
- Tests must pass on `native_sim` without hardware.

## Running tests locally

```bash
west twister -T tests/unit -p native_sim -v
```

All tests must pass before submitting a pull request.

## Pull requests

- One logical change per PR.
- Include tests for new functionality.
- Make sure the CI pipeline passes (build + test + lint).
