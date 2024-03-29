/*
 * Copyright (c) [2023] [Mitch Cairns/Handheldlegend, LLC]
 * All rights reserved.
 *
 * This source code is licensed under the provisions of the license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "adapter_includes.h"
#include "xinput_driver.h"
#include "adapter.h"

#define XINPUT_REPORT_MAX 64

const tusb_desc_device_t xid_device_descriptor =
    {
        .bLength = sizeof(tusb_desc_device_t),
        .bDescriptorType = TUSB_DESC_DEVICE,
        .bcdUSB = 0x0200,
        .bDeviceClass = 0xFF,
        .bDeviceSubClass = 0xFF,
        .bDeviceProtocol = 0xFF,
        .bMaxPacketSize0 =
            CFG_TUD_ENDPOINT0_SIZE,

        .idVendor = 0x045E,
        .idProduct = 0x028E,
        .bcdDevice = 0x0572,

        .iManufacturer = 0x01,
        .iProduct = 0x02,
        .iSerialNumber = 0x03,

        .bNumConfigurations = 0x01};

#define XINPUT_EP_SIZE_LENGTH 39
#define XINPUT_EP_SIZE_BASE 9
#define XINPUT_CONFIG_DESC_SIZE (XINPUT_EP_SIZE_BASE + (XINPUT_EP_SIZE_LENGTH*ADAPTER_PORT_COUNT))

const uint8_t xid_configuration_descriptor[] = {
    0x09,       // bLength
    0x02,       // bDescriptorType (Configuration)
    XINPUT_CONFIG_DESC_SIZE, 0x00, // wTotalLength 48
    ADAPTER_PORT_COUNT,       // bNumInterfaces 4
    0x01,       // bConfigurationValue
    0x00,       // iConfiguration (String Index)
    0x80,       // bmAttributes
    0xFA,       // bMaxPower 500mA

    0x09, // bLength
    0x04, // bDescriptorType (Interface)
    0x00, // bInterfaceNumber 0
    0x00, // bAlternateSetting
    0x02, // bNumEndpoints 2
    0xFF, // bInterfaceClass
    0x5D, // bInterfaceSubClass
    0x01, // bInterfaceProtocol
    0x00, // iInterface (String Index)

    0x10,       // bLength
    0x21,       // bDescriptorType (HID)
    0x10, 0x01, // bcdHID 1.10
    0x01,       // bCountryCode
    0x24,       // bNumDescriptors
    0x81,       // bDescriptorType[0] (Unknown 0x81)
    0x14, 0x03, // wDescriptorLength[0] 788
    0x00,       // bDescriptorType[1] (Unknown 0x00)
    0x03, 0x13, // wDescriptorLength[1] 4867
    0x01,       // bDescriptorType[2] (Unknown 0x01)
    0x00, 0x03, // wDescriptorLength[2] 768
    0x00,       // bDescriptorType[3] (Unknown 0x00)

    0x07,       // bLength
    0x05,       // bDescriptorType (Endpoint)
    0x81,       // bEndpointAddress (IN/D2H)
    0x03,       // bmAttributes (Interrupt)
    0x20, 0x00, // wMaxPacketSize 32
    0x01,       // bInterval 4 (unit depends on device speed)

    0x07,       // bLength
    0x05,       // bDescriptorType (Endpoint)
    0x01,       // bEndpointAddress (OUT/H2D)
    0x03,       // bmAttributes (Interrupt)
    0x20, 0x00, // wMaxPacketSize 32
    0x04,       // bInterval 8 (unit depends on device speed)

    #if(ADAPTER_PORT_COUNT>1)
    // ITF 1
    0x09, // bLength
    0x04, // bDescriptorType (Interface)
    0x01, // bInterfaceNumber 1
    0x00, // bAlternateSetting
    0x02, // bNumEndpoints 2
    0xFF, // bInterfaceClass
    0x5D, // bInterfaceSubClass
    0x01, // bInterfaceProtocol
    0x00, // iInterface (String Index)

    0x10,       // bLength
    0x21,       // bDescriptorType (HID)
    0x10, 0x01, // bcdHID 1.10
    0x01,       // bCountryCode
    0x24,       // bNumDescriptors
    0x82,       // bDescriptorType[0] (Unknown 0x82)
    0x14, 0x03, // wDescriptorLength[0] 788
    0x00,       // bDescriptorType[1] (Unknown 0x00)
    0x03, 0x13, // wDescriptorLength[1] 4867
    0x02,       // bDescriptorType[2] (Unknown 0x02)
    0x00, 0x03, // wDescriptorLength[2] 768
    0x00,       // bDescriptorType[3] (Unknown 0x00)

    0x07,       // bLength
    0x05,       // bDescriptorType (Endpoint)
    0x82,       // bEndpointAddress (IN/D2H)
    0x03,       // bmAttributes (Interrupt)
    0x20, 0x00, // wMaxPacketSize 32
    0x01,       // bInterval 4 (unit depends on device speed)

    0x07,       // bLength
    0x05,       // bDescriptorType (Endpoint)
    0x02,       // bEndpointAddress (OUT/H2D)
    0x03,       // bmAttributes (Interrupt)
    0x20, 0x00, // wMaxPacketSize 32
    0x04,       // bInterval 8 (unit depends on device speed)
    #endif

    #if(ADAPTER_PORT_COUNT>2)
    // ITF 2
    0x09, // bLength
    0x04, // bDescriptorType (Interface)
    0x02, // bInterfaceNumber 2
    0x00, // bAlternateSetting
    0x02, // bNumEndpoints 2
    0xFF, // bInterfaceClass
    0x5D, // bInterfaceSubClass
    0x01, // bInterfaceProtocol
    0x00, // iInterface (String Index)

    0x10,       // bLength
    0x21,       // bDescriptorType (HID)
    0x10, 0x01, // bcdHID 1.10
    0x01,       // bCountryCode
    0x24,       // bNumDescriptors
    0x83,       // bDescriptorType[0] (Unknown 0x83)
    0x14, 0x03, // wDescriptorLength[0] 788
    0x00,       // bDescriptorType[1] (Unknown 0x00)
    0x03, 0x13, // wDescriptorLength[1] 4867
    0x03,       // bDescriptorType[2] (Unknown 0x03)
    0x00, 0x03, // wDescriptorLength[2] 768
    0x00,       // bDescriptorType[3] (Unknown 0x00)

    0x07,       // bLength
    0x05,       // bDescriptorType (Endpoint)
    0x83,       // bEndpointAddress (IN/D2H)
    0x03,       // bmAttributes (Interrupt)
    0x20, 0x00, // wMaxPacketSize 32
    0x01,       // bInterval 4 (unit depends on device speed)

    0x07,       // bLength
    0x05,       // bDescriptorType (Endpoint)
    0x03,       // bEndpointAddress (OUT/H2D)
    0x03,       // bmAttributes (Interrupt)
    0x20, 0x00, // wMaxPacketSize 32
    0x04,       // bInterval 8 (unit depends on device speed)
    #endif

    #if(ADAPTER_PORT_COUNT>3)
    // ITF 3
    0x09, // bLength
    0x04, // bDescriptorType (Interface)
    0x03, // bInterfaceNumber 3
    0x00, // bAlternateSetting
    0x02, // bNumEndpoints 2
    0xFF, // bInterfaceClass
    0x5D, // bInterfaceSubClass
    0x01, // bInterfaceProtocol
    0x00, // iInterface (String Index)

    0x10,       // bLength
    0x21,       // bDescriptorType (HID)
    0x10, 0x01, // bcdHID 1.10
    0x01,       // bCountryCode
    0x24,       // bNumDescriptors
    0x84,       // bDescriptorType[0] (Unknown 0x84)
    0x14, 0x03, // wDescriptorLength[0] 788
    0x00,       // bDescriptorType[1] (Unknown 0x00)
    0x03, 0x13, // wDescriptorLength[1] 4867
    0x04,       // bDescriptorType[2] (Unknown 0x04)
    0x00, 0x03, // wDescriptorLength[2] 768
    0x00,       // bDescriptorType[3] (Unknown 0x00)

    0x07,       // bLength
    0x05,       // bDescriptorType (Endpoint)
    0x84,       // bEndpointAddress (IN/D2H)
    0x03,       // bmAttributes (Interrupt)
    0x20, 0x00, // wMaxPacketSize 32
    0x01,       // bInterval 4 (unit depends on device speed)

    0x07,       // bLength
    0x05,       // bDescriptorType (Endpoint)
    0x04,       // bEndpointAddress (OUT/H2D)
    0x03,       // bmAttributes (Interrupt)
    0x20, 0x00, // wMaxPacketSize 32
    0x04,       // bInterval 8 (unit depends on device speed)
    #endif
};

// string descriptor table
const char *xid_string_descriptor[] = {
    (const char[]){0x09, 0x04}, // 0: is supported language is English (0x0409)
    "GENERIC",                  // 1: Manufacturer
    "XINPUT CONTROLLER",        // 2: Product
    "1.0"                       // 3: Serials
    "XINPUT Port 1",
    "XINPUT Port 2",
    "XINPUT Port 3",
    "XINPUT Port 4",
};

// XINPUT TinyUSB Driver

//--------------------------------------------------------------------+
// MACRO CONSTANT TYPEDEF
//--------------------------------------------------------------------+
typedef struct
{
  uint8_t itf_num;
  uint8_t ep_in;
  uint8_t ep_out;       

  CFG_TUSB_MEM_ALIGN uint8_t epin_buf[XINPUT_REPORT_MAX];
  CFG_TUSB_MEM_ALIGN uint8_t epout_buf[XINPUT_REPORT_MAX];

  // TODO save hid descriptor since host can specifically request this after enumeration
  // Note: HID descriptor may be not available from application after enumeration
  //tusb_hid_descriptor_hid_t const * hid_descriptor;
} xinputd_interface_t;

CFG_TUSB_MEM_SECTION static xinputd_interface_t _xinputd_itf[CFG_TUD_XINPUT];

/*------------- Helpers -------------*/
static inline uint8_t get_index_by_itfnum(uint8_t itf_num)
{
	for (uint8_t i=0; i < CFG_TUD_XINPUT; i++ )
	{
		if ( itf_num == _xinputd_itf[i].itf_num ) return i;
	}

	return 0xFF;
}

