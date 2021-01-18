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

//  STM32 OTG usb hardware handler
#include "teeny_usb.h"
#include "string.h"
#if defined(USB_OTG_FS) || defined(USB_OTG_HS)

// #define  ENABLE_VBUS_DETECT

// Macro check

// FS core always work in full speed
#ifndef   OTG_FS_SPEED
    #define   OTG_FS_SPEED      USB_OTG_SPEED_FULL
#endif

// HS core always work in high speed
#ifndef   OTG_HS_SPEED
    #if defined(USB_OTG_HS)
        #define   OTG_HS_SPEED      USB_OTG_SPEED_HIGH
    #else
        #define   OTG_HS_SPEED      OTG_FS_SPEED
    #endif
#endif

#if defined(OTG_HS_EMBEDDED_PHY) && defined(OTG_HS_EXTERNAL_PHY)
    #error Embedded and external HS phy can not be used together
#endif

#if defined(OTG_FS_ENABLE_DMA)
    #ifndef DESCRIPTOR_BUFFER_SIZE
        #error DMA enabled, but descriptor buffer size not defined
    #elif DESCRIPTOR_BUFFER_SIZE == 0
        #error DMA enabled, but descriptor buffer size too small
    #endif
#endif  // OTG_FS_ENABLE_DMA

#if defined(OTG_HS_EXTERNAL_PHY)

    #if !defined(OTG_HS_ULPI_IO_CLK_ENABLE)
        #error External high speed phy used, but ULPI IO clock not defined
    #endif

    #if defined(OTG_HS_ULPI_D0) && defined(OTG_HS_ULPI_D1) &&   \
        defined(OTG_HS_ULPI_D2) && defined(OTG_HS_ULPI_D3) &&   \
        defined(OTG_HS_ULPI_D4) && defined(OTG_HS_ULPI_D5) &&   \
        defined(OTG_HS_ULPI_D6) && defined(OTG_HS_ULPI_D7) &&   \
        defined(OTG_HS_ULPI_DIR) && defined(OTG_HS_ULPI_STP) && \
        defined(OTG_HS_ULPI_NXT) && defined(OTG_HS_ULPI_CK)
    #else
        #error External high speed phy used, but some ULPI IO not defined
    #endif

#endif  //OTG_HS_EXTERNAL_PHY

void flush_rx(USB_OTG_GlobalTypeDef *USBx)
{
    USBx->GRSTCTL = USB_OTG_GRSTCTL_RXFFLSH;
    while ((USBx->GRSTCTL & USB_OTG_GRSTCTL_RXFFLSH) == USB_OTG_GRSTCTL_RXFFLSH);
}

void flush_tx(USB_OTG_GlobalTypeDef *USBx, uint32_t num)
{
    USBx->GRSTCTL = (USB_OTG_GRSTCTL_TXFFLSH | (uint32_t)(num << 6));
    while ((USBx->GRSTCTL & USB_OTG_GRSTCTL_TXFFLSH) == USB_OTG_GRSTCTL_TXFFLSH);
}

static void Wait_CoreReset(USB_OTG_GlobalTypeDef *USBx)
{
    /* Wait for AHB master IDLE state. */
    while ((USBx->GRSTCTL & USB_OTG_GRSTCTL_AHBIDL) == 0);
    /* Core Soft Reset */
    USBx->GRSTCTL |= USB_OTG_GRSTCTL_CSRST;
    while ((USBx->GRSTCTL & USB_OTG_GRSTCTL_CSRST) == USB_OTG_GRSTCTL_CSRST);
}

#if defined(USB_HS_PHYC)
void tusb_delay_ms(uint32_t ms);
static void USB_HS_PHYCInit(USB_OTG_GlobalTypeDef *USBx)
{
    /* Enable LDO */
    USB_HS_PHYC->USB_HS_PHYC_LDO |= USB_HS_PHYC_LDO_ENABLE;
    /* wait for LDO Ready */
    while ((USB_HS_PHYC->USB_HS_PHYC_LDO & USB_HS_PHYC_LDO_STATUS) == RESET);

    /* Controls PHY frequency operation selection */
    if (HSE_VALUE == 12000000)        /* HSE = 12MHz */
    {
        USB_HS_PHYC->USB_HS_PHYC_PLL = (uint32_t)(0x0 << 1);
    }
    else if (HSE_VALUE == 12500000)   /* HSE = 12.5MHz */
    {
        USB_HS_PHYC->USB_HS_PHYC_PLL = (uint32_t)(0x2 << 1);
    }
    else if (HSE_VALUE == 16000000)   /* HSE = 16MHz */
    {
        USB_HS_PHYC->USB_HS_PHYC_PLL = (uint32_t)(0x3 << 1);
    }
    else if (HSE_VALUE == 24000000)   /* HSE = 24MHz */
    {
        USB_HS_PHYC->USB_HS_PHYC_PLL = (uint32_t)(0x4 << 1);
    }
    else if (HSE_VALUE == 25000000)   /* HSE = 25MHz */
    {
        USB_HS_PHYC->USB_HS_PHYC_PLL = (uint32_t)(0x5 << 1);
    }
    else if (HSE_VALUE == 32000000)   /* HSE = 32MHz */
    {
        USB_HS_PHYC->USB_HS_PHYC_PLL = (uint32_t)(0x7 << 1);
    }
    /* Control the tuning interface of the High Speed PHY */
    USB_HS_PHYC->USB_HS_PHYC_TUNE |= USB_HS_PHYC_TUNE_VALUE;
    /* Enable PLL internal PHY */
    USB_HS_PHYC->USB_HS_PHYC_PLL |= USB_HS_PHYC_PLL_PLLEN;
    /* 2ms Delay required to get internal phy clock stable */
    tusb_delay_ms(20);
}
#endif


