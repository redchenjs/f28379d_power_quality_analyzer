/*
 * task_menu.c
 *
 *  Created on: 2017-7-25
 *      Author: redchenjs
 */
#include <driverlib.h>
#include <device.h>

#include "inc/device/adc.h"
#include "inc/device/epwm.h"
#include "inc/device/ecap.h"
#include "inc/device/eqep.h"

#include "inc/system/fonts.h"

#include "inc/tasks/task_adc.h"
#include "inc/tasks/task_sci.h"
#include "inc/tasks/task_ecap.h"
#include "inc/tasks/task_eqep.h"
#include "inc/tasks/task_disp.h"
#include "inc/tasks/task_menu.h"
#include "inc/tasks/task_status.h"

const uint16_t menu_item_num = 6;

enum menu_item_t {
    MENU_ITEM_MEASURE   = 0x0,
    MENU_ITEM_SPECTRUM  = 0x1,
    MENU_ITEM_HARMONIC  = 0x2,
    MENU_ITEM_LEARN     = 0x3,
    MENU_ITEM_VIEW      = 0x4,
    MENU_ITEM_RESET     = 0x5
};

uint16_t menu_item    = 0;
uint16_t menu_level   = 0;
uint16_t menu_refresh = 0;

const char menu_item_string[6][24] = {
                                   "Measure      ",
                                   "Spectrum     ",
                                   "Harmonic     ",
                                   "Learn        ",
                                   "View         ",
                                   "Reset        "
};

const int menu_item_fore_color[6] = {White, White, White, White, White, White};
const int menu_item_back_color[6] = {Black, Black, Black, Black, Black, Black};

void menu_run(void)
{
    extern volatile uint16_t adc1_buffer_read;
    extern volatile uint16_t adc2_buffer_read;
    extern volatile uint16_t ecap1_ready;
    extern volatile uint16_t ecap2_ready;
    extern uint32_t eqep1_position;
    extern uint16_t func_flag;

    eqep1_position_caculate();

    disp_refresh();
    switch (menu_level) {
        case 0:
            menu_item = (uint16_t)eqep1_position % menu_item_num;
            disp_menu();
            disp_time();
            break;
        case 1:
            switch (menu_item) {
                case MENU_ITEM_MEASURE:
                    func_flag = 0;
                    adc1_2_start();
                    if (adc1_buffer_read == 0) {
                        adc1_buffer_read = 1;
                        adc1_voltage_caculate();
                        disp_adc1_voltage();
                    }
                    if (adc2_buffer_read == 0) {
                        adc2_buffer_read = 1;
                        adc2_current_caculate();
                        disp_adc2_current();
                    }
                    ecap1_start();
                    ecap1_frequency_caculate();
                    disp_ecap1_frequency();

                    ecap2_start();
                    ecap2_phase_caculate();
                    disp_ecap2_phase();

                    adc1_2_power_caculate();
                    disp_adc1_2_power();

                    status_update();
                    disp_status();

                    sci2_transmit_data();
                    break;
                case MENU_ITEM_SPECTRUM:
                    func_flag = 0;
                    adc1_2_start();
//                    if (adc1_buffer_read == 0) {
//                        adc1_buffer_read = 1;
//                        epwm1_stop();
//                        adc1_fft();
//                        adc1_spectrum_caculate();
//                        disp_adc1_spectrum();
//                    }
                    if (adc2_buffer_read == 0) {
                        adc2_buffer_read = 1;
                        adc2_fft();
                        adc2_spectrum_caculate();
                        disp_adc2_spectrum();
                    }
                    break;
                case MENU_ITEM_HARMONIC:
                    func_flag = 0;
                    adc1_2_start();
//                    if (adc1_buffer_read == 0) {
//                        adc1_buffer_read = 1;
//                        epwm1_stop();
//                        adc1_fft();
//                        adc1_spectrum_caculate();
//                        adc1_harmonic_caculate();
//                        disp_adc1_harmonic();
//                    }
                    if (adc2_buffer_read == 0) {
                        adc2_buffer_read = 1;
                        adc2_fft();
                        adc2_spectrum_caculate();
                        adc2_harmonic_caculate();
                        disp_adc2_harmonic();
                    }
                    break;
                case MENU_ITEM_LEARN:
                    func_flag = 1;
                    adc1_2_start();
                    if (adc2_buffer_read == 0) {
                        adc2_buffer_read = 1;
                        adc2_current_caculate();
//                        adc2_fft();
//                        adc2_spectrum_caculate();
//                        adc2_harmonic_caculate();
                    }
                    status_learn();
                    break;
                case MENU_ITEM_VIEW:
                    func_flag = 0;
                    status_view();
                    break;
                case MENU_ITEM_RESET:
                    func_flag = 0;
                    status_clear();
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
}

interrupt void xint1_isr(void)
{
    extern uint16_t disp_refresh_flag;
    extern uint16_t learn_flag;
    extern uint16_t func_flag;

    if (func_flag && learn_flag == 0) {
        learn_flag = 1;
    }

    if (menu_level == MENU_LEVEL_1) {
        menu_level = MENU_LEVEL_2;
        disp_refresh_flag = 1;
    }

    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}

interrupt void xint2_isr(void)
{
    extern uint16_t disp_refresh_flag;

    if (menu_level == MENU_LEVEL_2) {
        menu_level = MENU_LEVEL_1;
        disp_refresh_flag = 1;
    }

    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}
