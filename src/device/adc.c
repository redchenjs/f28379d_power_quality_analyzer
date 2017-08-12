/*
 * adc.c
 *
 *  Created on: 2017年6月29日
 *      Author: redchenjs
 */
#include "driverlib.h"
#include "device.h"

#include "fpu_vector.h"

#include "inc/device/epwm.h"
//
// Globals
//
int16_t adc1_results[2048];   // Buffer for results
int16_t adc2_results[2048];   // Buffer for results

uint16_t adc1_index;                          // Index into result buffer
volatile uint16_t adc1_ready;                // Flag to indicate buffer is full
volatile uint16_t adc1_buffer_read;

uint16_t adc2_index;                          // Index into result buffer
volatile uint16_t adc2_ready;                // Flag to indicate buffer is full
volatile uint16_t adc2_buffer_read;
//
// Function Prototypes
//
__interrupt void adc1_isr(void);
__interrupt void adc2_isr(void);
//
// Main
//
void adc1_init(void)
{
    Interrupt_register(INT_ADCC1, &adc1_isr);

    //
    // Set up the ADC and the ePWM and initialize the SOC
    //
    //
    // Set ADCCLK divider to /4
    //

    ADC_setPrescaler(ADCC_BASE, ADC_CLK_DIV_4_0);

    //
    // Set resolution and signal mode (see #defines above) and load
    // corresponding trims.
    //
    ADC_setMode(ADCC_BASE, ADC_RESOLUTION_16BIT, ADC_MODE_SINGLE_ENDED);

    //
    // Set pulse positions to late
    //
    ADC_setInterruptPulseMode(ADCC_BASE, ADC_PULSE_END_OF_CONV);

    //
    // Power up the ADC and then delay for 1 ms
    //
    ADC_enableConverter(ADCC_BASE);
    DEVICE_DELAY_US(1000);

    ADC_setupSOC(ADCC_BASE, ADC_SOC_NUMBER0, ADC_TRIGGER_EPWM1_SOCA,
                 ADC_CH_ADCIN5, 64);

    //
    // Set SOC0 to set the interrupt 1 flag. Enable the interrupt and make
    // sure its flag is cleared.
    //
    ADC_setInterruptSource(ADCC_BASE, ADC_INT_NUMBER1, ADC_SOC_NUMBER0);
    ADC_enableInterrupt(ADCC_BASE, ADC_INT_NUMBER1);
    ADC_clearInterruptStatus(ADCC_BASE, ADC_INT_NUMBER1);

    //
    // Initialize results buffer
    //
    for (adc1_index = 0; adc1_index < 2048; adc1_index++) {
        adc1_results[adc1_index] = 0;
    }

    adc1_index = 0;
    adc1_ready = 1;
    adc1_buffer_read = 1;
}

void adc2_init(void)
{
    Interrupt_register(INT_ADCD1, &adc2_isr);

    //
    // Set up the ADC and the ePWM and initialize the SOC
    //
    //
    // Set ADCCLK divider to /4
    //

    ADC_setPrescaler(ADCD_BASE, ADC_CLK_DIV_4_0);

    //
    // Set resolution and signal mode (see #defines above) and load
    // corresponding trims.
    //
    ADC_setMode(ADCD_BASE, ADC_RESOLUTION_16BIT, ADC_MODE_DIFFERENTIAL);

    //
    // Set pulse positions to late
    //
    ADC_setInterruptPulseMode(ADCD_BASE, ADC_PULSE_END_OF_CONV);

    //
    // Power up the ADC and then delay for 1 ms
    //
    ADC_enableConverter(ADCD_BASE);
    DEVICE_DELAY_US(1000);

    ADC_setupSOC(ADCD_BASE, ADC_SOC_NUMBER0, ADC_TRIGGER_EPWM1_SOCA,
                 ADC_CH_ADCIN4_ADCIN5, 64);

    //
    // Set SOC0 to set the interrupt 1 flag. Enable the interrupt and make
    // sure its flag is cleared.
    //
    ADC_setInterruptSource(ADCD_BASE, ADC_INT_NUMBER1, ADC_SOC_NUMBER0);
    ADC_enableInterrupt(ADCD_BASE, ADC_INT_NUMBER1);
    ADC_clearInterruptStatus(ADCD_BASE, ADC_INT_NUMBER1);

    //
    // Initialize results buffer
    //
    for (adc2_index = 0; adc2_index < 2048; adc2_index++) {
        adc2_results[adc2_index] = 0;
    }

    adc2_index = 0;
    adc2_ready = 1;
    adc2_buffer_read = 1;
}

void adc1_2_start(void)
{
    if (adc1_ready != 1 || adc2_ready != 1) {
        return;
    } else {
        adc1_ready = 0;
        adc2_ready = 0;
        Interrupt_enable(INT_ADCC1);
        Interrupt_enable(INT_ADCD1);
        epwm1_start();
    }
}

//
// ADC A Interrupt 1 ISR
//
__interrupt void adc1_isr(void)
{
    extern int16_t adc1_buffer_sample[2048];
    //
    // Add the latest result to the buffer
    //
    adc1_results[adc1_index] = ADC_readResult(ADCCRESULT_BASE, ADC_SOC_NUMBER0) - 31389;
    adc1_index += 2;
    //
    // Set the bufferFull flag if the buffer is full
    //
    if (2048 <= adc1_index) {
        adc1_index = 0;

        epwm1_stop();
        Interrupt_disable(INT_ADCC1);

        if (adc1_buffer_read == 1) {
            adc1_buffer_read = 0;
            memcpy_fast(adc1_buffer_sample, adc1_results, sizeof(adc1_buffer_sample));
        }
        adc1_ready = 1;
    }

    //
    // Clear the interrupt flag and issue ACK
    //
    ADC_clearInterruptStatus(ADCC_BASE, ADC_INT_NUMBER1);
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}

__interrupt void adc2_isr(void)
{
    extern int16_t adc2_buffer_sample[2048];
    //
    // Add the latest result to the buffer
    //
    adc2_results[adc2_index] = ADC_readResult(ADCDRESULT_BASE, ADC_SOC_NUMBER0) - 32768;
    adc2_index += 2;
    //
    // Set the bufferFull flag if the buffer is full
    //
    if (2048 <= adc2_index) {
        adc2_index = 0;

        epwm1_stop();
        Interrupt_disable(INT_ADCD1);

        if (adc2_buffer_read == 1) {
            adc2_buffer_read = 0;
            memcpy_fast(adc2_buffer_sample, adc2_results, sizeof(adc2_buffer_sample));
        }
        adc2_ready = 1;
    }

    //
    // Clear the interrupt flag and issue ACK
    //
    ADC_clearInterruptStatus(ADCD_BASE, ADC_INT_NUMBER1);
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}
