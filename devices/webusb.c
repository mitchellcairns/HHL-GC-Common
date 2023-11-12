#include "webusb.h"

uint8_t _webusb_out_buffer[64] = {0x00};
bool _web_usb_indicate = false;

void webusb_set_indicate()
{
    _web_usb_indicate = true;
}

void webusb_save_confirm()
{
    if(!_web_usb_indicate) return;

    printf("Sending Save receipt...\n");
    memset(_webusb_out_buffer, 0, 64);
    _webusb_out_buffer[0] = 0xF1;
    tud_vendor_n_write(0, _webusb_out_buffer, 64);
    tud_vendor_n_flush(0);
    _web_usb_indicate = false;
}

void webusb_command_processor(uint8_t *data)
{
    switch(data[0])
    {
        default:

            break;

        case WEBUSB_CMD_FW_SET:
            {
                adapter_ll_reboot_bootloader();
            }
            break;

        case WEBUSB_CMD_FW_GET:
            {
                _webusb_out_buffer[0] = WEBUSB_CMD_FW_GET;
                _webusb_out_buffer[1] = (ADAPTER_FIRMWARE_VERSION & 0xFF00)>>8;
                _webusb_out_buffer[2] = ADAPTER_FIRMWARE_VERSION & 0xFF;
                tud_vendor_n_write(0, _webusb_out_buffer, 64);
                tud_vendor_n_flush(0);
            }
            break;

        case WEBUSB_CMD_SAVEALL:
            {
                printf("WebUSB: Got SAVE command.\n");
                webusb_set_indicate();
                settings_save();
            }
            break;
    }
}