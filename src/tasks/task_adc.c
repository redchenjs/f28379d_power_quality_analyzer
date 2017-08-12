/*
 * task_adc.c
 *
 *  Created on: 2017-6-27
 *      Author: redchenjs
 */
#include <driverlib.h>
#include <device.h>

#include <fpu_vector.h>

#include <vcu2_types.h>
#include <vcu2_fft.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "inc/device/adc.h"
#include "inc/tasks/task_disp.h"

#define ADC2_CURRENT_X200_RATIO   1.0
#define ADC2_CURRENT_X2000_RATIO  1.0
#define ADC2_CURRENT_X20000_RATIO 1.0

double adc1_voltage_peak  = 0.0;
double adc1_voltage_ave   = 0.0;
double adc1_voltage_rms   = 0.0;
double adc1_voltage_rms_ratio   = 320.102173482;

double adc1_spectrum_abs[512]   = {0.0};
double adc1_spectrum_phase[512] = {0.0};

double adc1_harmonic_abs[10] = {0.0};
double adc1_harmonic_amp[10] = {0.0};
double adc1_harmonic_rms[10] = {0.0};
double adc1_harmonic_percent[10] = {0.0};

double adc1_phase = 0.0;

double adc2_current_peak  = 0.0;
double adc2_current_ave   = 0.0;
double adc2_current_rms   = 0.0;
double adc2_current_rms_ratio   = ADC2_CURRENT_X200_RATIO;

double adc2_spectrum_abs[512]   = {0.0};
double adc2_spectrum_phase[512] = {0.0};

double adc2_harmonic_abs[10] = {0.0};
double adc2_harmonic_amp[10] = {0.0};
double adc2_harmonic_rms[10] = {0.0};
double adc2_harmonic_percent[10] = {0.0};

double adc2_phase = 0.0;

double adc1_2_apparent_power = 0.0;
double adc1_2_active_power   = 0.0;
double adc1_2_reactive_power = 0.0;
double adc1_2_power_factor   = 0.0;

uint16_t adc1_first_harmonic_index = 0;
uint16_t adc2_first_harmonic_index = 0;

#pragma DATA_SECTION(adc1_buffer_sample,"fft1_in_buffer")
int16_t adc1_buffer_sample[2048];

#pragma DATA_SECTION(adc2_buffer_sample,"fft2_in_buffer")
int16_t adc2_buffer_sample[2048];

#pragma DATA_SECTION(adc1_buffer_fft,"fft1_out_buffer")
int16_t adc1_buffer_fft[2048];

#pragma DATA_SECTION(adc2_buffer_fft,"fft2_out_buffer")
int16_t adc2_buffer_fft[2048];

void adc1_fft(void)
{
    static CFFT_Obj CFFT;
    static CFFT_Handle handleCFFT;

    // Initialize CFFT object
    CFFT.pInBuffer  = adc1_buffer_sample;
    CFFT.pOutBuffer = adc1_buffer_fft;
    CFFT.init = (void (*)(void *))CFFT_init1024Pt;
    CFFT.run  = (void (*)(void *))CFFT_run1024Pt;

    // Initialize the handle
    handleCFFT = &CFFT;

    // Calling the init() will setup the twiddle factor table
    CFFT.init(handleCFFT);
    // Run the Forward FFT followed by an unpacking routine
    CFFT.run(handleCFFT);
//        CFFT_unpack(handleCFFT);
}

void adc1_voltage_caculate(void)
{
    extern int16_t adc1_buffer_sample[2048];

    double rms_sum = 0;

    uint16_t i;

    for (i=0; i<2048; i+=2) {
        adc1_voltage_rms = 3.0 * (adc1_buffer_sample[i] / 65536.0);
        rms_sum += pow(adc1_voltage_rms, 2);
    }

    adc1_voltage_rms = rms_sum / 1024.0;
    adc1_voltage_rms = sqrt(adc1_voltage_rms)  * adc1_voltage_rms_ratio;
}

void adc1_spectrum_caculate(void)
{
    extern int16_t adc1_buffer_fft[2048];

    uint16_t i;

    for (i=1; i<512; i++) {
        adc1_spectrum_abs[i] = sqrt(pow(adc1_buffer_fft[i*2], 2) + pow(adc1_buffer_fft[i*2+1], 2)) / 1024.0 * 2.0;
        adc1_spectrum_phase[i] = atan2(adc1_buffer_fft[i*2+1], adc1_buffer_fft[i*2]);
    }

    adc1_spectrum_abs[0] = (double)adc1_buffer_fft[0] / 1024.0;
    adc1_spectrum_phase[0] = 0.0;
}

