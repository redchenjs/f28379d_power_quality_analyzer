/*
 * task_eqep.c
 *
 *  Created on: 2017-4-16
 *      Author: redchenjs
 */
#include <driverlib.h>
#include <device.h>

#include "inc/device/eqep.h"

uint32_t eqep1_position = 0;
uint32_t eqep2_position = 0;

double eqep3_freq = 0;

void eqep1_position_caculate(void)
{
    if (EQEP_getPosition(EQEP1_BASE) > 384 && EQEP_getPosition(EQEP1_BASE) < 32768) {
        EQEP_setPosition(EQEP1_BASE, 384);
    }
    if (EQEP_getPosition(EQEP1_BASE) > 32768) {
        EQEP_setPosition(EQEP1_BASE, 0);
    }
    eqep1_position = EQEP_getPosition(EQEP1_BASE) / 2;
}

void eqep2_position_caculate(void)
{
    if (EQEP_getPosition(EQEP2_BASE) > 384 && EQEP_getPosition(EQEP2_BASE) < 32768) {
        EQEP_setPosition(EQEP2_BASE, 384);
    }
    if (EQEP_getPosition(EQEP2_BASE) > 32768) {
        EQEP_setPosition(EQEP2_BASE, 0);
    }
    eqep2_position = EQEP_getPosition(EQEP2_BASE) / 2;
}

void eqep3_frequency_caculate(void)
{
    extern uint32_t eqep3IntCount;

    if ((EQEP_getInterruptStatus(EQEP3_BASE) & EQEP_INT_UNIT_TIME_OUT) != 0) {
        eqep3IntCount++;
        //
        // Get latched POSCNT value
        //
        eqep3_freq = (double)EQEP_getPositionLatch(EQEP3_BASE) / 2.0;
        EQEP_clearInterruptStatus(EQEP3_BASE, EQEP_INT_UNIT_TIME_OUT);
    }
}
