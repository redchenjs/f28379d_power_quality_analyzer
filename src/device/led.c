#include "driverlib.h"
#include "device.h"
/*
 * led.h
 *
 *  Created on: 2017-4-16
 *      Author: redchenjs
 */

void led_init(void)
{
    GPIO_setPadConfig(DEVICE_GPIO_PIN_LED1, GPIO_PIN_TYPE_STD);
    GPIO_setPadConfig(DEVICE_GPIO_PIN_LED2, GPIO_PIN_TYPE_STD);

    GPIO_setDirectionMode(DEVICE_GPIO_PIN_LED1, GPIO_DIR_MODE_OUT);
    GPIO_setDirectionMode(DEVICE_GPIO_PIN_LED2, GPIO_DIR_MODE_OUT);

    GPIO_writePin(DEVICE_GPIO_PIN_LED1, 1);
    GPIO_writePin(DEVICE_GPIO_PIN_LED2, 1);
}
