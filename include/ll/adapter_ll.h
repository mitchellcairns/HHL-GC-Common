
#ifndef ADAPTER_LL_H
#define ADAPTER_LL_H

#include "adapter_includes.h"

void adapter_ll_write_settings_to_memory(adapter_settings_s *input);
void adapter_ll_load_settings_from_memory(adapter_settings_s *output);
void adapter_ll_save_check();
uint8_t adapter_ll_generate_random_8();
void adapter_ll_reboot_bootloader();
void adapter_ll_reboot_with_memory(adapter_reboot_memory_u *msg);
void adapter_ll_get_reboot_memory(adapter_reboot_memory_u *msg);
bool adapter_ll_gpio_read(uint32_t gpio);
void adapter_ll_hardware_setup();
uint32_t adapter_ll_get_timestamp_us_32();

#endif