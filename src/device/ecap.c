//! eCAP1 is configured to capture the time between rising
//! and falling edge.
//!
//! - eCAP1 is on GPIO16

//
// Included Files
//
#include "driverlib.h"
#include "device.h"

//
// Defines
//

//
// Globals
//
uint32_t ecap1IntCount;
volatile uint32_t ecap1Cap1Count;
volatile uint32_t ecap1Cap2Count;
volatile uint32_t ecap1Cap3Count;
volatile uint32_t ecap1Cap4Count;

uint32_t ecap2IntCount;
volatile uint32_t ecap2Cap1Count;
volatile uint32_t ecap2Cap2Count;
volatile uint32_t ecap2Cap3Count;
volatile uint32_t ecap2Cap4Count;

volatile uint16_t ecap1_ready;
volatile uint16_t ecap2_ready;
//
// Function Prototypes
//
__interrupt void ecap1ISR(void);
__interrupt void ecap2ISR(void);
//
// Main
//
void ecap1_init(void)
{
    //
    // Configure GPIO 16 as eCAP input
    //
    XBAR_setInputPin(XBAR_INPUT7, 16);
    GPIO_setPinConfig(GPIO_16_GPIO16);
    GPIO_setDirectionMode(16, GPIO_DIR_MODE_IN);
    GPIO_setQualificationMode(16, GPIO_QUAL_ASYNC);

    //
    // Interrupts that are used in this example are re-mapped to ISR functions
    // found within this file.
    //
    Interrupt_register(INT_ECAP1, &ecap1ISR);

    //
    // Disable ,clear all capture flags and interrupts
    //
    ECAP_disableInterrupt(ECAP1_BASE,
                          (ECAP_ISR_SOURCE_CAPTURE_EVENT_1  |
                           ECAP_ISR_SOURCE_CAPTURE_EVENT_2  |
                           ECAP_ISR_SOURCE_CAPTURE_EVENT_3  |
                           ECAP_ISR_SOURCE_CAPTURE_EVENT_4  |
                           ECAP_ISR_SOURCE_COUNTER_OVERFLOW |
                           ECAP_ISR_SOURCE_COUNTER_PERIOD   |
                           ECAP_ISR_SOURCE_COUNTER_COMPARE));
    ECAP_clearInterrupt(ECAP1_BASE,
                        (ECAP_ISR_SOURCE_CAPTURE_EVENT_1  |
                         ECAP_ISR_SOURCE_CAPTURE_EVENT_2  |
                         ECAP_ISR_SOURCE_CAPTURE_EVENT_3  |
                         ECAP_ISR_SOURCE_CAPTURE_EVENT_4  |
                         ECAP_ISR_SOURCE_COUNTER_OVERFLOW |
                         ECAP_ISR_SOURCE_COUNTER_PERIOD   |
                         ECAP_ISR_SOURCE_COUNTER_COMPARE));

    //
    // Disable CAP1-CAP4 register loads
    //
    ECAP_disableTimeStampCapture(ECAP1_BASE);

    //
    // Configure eCAP
    //    Enable capture mode.
    //    One shot mode, stop capture at event 4.
    //    Set polarity of the events to rising, falling, rising, falling edge.
    //    Set capture in time difference mode.
    //    Select input from XBAR7.
    //    Enable eCAP module.
    //    Enable interrupt.
    //
    ECAP_stopCounter(ECAP1_BASE);
    ECAP_enableCaptureMode(ECAP1_BASE);

    ECAP_setCaptureMode(ECAP1_BASE, ECAP_ONE_SHOT_CAPTURE_MODE, ECAP_EVENT_4);

    ECAP_setEventPolarity(ECAP1_BASE, ECAP_EVENT_1, ECAP_EVNT_FALLING_EDGE);
    ECAP_setEventPolarity(ECAP1_BASE, ECAP_EVENT_2, ECAP_EVNT_RISING_EDGE);
    ECAP_setEventPolarity(ECAP1_BASE, ECAP_EVENT_3, ECAP_EVNT_FALLING_EDGE);
    ECAP_setEventPolarity(ECAP1_BASE, ECAP_EVENT_4, ECAP_EVNT_RISING_EDGE);

    ECAP_enableCounterResetOnEvent(ECAP1_BASE, ECAP_EVENT_1);
    ECAP_enableCounterResetOnEvent(ECAP1_BASE, ECAP_EVENT_2);
    ECAP_enableCounterResetOnEvent(ECAP1_BASE, ECAP_EVENT_3);
    ECAP_enableCounterResetOnEvent(ECAP1_BASE, ECAP_EVENT_4);

    XBAR_setInputPin(XBAR_INPUT7, 16);

    ECAP_enableLoadCounter(ECAP1_BASE);
    ECAP_setSyncOutMode(ECAP1_BASE, ECAP_SYNC_OUT_SYNCI);
    ECAP_stopCounter(ECAP1_BASE);
    ECAP_enableTimeStampCapture(ECAP1_BASE);
    ECAP_reArm(ECAP1_BASE);

    ECAP_enableInterrupt(ECAP1_BASE, ECAP_ISR_SOURCE_CAPTURE_EVENT_4);

    //
    // Initialize counters:
    //
    ecap1Cap1Count = 0U;
    ecap1Cap2Count = 0U;
    ecap1Cap3Count = 0U;
    ecap1Cap4Count = 0U;
    ecap1IntCount  = 0U;

    ecap1_ready = 1;
    //
    // Enable interrupts required for this example
    //
    Interrupt_enable(INT_ECAP1);
}

