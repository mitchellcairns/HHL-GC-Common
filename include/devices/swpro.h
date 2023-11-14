#ifndef SWPRO_H
#define SWPRO_H

#include "adapter_includes.h"

#define SWPRO_HID_REPORT_LEN (ADAPTER_MCU_TYPE==MCU_TYPE_ESP32 ? 63 : 64)

extern const tusb_desc_device_t swpro_device_descriptor;
extern const uint8_t swpro_hid_report_descriptor[];
extern const uint8_t swpro_configuration_descriptor[];

void swpro_hid_idle(joybus_input_s *joybus_data);
void swpro_hid_report(joybus_input_s *joybus_data);

#endif
