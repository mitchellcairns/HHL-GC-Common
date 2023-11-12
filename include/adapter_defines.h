#ifndef ADAPTER_DEFINES_H
#define ADAPTER_DEFINES_H

#define MCU_TYPE_RP2040 1
#define MCU_TYPE_ESP32  2

#define TUSB_DESC_TOTAL_LEN      (TUD_CONFIG_DESC_LEN + CFG_TUD_HID * TUD_HID_DESC_LEN)

extern const char* global_string_descriptor[];

#define VENDOR_REQUEST_GET_MS_OS_DESCRIPTOR 7


#endif
