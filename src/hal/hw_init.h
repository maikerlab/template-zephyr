#pragma once

#include "hal_iface.h"

/**
 * Initializes the hardware and returns a pointer to the hardware interface.
 * @return Pointer to the hardware interface, or NULL on failure.
 */
const hal_iface_t *hw_init(void);