void tusb_close_core(tusb_core_t *core)
{
    USB_OTG_GlobalTypeDef *USBx = GetUSB(core);
    uint32_t i;

    USBx->GAHBCFG &= ~USB_OTG_GAHBCFG_GINT;

    if (USBx->GINTSTS & USB_OTG_GINTSTS_CMOD)
    {
        // current work in host mode
        uint32_t MAX_CH_NUM = USBx == USB_OTG_FS ? USB_OTG_FS_MAX_CH_NUM : USB_OTG_HS_MAX_CH_NUM;

        flush_rx(USBx);
        flush_tx(USBx,  0x10);

        for (i = 0U; i <= MAX_CH_NUM; i++)
        {
            uint32_t value = USBx_HC(i)->HCCHAR;
            value |=  USB_OTG_HCCHAR_CHDIS;
            value &= ~USB_OTG_HCCHAR_CHENA;
            value &= ~USB_OTG_HCCHAR_EPDIR;
            USBx_HC(i)->HCCHAR = value;
        }

        /* Halt all channels to put them into a known state. */
        for (i = 0U; i <= MAX_CH_NUM; i++)
        {
            uint32_t count = 0U;
            uint32_t value = USBx_HC(i)->HCCHAR;
            value |= USB_OTG_HCCHAR_CHDIS;
            value |= USB_OTG_HCCHAR_CHENA;
            value &= ~USB_OTG_HCCHAR_EPDIR;
            USBx_HC(i)->HCCHAR = value;
            do
            {
                if (++count > 1000U)
                {
                    break;
                }
            }
            while ((USBx_HC(i)->HCCHAR & USB_OTG_HCCHAR_CHENA) == USB_OTG_HCCHAR_CHENA);
        }
        USBx_HOST->HAINT = 0xFFFFFFFFU;
        USBx->GINTSTS = 0xFFFFFFFFU;
    }
    else
    {
        // work in device mode

        /* Clear Pending interrupt */
        for (i = 0; i < EP_NUM ; i++)
        {
            USBx_INEP(i)->DIEPINT  = 0xFF;
            USBx_OUTEP(i)->DOEPINT  = 0xFF;
        }
        USBx_DEVICE->DAINT = 0xFFFFFFFF;
        /* Clear interrupt masks */
        USBx_DEVICE->DIEPMSK  = 0;
        USBx_DEVICE->DOEPMSK  = 0;
        USBx_DEVICE->DAINTMSK = 0;

        /* Flush the FIFO */
        flush_rx(USBx);
        flush_tx(USBx,  0x10);

        // disconnect
        USBx_DEVICE->DCTL |= USB_OTG_DCTL_SDIS ;
    }

    if (USBx == USB_OTG_FS)
    {
        __HAL_RCC_USB_OTG_FS_CLK_DISABLE();
        NVIC_DisableIRQ(OTG_FS_IRQn);
        // maybe need de-init the IOs
    }
#if defined(USB_OTG_HS)
    else if (USBx == USB_OTG_HS)
    {
#if defined(USB_HS_PHYC)
        __HAL_RCC_OTGPHYC_CLK_DISABLE();
#endif
        __HAL_RCC_USB_OTG_HS_CLK_DISABLE();
        __HAL_RCC_USB_OTG_HS_ULPI_CLK_DISABLE();
        NVIC_DisableIRQ(OTG_HS_IRQn);
    }
#endif
}

void tusb_close_device(tusb_device_t *dev)
{
    tusb_close_core((tusb_core_t *) dev);
}

void tusb_close_host(tusb_host_t *host)
{
    tusb_close_core((tusb_core_t *) host);
}

void tusb_close_otg(tusb_otg_t *otg)
{
    tusb_close_core((tusb_core_t *) &otg->host);
}


