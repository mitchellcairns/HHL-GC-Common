#ifndef JOYBUS_H
#define JOYBUS_H

#include "adapter_includes.h"

void joybus_init();

// Define these functions in user-space
void joybus_itf_init();

// This function causes the joybus to poll
void joybus_itf_poll(joybus_input_s **out);

void joybus_itf_enable_rumble(uint8_t interface, bool enable);

#endif