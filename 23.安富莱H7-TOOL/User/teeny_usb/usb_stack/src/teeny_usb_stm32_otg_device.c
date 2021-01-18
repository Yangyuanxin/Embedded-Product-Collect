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

#include "string.h"
#include "teeny_usb.h"

#ifndef NO_DEVICE
#if defined(USB_OTG_FS) || defined(USB_OTG_HS)

// Private functions used by teeny usb core
void tusb_send_data_done(tusb_device_t *dev, uint8_t EPn);
uint32_t tusb_read_ep0(tusb_device_t *dev, void *buf);
void tusb_recv_data(tusb_device_t *dev, uint8_t EPn, uint16_t EP);
static void tusb_otg_device_prepare_setup(tusb_device_t *dev);
void tusb_setup_handler(tusb_device_t *dev);

#ifdef HAS_DOUBLE_BUFFER
    #define  DOUBLE_BUFF 1
#else
    #define  DOUBLE_BUFF 0
#endif
#ifdef HAS_ISO_EP
    #define  ISO_EP  1
#else
    #define  ISO_EP  0
#endif


void tusb_otg_read_data(USB_OTG_GlobalTypeDef *USBx, void *buf, uint32_t len)
{
    uint32_t *dest = (uint32_t *)buf;
    len = (len + 3) / 4;
    while (len)
    {
        if (dest)
        {
            *(uint32_t *)dest = USBx_DFIFO(0);
        }
        else
        {
            uint32_t t = USBx_DFIFO(0);
            (void)t;
        }
        dest++;
        len--;
    }
}

static uint32_t get_max_in_packet_size(PCD_TypeDef *USBx, uint8_t EPn)
{
    uint32_t maxpacket = USBx_INEP(EPn)->DIEPCTL & USB_OTG_DIEPCTL_MPSIZ;
    if (EPn == 0)
    {
        if (USBx == USB_OTG_FS)
        {
            // for the ep0 in otg FS core, the max packet size is defined by
            // DEP0CTL_MPS_64, DEP0CTL_MPS_32, DEP0CTL_MPS_16, DEP0CTL_MPS_8
            maxpacket = 8ul * (1ul << (3 - (maxpacket & 3)));
        }
        else
        {
            // For high speed core, ep0 always 64 bytes, there is no mps info in DIEPCTL
            maxpacket = 64;
        }
    }
    return maxpacket;
}

static uint32_t get_max_out_packet_size(PCD_TypeDef *USBx, uint8_t EPn)
{
    uint32_t maxpacket = USBx_OUTEP(EPn)->DOEPCTL & USB_OTG_DOEPCTL_MPSIZ;
    if (EPn == 0)
    {
        // for the ep0 in otg FS/HS core, the max packet size is defined by
        // DEP0CTL_MPS_64, DEP0CTL_MPS_32, DEP0CTL_MPS_16, DEP0CTL_MPS_8
        maxpacket = 8ul * (1ul << (3 - (maxpacket & 3)));
    }
    return maxpacket;
}

