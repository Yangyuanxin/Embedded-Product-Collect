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

//  STM32F1 usb hardware handler
#include "teeny_usb.h"
#if defined(USB)

//#define IMR_MSK (USB_CNTR_CTRM  | USB_CNTR_WKUPM | USB_CNTR_SUSPM | USB_CNTR_ERRM  | USB_CNTR_SOFM | USB_CNTR_ESOFM | USB_CNTR_RESETM)
#define IMR_MSK   ((USB_CNTR_CTRM  | USB_CNTR_WKUPM | USB_CNTR_SUSPM | USB_CNTR_RESETM))

#ifndef USB_CLR_DIR
    #define  USB_CLR_DIR      ((uint16_t)~USB_ISTR_DIR   )
#endif
#ifndef USB_CLR_ESOF
    #define  USB_CLR_ESOF     ((uint16_t)~USB_ISTR_ESOF  )
#endif
#ifndef USB_CLR_SOF
    #define  USB_CLR_SOF      ((uint16_t)~USB_ISTR_SOF   )
#endif
#ifndef USB_CLR_RESET
    #define  USB_CLR_RESET    ((uint16_t)~USB_ISTR_RESET )
#endif
#ifndef USB_CLR_SUSP
    #define  USB_CLR_SUSP     ((uint16_t)~USB_ISTR_SUSP  )
#endif
#ifndef USB_CLR_WKUP
    #define  USB_CLR_WKUP     ((uint16_t)~USB_ISTR_WKUP  )
#endif
#ifndef USB_CLR_ERR
    #define  USB_CLR_ERR      ((uint16_t)~USB_ISTR_ERR   )
#endif
#ifndef USB_CLR_PMAOVR
    #define  USB_CLR_PMAOVR   ((uint16_t)~USB_ISTR_PMAOVR)
#endif
#ifndef USB_CLR_CTR
    #define  USB_CLR_CTR      ((uint16_t)~USB_ISTR_CTR   )
#endif

#if defined(STM32F3)
#define AFL(val, pin)   ((val)<< (( (pin))*4))
#define AFH(val, pin)   ((val)<< (( (pin)-8)*4))
static void set_io_af_mode(GPIO_TypeDef *GPIO, uint8_t pin, uint8_t af)
{
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
}
#endif

static void tusb_disconnect(tusb_device_t *dev)
{
#if defined(STM32F0)
#ifdef USB_FS_INTERNAL_PULLUP
    RCC->APB1ENR |= RCC_APB1ENR_USBEN;
    // disable interal pull up resistor
    GetUSB(dev)->BCDR &= ~USB_BCDR_DPPU;
#else
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    // PA12 = PushPull = 0
    GPIOA->OTYPER |= GPIO_OTYPER_OT_12;
    GPIOA->MODER &= ~GPIO_MODER_MODER12;
    GPIOA->MODER |= GPIO_MODER_MODER12_0;
    GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEEDR12;
    GPIOA->BRR = GPIO_BRR_BR_12;
#endif
#endif

#if defined(STM32F1)
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    // PA_12 output mode: OD = 0
    GPIOA->CRH |= GPIO_CRH_CNF12_0;
    GPIOA->CRH &= (~GPIO_CRH_CNF12_1);
    GPIOA->CRH |= GPIO_CRH_MODE12;// PA_12 set as: Output mode, max speed 50 MHz.
    GPIOA->BRR = GPIO_BRR_BR12;
#endif

#if defined(STM32F3)
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    GPIOA->MODER &= ~GPIO_MODER_MODER12;
    GPIOA->MODER |= GPIO_MODER_MODER12_0;
    GPIOA->BRR = GPIO_BRR_BR_12;
#endif
}