void xinputd_init(void)
{
    xinputd_reset(0);
}

void xinputd_reset(uint8_t rhport)
{
    (void) rhport;
    tu_memclr(&_xinputd_itf, sizeof(_xinputd_itf));
}

uint16_t xinputd_open(uint8_t rhport, tusb_desc_interface_t const * desc_itf, uint16_t max_len)
{
    const char* TAG = "xinputd_open";
    // Verify our descriptor is the correct class
    TU_VERIFY((adapter_get_current_mode() == INPUT_MODE_XINPUT));

    // len = interface + n*endpoints
    uint16_t const drv_len = (uint16_t) (sizeof(tusb_desc_interface_t) +
                                        desc_itf->bNumEndpoints * sizeof(tusb_desc_endpoint_t)) + 16;

    TU_ASSERT(max_len >= drv_len, 0);

    // Find available interface
    xinputd_interface_t * p_xid = NULL;
    uint8_t xid_id;

    for(xid_id=0; xid_id<CFG_TUD_XINPUT; xid_id++)
    {
        if(_xinputd_itf[xid_id].ep_in == 0)
        {
            p_xid = &_xinputd_itf[xid_id];
            break;
        }
    }
    TU_ASSERT(p_xid, 0);

    uint8_t const * p_desc = (uint8_t const *) desc_itf;
    
    // Endpoint descriptor
    p_desc = tu_desc_next(p_desc);
    p_desc = tu_desc_next(p_desc);
    TU_ASSERT(usbd_open_edpt_pair(rhport, p_desc, desc_itf->bNumEndpoints, TUSB_XFER_INTERRUPT, &p_xid->ep_out, &p_xid->ep_in), 0);
    p_xid->itf_num = desc_itf->bInterfaceNumber;

    // Prepare for output endpoint
    if (p_xid->ep_out)
    {
        if ( !usbd_edpt_xfer(rhport, p_xid->ep_out, p_xid->epout_buf, sizeof(p_xid->epout_buf)) )
        {
        TU_LOG_FAILED();
        TU_BREAKPOINT();
        }
    }
    
    return drv_len;
}

