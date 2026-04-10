/**
 * @file hal.h
 * @brief Private hardware initialisation header
 *
 * Declares hw_init(), which configures GPIOs and returns the production
 * HAL interface. This header is private to src/ -- tests never include it.
 */

#pragma once

#include "hal_iface.h"

/**
 * @brief Initialise hardware and return the HAL interface.
 *
 * Configures GPIO for LEDs (output) and buttons (input with edge interrupt).
 * Must be called once during application startup.
 *
 * @return Pointer to a static hal_iface_t on success, or NULL if any GPIO
 *         initialisation step fails (errors are logged via LOG_ERR).
 */
const hal_iface_t *hw_init(void);
