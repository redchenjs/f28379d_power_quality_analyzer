/*
 * key.c
 *
 *  Created on: 2017-7-6
 *      Author: redchenjs
 */
#include "driverlib.h"
#include "device.h"
//
// Function Prototypes
//
extern interrupt void xint1_isr(void);
extern interrupt void xint2_isr(void);

void key_init(void)
{
    Interrupt_register(INT_XINT1, &xint1_isr);
    Interrupt_register(INT_XINT2, &xint2_isr);

    Interrupt_enable(INT_XINT1);
    Interrupt_enable(INT_XINT2);

    GPIO_setDirectionMode(99, GPIO_DIR_MODE_IN);
    GPIO_setDirectionMode(57, GPIO_DIR_MODE_IN);
    GPIO_setQualificationMode(99, GPIO_QUAL_6SAMPLE);
    GPIO_setQualificationMode(57, GPIO_QUAL_6SAMPLE);
    GPIO_setQualificationPeriod(99, 510);
    GPIO_setQualificationPeriod(57, 510);

    GPIO_setInterruptPin(99, GPIO_INT_XINT1);
    GPIO_setInterruptPin(57, GPIO_INT_XINT2);

    GPIO_setInterruptType(GPIO_INT_XINT1, GPIO_INT_TYPE_FALLING_EDGE);
    GPIO_setInterruptType(GPIO_INT_XINT2, GPIO_INT_TYPE_FALLING_EDGE);

    GPIO_enableInterrupt(GPIO_INT_XINT1);
    GPIO_enableInterrupt(GPIO_INT_XINT2);
}