int tusb_send_data(tusb_device_t *dev, uint8_t EPn, const void *data, uint16_t len, uint8_t option)
{
    PCD_TypeDef *USBx = GetUSB(dev);
    tusb_ep_data *ep = &dev->Ep[EPn];
    uint32_t maxpacket = GetInMaxPacket(dev, EPn);
    uint32_t pktCnt;
    uint32_t total_len = len;
    ep->tx_total_size = len;
    ep->tx_need_zlp = (TUSB_TXF_ZLP & option) != 0;
    USB_OTG_INEndpointTypeDef *epin = USBx_INEP(EPn);
    ep->tx_buf = (const uint8_t *)data;
    if (epin->DIEPCTL & USB_OTG_DIEPCTL_EPENA)
    {
        return -1;
    }

    if (EPn == 0)
    {
        // EP0 always send 1 packet
        if (len > maxpacket)
        {
            len = maxpacket;
        }
    }
    // set remain size of tx buffer, current tx size is saved in the DIEPTSIZ register
    ep->tx_remain_size = total_len - len;
    // calculate last packet size
    ep->tx_last_size = len ? (len - 1) % maxpacket + 1 : 0;

    pktCnt = (((len + maxpacket - 1) / maxpacket) << 19);
    if (pktCnt == 0)
    {
        // avoid zero packet count, used to send ZLP(zero length packet)
        pktCnt = 1 << 19;
    }
    // clear and set the EPT size field
    epin->DIEPTSIZ =  pktCnt | len;

    if (ISO_EP && ((epin->DIEPCTL & USB_OTG_DIEPCTL_EPTYP)  == ((USB_EP_ISOCHRONOUS) << (USB_OTG_DIEPCTL_EPTYP_Pos))))
    {
        // ISO ep
        //USBx_INEP(EPn)->DIEPTSIZ &= ~(USB_OTG_DIEPTSIZ_MULCNT);
        epin->DIEPTSIZ |= (USB_OTG_DIEPTSIZ_MULCNT & (1 << 29));

        if ((USBx_DEVICE->DSTS & (1 << 8)) == 0)
        {
            epin->DIEPCTL |= USB_OTG_DIEPCTL_SODDFRM;
        }
        else
        {
            epin->DIEPCTL |= USB_OTG_DIEPCTL_SD0PID_SEVNFRM;
        }
    }


    if (USBx->GAHBCFG & USB_OTG_GAHBCFG_DMAEN)
    {
        if ((uint32_t)ep->tx_buf & 3)
        {
            while (1);
        }
        epin->DIEPDMA = (uint32_t)ep->tx_buf;
    }
    else
    {
        if (len > 0)
        {
            USBx_DEVICE->DIEPEMPMSK |= (1 << EPn);
        }
    }
    // do not fill data here, data will be filled in the empty interrupt
    //copy_data_to_fifo(dev, ep, EPn, data, len, total_len);
    epin->DIEPCTL |= (USB_OTG_DIEPCTL_CNAK | USB_OTG_DIEPCTL_EPENA);

    return 0;
}


// when fifo empty call this function
void tusb_fifo_empty(tusb_device_t *dev, uint8_t EPn)
{
    tusb_ep_data *ep = &dev->Ep[EPn];
    PCD_TypeDef *USBx = GetUSB(dev);
    USB_OTG_INEndpointTypeDef *epin = USBx_INEP(EPn);
    uint32_t xfer_size = epin->DIEPTSIZ & USB_OTG_DIEPTSIZ_XFRSIZ;
    uint32_t maxpacket = GetInMaxPacket(dev, EPn);
    uint32_t fifo_len = epin->DTXFSTS & USB_OTG_DTXFSTS_INEPTFSAV;
    const uint8_t *src = (const uint8_t *)ep->tx_buf;
    uint32_t len32b;
    // round transfer size to max packet boundary
    if (xfer_size > fifo_len * 4)
    {
        xfer_size = (fifo_len * 4 / maxpacket) * maxpacket;
    }
    len32b = (xfer_size + 3) / 4;
    // push data to fifo
    while (len32b)
    {
        USBx_DFIFO(EPn) = *((uint32_t *)src);
        src += 4;
        len32b--;
    }
    // adjust the dat buffer
    ep->tx_buf = src;
    //calculate last packet size
    ep->tx_last_size = xfer_size ? (xfer_size - 1) % maxpacket + 1 : 0;
    //if( xfer_size == 0){
    if ((epin->DIEPTSIZ & USB_OTG_DIEPTSIZ_XFRSIZ) == 0)
    {
        // this xfer complete, so no need the fifo empty interrupt
        USBx_DEVICE->DIEPEMPMSK &= ~(1ul << EPn);
    }

    return;
}

