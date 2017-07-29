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
double ecap1_duty  = 0.0;

// adjust
double ecap1_50_mHz_ratio = 0.999957721364754;
double ecap1_50_mHz_start = 0.050002113;

double ecap1_100_mHz_ratio = 0.999957469723837;
double ecap1_100_mHz_start = 0.100004248;

double ecap1_500_mHz_ratio = 0.999957401772086;
double ecap1_500_mHz_start = 0.500021279;

double ecap1_1_Hz_ratio = 0.999957267817571;
double ecap1_1_Hz_start = 1.00004256;

double ecap1_5_Hz_ratio = 0.999957371812977;
double ecap1_5_Hz_start = 5.00021315;

double ecap1_10_Hz_ratio = 0.999956799865468;
double ecap1_10_Hz_start = 10.0004263;

double ecap1_50_Hz_ratio = 0.999956501891733;
double ecap1_50_Hz_start = 50.0021477;

double ecap1_100_Hz_ratio = 0.999952514254908;
double ecap1_100_Hz_start = 100.004402;

double ecap1_500_Hz_ratio = 0.999950011498925;
double ecap1_500_Hz_start = 500.023743;

double ecap1_1_kHz_ratio = 0.999899810018998;
double ecap1_1_kHz_start = 1000.04999;

double ecap1_5_kHz_ratio = 0.999849922511623;
double ecap1_5_kHz_start = 5000.5;

double ecap1_10_kHz_start = 10001.5;

double ecap2_phase_degree = 0.0;
double ecap2_phase_radian = 0.0;

void ecap1_frequency_caculate(void)
{
    extern volatile uint32_t ecap1Cap2Count;
    extern volatile uint32_t ecap1Cap3Count;

    ecap1_freq = (double)200000000.0 / (double)(ecap1Cap2Count + ecap1Cap3Count);
    ecap1_duty = (double)ecap1Cap3Count / (double)(ecap1Cap2Count + ecap1Cap3Count) * 100.0;

    if (ecap1_freq < ecap1_50_mHz_start) {
        // 50.000 mHz ~ 99.999 mHz
        ecap1_freq = 0.0;
    } else if (ecap1_freq >= ecap1_50_mHz_start && ecap1_freq < ecap1_100_mHz_start) {
        // 50.000 mHz ~ 99.999 mHz
        ecap1_freq *= ecap1_50_mHz_ratio;
    } else if (ecap1_freq >= ecap1_100_mHz_start && ecap1_freq < ecap1_500_mHz_start) {
        // 100.000 mHz ~ 499.999 mHz
        ecap1_freq *= ecap1_100_mHz_ratio;
    } else if (ecap1_freq >= ecap1_500_mHz_start && ecap1_freq < ecap1_1_Hz_start) {
        // 500.000 mHz ~ 999.999 mHz
        ecap1_freq *= ecap1_500_mHz_ratio;
    } else if (ecap1_freq >= ecap1_1_Hz_start && ecap1_freq < ecap1_5_Hz_start) {
        // 1.000000 Hz ~ 4.999999 Hz
        ecap1_freq *= ecap1_1_Hz_ratio;
    } else if (ecap1_freq >= ecap1_5_Hz_start && ecap1_freq < ecap1_10_Hz_start) {
        // 5.000000 Hz ~ 9.999999 Hz
        ecap1_freq *= ecap1_5_Hz_ratio;
    } else if (ecap1_freq >= ecap1_10_Hz_start && ecap1_freq < ecap1_50_Hz_start) {
        // 10.0000 Hz ~ 49.9999 Hz
        ecap1_freq *= ecap1_10_Hz_ratio;
    } else if (ecap1_freq >= ecap1_50_Hz_start && ecap1_freq < ecap1_100_Hz_start) {
        // 50.0000 Hz ~ 99.9999 Hz
        ecap1_freq *= ecap1_50_Hz_ratio;
    } else if (ecap1_freq >= ecap1_100_Hz_start && ecap1_freq < ecap1_500_Hz_start) {
        // 100.00 Hz ~ 499.99 Hz
        ecap1_freq *= ecap1_100_Hz_ratio;
    } else if (ecap1_freq >= ecap1_500_Hz_start && ecap1_freq < ecap1_1_kHz_start) {
        // 500.00 Hz ~ 999.99 Hz
        ecap1_freq *= ecap1_500_Hz_ratio;
    } else if (ecap1_freq >= ecap1_1_kHz_start && ecap1_freq < ecap1_5_kHz_start) {
        // 1.000 kHz ~ 4.999 kHz
        ecap1_freq *= ecap1_1_kHz_ratio;
    } else if (ecap1_freq >= ecap1_5_kHz_start && ecap1_freq < ecap1_10_kHz_start) {
        // 5.000 kHz ~ 9.999 kHz
        ecap1_freq *= ecap1_5_kHz_ratio;
    } else if (ecap1_freq >= ecap1_10_kHz_start) {
        // above 10 kHz
        ecap1_freq = 10000.0;
    }
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
