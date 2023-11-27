#include "adapter_includes.h"

#if(ADAPTER_MCU_TYPE==1)

volatile bool _save_flag = false;
adapter_settings_s* _mem_settings_ptr = NULL;

void adapter_ll_write_settings_to_memory(adapter_settings_s *input)
{
    _mem_settings_ptr = input;
    _save_flag = true;
}

#define FLASH_TARGET_OFFSET (1200 * 1024)

void adapter_ll_load_settings_from_memory(adapter_settings_s *output)
{
    static_assert(sizeof(adapter_settings_s) <= FLASH_SECTOR_SIZE);
    const uint8_t *target_read = (const uint8_t *)(XIP_BASE + FLASH_TARGET_OFFSET + (FLASH_SECTOR_SIZE));
    memcpy(output, target_read, sizeof(adapter_settings_s));
}

void adapter_ll_save_check()
{
    if (_save_flag)
    {
        //multicore_lockout_start_blocking();
        // Check that we are less than our flash sector size
        static_assert(sizeof(adapter_settings_s) <= FLASH_SECTOR_SIZE);

        // Store interrupts status and disable
        uint32_t ints = save_and_disable_interrupts();

        // Calculate storage bank address via index
        uint32_t memoryAddress = FLASH_TARGET_OFFSET + (FLASH_SECTOR_SIZE);

        // Create blank page data
        uint8_t page[FLASH_SECTOR_SIZE] = {0x00};
        // Copy settings into our page buffer
        memcpy(page, _mem_settings_ptr, sizeof(adapter_settings_s));

        // Erase the settings flash sector
        flash_range_erase(memoryAddress, FLASH_SECTOR_SIZE);

        // Program the flash sector with our page
        flash_range_program(memoryAddress, page, FLASH_SECTOR_SIZE);

        // Restore interrups
        restore_interrupts(ints);
        //multicore_lockout_end_blocking();

        // Indicate change
        webusb_save_confirm();

        _save_flag = false;
    }
}

uint8_t adapter_ll_generate_random_8() 
{
    return get_rand_32() & 0xFF;
}

#define SCRATCH_OFFSET 0xC
#define MAX_INDEX     7
#define WD_READOUT_IDX 5

uint32_t _scratch_get(uint8_t index) {
    if (index > MAX_INDEX) {
        // Handle the error, maybe by returning an error code or logging a message.
        // Here we just return 0 as a simple example.
        return 0;
    }
    return *((volatile uint32_t *) (WATCHDOG_BASE + SCRATCH_OFFSET + (index * 4)));
}

void _scratch_set(uint8_t index, uint32_t value) {
    if (index > MAX_INDEX) {
        // Handle the error here. For simplicity, we'll just return in this example.
        return;
    }
    *((volatile uint32_t *) (WATCHDOG_BASE + SCRATCH_OFFSET + (index * 4))) = value;
}

void _software_reset()
{
  rgb_set_instant(0x00);
  // Configure the watchdog to reset the chip after a short delay
  watchdog_reboot(0, 0, 0);
  ; // Loop forever, waiting for the watchdog to reset the chip
}

void adapter_ll_reboot_with_memory(adapter_reboot_memory_u *msg)
{
    _scratch_set(WD_READOUT_IDX, msg->value);
    _software_reset();
}

void adapter_ll_get_reboot_memory(adapter_reboot_memory_u *msg)
{
    adapter_reboot_memory_u _mem = {0};
    _mem.value = _scratch_get(WD_READOUT_IDX);
    msg->value = _mem.value;
}

void adapter_ll_reboot_bootloader()
{
    reset_usb_boot(0, 0);
}

bool adapter_ll_gpio_read(uint32_t gpio)
{
    return gpio_get(gpio);
}

void adapter_ll_hardware_setup()
{

    stdio_init_all();

    gpio_init(ADAPTER_BUTTON_1);
    gpio_pull_up(ADAPTER_BUTTON_1);
    gpio_set_dir(ADAPTER_BUTTON_1, GPIO_IN);

    gpio_init(ADAPTER_BUTTON_2);
    gpio_pull_up(ADAPTER_BUTTON_2);
    gpio_set_dir(ADAPTER_BUTTON_2, GPIO_IN);

    // Handle early USB bootloader stuff
    if (!gpio_get(ADAPTER_BUTTON_1) && !gpio_get(ADAPTER_BUTTON_2))
    {
        reset_usb_boot(0, 0);
    }
}

uint32_t adapter_ll_get_timestamp_us()
{
    return time_us_32();
}

void adapter_ll_usb_task_start()
{
    // Unused
}

#endif