#ifndef ADAPTER_H
#define ADAPTER_H

#include "adapter_includes.h"

void adapter_mode_cycle(bool forwards);

input_mode_t adapter_get_current_mode();

bool adapter_usb_start(input_mode_t mode);

void adapter_main_init();
void adapter_main_loop();

#endif