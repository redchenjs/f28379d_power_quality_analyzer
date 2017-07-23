/*
 * task_ecap.c
 *
 *  Created on: 2017-4-16
 *      Author: redchenjs
 */
#include <driverlib.h>
#include <device.h>

#include "inc/device/ecap.h"

double ecap1_freq  = 0;
double ecap1_duty  = 0;
double ecap2_phase = 0;

void ecap1_freq_caculate(void)
{
    extern volatile uint32_t ecap1Cap2Count;
    extern volatile uint32_t ecap1Cap3Count;

    ecap1_freq = (double)(200000000.0 / ecap1Cap3Count);
    ecap1_duty = (double)(ecap1Cap3Count - ecap1Cap2Count) / ecap1Cap3Count * 100.0;
}

void ecap2_phase_caculate(void)
{
    extern volatile uint32_t ecap1Cap1Count;
    extern volatile uint32_t ecap1Cap2Count;
    extern volatile uint32_t ecap1Cap3Count;
    extern volatile uint32_t ecap2Cap1Count;

    if (ecap1Cap1Count >= ecap2Cap1Count) {
        ecap2_phase = (double)(ecap1Cap1Count - ecap2Cap1Count) / ecap1Cap3Count;
    } else {
        ecap2_phase = -1 * (double)(ecap2Cap1Count - ecap1Cap1Count) / ecap1Cap3Count;
    }
}

void task_ecap(void)
{
    ecap1_start();
    ecap2_start();
    ecap1_freq_caculate();
    ecap2_phase_caculate();
}