void tusb_close_device(tusb_device_t *dev)
{
#if defined(STM32F0)
    NVIC_DisableIRQ(USB_IRQn);
#endif

#if defined(STM32F1)
#if defined(HAS_DOUBLE_BUFFER)
    NVIC_DisableIRQ(USB_HP_CAN1_TX_IRQn);
#endif
    NVIC_DisableIRQ(USB_LP_CAN1_RX0_IRQn);
#endif

#if defined(STM32F3)
#if defined(HAS_DOUBLE_BUFFER)
    NVIC_DisableIRQ(USB_HP_CAN_TX_IRQn);
#endif
    NVIC_DisableIRQ(USB_LP_CAN_RX0_IRQn);
#endif


    GetUSB()->ISTR = (0);
    // Turn USB Macrocell off (FRES + PWDN)
    GetUSB()->CNTR = (USB_CNTR_FRES | USB_CNTR_PDWN);

    tusb_disconnect(dev);

    // Disable USB Clock on APB1
    RCC->APB1ENR &=  ~RCC_APB1ENR_USBEN;
}

void tusb_open_device(tusb_device_t *dev)
{

    tusb_disconnect(dev);

    tusb_delay_ms(20);

#if defined(STM32F0)
#if defined USB_FS_CLOCK_SOURCE_CRS
    RCC->CFGR3 &= ~RCC_CFGR3_USBSW;
    RCC->APB1ENR |= RCC_APB1ENR_CRSEN;
    CRS->CFGR &= ~CRS_CFGR_SYNCSRC;
    CRS->CFGR |= RCC_CRS_SYNC_SOURCE_USB;
    CRS->CR |= (CRS_CR_AUTOTRIMEN | CRS_CR_CEN);
#else
    // otherwise use pll clk
    RCC->CFGR3 |= RCC_CFGR3_USBSW_PLLCLK;
#endif
#endif

    RCC->APB1ENR |= RCC_APB1ENR_USBEN;

    GetUSB(dev)->CNTR = (USB_CNTR_FRES);
    GetUSB(dev)->CNTR = (0);

    // wait reset finish
    while (!((GetUSB(dev)->ISTR) & USB_ISTR_RESET));

    GetUSB(dev)->ISTR = (0);
    GetUSB(dev)->BTABLE = (BTABLE_ADDRESS);
    GetUSB(dev)->CNTR = (IMR_MSK);

#if defined(STM32F0)
#ifdef USB_FS_INTERNAL_PULLUP
    // USE the interal pull up resistor
    GetUSB(dev)->BCDR |= USB_BCDR_DPPU;
#else
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    // PA12 = In float
    GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR12;
    GPIOA->MODER &= ~GPIO_MODER_MODER12;
#endif
    NVIC_EnableIRQ(USB_IRQn);
#endif

#if defined(STM32F1)
    // PA12 = Input
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    GPIOA->CRH |= GPIO_CRH_CNF12_0;
    GPIOA->CRH &= ~GPIO_CRH_CNF12_1;
    GPIOA->CRH &= ~GPIO_CRH_MODE12;

#if defined(HAS_DOUBLE_BUFFER)
    NVIC_EnableIRQ(USB_HP_CAN1_TX_IRQn);
#endif
    NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);
#endif

#if defined(STM32F3)
    // PA12 = Input
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    set_io_af_mode(GPIOA, 11, GPIO_AF14_USB);
    set_io_af_mode(GPIOA, 12, GPIO_AF14_USB);

#if defined(HAS_DOUBLE_BUFFER)
    NVIC_EnableIRQ(USB_HP_CAN_TX_IRQn);
#endif
    NVIC_EnableIRQ(USB_LP_CAN_RX0_IRQn);
#endif

}

#ifndef USB_CNTR_LP_MODE
    #define USB_CNTR_LP_MODE USB_CNTR_LPMODE
#endif
static void tsub_suspend(tusb_device_t *dev)
{
    uint16_t wCNTR;

    wCNTR = GetUSB(dev)->CNTR;
    wCNTR |= USB_CNTR_FSUSP;
    GetUSB(dev)->CNTR = (wCNTR);

    wCNTR = GetUSB(dev)->CNTR;
    wCNTR |= USB_CNTR_LP_MODE;
    GetUSB(dev)->CNTR = (wCNTR);
}

