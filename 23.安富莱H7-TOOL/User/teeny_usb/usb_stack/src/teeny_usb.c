/*
 *         _______                    _    _  _____ ____
 *        |__   __|                  | |  | |/ ____|  _ \
 *           | | ___  ___ _ __  _   _| |  | | (___ | |_) |
 *           | |/ _ \/ _ \ '_ \| | | | |  | |\___ \|  _ <
 *           | |  __/  __/ | | | |_| | |__| |____) | |_) |
 *           |_|\___|\___|_| |_|\__, |\____/|_____/|____/
 *                               __/ |
 *                              |___/
 *
 * TeenyUSB - light weight usb stack for STM32 micro controllers
 *
 * Copyright (c) 2019 XToolBox  - admin@xtoolbox.org
 *                         www.tusb.org
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "teeny_usb.h"
#include "string.h"

// Default class request handler
// User need override it if the class need some special request
WEAK int tusb_class_request(tusb_device_t *dev, tusb_setup_packet *setup_req)
{
    return 0;
}

// Standard get descriptor function
static void tusb_get_descriptor(tusb_device_t *dev, tusb_setup_packet *req)
{
    const uint8_t *desc = 0;
    uint16_t len = 0;
    switch (HI_BYTE(req->wValue))
    {
    case USB_DESC_TYPE_DEVICE:
        desc = dev->descriptors->device;
        if (desc) len = desc[0];
        break;
    case USB_DESC_TYPE_CONFIGURATION:
        desc = dev->descriptors->config;
        if (desc)len = *((uint16_t *)desc + 1);
        break;
    case USB_DESC_TYPE_STRING:
    {
        uint8_t index = LO_BYTE(req->wValue);
        if (index < dev->descriptors->string_cnt)
        {
            desc = dev->descriptors->strings[index];
            if (desc)len = desc[0];
        }
#if defined(HAS_WCID)
        else if (index == 0xee && dev->descriptors->wcid_desc)
        {
            desc = WCID_StringDescriptor_MSOS;
            len = desc[0];
        }
#endif
        break;
    }
#if defined(HAS_WCID_20)
    case USB_DESC_TYPE_BOS:
    {
        desc = dev->descriptors->wcid_bos;
        if (desc)len = *((uint16_t *)desc + 1);
        break;
    }
#endif

#if defined(SUPPORT_OTHER_SPEED)
    // For High speed device, support the DEVICE_QUALIFIER
    // and OTHER_SPEED_CONFIGURATION request
    case USB_DESC_TYPE_DEVICE_QUALIFIER:
    {
#if defined(DESCRIPTOR_BUFFER_SIZE) && DESCRIPTOR_BUFFER_SIZE > 0
        dev->desc_buffer[0] = USB_LEN_DEV_QUALIFIER_DESC;
        dev->desc_buffer[1] = USB_DESC_TYPE_DEVICE_QUALIFIER;
        memcpy(dev->desc_buffer + 2, dev->descriptors->device + 2, 6);
        // set the bNumConfigurations field
        dev->desc_buffer[8] = dev->descriptors->device[17];
        // set bReserved field to zero
        dev->desc_buffer[9] = 0;
        len = USB_LEN_DEV_QUALIFIER_DESC;
        desc = dev->desc_buffer;
#else
        desc = 0;
#endif

        break;
    }
    case USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION:
    {
#if defined(DESCRIPTOR_BUFFER_SIZE) && DESCRIPTOR_BUFFER_SIZE > 0
        desc = dev->descriptors->config;
        if (desc)len = *((uint16_t *)desc + 1);
        memcpy(dev->desc_buffer, desc, len);
        dev->desc_buffer[1] = USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION;
        desc = dev->desc_buffer;
#else
        desc = 0;
#endif
        break;
    }
#endif  // SUPPORT_OTHER_SPEED
    }
    if (desc)
    {
        len = req->wLength > len ? len : req->wLength;
#if defined(DESCRIPTOR_BUFFER_SIZE) && DESCRIPTOR_BUFFER_SIZE > 0
        if (dev->desc_buffer != desc)
        {
            memcpy(dev->desc_buffer, desc, len);
            desc = dev->desc_buffer;
        }
#endif
        tusb_control_send(dev, desc, len);
    }
    else
    {
        tusb_set_stall(dev, 0);
    }
}

#if defined(HAS_WCID) || defined(HAS_WCID_20)
static int tusb_vendor_request(tusb_device_t *dev, tusb_setup_packet *setup_req)
{
    uint32_t len = 0;
    const uint8_t *desc = 0;
    if (setup_req->bRequest == WCID_VENDOR_CODE)
    {
        switch (setup_req->wIndex)
        {
#if defined(HAS_WCID)
        case 4:
        {
            desc = dev->descriptors->wcid_desc;
            len = desc[0] + (desc[1] << 8) + (desc[2] << 16) + (desc[3] << 24);
            break;
        }
        case 5:
        {
            desc = dev->descriptors->wcid_properties[setup_req->wValue];
            if (desc)
            {
                len = desc[0] + (desc[1] << 8) + (desc[2] << 16) + (desc[3] << 24);
            }
            break;
        }
#endif
#if defined(HAS_WCID_20)
        case 7:
        {
            desc = dev->descriptors->wcid_desc_set;
            if (desc)len = *(uint16_t *)(desc + 8);
            break;
        }
#endif
        }
    }
    // TODO: Handle length more than 0xffff
    len = setup_req->wLength > len ? len : setup_req->wLength;
#if defined(DESCRIPTOR_BUFFER_SIZE) && DESCRIPTOR_BUFFER_SIZE > 0
    if (desc)
    {
        if (dev->desc_buffer != desc)
        {
            memcpy(dev->desc_buffer, desc, len);
            desc = dev->desc_buffer;
        }
    }
#endif
    if (desc)
    {
        tusb_control_send(dev, desc, len);
        return 1;
    }
    return 0;
}
#endif

// Set device address before send status
WEAK  void tusb_set_addr_before_status(tusb_device_t *dev) {}

// Set device address after send status
WEAK  void tusb_set_addr_after_status(tusb_device_t *dev) {}

// Standard request process function
static void tusb_standard_request(tusb_device_t *dev, tusb_setup_packet *setup_req)
{
    // TODO: need process the interface and end point standard request
    /** for example:
     switch(setup_req->bmRequestType & USB_REQ_RECIPIENT_MASK ){
      USB_REQ_RECIPIENT_DEVICE:
      // standard device request
      break;
      USB_REQ_RECIPIENT_INTERFACE:
        // standard interface request
        uint16_t interface_index  = setup_req->wIndex;
        if(setup_req->bRequest  == USB_REQ_CLEAR_FEATURE){
        }else if(setup_req->bRequest  == USB_REQ_SET_FEATURE){
        }
      break;
      USB_REQ_RECIPIENT_ENDPOINT:
        // standard end point request
        uint8_t ep_addr  = LO_BYTE(setup_req->wIndex);
        if(setup_req->bRequest  == USB_REQ_CLEAR_FEATURE){
            // ep clear feature
        }else if(setup_req->bRequest  == USB_REQ_SET_FEATURE){
            // ep set feature
        }
      break;
     }
     */
    static __ALIGN_BEGIN uint32_t temp __ALIGN_END;
    switch (setup_req->bRequest)
    {
    case USB_REQ_SET_ADDRESS:
        dev->addr = LO_BYTE(setup_req->wValue);
        tusb_set_addr_before_status(dev);
        dev->ep0_tx_done = tusb_set_addr_after_status;
        tusb_send_status(dev);
        break;

    case USB_REQ_GET_DESCRIPTOR:
        tusb_get_descriptor(dev, setup_req);
        break;

    case USB_REQ_GET_STATUS:
        temp = dev->status;
        tusb_control_send(dev, &temp, 2);
        break;

    // Only support one configuration, so just save and return the config value
    case USB_REQ_GET_CONFIGURATION:
        temp = dev->config;
        tusb_control_send(dev, &temp, 1);
        break;

    case USB_REQ_SET_CONFIGURATION:
        dev->config = LO_BYTE(setup_req->wValue);
        tusb_send_status(dev);
        break;
    // Only support one alt setting, so just save and return the alt value
    case USB_REQ_GET_INTERFACE:
        temp = dev->alt_cfg;
        tusb_control_send(dev, &temp, 1);
        break;
    case USB_REQ_SET_INTERFACE:
        dev->alt_cfg = LO_BYTE(setup_req->wValue);
        tusb_send_status(dev);
        break;
    case USB_REQ_SET_FEATURE:
        if ((setup_req->bmRequestType & USB_REQ_RECIPIENT_MASK)  == USB_REQ_RECIPIENT_ENDPOINT)
        {
            uint8_t ep_addr  = LO_BYTE(setup_req->wIndex);
            if ((ep_addr & 0x7f) < EP_NUM)
            {
                // In some case, we may report more endpoint than the device support
                tusb_set_stall(dev, ep_addr);
                break;
            }
        }
        else if (setup_req->wValue == USB_FEATURE_REMOTE_WAKEUP)
        {
            dev->remote_wakeup = 1;
            tusb_send_status(dev);
            break;
        }
        tusb_set_stall(dev, 0);
        break;
    // otherwise fall to default
    case USB_REQ_CLEAR_FEATURE:
        if ((setup_req->bmRequestType & USB_REQ_RECIPIENT_MASK)  == USB_REQ_RECIPIENT_ENDPOINT)
        {
            uint8_t ep_addr  = LO_BYTE(setup_req->wIndex);
            if ((ep_addr & 0x7f) < EP_NUM)
            {
                tusb_clear_stall(dev, ep_addr);
                break;
            }
        }
        else if (setup_req->wValue == USB_FEATURE_REMOTE_WAKEUP)
        {
            dev->remote_wakeup = 0;
            tusb_send_status(dev);
            break;
        }
        tusb_set_stall(dev, 0);
        break;
    // otherwise fall to default
    default:
        // Error condition, stall ep0
        tusb_set_stall(dev, 0);
        break;
    }
}