#define AFL(val, pin)   ((val)<< (( (pin))*4))
#define AFH(val, pin)   ((val)<< (( (pin)-8)*4))
void set_io_af_mode(GPIO_TypeDef *GPIO, uint8_t pin, uint8_t af)
{
#if defined(STM32F1)
#else
    GPIO->MODER &= ~(GPIO_MODER_MODER0 << (pin * 2));
    GPIO->MODER |= (GPIO_MODER_MODER0_1 << (pin * 2));
    GPIO->OTYPER &= ~(GPIO_OTYPER_OT_0 << pin);
    GPIO->OSPEEDR |= ((GPIO_OSPEEDER_OSPEEDR0_0 | GPIO_OSPEEDER_OSPEEDR0_1)  << (pin * 2));
    if (pin > 7)
    {
        GPIO->AFR[1] &= ~(AFH(0xf, pin));
        GPIO->AFR[1] |= (AFH(af, pin));
    }
    else
    {
        GPIO->AFR[0] &= ~(AFL(0xf, pin));
        GPIO->AFR[0] |= (AFL(af, pin));
    }
#endif
}


// Setup IO for OTG_FS core
#if defined(OTG_FS_EMBEDDED_PHY)
static void tusb_setup_otg_fs_io(void)
{
    /**USB_OTG_FS GPIO Configuration
    PA8     ------> USB_OTG_FS_SOF
    PA9     ------> USB_OTG_FS_VBUS
    PA10     ------> USB_OTG_FS_ID
    PA11     ------> USB_OTG_FS_DM
    PA12     ------> USB_OTG_FS_DP
    */
#if defined(STM32F1)
#else
    __HAL_RCC_GPIOA_CLK_ENABLE();
    set_io_af_mode(GPIOA, 11, GPIO_AF10_OTG_FS);
    set_io_af_mode(GPIOA, 12, GPIO_AF10_OTG_FS);
#endif
#if defined(ENABLE_VBUS_DETECT)
    // Mode = Input
    GPIOA->MODER &= ~(GPIO_MODER_MODER9);
    // No pull
    GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR9_0 | GPIO_PUPDR_PUPDR9_1);
#endif
}
#endif


#if defined(OTG_HS_EMBEDDED_PHY)
// Setup IO for OTG_HS core with embedded HS phy
static void tusb_setup_otg_hs_io(void)
{
    __HAL_RCC_GPIOB_CLK_ENABLE();
    set_io_af_mode(GPIOB, 14, GPIO_AF12_OTG_HS_FS);
    set_io_af_mode(GPIOB, 15, GPIO_AF12_OTG_HS_FS);
#if defined(ENABLE_VBUS_DETECT)
    // Mode = Input
    GPIOB->MODER &= ~(GPIO_MODER_MODER13);
    // No pull
    GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPDR13_0 | GPIO_PUPDR_PUPDR9_13);
#endif
}
#endif

