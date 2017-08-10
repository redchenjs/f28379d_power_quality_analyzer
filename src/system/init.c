/*
 * init.c
 *
 *  Created on: 2017-4-16
 *      Author: redchenjs
 */
#include "driverlib.h"
#include "device.h"

#include "inc/device/sci.h"
#include "inc/device/led.h"
#include "inc/device/spi.h"
#include "inc/device/adc.h"
#include "inc/device/key.h"
#include "inc/device/epwm.h"
#include "inc/device/eqep.h"
#include "inc/device/ecap.h"
#include "inc/device/cpu_timer.h"

#include "inc/driver/ssd1351.h"

void device_init(void)
{
    Device_init();      // 器件初始化配置
    Device_initGPIO();  // 器件GPIO初始化配置

    Interrupt_initModule();         // 初始化外设模块时钟
    Interrupt_initVectorTable();    // 初始化中断向量表

    EINT;   // Enable interrupts
    ERTM;   // Enable debug events

    sci1_init();
    sci2_init();
    spi1_init();         // SPI总线配置
    led_init();
    key_init();         // 按键配置
    epwm1_init();
    adc1_init();        // ADC配置
    adc2_init();        // ADC配置
    eqep1_init();       // eQEP配置
    eqep3_init();       // eQEP配置
    ecap1_init();       // eQEP配置
    ecap2_init();       // eQEP配置
    cpu_timer_init();
}

void driver_init(void)
{
    ssd1351_init();     // SSD1351显示初始化
}

void device_suspend(void)
{
    DEVICE_DELAY_US(1000);
}
