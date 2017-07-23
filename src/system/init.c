/*
 * init.c
 *
 *  Created on: 2017-4-16
 *      Author: redchenjs
 */
#include "driverlib.h"
#include "device.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <file.h>

#include "F2837xD_sci_io.h"

#include "inc/device/led.h"
#include "inc/device/spi.h"
#include "inc/device/adc.h"
#include "inc/device/key.h"
#include "inc/device/eqep.h"
#include "inc/device/ecap.h"
#include "inc/device/cpu_timer.h"

#include "inc/driver/ssd1351.h"

volatile int scia_status = 0;
volatile FILE *scia_fid;

void device_init(void)
{
    led_init();
    spi_init();         // SPI总线配置
    key_init();         // 按键配置
    adc1_init();        // ADC配置
    eqep1_init();       // eQEP配置
//    eqep3_init();       // eQEP配置
//    ecap1_init();       // eQEP配置
//    ecap2_init();       // eQEP配置
    cpu_timer_init();
}

void driver_init(void)
{
    ssd1351_init();     // SSD1351显示初始化
}

void system_init(void)
{
    Device_init();      // 器件初始化配置
    Device_initGPIO();  // 器件GPIO初始化配置

    Interrupt_initModule();         // 初始化外设模块时钟
    Interrupt_initVectorTable();    // 初始化中断向量表

    device_init();  // 器件外设功能模块初始化
    driver_init();  // 外部设备驱动初始化

    //
    // Redirect STDOUT to SCI
    //
    scia_status = add_device("scia", _SSA, SCI_open, SCI_close, SCI_read, SCI_write,
                        SCI_lseek, SCI_unlink, SCI_rename);
    scia_fid = fopen("scia","w");
    freopen("scia:", "w", stdout);
    setvbuf(stdout, NULL, _IONBF, 0);

    EINT;   // Enable interrupts
    ERTM;   // Enable debug events
}

void system_suspend(void)
{
    DEVICE_DELAY_US(50000);
}