// init the IO and OTG core
static void tusb_otg_core_init(tusb_core_t *core)
{
    USB_OTG_GlobalTypeDef *USBx = GetUSB(core);
    (void)USBx;
    if (GetUSB(core) == USB_OTG_FS)
    {
        // Init the FS core
#if defined(OTG_FS_EMBEDDED_PHY)
        // 1. Init the IO
        tusb_setup_otg_fs_io();
        // 2. Init the interrupt
        NVIC_SetPriority(OTG_FS_IRQn, 0);
        NVIC_EnableIRQ(OTG_FS_IRQn);
        // 3. Init the core
        __HAL_RCC_USB_OTG_FS_CLK_ENABLE();
        /* Select FS Embedded PHY */
        USBx->GUSBCFG |= USB_OTG_GUSBCFG_PHYSEL;
        /* Reset after a PHY select and set Host mode */
        Wait_CoreReset(USBx);
        /* Deactivate the power down*/
        USBx->GCCFG = USB_OTG_GCCFG_PWRDWN;
#endif
    }
#if defined(OTG_HS_EXTERNAL_PHY) || defined(OTG_HS_EMBEDDED_PHY)
    else if (GetUSB(core) == USB_OTG_HS)
    {
        // Init the HS core
        // 1. Init the IO
        // 2. Setup Interrupt
#if defined(OTG_HS_EMBEDDED_PHY)
        tusb_setup_otg_hs_io();

        NVIC_SetPriority(OTG_HS_IRQn, 0);
        NVIC_EnableIRQ(OTG_HS_IRQn);

        __HAL_RCC_OTGPHYC_CLK_ENABLE();
        __HAL_RCC_USB_OTG_HS_CLK_ENABLE();
        __HAL_RCC_USB_OTG_HS_ULPI_CLK_ENABLE();

#elif defined(OTG_HS_EXTERNAL_PHY)
        OTG_HS_ULPI_IO_CLK_ENABLE();
        set_io_af_mode(OTG_HS_ULPI_D0);
        set_io_af_mode(OTG_HS_ULPI_D1);
        set_io_af_mode(OTG_HS_ULPI_D2);
        set_io_af_mode(OTG_HS_ULPI_D3);
        set_io_af_mode(OTG_HS_ULPI_D4);
        set_io_af_mode(OTG_HS_ULPI_D5);
        set_io_af_mode(OTG_HS_ULPI_D6);
        set_io_af_mode(OTG_HS_ULPI_D7);
        set_io_af_mode(OTG_HS_ULPI_DIR);
        set_io_af_mode(OTG_HS_ULPI_STP);
        set_io_af_mode(OTG_HS_ULPI_NXT);
        set_io_af_mode(OTG_HS_ULPI_CK);

        NVIC_SetPriority(OTG_HS_IRQn, 0);
        NVIC_EnableIRQ(OTG_HS_IRQn);

        __HAL_RCC_USB_OTG_HS_CLK_ENABLE();
        __HAL_RCC_USB_OTG_HS_ULPI_CLK_ENABLE();
#endif
        // 3. Init the OTG HS core
#if defined(OTG_HS_EMBEDDED_PHY)
        // init embedded phy
        USBx->GCCFG &= ~(USB_OTG_GCCFG_PWRDWN);
        /* Init The UTMI Interface */
        USBx->GUSBCFG &= ~(USB_OTG_GUSBCFG_TSDPS | USB_OTG_GUSBCFG_ULPIFSLS | USB_OTG_GUSBCFG_PHYSEL);
        /* Select vbus source */
        USBx->GUSBCFG &= ~(USB_OTG_GUSBCFG_ULPIEVBUSD | USB_OTG_GUSBCFG_ULPIEVBUSI);
        /* Select UTMI Interace */
        USBx->GUSBCFG &= ~ USB_OTG_GUSBCFG_ULPI_UTMI_SEL;
        USBx->GCCFG |= USB_OTG_GCCFG_PHYHSEN;
        /* Enables control of a High Speed USB PHY */
        USB_HS_PHYCInit(USBx);
#elif defined(OTG_HS_EXTERNAL_PHY)
        // Init external phy
        __HAL_RCC_USB_OTG_HS_ULPI_CLK_ENABLE();
        USBx->GCCFG &= ~(USB_OTG_GCCFG_PWRDWN);
        /* Init The ULPI Interface */
        USBx->GUSBCFG &= ~(USB_OTG_GUSBCFG_TSDPS | USB_OTG_GUSBCFG_ULPIFSLS | USB_OTG_GUSBCFG_PHYSEL);
        /* Select vbus source */
        USBx->GUSBCFG &= ~(USB_OTG_GUSBCFG_ULPIEVBUSD | USB_OTG_GUSBCFG_ULPIEVBUSI);
#endif

#if defined(ENABLE_VBUS_DETECT)
        USBx->GUSBCFG |= USB_OTG_GUSBCFG_ULPIEVBUSD;
#endif
        /* Reset after a PHY select  */
        Wait_CoreReset(USBx);
    }
#endif // defined(OTG_HS_EXTERNAL_PHY) || defined(OTG_HS_EMBEDDED_PHY)

#if defined(OTG_HS_ENABLE_DMA)
    if (GetUSB(core) == USB_OTG_HS)
    {
        // only HS core has DMA feature
        USBx->GAHBCFG |= USB_OTG_GAHBCFG_HBSTLEN_2;
        USBx->GAHBCFG |= USB_OTG_GAHBCFG_DMAEN;
    }
#endif
}

