#ifndef ADAPTER_H
#define ADAPTER_H

#include "adapter_includes.h"

void adapter_timer_reset();

bool adapter_usb_is_clear();

void adapter_usb_set_clear(uint8_t itf);

void adapter_usb_unset_clear();

// This function should return a bool after indicating whether or not a hardware test passed.
bool cb_adapter_hardware_test();

void adapter_mode_cycle(bool forwards);

input_mode_t adapter_get_current_mode();

bool adapter_usb_start(input_mode_t mode);

void adapter_main_init();
void adapter_main_loop();

#endif