// called by the ep data interrupt handler when last packet tranfer done
void tusb_send_data_done(tusb_device_t *dev, uint8_t EPn)
{
    PCD_TypeDef *USBx = GetUSB(dev);
    tusb_ep_data *ep = &dev->Ep[EPn];
    uint32_t maxpacket = get_max_in_packet_size(USBx, EPn);
    if (EPn == 0)
    {
        if (ep->tx_remain_size)
        {
            tusb_send_data(dev, EPn, ep->tx_buf, ep->tx_remain_size, 0);
        }
        else if (ep->tx_last_size == maxpacket)
        {
            // Send a ZLP
            tusb_send_data(dev, EPn, ep->tx_buf, 0, 0);
            // Set the total size to 1, so ep0 done will start to recv status out
            ep->tx_total_size = 1;
        }
        else
        {
            // EP0 send done
            if (dev->ep0_tx_done)
            {
                // invoke status transmitted call back for ep0
                dev->ep0_tx_done(dev);
                dev->ep0_tx_done = 0;
            }
            if (ep->tx_total_size)
            {
                // borrow the setup buffer to recv status packet
                tusb_set_recv_buffer(dev, 0, &dev->setup, 24);
                // prepare receive status packet
                tusb_set_rx_valid(dev, 0);
            }
            else
            {
                // ep0 tx size = 1, status IN, prepare for setup
                tusb_otg_device_prepare_setup(dev);
            }
        }
    }
    else
    {
        if (ep->tx_last_size == maxpacket && ep->tx_need_zlp)
        {
            // Send a ZLP
            tusb_send_data(dev, EPn, ep->tx_buf, 0, 0);
            return;
        }
        // clear the fifo empty mask
        tusb_on_tx_done(dev, EPn);
    }
}

// un-like the USB FS core, we need buffer size to set the ep valid
void tusb_set_rx_valid(tusb_device_t *dev, uint8_t EPn)
{
    PCD_TypeDef *USBx = GetUSB(dev);
    USB_OTG_OUTEndpointTypeDef *epout = USBx_OUTEP(EPn);
    tusb_ep_data *ep = &dev->Ep[EPn];
    uint32_t maxpacket = GetOutMaxPacket(dev, EPn);
    uint32_t pktCnt;
    uint32_t len = ep->rx_size;
    ep->rx_count = 0;

    if (EPn == 0)
    {
        // EP0 always recv 1 packet
        if (len > maxpacket)
        {
            len = maxpacket;
        }
    }

    pktCnt = (((len + maxpacket - 1) / maxpacket));
    if (pktCnt == 0)
    {
        // avoid zero packet count, used to send ZLP(zero length packet)
        pktCnt = 1;
    }
    len = pktCnt * maxpacket;
    // clear and set the EPT size field

    epout->DOEPTSIZ &= ~(USB_OTG_DOEPTSIZ_XFRSIZ);
    epout->DOEPTSIZ &= ~(USB_OTG_DOEPTSIZ_PKTCNT);


    epout->DOEPTSIZ |= (pktCnt << 19) | len;

    if (USBx->GAHBCFG & USB_OTG_GAHBCFG_DMAEN)
    {
        epout->DOEPDMA = (uint32_t)ep->rx_buf;
    }

    if (ISO_EP && ((epout->DOEPCTL & USB_OTG_DOEPCTL_EPTYP)  == ((USB_EP_ISOCHRONOUS) << (USB_OTG_DOEPCTL_EPTYP_Pos))))
    {
        if ((USBx_DEVICE->DSTS & (1 << 8)) == 0)
        {
            epout->DOEPCTL |= USB_OTG_DOEPCTL_SODDFRM;
        }
        else
        {
            epout->DOEPCTL |= USB_OTG_DOEPCTL_SD0PID_SEVNFRM;
        }
    }
    epout->DOEPCTL |= (USB_OTG_DOEPCTL_CNAK | USB_OTG_DOEPCTL_EPENA);
}

static void tusb_otg_device_prepare_setup(tusb_device_t *dev)
{
    USB_OTG_GlobalTypeDef *USBx = GetUSB(dev);
    USB_OTG_OUTEndpointTypeDef *outep = USBx_OUTEP(0);
    outep->DOEPTSIZ = (USB_OTG_DOEPTSIZ_PKTCNT & (1 << 19)) | (3 * 8) | USB_OTG_DOEPTSIZ_STUPCNT;
    if (USBx->GAHBCFG & USB_OTG_GAHBCFG_DMAEN)
    {
        outep->DOEPDMA = (uint32_t)&dev->setup;
        /* EP enable */
        outep->DOEPCTL = 0x80008000;
    }
}