static void tusb_init_otg_device(tusb_device_t *dev)
{
    USB_OTG_GlobalTypeDef *USBx = GetUSB(dev);
    uint32_t MAX_EP_NUM = GetUSB(dev) == USB_OTG_FS ? USB_OTG_FS_MAX_EP_NUM : USB_OTG_HS_MAX_EP_NUM;
    uint32_t i;
#if defined(USB_OTG_GCCFG_VBDEN)
#if defined(ENABLE_VBUS_DETECT)
    USBx->GCCFG |= USB_OTG_GCCFG_VBDEN;
#else
    USBx->GCCFG &= ~ USB_OTG_GCCFG_VBDEN;
    USBx->GOTGCTL |= USB_OTG_GOTGCTL_BVALOEN;
    USBx->GOTGCTL |= USB_OTG_GOTGCTL_BVALOVAL;
#endif
#endif

#if defined(USB_OTG_GCCFG_VBUSBSEN)
#if defined(ENABLE_VBUS_DETECT)
    USBx->GCCFG |= USB_OTG_GCCFG_VBUSBSEN;
#else
#if defined(USB_OTG_GCCFG_NOVBUSSENS)
    /* Enable VBUS */
    USBx->GCCFG |= USB_OTG_GCCFG_NOVBUSSENS;
#endif
#endif
#endif

#if defined(STM32F1)
    USBx->GCCFG |= USB_OTG_GCCFG_VBUSBSEN;
#endif

    USBx_PCGCCTL = 0;
    USBx_DEVICE->DCFG |= DCFG_FRAME_INTERVAL_80;

    USBx_DEVICE->DCFG |= USBx == USB_OTG_FS ? OTG_FS_SPEED : OTG_HS_SPEED;

    flush_tx(USBx, 0x10);
    flush_rx(USBx);
    USBx_DEVICE->DIEPMSK = 0;
    USBx_DEVICE->DOEPMSK = 0;
    USBx_DEVICE->DAINT = 0xFFFFFFFF;
    USBx_DEVICE->DAINTMSK = 0;

    // ep will init in the reset handler

    for (i = 0U; i < MAX_EP_NUM; i++)
    {
        if ((USBx_INEP(i)->DIEPCTL & USB_OTG_DIEPCTL_EPENA) == USB_OTG_DIEPCTL_EPENA)
        {
            if (i == 0U)
                USBx_INEP(i)->DIEPCTL = (USB_OTG_DIEPCTL_SNAK);
            else
                USBx_INEP(i)->DIEPCTL = (USB_OTG_DIEPCTL_EPDIS | USB_OTG_DIEPCTL_SNAK);
        }
        else
        {
            USBx_INEP(i)->DIEPCTL = 0U;
        }
        USBx_INEP(i)->DIEPTSIZ = 0U;
        USBx_INEP(i)->DIEPINT  = 0xFB7FU;
    }

    for (i = 0U; i < MAX_EP_NUM; i++)
    {
        if ((USBx_OUTEP(i)->DOEPCTL & USB_OTG_DOEPCTL_EPENA) == USB_OTG_DOEPCTL_EPENA)
        {
            if (i == 0U)
                USBx_OUTEP(i)->DOEPCTL = (USB_OTG_DOEPCTL_SNAK);
            else
                USBx_OUTEP(i)->DOEPCTL = (USB_OTG_DOEPCTL_EPDIS | USB_OTG_DOEPCTL_SNAK);
        }
        else
        {
            USBx_OUTEP(i)->DOEPCTL = 0U;
        }
        USBx_OUTEP(i)->DOEPTSIZ = 0U;
        USBx_OUTEP(i)->DOEPINT  = 0xFB7FU;
    }

    USBx_DEVICE->DIEPMSK &= ~(USB_OTG_DIEPMSK_TXFURM);
    // disable global int
    USBx->GAHBCFG &= ~USB_OTG_GAHBCFG_GINT;
    USBx->GINTMSK = 0;
    USBx->GINTSTS = 0xBFFFFFFF;

    //TODO: support LPM, BCD feature
    if (USBx->GAHBCFG & USB_OTG_GAHBCFG_DMAEN)
    {
        // If DMA enabled, setup the threshold value
        __IO uint32_t temp;
        USBx_DEVICE->DTHRCTL = (USB_OTG_DTHRCTL_TXTHRLEN_6 | USB_OTG_DTHRCTL_RXTHRLEN_6);
        USBx_DEVICE->DTHRCTL |= (USB_OTG_DTHRCTL_RXTHREN | USB_OTG_DTHRCTL_ISOTHREN | USB_OTG_DTHRCTL_NONISOTHREN);
        temp = USBx_DEVICE->DTHRCTL;
        (void)temp;
    }
    else
    {
        USBx->GINTMSK |= USB_OTG_GINTMSK_RXFLVLM;
    }

    /* Enable interrupts matching to the Device mode ONLY */
    USBx->GINTMSK |= (USB_OTG_GINTMSK_USBSUSPM | USB_OTG_GINTMSK_USBRST |
                      USB_OTG_GINTMSK_ENUMDNEM | USB_OTG_GINTMSK_IEPINT |
                      USB_OTG_GINTMSK_OEPINT   | USB_OTG_GINTMSK_IISOIXFRM |
                      USB_OTG_GINTMSK_PXFRM_IISOOXFRM | USB_OTG_GINTMSK_WUIM
#if defined(ENABLE_VBUS_DETECT)
                      | (USB_OTG_GINTMSK_SRQIM | USB_OTG_GINTMSK_OTGINT)
#endif
                     );

    USBx_DEVICE->DCTL |= USB_OTG_DCTL_SDIS;
    tusb_delay_ms(20);

    USBx_DEVICE->DCTL &= ~USB_OTG_DCTL_SDIS;
    tusb_delay_ms(3);
    USBx->GAHBCFG |= USB_OTG_GAHBCFG_GINT;
}


