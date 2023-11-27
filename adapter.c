#include "adapter.h"

input_mode_t _adapter_input_mode = INPUT_MODE_SWPRO;
uint32_t _adapter_interval = USBRATE_8;
joybus_input_s *_adapter_joybus_inputs = NULL;

bool _usb_sent_ok = false;

typedef void (*usb_cb_t)(joybus_input_s *);
typedef void (*usb_idle_cb_t)(joybus_input_s *);

bool _usb_clear = false;
usb_cb_t _usb_hid_cb = NULL;
usb_idle_cb_t _usb_idle_cb = NULL;

void adapter_set_interval(uint32_t interval)
{
    _adapter_interval = interval;
}

void adapter_mode_cycle(bool forwards)
{
    if (forwards)
    {
        if (_adapter_input_mode + 1 >= INPUT_MODE_MAX)
            _adapter_input_mode = 0;
        else
            _adapter_input_mode += 1;
    }
    else
    {
        if (!_adapter_input_mode)
            _adapter_input_mode = (INPUT_MODE_MAX - 1);
        else
            _adapter_input_mode -= 1;
    }

    adapter_reboot_memory_u _mem = {
        .adapter_mode = _adapter_input_mode,
        .reboot_reason = ADAPTER_REBOOT_REASON_MODECHANGE};

    adapter_ll_reboot_with_memory(&_mem);
}

void adapter_mode_cycle_task(uint32_t timestamp)
{
    static interval_s _i_state = {.last_time = 0, .this_time = 0};
    static bool fwd_press = false;
    static bool back_press = false;

    if(interval_run(timestamp, 16000, &_i_state))
    {
        if(!adapter_ll_gpio_read(ADAPTER_BUTTON_1) && !back_press)
        {
            back_press = true;
        }
        else if(adapter_ll_gpio_read(ADAPTER_BUTTON_1) && back_press)
        {
            back_press = false;
            adapter_mode_cycle(false);
        }

        if(!adapter_ll_gpio_read(ADAPTER_BUTTON_2) && !fwd_press)
        {
            fwd_press = true;
        }
        else if (adapter_ll_gpio_read(ADAPTER_BUTTON_2) && fwd_press)
        {
            fwd_press = false;
            adapter_mode_cycle(true);
        }
    }
}

input_mode_t adapter_get_current_mode()
{
    return _adapter_input_mode;
}

bool adapter_usb_start(input_mode_t mode)
{
    switch (mode)
    {
    default:
        _usb_hid_cb = NULL;
        _usb_idle_cb = NULL;
        break;

    case INPUT_MODE_GCADAPTER:
        _usb_hid_cb = gcinput_hid_report;
        _usb_idle_cb = gcinput_hid_idle;
        adapter_set_interval(7000);
        break;

    case INPUT_MODE_SLIPPI:
        _usb_hid_cb = gcinput_hid_report;
        //_usb_idle_cb = gcinput_hid_idle;
        adapter_set_interval(500);
        break;

    case INPUT_MODE_SWPRO:
        _usb_hid_cb = swpro_hid_report;
        _usb_idle_cb = swpro_hid_idle;
        adapter_set_interval(7000);
        break;

    case INPUT_MODE_XINPUT:
        _usb_hid_cb = xinput_hid_report;
        _usb_idle_cb = xinput_hid_idle;
        adapter_set_interval(7000);
        break;
    }

    _adapter_input_mode = mode;

    return tusb_init();
}

void adapter_usb_report(joybus_input_s *input)
{
  if(!_usb_hid_cb) return;

  _usb_hid_cb(input);
}

void adapter_usb_idle(joybus_input_s *input)
{
  if(!_usb_idle_cb) return;

  _usb_idle_cb(input);
}

void adapter_port_status_led(uint32_t timestamp, joybus_input_s *input)
{
    static interval_s _i_state = {.last_time = 0, .this_time = 0};
    static bool _port_plug[4] = {0,0,0,0};

    if(interval_run(timestamp, 100000, &_i_state))
    {
        for(uint8_t i = 0; i<ADAPTER_PORT_COUNT; i++)
        {
            if(_port_plug[i] != (input[i].port_itf > -1))
            {
                _port_plug[i] = (input[i].port_itf > -1);

                if(_port_plug[i])
                {
                    rgb_set_single(COLOR_WHITE.color, i);
                }
                else
                {
                    rgb_set_single(COLOR_RED.color, i);
                }
                rgb_set_dirty();
            }
        }
    }
}

void adapter_comms_task(uint32_t timestamp)
{
    static interval_s _i_state = {.last_time = 0, .this_time = 0};

    if(interval_run(timestamp, _adapter_interval, &_i_state))
    {
        joybus_itf_poll(&_adapter_joybus_inputs);
        adapter_usb_report(_adapter_joybus_inputs);
    }
    else 
    {
        #if (ADAPTER_MCU_TYPE == MCU_TYPE_RP2040)
        tud_task();
        #endif
        adapter_usb_idle(_adapter_joybus_inputs);

        adapter_port_status_led(timestamp, _adapter_joybus_inputs);
    }

}

void adapter_main_init()
{
    adapter_ll_hardware_setup();

    switch_analog_calibration_init();

    rgb_init();

    joybus_init();

    // Load boot reason if applicable
    adapter_reboot_memory_u _reboot_mem = {0};

    adapter_ll_get_reboot_memory(&_reboot_mem);


    input_mode_t mode = INPUT_MODE_SWPRO;

    if (settings_load())
    {

        if(!global_loaded_settings.adapter_hardware_test)
        {
            // Run HW test again
            if(!cb_adapter_hardware_test())
            {
                rgb_set_all(COLOR_ORANGE.color);
                // Halt
                for(;;) {}
            }
            // Otherwise run normally and save
            global_loaded_settings.adapter_hardware_test = true;
            settings_save();
        }

        if (_reboot_mem.reboot_reason == ADAPTER_REBOOT_REASON_MODECHANGE)
        {
            mode = _reboot_mem.adapter_mode;
        }
        else
        {
            mode = global_loaded_settings.input_mode;
        }

        switch (mode)
        {
            default:
            case INPUT_MODE_SWPRO:
                rgb_set_all(COLOR_YELLOW.color);
                break;

            case INPUT_MODE_SLIPPI:
                rgb_set_all(COLOR_CYAN.color);
                break;

            case INPUT_MODE_XINPUT:
                rgb_set_all(COLOR_GREEN.color);
                break;

            case INPUT_MODE_GCADAPTER:
                rgb_set_all(COLOR_PURPLE.color);
                break;
        }
    }
    else 
    {
        rgb_set_all(COLOR_ORANGE.color);
        // Halt
        for(;;) {}
    }

    rgb_set_dirty();

    if(!adapter_usb_start(mode))
    {
        rgb_set_all(COLOR_RED.color);
        rgb_set_dirty();
    }
    else
    {
        adapter_ll_usb_task_start();
    }
}

void adapter_main_loop()
{
    for(;;)
    {
        uint32_t _t = adapter_ll_get_timestamp_us();

        adapter_ll_save_check();

        rgb_task(_t);
        adapter_comms_task(_t);
        adapter_mode_cycle_task(_t);

    }
}