// For OTG core, the address will auto take effect after status sent,
// So we need set address before send status
//void tusb_set_addr_after_status(tusb_device_t* dev);
void tusb_set_addr_before_status(tusb_device_t *dev)
{
    USB_OTG_GlobalTypeDef *USBx = GetUSB(dev);
    USBx_DEVICE->DCFG &= ~(USB_OTG_DCFG_DAD);
    USBx_DEVICE->DCFG |= (dev->addr << 4) & USB_OTG_DCFG_DAD ;
}

void tusb_set_stall(tusb_device_t *dev, uint8_t EPn)
{
    PCD_TypeDef *USBx =  GetUSB(dev);
    uint8_t ep = EPn & 0x7f;
    if (ep == 0)
    {
        USBx_INEP(0)->DIEPCTL |= USB_OTG_DIEPCTL_STALL;
        USBx_OUTEP(0)->DOEPCTL |= USB_OTG_DOEPCTL_STALL;
        tusb_otg_device_prepare_setup(dev);
    }
    else
    {
        if (EPn & 0x80)
        {
            USBx_INEP(ep)->DIEPCTL |= USB_OTG_DIEPCTL_STALL;
        }
        else
        {
            USBx_OUTEP(ep)->DOEPCTL |= USB_OTG_DOEPCTL_STALL;
        }
    }
}

void tusb_clear_stall(tusb_device_t *dev, uint8_t EPn)
{
    PCD_TypeDef *USBx =  GetUSB(dev);
    uint8_t ep = EPn & 0x7f;
    if (ep == 0)
    {
        USBx_INEP(0)->DIEPCTL &= ~USB_OTG_DIEPCTL_STALL;
        USBx_OUTEP(0)->DOEPCTL &= ~USB_OTG_DOEPCTL_STALL;
    }
    else
    {
        if (EPn & 0x80)
        {
            uint32_t type = (USBx_INEP(ep)->DIEPCTL & USB_OTG_DIEPCTL_EPTYP) >> USB_OTG_DIEPCTL_EPTYP_Pos;
            USBx_INEP(ep)->DIEPCTL &= ~USB_OTG_DIEPCTL_STALL;
            if (type == USB_EP_INTERRUPT || type == USB_EP_BULK)
            {
                USBx_INEP(ep)->DIEPCTL |= USB_OTG_DIEPCTL_SD0PID_SEVNFRM; /* DATA0 */
            }
        }
        else
        {
            uint32_t type = (USBx_OUTEP(ep)->DOEPCTL & USB_OTG_DOEPCTL_EPTYP) >> USB_OTG_DOEPCTL_EPTYP_Pos;
            USBx_OUTEP(ep)->DOEPCTL &= ~USB_OTG_DOEPCTL_STALL;
            if (type == USB_EP_INTERRUPT || type == USB_EP_BULK)
            {
                USBx_OUTEP(ep)->DOEPCTL |= USB_OTG_DOEPCTL_SD0PID_SEVNFRM; /* DATA0 */
            }
        }
    }
}