void ecap2_init(void)
{
    GPIO_setPinConfig(GPIO_5_GPIO5);
    GPIO_setPadConfig(5, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(5, GPIO_DIR_MODE_IN);
    GPIO_setQualificationMode(5, GPIO_QUAL_6SAMPLE);
    GPIO_setQualificationPeriod(5, 510);
    //
    // Configure GPIO 24 as eCAP input
    //
    XBAR_setInputPin(XBAR_INPUT8, 24);
    GPIO_setPinConfig(GPIO_24_GPIO24);
    GPIO_setDirectionMode(24, GPIO_DIR_MODE_IN);
    GPIO_setQualificationMode(24, GPIO_QUAL_ASYNC);

    //
    // Interrupts that are used in this example are re-mapped to ISR functions
    // found within this file.
    //
    Interrupt_register(INT_ECAP2, &ecap2ISR);

    //
    // Disable ,clear all capture flags and interrupts
    //
    ECAP_disableInterrupt(ECAP2_BASE,
                          (ECAP_ISR_SOURCE_CAPTURE_EVENT_1  |
                           ECAP_ISR_SOURCE_CAPTURE_EVENT_2  |
                           ECAP_ISR_SOURCE_CAPTURE_EVENT_3  |
                           ECAP_ISR_SOURCE_CAPTURE_EVENT_4  |
                           ECAP_ISR_SOURCE_COUNTER_OVERFLOW |
                           ECAP_ISR_SOURCE_COUNTER_PERIOD   |
                           ECAP_ISR_SOURCE_COUNTER_COMPARE));
    ECAP_clearInterrupt(ECAP2_BASE,
                        (ECAP_ISR_SOURCE_CAPTURE_EVENT_1  |
                         ECAP_ISR_SOURCE_CAPTURE_EVENT_2  |
                         ECAP_ISR_SOURCE_CAPTURE_EVENT_3  |
                         ECAP_ISR_SOURCE_CAPTURE_EVENT_4  |
                         ECAP_ISR_SOURCE_COUNTER_OVERFLOW |
                         ECAP_ISR_SOURCE_COUNTER_PERIOD   |
                         ECAP_ISR_SOURCE_COUNTER_COMPARE));

    //
    // Disable CAP1-CAP4 register loads
    //
    ECAP_disableTimeStampCapture(ECAP2_BASE);

    //
    // Configure eCAP
    //    Enable capture mode.
    //    One shot mode, stop capture at event 4.
    //    Set polarity of the events to rising, falling, rising, falling edge.
    //    Set capture in time difference mode.
    //    Select input from XBAR7.
    //    Enable eCAP module.
    //    Enable interrupt.
    //
    ECAP_stopCounter(ECAP2_BASE);
    ECAP_enableCaptureMode(ECAP2_BASE);

    ECAP_setCaptureMode(ECAP2_BASE, ECAP_ONE_SHOT_CAPTURE_MODE, ECAP_EVENT_4);

    ECAP_setEventPolarity(ECAP2_BASE, ECAP_EVENT_1, ECAP_EVNT_FALLING_EDGE);
    ECAP_setEventPolarity(ECAP2_BASE, ECAP_EVENT_2, ECAP_EVNT_RISING_EDGE);
    ECAP_setEventPolarity(ECAP2_BASE, ECAP_EVENT_3, ECAP_EVNT_FALLING_EDGE);
    ECAP_setEventPolarity(ECAP2_BASE, ECAP_EVENT_4, ECAP_EVNT_RISING_EDGE);

    ECAP_enableCounterResetOnEvent(ECAP2_BASE, ECAP_EVENT_1);
    ECAP_enableCounterResetOnEvent(ECAP2_BASE, ECAP_EVENT_2);
    ECAP_enableCounterResetOnEvent(ECAP2_BASE, ECAP_EVENT_3);
    ECAP_enableCounterResetOnEvent(ECAP2_BASE, ECAP_EVENT_4);

    XBAR_setInputPin(XBAR_INPUT8, 24);

    ECAP_enableLoadCounter(ECAP2_BASE);
    ECAP_setSyncOutMode(ECAP2_BASE, ECAP_SYNC_OUT_SYNCI);
    ECAP_stopCounter(ECAP2_BASE);
    ECAP_enableTimeStampCapture(ECAP2_BASE);
    ECAP_reArm(ECAP2_BASE);

    ECAP_enableInterrupt(ECAP2_BASE, ECAP_ISR_SOURCE_CAPTURE_EVENT_4);

    //
    // Initialize counters:
    //
    ecap2Cap1Count = 0U;
    ecap2Cap2Count = 0U;
    ecap2Cap3Count = 0U;
    ecap2Cap4Count = 0U;
    ecap2IntCount  = 0U;

    ecap2_ready = 1;
    //
    // Enable interrupts required for this example
    //
    Interrupt_enable(INT_ECAP2);
}

void ecap1_start(void)
{
    if (ecap1_ready != 1) {
        return;
    }
    else {
        //
        // Start eCAP
        //
        ecap1_ready = 0;
        ECAP_reArm(ECAP1_BASE);
        ECAP_loadCounter(ECAP1_BASE);
        ECAP_startCounter(ECAP1_BASE);
    }
}

void ecap2_start(void)
{
    if (ecap2_ready != 1) {
        return;
    }
    else {
        //
        // Start eCAP
        //
        ecap2_ready = 0;
        ECAP_reArm(ECAP2_BASE);
        ECAP_loadCounter(ECAP2_BASE);
        ECAP_startCounter(ECAP2_BASE);
    }
}

//
// eCAP 1 ISR
//
__interrupt void ecap1ISR(void)
{
    ECAP_stopCounter(ECAP1_BASE);
    //
    // Get the capture counts. Each capture should be 4x the ePWM count
    // because of the ePWM clock dividers.
    //
    ecap1Cap1Count = ECAP_getEventTimeStamp(ECAP1_BASE, ECAP_EVENT_1);
    ecap1Cap2Count = ECAP_getEventTimeStamp(ECAP1_BASE, ECAP_EVENT_2);
    ecap1Cap3Count = ECAP_getEventTimeStamp(ECAP1_BASE, ECAP_EVENT_3);
    ecap1Cap4Count = ECAP_getEventTimeStamp(ECAP1_BASE, ECAP_EVENT_4);

    ecap1IntCount++;

    //
    // Clear interrupt flags for more interrupts.
    //
    ECAP_clearInterrupt(ECAP1_BASE,ECAP_ISR_SOURCE_CAPTURE_EVENT_4);
    ECAP_clearGlobalInterrupt(ECAP1_BASE);

    ecap1_ready = 1;
    //
    // Acknowledge the group interrupt for more interrupts.
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP4);
}
//
// eCAP 2 ISR
//
__interrupt void ecap2ISR(void)
{
    ECAP_stopCounter(ECAP2_BASE);
    //
    // Get the capture counts. Each capture should be 4x the ePWM count
    // because of the ePWM clock dividers.
    //
    ecap2Cap1Count = ECAP_getEventTimeStamp(ECAP2_BASE, ECAP_EVENT_1);
    ecap2Cap2Count = ECAP_getEventTimeStamp(ECAP2_BASE, ECAP_EVENT_2);
    ecap2Cap3Count = ECAP_getEventTimeStamp(ECAP2_BASE, ECAP_EVENT_3);
    ecap2Cap4Count = ECAP_getEventTimeStamp(ECAP2_BASE, ECAP_EVENT_4);

    ecap2IntCount++;

    //
    // Clear interrupt flags for more interrupts.
    //
    ECAP_clearInterrupt(ECAP2_BASE,ECAP_ISR_SOURCE_CAPTURE_EVENT_4);
    ECAP_clearGlobalInterrupt(ECAP2_BASE);

    ecap2_ready = 1;
    //
    // Acknowledge the group interrupt for more interrupts.
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP4);
}