void tusb_otg_driver_vbus(USB_OTG_GlobalTypeDef *USBx, uint8_t state)
{
    __IO uint32_t hprt0 = 0U;
    hprt0 = USBx_HPRT0;
    hprt0 &= ~(USB_OTG_HPRT_PENA | USB_OTG_HPRT_PCDET |
               USB_OTG_HPRT_PENCHNG | USB_OTG_HPRT_POCCHNG);
    if (((hprt0 & USB_OTG_HPRT_PPWR) == 0U) && (state == 1U))
    {
        USBx_HPRT0 = (USB_OTG_HPRT_PPWR | hprt0);
    }
    if (((hprt0 & USB_OTG_HPRT_PPWR) == USB_OTG_HPRT_PPWR) && (state == 0U))
    {
        USBx_HPRT0 = ((~USB_OTG_HPRT_PPWR) & hprt0);
    }
}

static void tusb_init_otg_host(tusb_host_t *dev)
{
    USB_OTG_GlobalTypeDef *USBx = GetUSB(dev);
    //uint32_t USBx_BASE = (uint32_t)USBx;
    uint32_t MAX_CH_NUM = USBx == USB_OTG_FS ? USB_OTG_FS_MAX_CH_NUM : USB_OTG_HS_MAX_CH_NUM;

    uint32_t i;
    /* Restart the Phy Clock */
    USBx_PCGCCTL = 0U;
#if defined(USB_OTG_GCCFG_VBDEN)
    /* Disable VBUS sensing */
    USBx->GCCFG &= ~(USB_OTG_GCCFG_VBDEN);
#endif
    USBx_HOST->HCFG &= ~(USB_OTG_HCFG_FSLSS);

    /* Make sure the FIFOs are flushed. */
    flush_tx(USBx, 0x10U); /* all Tx FIFOs */
    flush_rx(USBx);

    /* Clear all pending HC Interrupts */
    for (i = 0U; i < MAX_CH_NUM; i++)
    {
        USBx_HC(i)->HCINT = 0xFFFFFFFFU;
        USBx_HC(i)->HCINTMSK = 0U;
    }

    /* Enable VBUS driving */
    tusb_otg_driver_vbus(USBx, 1U);

    tusb_delay_ms(200);

    /* Disable all interrupts. */
    USBx->GINTMSK = 0U;

    /* Clear any pending interrupts */
    USBx->GINTSTS = 0xFFFFFFFFU;

    // The FIFO will not set success if remove the delay function before
    // How to wait is not mentioned
    // FIFO did not allocate all memory, the remain memory maybe used for the request queue
    // How request queue work is not describe in the RM
    if (USBx == USB_OTG_FS)
    {
        /* set Rx/Tx FIFO size */
        USBx->GRXFSIZ  = 0x80U;
        do
        {
            USBx->HPTXFSIZ = (uint32_t)(((0x40U << 16)& USB_OTG_HPTXFSIZ_PTXFD) | 0xE0U);
        }
        while (USBx->HPTXFSIZ != (uint32_t)(((0x40U << 16)& USB_OTG_HPTXFSIZ_PTXFD) | 0xE0U));
        do
        {
            USBx->DIEPTXF0_HNPTXFSIZ = (uint32_t)(((0x60U << 16) & USB_OTG_NPTXFD) | 0x80U);
        }
        while (USBx->DIEPTXF0_HNPTXFSIZ != (uint32_t)(((0x60U << 16) & USB_OTG_NPTXFD) | 0x80U));
    }
    else
    {
        /* set Rx/Tx FIFO size */
        USBx->GRXFSIZ  = 0x200U;
        do
        {
            USBx->HPTXFSIZ = (uint32_t)(((0xE0U << 16) & USB_OTG_HPTXFSIZ_PTXFD) | 0x300U);
        }
        while (USBx->HPTXFSIZ != (uint32_t)(((0xE0U << 16) & USB_OTG_HPTXFSIZ_PTXFD) | 0x300U));
        do
        {
            USBx->DIEPTXF0_HNPTXFSIZ = (uint32_t)(((0x100U << 16) & USB_OTG_NPTXFD) | 0x200U);
        }
        while (USBx->DIEPTXF0_HNPTXFSIZ != (uint32_t)(((0x100U << 16) & USB_OTG_NPTXFD) | 0x200U));
    }

    if ((USBx->GAHBCFG & USB_OTG_GAHBCFG_DMAEN) == 0)
    {
        // If DMA not enabled
        // copy data in fifo non-empty interrupt
        USBx->GINTMSK |= USB_OTG_GINTMSK_RXFLVLM;
    }

    /* Enable interrupts matching to the Host mode ONLY */
    USBx->GINTMSK |= (USB_OTG_GINTMSK_PRTIM            | USB_OTG_GINTMSK_HCIM |
                      USB_OTG_GINTMSK_SOFM             | USB_OTG_GINTMSK_DISCINT |
                      USB_OTG_GINTMSK_PXFRM_IISOOXFRM  | USB_OTG_GINTMSK_WUIM);


    USBx->GAHBCFG |= USB_OTG_GAHBCFG_GINT;

    /* Enable VBUS driving */
    tusb_otg_driver_vbus(USBx, 1U);
}

