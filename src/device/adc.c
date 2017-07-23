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
// Defines
//
#define RESULTS_BUFFER_SIZE  2048
#define EX_ADC_RESOLUTION    ADC_RESOLUTION_16BIT   // Or ADC_RESOLUTION_16BIT
#define EX_ADC_SIGNAL_MODE   ADC_MODE_DIFFERENTIAL   // Or ADC_MODE_DIFFERENTIAL

//
// Globals
//
//int16_t
//uint16_t adcAResults[RESULTS_BUFFER_SIZE];   // Buffer for results

//#pragma DATA_SECTION(adcAResultsSig,"buffer1")
int16_t adcAResults[RESULTS_BUFFER_SIZE];   // Buffer for results

uint16_t index;                              // Index into result buffer
volatile uint16_t adc1_ready;                // Flag to indicate buffer is full
volatile uint16_t adc1_buffer_read;
//
// Function Prototypes
//
void initADC(void);
void initEPWM(void);
void initADCSOC(void);
__interrupt void adcD1ISR(void);

//
// Main
//
void adc1_init(void)
{
    Interrupt_register(INT_ADCD1, &adcD1ISR);

    //
    // Set up the ADC and the ePWM and initialize the SOC
    //
    initADC();
    initEPWM();
    initADCSOC();

    //
    // Initialize results buffer
    //
    for (index = 0; index < RESULTS_BUFFER_SIZE; index++) {
//        adcAResults[index] = 0;
        adcAResults[index] = 0;
    }

    index = 0;
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
// Function to configure and power up ADCD.
//
void initADC(void)
{
    //
    // Set ADCCLK divider to /4
    //
    ADC_setPrescaler(ADCD_BASE, ADC_CLK_DIV_4_0);

    //
    // Set resolution and signal mode (see #defines above) and load
    // corresponding trims.
    //
    ADC_setMode(ADCD_BASE, EX_ADC_RESOLUTION, EX_ADC_SIGNAL_MODE);

    //
    // Set pulse positions to late
    //
    ADC_setInterruptPulseMode(ADCD_BASE, ADC_PULSE_END_OF_CONV);

    //
    // Power up the ADC and then delay for 1 ms
    //
    ADC_enableConverter(ADCD_BASE);
    DEVICE_DELAY_US(1000);
}

//
// Function to configure ePWM1 to generate the SOC.
//
void initEPWM(void)
{
    //
    // Disable SOCA
    //
    EPWM_disableADCTrigger(EPWM1_BASE, EPWM_SOC_A);

    //
    // Configure the SOC to occur on the first up-count event
    //
    EPWM_setADCTriggerSource(EPWM1_BASE, EPWM_SOC_A, EPWM_SOC_TBCTR_U_CMPA);
    EPWM_setADCTriggerEventPrescale(EPWM1_BASE, EPWM_SOC_A, 1);

    //
    // Set the compare A value to 2048 and the period to 4096
    //
    EPWM_setClockPrescaler(EPWM1_BASE, EPWM_CLOCK_DIVIDER_2, EPWM_HSCLOCK_DIVIDER_1);
    EPWM_setCounterCompareValue(EPWM1_BASE, EPWM_COUNTER_COMPARE_A, 0x0000);
    EPWM_setTimeBasePeriod(EPWM1_BASE, 0xBEBC);

    //
    // Freeze the counter
    //
    EPWM_setTimeBaseCounterMode(EPWM1_BASE, EPWM_COUNTER_MODE_STOP_FREEZE);
}

//
// Function to configure ADCD's SOC0 to be triggered by ePWM1.
//
void initADCSOC(void)
{
    //
    // Configure SOC0 of ADCD to convert pin A0. The EPWM1SOCA signal will be
    // the trigger.
    //
    // For 12-bit resolution, a sampling window of 15 (75 ns at a 200MHz
    // SYSCLK rate) will be used.  For 16-bit resolution, a sampling window of
    // 64 (320 ns at a 200MHz SYSCLK rate) will be used.
    //
//#if(EX_ADC_RESOLUTION == ADC_RESOLUTION_12BIT)
//    ADC_setupSOC(ADCD_BASE, ADC_SOC_NUMBER0, ADC_TRIGGER_EPWM1_SOCA,
//                 ADC_CH_ADCIN2, 15);
//#elif(EX_ADC_RESOLUTION == ADC_RESOLUTION_16BIT)
    ADC_setupSOC(ADCD_BASE, ADC_SOC_NUMBER0, ADC_TRIGGER_EPWM1_SOCA,
                 ADC_CH_ADCIN2, 64);
//#endif

    //
    // Set SOC0 to set the interrupt 1 flag. Enable the interrupt and make
    // sure its flag is cleared.
    //
    ADC_setInterruptSource(ADCD_BASE, ADC_INT_NUMBER1, ADC_SOC_NUMBER0);
    ADC_enableInterrupt(ADCD_BASE, ADC_INT_NUMBER1);
    ADC_clearInterruptStatus(ADCD_BASE, ADC_INT_NUMBER1);
}

//
// ADC A Interrupt 1 ISR
//
__interrupt void adcD1ISR(void)
{
    extern int16_t adc1_buffer_sample[RESULTS_BUFFER_SIZE];
    //
    // Add the latest result to the buffer
    //
//    adcAResults[index++] = ADC_readResult(ADCDRESULT_BASE, ADC_SOC_NUMBER0);
    adcAResults[index] = ADC_readResult(ADCDRESULT_BASE, ADC_SOC_NUMBER0) - 32768;
    index += 2;
    //
    // Set the bufferFull flag if the buffer is full
    //
    if (RESULTS_BUFFER_SIZE <= index) {
        index = 0;
        //
        // Stop ePWM1, disabling SOCA and freezing the counter
        //
        EPWM_disableADCTrigger(EPWM1_BASE, EPWM_SOC_A);
        EPWM_setTimeBaseCounterMode(EPWM1_BASE, EPWM_COUNTER_MODE_STOP_FREEZE);

        if (adc1_buffer_read == 1) {
            adc1_buffer_read = 0;
            memcpy_fast(adc1_buffer_sample, adcAResults, sizeof(adc1_buffer_sample));
        }
        adc1_ready = 1;
    }

    //
    // Clear the interrupt flag and issue ACK
    //
    ADC_clearInterruptStatus(ADCD_BASE, ADC_INT_NUMBER1);
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}
