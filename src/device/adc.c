/*
 * adc.c
 *
 *  Created on: 2017年6月29日
 *      Author: redchenjs
 */
#include "driverlib.h"
#include "device.h"

#include "fpu_vector.h"

//
// Globals
//
int16_t adc1_results[2048];   // Buffer for results

uint16_t adc1_index;                          // Index into result buffer
volatile uint16_t adc1_ready;                // Flag to indicate buffer is full
volatile uint16_t adc1_buffer_read;
//
// Function Prototypes
//
__interrupt void adc1_isr(void);

//
// Main
//
void adc1_init(void)
{
    Interrupt_register(INT_ADCD1, &adc1_isr);

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
                 ADC_CH_ADCIN2, 64);

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
    for (adc1_index = 0; adc1_index < 2048; adc1_index++) {
        adc1_results[adc1_index] = 0;
    }

    adc1_index = 0;
    adc1_ready = 1;
    adc1_buffer_read = 1;

    //
    // Enable ADC interrupt
    //
    Interrupt_enable(INT_ADCD1);
}

void adc1_start(void)
{
    if (adc1_ready != 1) {
        return;
    } else {
        adc1_ready = 0;
        //
        // Start ePWM1, enabling SOCA and putting the counter in up-count mode
        //
        EPWM_enableADCTrigger(EPWM1_BASE, EPWM_SOC_A);
        EPWM_setTimeBaseCounterMode(EPWM1_BASE, EPWM_COUNTER_MODE_UP);
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
    adc1_results[adc1_index] = ADC_readResult(ADCDRESULT_BASE, ADC_SOC_NUMBER0) - 32768;
    adc1_index += 2;
    //
    // Set the bufferFull flag if the buffer is full
    //
    if (2048 <= adc1_index) {
        adc1_index = 0;
        //
        // Stop ePWM1, disabling SOCA and freezing the counter
        //
        EPWM_disableADCTrigger(EPWM1_BASE, EPWM_SOC_A);
        EPWM_setTimeBaseCounterMode(EPWM1_BASE, EPWM_COUNTER_MODE_STOP_FREEZE);

        if (adc1_buffer_read == 1) {
            adc1_buffer_read = 0;
            memcpy_fast(adc1_buffer_sample, adc1_results, sizeof(adc1_buffer_sample));
        }
        adc1_ready = 1;
    }

    //
    // Clear the interrupt flag and issue ACK
    //
    ADC_clearInterruptStatus(ADCD_BASE, ADC_INT_NUMBER1);
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}
