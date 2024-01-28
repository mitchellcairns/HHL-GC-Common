#ifndef ADAPTER_INCLUDES_H
#define ADAPTER_INCLUDES_H

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <time.h>

#include "adapter_config.h"
#include "adapter_defines.h"
#include "adapter_types.h"

#if(ADAPTER_MCU_TYPE==MCU_TYPE_RP2040)

    #include "bsp/board.h"
    #include "tusb.h"

    #include "pico/stdlib.h"
    #include "pico/bootrom.h"
    #include "pico/multicore.h"
    #include "pico/rand.h"

    #include "hardware/pio.h"
    #include "hardware/irq.h"
    #include "hardware/gpio.h"
    #include "hardware/clocks.h"
    #include "hardware/dma.h"
    #include "hardware/pwm.h"
    #include "hardware/spi.h"
    #include "hardware/timer.h"
    #include "hardware/uart.h"
    #include "hardware/flash.h"
    #include "hardware/watchdog.h"
    #include "hardware/regs/addressmap.h"
#endif 

#if(ADAPTER_MCU_TYPE==MCU_TYPE_ESP32)

    // ESP includes
    #include "esp_system.h"
    #include "esp_log.h"
    #include "esp_err.h"
    #include "esp_intr_alloc.h"
    #include "esp_heap_caps.h"
    #include "esp_pm.h"
    #include "esp_attr.h"
    #include "esp_private/gdma.h"
    #include "esp_check.h"
    #include "rom/ets_sys.h"
    #include "esp_rom_sys.h"
    #include "driver/gptimer.h"
    #include "rom/ets_sys.h"
    #include "esp_random.h"
    #include "esp_timer.h"

    // Atomic stuff
    #include <stdatomic.h>

    // Drivers
    #include "driver/gpio.h"
    #include "esp32_neopixel.h"
    
    // SOC
    #include "soc/soc_caps.h"
    
    #include "soc/io_mux_reg.h"
    #include "soc/gpio_periph.h"

    // ROM
    #include "rom/gpio.h"

    // Storage
    #include "nvs_flash.h"
    #include "nvs.h"

    //USB
    #include "tusb_config.h"

    #include "tusb.h"
    #include "class/hid/hid_device.h"
    #include "class/vendor/vendor_device.h"
    #include "device/usbd_pvt.h"

    // FreeRTOS
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #include "freertos/queue.h"
    #include "freertos/semphr.h"


#endif

#include "interval.h"

#include "adapter.h"

#include "adapter_ll.h"

#include "util_common.h"

#include "adapter_settings.h"

#include "rgb.h"
#include "joybus.h"

#include "desc_bos.h"
// XInput TinyUSB Driver
#include "xinput_driver.h"
#include "ginput_driver.h"

#include "gcinput.h"
#include "xinput.h"
#include "swpro.h"

#include "webusb.h"

// Switch pro includes
#include "switch_analog.h"
#include "switch_spi.h"
#include "switch_commands.h"



#endif