void tusb_open_device(tusb_device_t *dev)
{
    USB_OTG_GlobalTypeDef *USBx = GetUSB(dev);
    tusb_otg_core_init((tusb_core_t *) dev);
    USBx->GUSBCFG &= ~(USB_OTG_GUSBCFG_FHMOD | USB_OTG_GUSBCFG_FDMOD);
    USBx->GUSBCFG |= USB_OTG_GUSBCFG_FDMOD;
    tusb_delay_ms(50);
    tusb_init_otg_device(dev);
}

void tusb_open_host(tusb_host_t *host)
{
    USB_OTG_GlobalTypeDef *USBx = GetUSB(host);
    tusb_otg_core_init((tusb_core_t *) host);
    // Force to device mode
    USBx->GUSBCFG &= ~(USB_OTG_GUSBCFG_FHMOD | USB_OTG_GUSBCFG_FDMOD);
    USBx->GUSBCFG |= USB_OTG_GUSBCFG_FHMOD;
    memset(&host->state, 0, (sizeof(tusb_host_t) - (uint32_t)(&((tusb_host_t *)0)->state)));
    tusb_init_otg_host(host);
}


void tusb_open_otg(tusb_otg_t *otg)
{
    USB_OTG_GlobalTypeDef *USBx = GetUSB(&otg->device);
    tusb_otg_core_init((tusb_core_t *) &otg->device);
    (void)USBx;

    /**
    The ID line changed event seems not working
    if(USBx == USB_OTG_FS){
      // Enable PA10, ID pin for OTG FS
      set_io_af_mode(GPIOA, 10, GPIO_AF10_OTG_FS);
    }else{
      // Enable PB12, ID pin for OTG FS
      set_io_af_mode(GPIOB, 12, GPIO_AF12_OTG_HS_FS);
    }

    USBx->GAHBCFG &= ~USB_OTG_GAHBCFG_GINT;
    USBx->GINTMSK = 0;
    USBx->GINTSTS = 0xBFFFFFFF;

    USBx->GINTMSK |= USB_OTG_GINTMSK_OTGINT;
    //USBx->GINTMSK |= USB_OTG_GINTMSK_CIDSCHGM;

    USBx->GAHBCFG |= USB_OTG_GAHBCFG_GINT;

    USBx->GUSBCFG &= ~(USB_OTG_GUSBCFG_FHMOD | USB_OTG_GUSBCFG_FDMOD);
    USBx->GUSBCFG |= USB_OTG_GUSBCFG_FDMOD;
    tusb_delay_ms(50);
    tusb_init_otg_device(&otg->device);
    */
}

void tusb_otg_device_handler(tusb_device_t *dev);
void tusb_otg_host_handler(tusb_host_t *dev);

#define TUSB_OTG_GetMode(USBx) ((USBx->GINTSTS ) & 0x1U)

#if defined(USB_OTG_FS)


#if defined(NO_DEVICE) || defined(NO_HOST)
    #if defined(NO_DEVICE)
        static tusb_host_t tusb_host_otg_fs;
    #endif
    #if defined(NO_HOST)
        static tusb_device_t tusb_dev_otg_fs;
    #endif

#else
    static tusb_otg_t tusb_otg_fs;

    #define  tusb_dev_otg_fs   (tusb_otg_fs.device)
    #define  tusb_host_otg_fs  (tusb_otg_fs.host)
#endif

void OTG_FS_IRQHandler(void)
{
#if defined(RTOS_INTERRUPT_ENTER)
    RTOS_INTERRUPT_ENTER();
#endif
    if (TUSB_OTG_GetMode(USB_OTG_FS) == USB_OTG_MODE_DEVICE)
    {
#if defined(NO_DEVICE)
        while (1);
#else
        tusb_otg_device_handler(&tusb_dev_otg_fs);
#endif
    }
    else
    {
#if defined(NO_HOST)
        while (1);
#else
        tusb_otg_host_handler(&tusb_host_otg_fs);
#endif
    }
#if defined(RTOS_INTERRUPT_LEAVE)
    RTOS_INTERRUPT_LEAVE();
#endif
}
#endif

#if defined(USB_OTG_HS)