#define  INTR()   (USBx->GINTSTS & USBx->GINTMSK)
// Interrupt handler of DEVICE mode
void tusb_otg_device_handler(tusb_device_t *dev)
{
    USB_OTG_GlobalTypeDef *USBx = GetUSB(dev);
    uint32_t MAX_EP_NUM = USBx == USB_OTG_FS ? USB_OTG_FS_MAX_EP_NUM : USB_OTG_HS_MAX_EP_NUM;
    /* ensure that we are in device mode */
    //if (USB_GetMode(USBx) == USB_OTG_MODE_DEVICE)
    {
        // no flag set, return directly
        if (INTR() == 0)
        {
            return;
        }

        if (INTR() & USB_OTG_GINTSTS_MMIS)
        {
            /* incorrect mode, acknowledge the interrupt */
            USBx->GINTSTS = USB_OTG_GINTSTS_MMIS;
        }

        // Handle output
        if (INTR() & USB_OTG_GINTSTS_OEPINT)
        {
            uint32_t ep_intr = ((USBx_DEVICE->DAINT & USBx_DEVICE->DAINTMSK)) >> 16;
            uint8_t EPn = 0;
            while (ep_intr)
            {
                if (EPn >= MAX_EP_NUM)
                {
                    break;
                }
                if (ep_intr & 0x1)
                {
                    uint32_t epint = USBx_OUTEP(EPn)->DOEPINT;
                    tusb_ep_data *ep = &dev->Ep[EPn];
                    if ((epint & USB_OTG_DOEPINT_XFRC) == USB_OTG_DOEPINT_XFRC)
                    {
                        uint32_t maxpacket = GetOutMaxPacket(dev, EPn);
                        if (USBx->GAHBCFG & USB_OTG_GAHBCFG_DMAEN)
                        {
                            // Calculate recv data length from the XFRSIZ field
                            uint32_t total_xfer_size;
                            if (EPn == 0)
                            {
                                total_xfer_size = maxpacket;
                            }
                            else
                            {
                                total_xfer_size = ((ep->rx_size + maxpacket - 1) / maxpacket) * maxpacket;
                            }
                            // DMA enabled, recv data count is total_xfer_size minus transfer remain length
                            ep->rx_count += total_xfer_size - (USBx_OUTEP(EPn)->DOEPTSIZ & USB_OTG_DOEPTSIZ_XFRSIZ);
                        }
                        // Get data from ep0, data of setup packet, the recv buffer is prepared in tusb_setup_handler
                        if (EPn == 0)
                        {
                            // rx_count = 0 or (rx_count % maxpacket) != 0 means got short packet, transfer complete
                            // rx_count >= rx_size, means got full data, transfer complete
                            if (ep->rx_count == 0 || ep->rx_count >= ep->rx_size || ep->rx_count % maxpacket)
                            {
                                if (dev->ep0_rx_done)
                                {
                                    dev->ep0_rx_done(dev);
                                    dev->ep0_rx_done = 0;
                                }
                                ep->rx_buf = 0;
                                if (ep->rx_count == 0)
                                {
                                    // ep0 rx count = 0, got status out, re-enable setup packet receive
                                    tusb_otg_device_prepare_setup(dev);
                                }
                            }
                            else
                            {
                                ep->rx_buf += ep->rx_count;
                                ep->rx_size -= ep->rx_count;
                                tusb_set_rx_valid(dev, EPn);
                            }
                        }
                        else
                        {
                            if (tusb_on_rx_done(dev, EPn, ep->rx_buf, ep->rx_count) == 0)
                            {
                                ep->rx_count = 0;
                                tusb_set_rx_valid(dev, EPn);
                            }
                            else
                            {
                                ep->rx_count = ep->rx_size;
                            }
                        }
                    }
                    // Read the DOEPINT again, to make sure the SETUP flag is set
                    epint = USBx_OUTEP(EPn)->DOEPINT;
                    if ((epint & USB_OTG_DOEPINT_STUP) == USB_OTG_DOEPINT_STUP)
                    {
                        ep->rx_buf = 0;
                        tusb_setup_handler(dev);
                        if (ep->rx_buf)
                        {
                            // Data write setup
                            // rx_buf is not null, means setup need write some data
                            tusb_set_rx_valid(dev, EPn);
                        }
                    }
                    // clear all interrupt flags
                    USBx_OUTEP(EPn)->DOEPINT = epint;
                }
                ep_intr >>= 1;
                EPn++;
            }
        }

        // handle input
        if (INTR() & USB_OTG_GINTSTS_IEPINT)
        {
            uint32_t ep_intr = (USBx_DEVICE->DAINT & USBx_DEVICE->DAINTMSK) & 0xffff;
            uint8_t EPn = 0;
            while (ep_intr)
            {
                if (EPn >= MAX_EP_NUM)
                {
                    break;
                }
                if (ep_intr & 0x1)
                {
                    USB_OTG_INEndpointTypeDef *epin = USBx_INEP(EPn);
                    uint32_t epint = epin->DIEPINT;
                    // Xfer complete interrupt handler
                    if (EPn == 0)
                    {
                        // Endpoint is 0, and DMA enabled, adjust the tx_buf pointer
                        // Endpoiny 0 always transfer max packet size
                        if (USBx->GAHBCFG & USB_OTG_GAHBCFG_DMAEN)
                        {
                            tusb_ep_data *ep = &dev->Ep[EPn];
                            ep->tx_buf += GetInMaxPacket(dev, EPn);
                        }
                    }


                    if (epint & USB_OTG_DIEPINT_XFRC)
                    {
                        USBx_DEVICE->DIEPEMPMSK &= ~(0x1ul << EPn);
                        epin->DIEPINT = USB_OTG_DIEPINT_XFRC;
                        tusb_send_data_done(dev, EPn);
                    }

//          if(( epint & USB_OTG_DIEPINT_TOC) == USB_OTG_DIEPINT_TOC){
//            epin->DIEPINT = USB_OTG_DIEPINT_TOC;
//          }
//          if(( epint & USB_OTG_DIEPINT_ITTXFE) == USB_OTG_DIEPINT_ITTXFE){
//            epin->DIEPINT = USB_OTG_DIEPINT_ITTXFE;
//          }
//          if(( epint & USB_OTG_DIEPINT_INEPNE) == USB_OTG_DIEPINT_INEPNE){
//            epin->DIEPINT = USB_OTG_DIEPINT_INEPNE;
//          }
//          if(( epint & USB_OTG_DIEPINT_EPDISD) == USB_OTG_DIEPINT_EPDISD){
//            epin->DIEPINT = USB_OTG_DIEPINT_EPDISD;
//          }
                    // FIFO empty interrupt handler
                    if (((epint & USB_OTG_DIEPINT_TXFE) == USB_OTG_DIEPINT_TXFE) && (USBx_DEVICE->DIEPEMPMSK & (1 << EPn)))
                    {
                        tusb_fifo_empty(dev, EPn);
                    }
                    // clear all interrupts
                    epin->DIEPINT = epint;
                }
                ep_intr >>= 1;
                EPn++;
            }
        }
        /* Handle Resume Interrupt */
        if (INTR() & USB_OTG_GINTSTS_WKUINT)
        {
            // TODO: leave low power mode
            /* Clear the Remote Wake-up Signaling */
            USBx_DEVICE->DCTL &= ~USB_OTG_DCTL_RWUSIG;
            USBx->GINTSTS = USB_OTG_GINTSTS_WKUINT;
        }

        /* Handle Suspend Interrupt */
        if (INTR()& USB_OTG_GINTSTS_USBSUSP)
        {
            // TODO: enter low power mode
            USBx->GINTSTS = USB_OTG_GINTSTS_USBSUSP;
        }
        /* Handle LPM Interrupt */
#if defined(USB_OTG_GINTSTS_LPMINT)
        if (INTR() & USB_OTG_GINTSTS_LPMINT)
        {
            USBx->GINTSTS = USB_OTG_GINTSTS_LPMINT;
        }
#endif
        /* Handle Reset Interrupt */
        if (INTR() & USB_OTG_GINTSTS_USBRST)
        {
            uint32_t i;
            USBx_DEVICE->DCTL &= ~USB_OTG_DCTL_RWUSIG;
            flush_tx(USBx, 0x10);
            for (i = 0; i < MAX_EP_NUM ; i++)
            {
                USBx_INEP(i)->DIEPINT = 0xFF;
                USBx_OUTEP(i)->DOEPINT = 0xFF;
            }
            USBx_DEVICE->DAINT = 0xFFFFFFFF;
            USBx_DEVICE->DAINTMSK |= 0x10001;
            {
                USBx_DEVICE->DOEPMSK |= (USB_OTG_DOEPMSK_STUPM | USB_OTG_DOEPMSK_XFRCM | USB_OTG_DOEPMSK_EPDM);
                USBx_DEVICE->DIEPMSK |= (USB_OTG_DIEPMSK_TOM | USB_OTG_DIEPMSK_XFRCM | USB_OTG_DIEPMSK_EPDM);
            }

            /* Set Default Address to 0 */
            USBx_DEVICE->DCFG &= ~USB_OTG_DCFG_DAD;

            USBx->GINTSTS = USB_OTG_GINTSTS_USBRST;
        }

        /* Handle Enumeration done Interrupt */
        if (INTR() & USB_OTG_GINTSTS_ENUMDNE)
        {
            tusb_reconfig(dev);
            USBx->GUSBCFG &= ~USB_OTG_GUSBCFG_TRDT;
            switch (USBx_DEVICE->DSTS & USB_OTG_DSTS_ENUMSPD)
            {
#if defined(DSTS_ENUMSPD_HS_PHY_30MHZ_OR_60MHZ)
            case DSTS_ENUMSPD_HS_PHY_30MHZ_OR_60MHZ:
                USBx->GUSBCFG |= (uint32_t)((USBD_HS_TRDT_VALUE << 10) & USB_OTG_GUSBCFG_TRDT);
                break;
#endif
            case DSTS_ENUMSPD_LS_PHY_6MHZ:
                USBx_INEP(0)->DIEPCTL |= 3; // force ep0 packet size to 8 when in LS mode
            case DSTS_ENUMSPD_FS_PHY_30MHZ_OR_60MHZ:
            case DSTS_ENUMSPD_FS_PHY_48MHZ:
                USBx->GUSBCFG |= (uint32_t)((0x6 << 10) & USB_OTG_GUSBCFG_TRDT);
                break;
            }
            /* setup EP0 to receive SETUP packets */
            tusb_otg_device_prepare_setup(dev);
            USBx_DEVICE->DCTL |= USB_OTG_DCTL_CGINAK;
            USBx->GINTSTS = USB_OTG_GINTSTS_ENUMDNE;
        }

        /* Handle RxQLevel Interrupt */
        if (INTR() & USB_OTG_GINTSTS_RXFLVL)
        {
            USB_MASK_INTERRUPT(USBx, USB_OTG_GINTSTS_RXFLVL);
            {
                uint32_t sts = USBx->GRXSTSP;
                uint8_t EPn = sts & USB_OTG_GRXSTSP_EPNUM;
                uint32_t len = (sts & USB_OTG_GRXSTSP_BCNT) >> 4;
                if (((sts & USB_OTG_GRXSTSP_PKTSTS) >> 17) ==  STS_DATA_UPDT)
                {
                    tusb_ep_data *ep = &dev->Ep[EPn];
                    if (ep->rx_count < ep->rx_size && ep->rx_buf)
                    {
                        // copy data packet
                        tusb_otg_read_data(USBx, ep->rx_buf + ep->rx_count, len);
                        ep->rx_count += len;
                    }
                    else
                    {
                        // drop the data because no memory to handle them
                        tusb_otg_read_data(USBx, 0, len);
                    }
                }
                else if (((sts & USB_OTG_GRXSTSP_PKTSTS) >> 17) ==  STS_SETUP_UPDT)
                {
                    // copy setup packet
                    tusb_otg_read_data(USBx, &dev->setup, len);
                }
            }
            USB_UNMASK_INTERRUPT(USBx, USB_OTG_GINTSTS_RXFLVL);
        }

        /* Handle SOF Interrupt */
        if (INTR() & USB_OTG_GINTSTS_SOF)
        {
            USBx->GINTSTS = USB_OTG_GINTSTS_SOF;
        }

        /* Handle Incomplete ISO IN Interrupt */
        if (INTR() & USB_OTG_GINTSTS_IISOIXFR)
        {
            USBx->GINTSTS = USB_OTG_GINTSTS_IISOIXFR;
        }

        /* Handle Incomplete ISO OUT Interrupt */
        if (INTR() & USB_OTG_GINTSTS_PXFR_INCOMPISOOUT)
        {
            USBx->GINTSTS = USB_OTG_GINTSTS_PXFR_INCOMPISOOUT;
        }

        /* Handle Connection event Interrupt */
        if (INTR() & USB_OTG_GINTSTS_SRQINT)
        {
            USBx->GINTSTS = USB_OTG_GINTSTS_SRQINT;
        }

        /* Handle Disconnection event Interrupt */
        if (INTR() & USB_OTG_GINTSTS_OTGINT)
        {
            uint32_t temp = USBx->GOTGINT;
            if ((temp & USB_OTG_GOTGINT_SEDET) == USB_OTG_GOTGINT_SEDET)
            {
                // dis connect event
            }
            USBx->GOTGINT |= temp;
        }
    }
}

#endif // #if defined(USB_OTG_FS) || defined(USB_OTG_HS)

#endif // #ifndef NO_DEVICE

