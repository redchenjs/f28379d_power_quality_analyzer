/*
 * task_ecap.c
 *
 *  Created on: 2017-4-16
 *      Author: redchenjs
 */
#include <driverlib.h>
#include <device.h>

#include "math.h"

#include "inc/device/ecap.h"

double ecap1_freq  = 0.0;
double ecap2_freq  = 0.0;
double ecap1_duty  = 0.0;
double ecap2_duty  = 0.0;
double ecap2_phase_degree = 0.0;
double ecap2_phase_radian = 0.0;

void ecap1_frequency_caculate(void)
{
    extern volatile uint32_t ecap1Cap2Count;
    extern volatile uint32_t ecap1Cap3Count;

    ecap1_freq = (double)200000000.0 / (double)(ecap1Cap2Count + ecap1Cap3Count);
    ecap1_duty = (double)ecap1Cap3Count / (double)(ecap1Cap2Count + ecap1Cap3Count) * 100.0;
}

void ecap2_phase_caculate(void)
{
    extern volatile uint32_t ecap2Cap2Count;
    extern volatile uint32_t ecap2Cap3Count;

    const double pi = 4.0 * atan(1.0);

    if ((ecap2Cap2Count + ecap2Cap3Count) > 0) {
        if (GPIO_readPin(5) == 0) {
            ecap2_phase_degree = (double)ecap2Cap3Count / (double)(ecap2Cap2Count + ecap2Cap3Count) * 180.0;
            ecap2_phase_radian = (double)ecap2Cap3Count / (double)(ecap2Cap2Count + ecap2Cap3Count) * pi;
        } else {
            ecap2_phase_degree = (double)ecap2Cap3Count / (double)(ecap2Cap2Count + ecap2Cap3Count) * 180.0 * (-1.0);
            ecap2_phase_radian = (double)ecap2Cap3Count / (double)(ecap2Cap2Count + ecap2Cap3Count) * pi * (-1.0);
        }
    } else {
        ecap2_phase_degree = 0.0;
        ecap2_phase_radian = 0.0;
    }
}
