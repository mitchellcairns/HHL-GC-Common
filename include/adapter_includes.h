#ifndef ADAPTER_INCLUDES_H
#define ADAPTER_INCLUDES_H

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

#include "adapter_config.h"

#if(ADAPTER_MCU_TYPE==MCU_TYPE_RP2040)
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

    #include "bsp/board.h"
    #include "tusb.h"
#endif


#include "adapter_defines.h"
#include "adapter_types.h"

#include "interval.h"

#include "adapter.h"

#include "adapter_ll.h"

#include "common.h"

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