// forward declare for some internal function
void tusb_recv_data(tusb_device_t *dev, uint8_t EPn);
void tusb_send_data_done(tusb_device_t *dev, uint8_t EPn);
uint32_t tusb_read_ep0(tusb_device_t *dev, void *buf);
void tusb_read_data(tusb_device_t *dev, void *buf, uint32_t len);

// Setup packet handler
void tusb_setup_handler(tusb_device_t *dev)
{
    tusb_setup_packet *setup_req = &dev->setup;
    if (tusb_class_request(dev, setup_req))
    {
        return;
    }
#if defined(HAS_WCID) || defined(HAS_WCID_20)
    if ((setup_req->bmRequestType & USB_REQ_TYPE_MASK) == USB_REQ_TYPE_VENDOR)
    {
        if (tusb_vendor_request(dev, setup_req))
        {
            return;
        }
    }
#endif
    tusb_standard_request(dev, setup_req);
}

// initial the ep recv buffer
int tusb_set_recv_buffer(tusb_device_t *dev, uint8_t EPn, void *data, uint16_t len)
{
    tusb_ep_data *ep = &dev->Ep[EPn];
    ep->rx_buf = (uint8_t *)data;
    ep->rx_size = len;
    ep->rx_count = 0;
    return 0;
}

// Public callback function override by user
// default data transmit done callback
WEAK void tusb_on_tx_done(tusb_device_t *dev, uint8_t EPn)
{}

// default data received  callback, return 0 will continue receive data in end point rx buffer
// return other value will cause the OUT ep NAX, user can active the OUT ep by call tusb_set_rx_valid
WEAK int tusb_on_rx_done(tusb_device_t *dev, uint8_t EPn, const void *data, uint16_t len)
{
    return 0;
}

WEAK void tusb_reconfig(tusb_device_t *dev)
{
}




// Macro check
#if defined(SUPPORT_OTHER_SPEED)
    #ifndef DESCRIPTOR_BUFFER_SIZE
        #error Other speed descriptor enabled, but descriptor buffer size not defined
    #elif DESCRIPTOR_BUFFER_SIZE == 0
        #error Other speed descriptor enabled, but descriptor buffer size too small
    #endif
#endif