bool xinputd_control_xfer_cb(uint8_t rhport, uint8_t stage, tusb_control_request_t const * request)
{
    return true;
}

bool xinputd_xfer_cb(uint8_t rhport, uint8_t ep_addr, xfer_result_t result, uint32_t xferred_bytes)
{
  (void) result;

  uint8_t instance;
  xinputd_interface_t * p_xid = _xinputd_itf;

  // Identify which interface to use
  for (instance = 0; instance < CFG_TUD_XINPUT; instance++)
  {
    p_xid = &_xinputd_itf[instance];
    if ( (ep_addr == p_xid->ep_out) || (ep_addr == p_xid->ep_in) ) break;
  }
  TU_ASSERT(instance < CFG_TUD_XINPUT);

  // Sent report successfully
  if (ep_addr == _xinputd_itf[instance].ep_in)
  {
    if (tud_hid_report_complete_cb)
    {
      tud_hid_report_complete_cb(instance, _xinputd_itf[instance].epin_buf, (uint16_t) xferred_bytes);
    }
  }
  // Received report
  else if (ep_addr == _xinputd_itf[instance].ep_out)
  {
    tud_hid_set_report_cb(instance, 0, HID_REPORT_TYPE_INVALID, _xinputd_itf[instance].epout_buf, (uint16_t) xferred_bytes);
    TU_ASSERT(usbd_edpt_xfer(rhport, _xinputd_itf[instance].ep_out, _xinputd_itf[instance].epout_buf, sizeof(_xinputd_itf[instance].epout_buf)));
  }

  return true;
}

