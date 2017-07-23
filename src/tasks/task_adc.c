/*
 * task_adc.c
 *
 *  Created on: 2017-6-27
 *      Author: redchenjs
 */
#include <driverlib.h>
#include <device.h>

#include <vcu2_types.h>
#include <vcu2_fft.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "inc/device/adc.h"

double adc1_spectrum_amp[128]   = {0};
double adc1_spectrum_phase[128] = {0};

double adc1_harmonic_abs[10] = {0};
double adc1_harmonic_amp[10] = {0};
double adc1_harmonic_percent[10] = {0};

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

void adc1_vrms1(void)
{

}

void adc2_vrms2(void)
{

}

void adc1_spectrum(void)
{
    extern int16_t adc1_buffer_fft[2048];
    extern uint32_t eqep1_position;
    int i, j;

    j = 0;
    for (i=0; i<128; i++) {
        adc1_spectrum_amp[i] = sqrt(pow(adc1_buffer_fft[j], 2) + pow(adc1_buffer_fft[j+1], 2)) / (1024 / 2.0);
        adc1_spectrum_phase[i] = atan2(adc1_buffer_fft[j+1], adc1_buffer_fft[j]);
        j += 2;
    }
}

void adc1_harmonic(void)
{
    extern int16_t adc1_buffer_fft[2048];

    int i;

    adc1_first_harmonic_index = 50*2;

    for (i=1*2; i<=100*2; i+=2) {
        if (abs(adc1_buffer_fft[i]) > abs(adc1_buffer_fft[adc1_first_harmonic_index])) {
            adc1_first_harmonic_index = i;
        }
    }

    for (i=1; i<10; i++) {
        adc1_harmonic_abs[i] = sqrt(pow(adc1_buffer_fft[adc1_first_harmonic_index*i], 2) + pow(adc1_buffer_fft[adc1_first_harmonic_index*i+1], 2)) / (1024.0 / 2.0);
        adc1_harmonic_amp[i] = 3000.0 * (2.0 * (adc1_harmonic_abs[i] + 32768) / 65536.0 - 1.0);
        adc1_harmonic_percent[i] = adc1_harmonic_amp[i] / adc1_harmonic_amp[1] * 100.0;
    }

    adc1_harmonic_abs[0] = (double)adc1_buffer_fft[0] / 1024;
    adc1_harmonic_amp[0] = 3000.0 * (2.0 * (adc1_harmonic_abs[0] + 32768) / 65536.0 - 1.0);
    adc1_harmonic_percent[0] = adc1_harmonic_amp[0] / adc1_harmonic_amp[1] * 100.0;
}

void task_adc(void)
{
    extern volatile uint16_t adc1_buffer_read;

    adc1_start();

    if (adc1_buffer_read == 0) {
        adc1_buffer_read = 1;

        adc1_fft();
        adc1_harmonic();
    }
}