#if defined(NO_DEVICE) || defined(NO_HOST)
    #if defined(NO_DEVICE)
        static tusb_host_t tusb_host_otg_hs;
    #endif
    #if defined(NO_HOST)
        static tusb_device_t tusb_dev_otg_hs;
    #endif

#else
    static tusb_otg_t tusb_otg_hs;

    #define  tusb_dev_otg_hs   (tusb_otg_hs.device)
    #define  tusb_host_otg_hs  (tusb_otg_hs.host)
#endif

void OTG_HS_IRQHandler(void)
{
#if defined(RTOS_INTERRUPT_ENTER)
    RTOS_INTERRUPT_ENTER();
#endif
    if (TUSB_OTG_GetMode(USB_OTG_HS) == USB_OTG_MODE_DEVICE)
    {
#if defined(NO_DEVICE)
        while (1);
#else
        tusb_otg_device_handler(&tusb_dev_otg_hs);
#endif
    }
    else
    {
#if defined(NO_HOST)
        while (1);
#else
        tusb_otg_host_handler(&tusb_host_otg_hs);
#endif
    }
#if defined(RTOS_INTERRUPT_LEAVE)
    RTOS_INTERRUPT_LEAVE();
#endif
}
#endif

#ifndef NO_DEVICE
tusb_device_t *tusb_get_device(uint8_t id)
{
    if (id != 0)
    {
#if defined(USB_OTG_HS)
#if defined(DESCRIPTOR_BUFFER_SIZE) && DESCRIPTOR_BUFFER_SIZE > 0
        static __ALIGN_BEGIN uint8_t otg_hs_desc_buf[DESCRIPTOR_BUFFER_SIZE] __ALIGN_END;
        tusb_dev_otg_hs.desc_buffer = otg_hs_desc_buf;
#endif
        SetUSB(&tusb_dev_otg_hs, USB_OTG_HS);
        return &tusb_dev_otg_hs;
#endif // USB_OTG_HS
    }
    {
#if defined(USB_OTG_FS)
#if defined(DESCRIPTOR_BUFFER_SIZE) && DESCRIPTOR_BUFFER_SIZE > 0
        static __ALIGN_BEGIN uint8_t otg_fs_desc_buf[DESCRIPTOR_BUFFER_SIZE] __ALIGN_END;
        tusb_dev_otg_fs.desc_buffer = otg_fs_desc_buf;
#endif
        SetUSB(&tusb_dev_otg_fs, USB_OTG_FS);
        return &tusb_dev_otg_fs;
#else // USB_OTG_FS
        return (tusb_device_t *)0;
#endif // USB_OTG_FS
    }
}
#endif

#ifndef NO_HOST
tusb_host_t *tusb_get_host(uint8_t id)
{
    if (id != 0)
    {
#if defined(USB_OTG_HS)
        SetUSB(&tusb_host_otg_hs, USB_OTG_HS);
        return &tusb_host_otg_hs;
#endif // USB_OTG_HS
    }
#if defined(USB_OTG_FS)
    SetUSB(&tusb_host_otg_fs, USB_OTG_FS);
    return &tusb_host_otg_fs;
#else
    return (tusb_host_t *)0;
#endif // USB_OTG_FS
}
#endif

#if defined(NO_DEVICE) || defined(NO_HOST)
#else
tusb_otg_t *tusb_get_otg(uint8_t id)
{
    if (id != 0)
    {
#if defined(USB_OTG_HS)
#if defined(DESCRIPTOR_BUFFER_SIZE) && DESCRIPTOR_BUFFER_SIZE > 0
        static __ALIGN_BEGIN uint8_t otg_hs_desc_buf[DESCRIPTOR_BUFFER_SIZE] __ALIGN_END;
        tusb_dev_otg_hs.desc_buffer = otg_hs_desc_buf;
#endif
        SetUSB(&tusb_dev_otg_hs, USB_OTG_HS);
        SetUSB(&tusb_host_otg_hs, USB_OTG_HS);

        return &tusb_otg_hs;
#endif // USB_OTG_HS
    }
#if defined(USB_OTG_FS)
    {
#if defined(DESCRIPTOR_BUFFER_SIZE) && DESCRIPTOR_BUFFER_SIZE > 0
        static __ALIGN_BEGIN uint8_t otg_fs_desc_buf[DESCRIPTOR_BUFFER_SIZE] __ALIGN_END;
        tusb_dev_otg_fs.desc_buffer = otg_fs_desc_buf;
#endif
        SetUSB(&tusb_dev_otg_fs, USB_OTG_FS);
        SetUSB(&tusb_host_otg_fs, USB_OTG_FS);
    }
    return &tusb_otg_fs;
#else
    return (tusb_host_t *)0;
#endif // USB_OTG_FS
}
#endif

#endif // #if defined(USB_OTG_FS) || defined(USB_OTG_HS)