void tsub_resume(tusb_device_t *dev)
{
    uint16_t wCNTR;
    wCNTR = GetUSB(dev)->CNTR;
    wCNTR &= (~USB_CNTR_LP_MODE);
    GetUSB(dev)->CNTR = (wCNTR);
    GetUSB(dev)->CNTR = (IMR_MSK);
}

static tusb_device_t tusb_dev;
// end point data handler, also called in USB_HP IRQ
void tusb_ep_handler(tusb_device_t *dev, uint8_t EPn);

#if defined(STM32F3)
    void USB_HP_CAN_TX_IRQHandler(void)
#else
    void USB_HP_CAN1_TX_IRQHandler(void)
#endif
{
    uint16_t wIstr;
    tusb_device_t *dev = &tusb_dev;
    while ((wIstr = GetUSB(dev)->ISTR) & USB_ISTR_CTR)
    {
        GetUSB(dev)->ISTR = (uint16_t)(USB_CLR_CTR);
        tusb_ep_handler(dev, wIstr & USB_ISTR_EP_ID);
    }
}

#if defined(STM32F0)
    void USB_IRQHandler(void)
#elif defined(STM32F3)
    void USB_LP_CAN_RX0_IRQHandler(void)
#else
    void USB_LP_CAN1_RX0_IRQHandler(void)
#endif
{
    tusb_device_t *dev = &tusb_dev;
    uint16_t wIstr;
    while ((wIstr = GetUSB(dev)->ISTR) & USB_ISTR_CTR)
    {
        GetUSB(dev)->ISTR = (uint16_t)(USB_CLR_CTR);
        tusb_ep_handler(dev, wIstr & USB_ISTR_EP_ID);
    }

    if (wIstr & USB_ISTR_RESET)
    {
        GetUSB(dev)->ISTR = (USB_CLR_RESET);
        GetUSB(dev)->BTABLE = (BTABLE_ADDRESS);
        tusb_reconfig(dev);
        GetUSB(dev)->DADDR = (0 | USB_DADDR_EF);
    }
    if (IMR_MSK & USB_ISTR_PMAOVR)
    {
        if (wIstr & USB_ISTR_PMAOVR)
        {
            GetUSB(dev)->ISTR = (USB_CLR_PMAOVR);
        }
    }
    if (IMR_MSK & USB_ISTR_SUSP)
    {
        if (wIstr & USB_ISTR_SUSP)
        {
            GetUSB(dev)->ISTR = (USB_CLR_SUSP);
            if (GetUSB(dev)->DADDR & 0x007f)
            {
                GetUSB(dev)->DADDR = (0);
                GetUSB(dev)->CNTR &= ~USB_CNTR_SUSPM;
            }
            tsub_suspend(dev);
        }
    }
    if (IMR_MSK & USB_ISTR_ERR)
    {
        if (wIstr & USB_ISTR_ERR)
        {
            GetUSB(dev)->ISTR = (USB_CLR_ERR);
        }
    }

    if (IMR_MSK & USB_ISTR_WKUP)
    {
        if (wIstr & USB_ISTR_WKUP)
        {
            GetUSB(dev)->ISTR = (USB_CLR_WKUP);
            tsub_resume(dev);
        }
    }

    if (IMR_MSK & USB_ISTR_SOF)
    {
        if (wIstr & USB_ISTR_SOF)
        {
            GetUSB(dev)->ISTR = (USB_CLR_SOF);
        }
    }
    if (IMR_MSK & USB_ISTR_ESOF)
    {
        if (wIstr & USB_ISTR_ESOF)
        {
            GetUSB(dev)->ISTR = (USB_CLR_ESOF);
        }
    }

    GetUSB(dev)->ISTR = (0);
}

tusb_device_t *tusb_get_device(uint8_t id)
{
#if defined(DESCRIPTOR_BUFFER_SIZE) && DESCRIPTOR_BUFFER_SIZE > 0
    static __ALIGN_BEGIN uint8_t desc_buf[DESCRIPTOR_BUFFER_SIZE] __ALIGN_END;
    tusb_dev.desc_buffer = desc_buf;
#endif
    return &tusb_dev;
}

#endif  // USB