void adc1_harmonic_caculate(void)
{
    uint16_t i;

    for (i=0; i<512; i++) {
        if (adc1_spectrum_abs[adc1_first_harmonic_index] < adc1_spectrum_abs[i]) {
            adc1_first_harmonic_index = i;
        }
    }

    if (adc1_first_harmonic_index == 0) {
        for (i=0; i<10; i++) {
            adc1_harmonic_abs[i] = 0.0;
            adc1_harmonic_amp[i] = 0.0;
            adc1_harmonic_rms[i] = 0.0;
            adc1_harmonic_percent[i] = 0.0;
        }
        adc1_harmonic_abs[0] = adc1_spectrum_abs[0];
        adc1_harmonic_amp[0] = 3.0 * (adc1_harmonic_abs[0] / 65536.0);;
        adc1_harmonic_rms[0] = adc1_harmonic_amp[0];
    } else {
        for (i=1; i<10; i++) {
            adc1_harmonic_abs[i] = adc1_spectrum_abs[adc1_first_harmonic_index*i];
            adc1_harmonic_amp[i] = 3.0 * (adc1_harmonic_abs[i] / 65536.0);
            adc1_harmonic_rms[i] = adc1_harmonic_amp[i] * (sqrt(2.0) / 2.0);
            if (adc1_harmonic_amp[1] != 0.0) {
                adc1_harmonic_percent[i] = adc1_harmonic_amp[i] / adc1_harmonic_amp[1] * 100.0;
            } else {
                adc1_harmonic_percent[i] = 0.0;
            }
        }
        adc1_harmonic_abs[0] = adc1_spectrum_abs[0];
        adc1_harmonic_amp[0] = 3.0 * (adc1_harmonic_abs[0] / 65536.0);
        adc1_harmonic_rms[0] = adc1_harmonic_amp[0];
        if (adc1_harmonic_amp[1] != 0.0) {
            adc1_harmonic_percent[0] = adc1_harmonic_amp[0] / adc1_harmonic_amp[1] * 100.0;
        } else {
            adc1_harmonic_percent[0] = 0.0;
        }
    }
}

void adc2_fft(void)
{
    static CFFT_Obj CFFT;
    static CFFT_Handle handleCFFT;

    // Initialize CFFT object
    CFFT.pInBuffer  = adc2_buffer_sample;
    CFFT.pOutBuffer = adc2_buffer_fft;
    CFFT.init = (void (*)(void *))CFFT_init1024Pt;
    CFFT.run  = (void (*)(void *))CFFT_run1024Pt;

    // Initialize the handle
    handleCFFT = &CFFT;

    // Calling the init() will setup the twiddle factor table
    CFFT.init(handleCFFT);
    // Run the Forward FFT followed by an unpacking routine
    CFFT.run(handleCFFT);
//        CFFT_unpack(handleCFFT);
}

void adc2_current_caculate(void)
{
    extern int16_t adc2_buffer_sample[2048];
//
//    uint16_t cp_max = 0, cp_min = 0;
//    uint16_t i;
//
//    for (i=0; i<512; i++) {
//        if (adc2_spectrum_abs[adc2_first_harmonic_index] < adc2_spectrum_abs[i]) {
//            adc2_first_harmonic_index = i;
//        }
//    }
//
//    for (i=0; i<(uint16_t)(1024/adc2_first_harmonic_index); i++) {
//        if (adc2_buffer_sample[cp_max] < adc2_buffer_sample[i]) {
//            cp_max = i;
//        }
//        if (adc2_buffer_sample[cp_min] > adc2_buffer_sample[i]) {
//            cp_min = i;
//        }
//        adc2_current_ave += adc2_buffer_sample[i];
//        adc2_current_rms += pow(adc2_buffer_sample[i], 2);
//    }
//
//    adc2_current_peak = adc2_buffer_sample[cp_max] - adc2_buffer_sample[cp_min];
//    adc2_current_peak = 3.0 * (2.0 * adc2_current_peak / 65536.0);
//
//    adc2_current_ave = adc2_current_ave / (uint16_t)(1024/adc2_first_harmonic_index);
//    adc2_current_ave = 3.0 * (2.0 * adc2_current_ave / 65536.0);
//
//    adc2_current_rms = adc2_current_rms / (uint16_t)(1024/adc2_first_harmonic_index);
//    adc2_current_rms = sqrt(adc2_current_rms);
//    adc2_current_rms = 3.0 * (2.0 * adc2_current_rms / 65536.0);

    double rms_sum = 0;

    uint16_t i;

    for (i=0; i<2048; i+=2) {
        adc2_current_rms = 3.0 * (adc2_buffer_sample[i] / 65536.0);
        rms_sum += pow(adc2_current_rms, 2);
    }

    adc2_current_rms = rms_sum / 1024.0;
    adc2_current_rms = sqrt(adc2_current_rms)  * adc2_current_rms_ratio;

}

