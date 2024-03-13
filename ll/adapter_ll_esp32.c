#include "adapter_includes.h"

#if (ADAPTER_MCU_TYPE == MCU_TYPE_ESP32)
#include <stdlib.h>
#include <stdio.h>

#include "esp_private/periph_ctrl.h"
#include "esp_private/usb_phy.h"
#include "soc/usb_pins.h"

volatile bool _save_flag = false;
adapter_settings_s *_mem_settings_ptr = NULL;
static usb_phy_handle_t phy_hdl;

// ESP32 specific
#define SETTINGS_NAMESPACE "adapt_settings"
#define SETTINGS_KEY "adp_set"
nvs_handle_t adapter_mem_handle;

static TaskHandle_t s_tusb_tskh;

void _esp32_usb_task(void *arg)
{
    while (1) { // RTOS forever loop
        tud_task();
    }
}

void _esp32_usb_hardware_init()
{
    const char* TAG = "_esp32_usb_hardware_init";

    // Configure USB PHY
    usb_phy_config_t phy_conf = {
        .controller = USB_PHY_CTRL_OTG,
        .otg_mode = USB_OTG_MODE_DEVICE,
        .target = USB_PHY_TARGET_INT,
    };

    // OTG IOs config
    const usb_phy_otg_io_conf_t otg_io_conf = USB_PHY_SELF_POWERED_DEVICE(-1);
    //if (config->self_powered)
    {
        phy_conf.otg_io_conf = &otg_io_conf;
    }
    esp_err_t err = usb_new_phy(&phy_conf, &phy_hdl);

    if(err==ESP_OK)
    {
        ESP_LOGI(TAG, "Init USB Phy ok.");
    }
}

void adapter_ll_write_settings_to_memory(adapter_settings_s *input)
{
    _mem_settings_ptr = input;
    _save_flag = true;
}

void adapter_ll_load_settings_from_memory(adapter_settings_s *output)
{
    const char *TAG = "adapter_ll_load_settings_from_memory";

    // Initialize NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    // Open storage
    err = nvs_open(SETTINGS_NAMESPACE, NVS_READWRITE, &adapter_mem_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "During Adapter load settings, NVS Open failed.");
    }

    size_t required_size = 0;
    err = nvs_get_blob(adapter_mem_handle, SETTINGS_KEY, NULL, &required_size);

    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND)
        return;

    ESP_LOGI(TAG, "Required size: %d", required_size);

    if (required_size > 0)
    {
        err = nvs_get_blob(adapter_mem_handle, SETTINGS_KEY, output, &required_size);
        if (err != ESP_OK)
        {
            ESP_LOGE(TAG, "Could not load settings.");
        }
        nvs_close(adapter_mem_handle);
    }
}

void adapter_ll_save_check()
{
    const char *TAG = "adapter_ll_save_check";

    if (_save_flag)
    {
        esp_err_t err;
        // Open
        err = nvs_open(SETTINGS_NAMESPACE, NVS_READWRITE, &adapter_mem_handle);
        if (err != ESP_OK)
        {
            ESP_LOGE(TAG, "During Adapter settings save, NVS storage failed to open.");
            return;
        }

        nvs_set_blob(adapter_mem_handle, SETTINGS_KEY, _mem_settings_ptr, sizeof(adapter_settings_s));

        nvs_commit(adapter_mem_handle);
        nvs_close(adapter_mem_handle);
        return;
    }
}

uint8_t adapter_ll_generate_random_8()
{
    return (uint8_t)(esp_random() & 0xFF);
}

void _software_reset()
{
    rgb_set_instant(0x00);
    esp_restart();
}

RTC_NOINIT_ATTR uint32_t _esp32_reboot_mem = 0;

void adapter_ll_reboot_with_memory(adapter_reboot_memory_u *msg)
{
    _esp32_reboot_mem = msg->value;
    _software_reset();
}

void adapter_ll_get_reboot_memory(adapter_reboot_memory_u *msg)
{
    msg->value = _esp32_reboot_mem;
}

void adapter_ll_reboot_bootloader()
{
}

#define PIN_MASK_GCP ((1ULL << ADAPTER_BUTTON_1) | (1ULL << ADAPTER_BUTTON_2))

bool adapter_ll_gpio_read(uint32_t gpio)
{
    uint32_t regread = 0;

    if (gpio >= 32)
    {
        gpio -= 32;
        regread = REG_READ(GPIO_IN1_REG);
    }
    else
        regread = REG_READ(GPIO_IN_REG);

    return util_getbit(regread, gpio);
}

volatile bool _ll_usb_free = false;

static SemaphoreHandle_t boolMutex = NULL;

void adapter_ll_hardware_setup()
{

    // Initialize button that will trigger HID reports
    const gpio_config_t boot_button_config = {
        .pin_bit_mask = PIN_MASK_GCP,
        .mode = GPIO_MODE_INPUT,
        .intr_type = GPIO_INTR_DISABLE,
        .pull_up_en = true,
        .pull_down_en = false,
    };

    ESP_ERROR_CHECK(gpio_config(&boot_button_config));

    // Handle early USB bootloader stuff
    // if (!gpio_get(ADAPTER_BUTTON_1) && !gpio_get(ADAPTER_BUTTON_2))
    //{
    //    reset_usb_boot(0, 0);
    //}

    // Create the semaphore
    boolMutex = xSemaphoreCreateMutex();

    // Initialize USB hardware
    _esp32_usb_hardware_init();

    
}

void adapter_ll_usb_task_start()
{
    // Pin our usb task to core 1 because otherwise the interrupts block? wowie zowie
    xTaskCreatePinnedToCore(_esp32_usb_task, "TinyUSB", 4096, NULL, 5, &s_tusb_tskh, 1); 
}

uint32_t adapter_ll_get_timestamp_us()
{
    int64_t t = esp_timer_get_time();

    if(t>0xFFFFFFFF) t-=0xFFFFFFFF;
    return (uint32_t)t;
}

void adapter_ll_usb_set_clear(int8_t itf, joybus_input_s *_adapter_joybus_inputs)
{
    if(xSemaphoreTake(boolMutex, portMAX_DELAY)) {
        uint8_t clear = 0x00;

        for(uint i = 0; i < 4; i++)
        {
            if(_adapter_joybus_inputs[i].port_itf==itf)
            {
                _adapter_joybus_inputs[i].usb_clear = true;
            }
        }

        for(uint i = 0; i < 4; i++)
        {
            if(_adapter_joybus_inputs[i].port_itf<0)
            {
                clear |= (1<<i);
            }
            else
            {
                clear |= (_adapter_joybus_inputs[i].usb_clear<<i);
            }
        }

        _ll_usb_free = (clear==0b1111) ? true : false;
        xSemaphoreGive(boolMutex);
    }
}

void adapter_ll_usb_unset_clear(joybus_input_s *_adapter_joybus_inputs)
{

    if(xSemaphoreTake(boolMutex, portMAX_DELAY)) {
        for(uint i = 0; i < 4; i++)
        {
            _adapter_joybus_inputs[i].usb_clear = false;
        }
        _ll_usb_free = false;
        xSemaphoreGive(boolMutex);
    }
}

bool adapter_ll_usb_get_clear()
{
    bool tmp = _ll_usb_free;
    return tmp;
}

#endif