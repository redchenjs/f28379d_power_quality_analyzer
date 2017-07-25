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

double adc1_voltage_vpp  = 0.0;
double adc1_voltage_ave  = 0.0;
double adc1_voltage_rms  = 0.0;

double adc1_spectrum_abs[512]   = {0.0};
double adc1_spectrum_phase[512] = {0.0};

double adc1_harmonic_abs[10] = {0.0};
double adc1_harmonic_amp[10] = {0.0};
double adc1_harmonic_rms[10] = {0.0};
double adc1_harmonic_percent[10] = {0.0};

uint16_t adc1_first_harmonic_index = 0;

#pragma DATA_SECTION(adc1_buffer_sample,"buffer1")
int16_t adc1_buffer_sample[2048];

#pragma DATA_SECTION(adc1_buffer_fft,"buffer2")
int16_t adc1_buffer_fft[2048];

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

void adc1_voltage(void)
{
    extern int16_t adc1_buffer_sample[2048];

    uint16_t vp_max = 0, vp_min = 0;
    uint16_t i;

    for (i=0; i<512; i++) {
        if (adc1_spectrum_abs[adc1_first_harmonic_index] < adc1_spectrum_abs[i]) {
            adc1_first_harmonic_index = i;
        }
    }

    for (i=0; i<(uint16_t)(1024/adc1_first_harmonic_index); i++) {
        if (adc1_buffer_sample[vp_max] < adc1_buffer_sample[i]) {
            vp_max = i;
        }
        if (adc1_buffer_sample[vp_min] > adc1_buffer_sample[i]) {
            vp_min = i;
        }
        adc1_voltage_ave += adc1_buffer_sample[i];
        adc1_voltage_rms += pow(adc1_buffer_sample[i], 2);
    }

    adc1_voltage_vpp = adc1_buffer_sample[vp_max] - adc1_buffer_sample[vp_min];
    adc1_voltage_vpp = 3.0 * (2.0 * (adc1_voltage_vpp + 32768) / 65536.0 - 1.0);

    adc1_voltage_ave = adc1_voltage_ave / (uint16_t)(1024/adc1_first_harmonic_index);
    adc1_voltage_ave = 3.0 * (2.0 * (adc1_voltage_ave + 32768) / 65536.0 - 1.0);

    adc1_voltage_rms = adc1_voltage_rms / (uint16_t)(1024/adc1_first_harmonic_index);
    adc1_voltage_rms = sqrt(adc1_voltage_rms);
    adc1_voltage_rms = 3.0 * (2.0 * (adc1_voltage_rms + 32768) / 65536.0 - 1.0);
}

void adc2_voltage(void)
{

}

void adc1_spectrum(void)
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

void adc1_harmonic(void)
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
        adc1_harmonic_amp[0] = 3.0 * (2.0 * (adc1_harmonic_abs[0] + 32768) / 65536.0 - 1.0);;
        adc1_harmonic_rms[0] = adc1_harmonic_amp[0];
    } else {
        for (i=1; i<10; i++) {
            adc1_harmonic_abs[i] = adc1_spectrum_abs[adc1_first_harmonic_index*i];
            adc1_harmonic_amp[i] = 3.0 * (2.0 * (adc1_harmonic_abs[i] + 32768) / 65536.0 - 1.0);
            adc1_harmonic_rms[i] = adc1_harmonic_amp[i] * (sqrt(2.0) / 2.0);
            if (adc1_harmonic_amp[1] != 0.0) {
                adc1_harmonic_percent[i] = adc1_harmonic_amp[i] / adc1_harmonic_amp[1] * 100.0;
            } else {
                adc1_harmonic_percent[i] = 0.0;
            }
        }
        adc1_harmonic_abs[0] = adc1_spectrum_abs[0];
        adc1_harmonic_amp[0] = 3.0 * (2.0 * (adc1_harmonic_abs[0] + 32768) / 65536.0 - 1.0);;
        adc1_harmonic_rms[0] = adc1_harmonic_amp[0];
        if (adc1_harmonic_amp[1] != 0.0) {
            adc1_harmonic_percent[0] = adc1_harmonic_amp[0] / adc1_harmonic_amp[1] * 100.0;
        } else {
            adc1_harmonic_percent[0] = 0.0;
        }
    }
}

void task_adc(void)
{
    extern volatile uint16_t adc1_buffer_read;
    extern uint16_t disp_menu_index;
    extern uint16_t disp_menu_level;

    adc1_start();

    if (adc1_buffer_read == 0 && disp_menu_level == MENU_LEVEL_2) {
        adc1_buffer_read = 1;

        switch (disp_menu_index) {
            case MENU_ITEM_VOLTAGE:
                adc1_fft();
                adc1_spectrum();
                adc1_voltage();
                break;
            case MENU_ITEM_SPECTRUM:
                adc1_fft();
                adc1_spectrum();
                break;
            case MENU_ITEM_HARMONIC:
                adc1_fft();
                adc1_spectrum();
                adc1_harmonic();
                break;
            case MENU_ITEM_FREQUENCY:
                break;
            default:
                break;
        }
    }
}