void adc2_spectrum_caculate(void)
{
    extern int16_t adc2_buffer_fft[2048];

    uint16_t i;

    for (i=1; i<512; i++) {
        adc2_spectrum_abs[i] = sqrt(pow(adc2_buffer_fft[i*2], 2) + pow(adc2_buffer_fft[i*2+1], 2)) / 1024.0 * 2.0;
        adc2_spectrum_phase[i] = atan2(adc2_buffer_fft[i*2+1], adc2_buffer_fft[i*2]);
    }

    adc2_spectrum_abs[0] = (double)adc2_buffer_fft[0] / 1024.0;
    adc2_spectrum_phase[0] = 0.0;
}

void adc2_harmonic_caculate(void)
{
    uint16_t i;

    for (i=0; i<512; i++) {
        if (adc2_spectrum_abs[adc2_first_harmonic_index] < adc2_spectrum_abs[i]) {
            adc2_first_harmonic_index = i;
        }
    }

    if (adc2_first_harmonic_index == 0) {
        for (i=0; i<10; i++) {
            adc2_harmonic_abs[i] = 0.0;
            adc2_harmonic_amp[i] = 0.0;
            adc2_harmonic_rms[i] = 0.0;
            adc2_harmonic_percent[i] = 0.0;
        }
        adc2_harmonic_abs[0] = adc2_spectrum_abs[0];
        adc2_harmonic_amp[0] = 3.0 * (adc2_harmonic_abs[0] / 65536.0);
        adc2_harmonic_rms[0] = adc2_harmonic_amp[0];
    } else {
        for (i=1; i<10; i++) {
            adc2_harmonic_abs[i] = adc2_spectrum_abs[adc2_first_harmonic_index*i];
            adc2_harmonic_amp[i] = 3.0 * (adc2_harmonic_abs[i] / 65536.0);
            adc2_harmonic_rms[i] = adc2_harmonic_amp[i] * (sqrt(2.0) / 2.0);
            if (adc2_harmonic_amp[1] != 0.0) {
                adc2_harmonic_percent[i] = adc2_harmonic_amp[i] / adc2_harmonic_amp[1] * 100.0;
            } else {
                adc2_harmonic_percent[i] = 0.0;
            }
        }
        adc2_harmonic_abs[0] = adc2_spectrum_abs[0];
        adc2_harmonic_amp[0] = 3.0 * (adc2_harmonic_abs[0] / 65536.0);;
        adc2_harmonic_rms[0] = adc2_harmonic_amp[0];
        if (adc2_harmonic_amp[1] != 0.0) {
            adc2_harmonic_percent[0] = adc2_harmonic_amp[0] / adc2_harmonic_amp[1] * 100.0;
        } else {
            adc2_harmonic_percent[0] = 0.0;
        }
    }
}

void adc1_2_phase_caculate(void)
{
    uint16_t i;

    for (i=0; i<512; i++) {
        if (adc1_spectrum_abs[adc1_first_harmonic_index] < adc1_spectrum_abs[i]) {
            adc1_first_harmonic_index = i;
        }
    }

    if (adc1_first_harmonic_index == 0) {

    } else {

    }
}

void adc1_2_power_caculate(void)
{
    extern double ecap2_phase_radian;

    adc1_2_apparent_power = adc1_voltage_rms * adc2_current_rms;
    adc1_2_active_power   = adc1_2_apparent_power * cos(ecap2_phase_radian);
    adc1_2_reactive_power = adc1_2_apparent_power * sin(ecap2_phase_radian);
    adc1_2_power_factor   = cos(ecap2_phase_radian);
}
