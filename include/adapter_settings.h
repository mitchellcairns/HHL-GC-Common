#ifndef ADAPTER_SETTINGS_H
#define ADAPTER_SETTINGS_H

#include "adapter_includes.h"

extern adapter_settings_s global_loaded_settings;

bool settings_load();
void settings_core0_save_check();
void settings_core1_save_check();
void settings_save_webindicate();
void settings_save();
void settings_set_mode(input_mode_t mode);
void settings_reset_to_default();

#endif