void tud_xinput_n_getout(uint8_t instance)
{
    if (tud_ready() && (!usbd_edpt_busy(0, _xinputd_itf[instance].ep_out)) )
    {
        usbd_edpt_claim(0, _xinputd_itf[instance].ep_out);
        usbd_edpt_xfer(0, _xinputd_itf[instance].ep_out, _xinputd_itf[instance].epout_buf, sizeof(_xinputd_itf[instance].epout_buf));
        usbd_edpt_release(0, _xinputd_itf[instance].ep_out);
    }
}

// USER API ACCESSIBLE
bool tud_xinput_n_report(uint8_t instance, void const * report, uint16_t len)
{
    uint8_t const rhport = 0;

    // Remote wakeup
    if (tud_suspended()) {
      // Wake up host if we are in suspend mode
      // and REMOTE_WAKEUP feature is enabled by host
      tud_remote_wakeup();
    }

    uint8_t ep_addr = _xinputd_itf[instance].ep_in;

    // claim endpoint
    TU_VERIFY( usbd_edpt_claim(rhport, ep_addr) );

    len = tu_min16(len, XINPUT_REPORT_MAX);
    memcpy(_xinputd_itf[instance].epin_buf, report, len);
    bool out = usbd_edpt_xfer(rhport, ep_addr, _xinputd_itf[instance].epin_buf, len);
    usbd_edpt_release(0, ep_addr);

    tud_xinput_n_getout(instance);

    return out;
}

bool tud_xinput_n_ready(uint8_t instance)
{
    uint8_t const rhport = 0;
    uint8_t const ep_in = _xinputd_itf[instance].ep_in;
    return (tud_ready()) && (ep_in != 0) && !usbd_edpt_busy(rhport, ep_in);
}

const usbd_class_driver_t tud_xinput_driver =
{
    #if CFG_TUSB_DEBUG >= 2
    .name = "XINPUT",
    #endif
    .init   = xinputd_init,
    .reset  = xinputd_reset,
    .open   = xinputd_open,
    .control_xfer_cb = xinputd_control_xfer_cb,
    .xfer_cb    = xinputd_xfer_cb,
    .sof = NULL,
};

// Descriptor callback functions
uint8_t const *xinput_descriptor_device_cb(void)
{
    return (uint8_t const *) &xid_device_descriptor;
}

uint8_t const *xinput_descriptor_configuration_cb(uint8_t index)
{
    (void) index;
    return xid_configuration_descriptor;
}
