/*
 * task_menu.c
 *
 *  Created on: 2017-7-25
 *      Author: redchenjs
 */
#include <driverlib.h>
#include <device.h>

#include "inc/device/adc.h"
#include "inc/device/ecap.h"
#include "inc/device/eqep.h"

#include "inc/system/fonts.h"

#include "inc/tasks/task_adc.h"
#include "inc/tasks/task_ecap.h"
#include "inc/tasks/task_eqep.h"
#include "inc/tasks/task_disp.h"

const uint16_t menu_item_num = 5;

enum menu_level_t {
    MENU_LEVEL_1   = 0x0,
    MENU_LEVEL_2   = 0x1
};

enum menu_item_t {
    MENU_ITEM_MEASURE   = 0x0,
    MENU_ITEM_SPECTRUM  = 0x1,
    MENU_ITEM_HARMONIC  = 0x2,
    MENU_ITEM_FREQUENCY = 0x3,
    MENU_ITEM_GRAPH     = 0x4
};

uint16_t menu_item    = 0;
uint16_t menu_level   = 0;
uint16_t menu_refresh = 0;

const char menu_item_string[5][24] = {
                                   "Measure      ",
                                   "Spectrum     ",
                                   "Harmonic     ",
                                   "Frequency    ",
                                   "Graph        "
};

const int menu_item_fore_color[5] = {White, White, White, White, White};
const int menu_item_back_color[5] = {Black, Black, Black, Black, Black};

void menu_run(void)
{
    extern volatile uint16_t adc1_buffer_read;
    extern uint32_t eqep1_position;

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
                    adc1_start();
                    if (adc1_buffer_read == 0) {
                        adc1_buffer_read = 1;
                        adc1_voltage_caculate();
                        disp_adc1_voltage();
                    }
                    break;
                case MENU_ITEM_SPECTRUM:
                    adc1_start();
                    if (adc1_buffer_read == 0) {
                        adc1_buffer_read = 1;
                        adc1_fft();
                        adc1_spectrum_caculate();
                        disp_adc1_spectrum();
                    }
                    break;
                case MENU_ITEM_HARMONIC:
                    adc1_start();
                    if (adc1_buffer_read == 0) {
                        adc1_buffer_read = 1;
                        adc1_fft();
                        adc1_spectrum_caculate();
                        adc1_harmonic_caculate();
                        disp_adc1_harmonic();
                    }
                    break;
                case MENU_ITEM_FREQUENCY:
                    ecap1_start();
                    ecap1_frequency_caculate();
                    disp_ecap1_frequency();
                    break;
                case MENU_ITEM_GRAPH:
                    ecap2_start();
                    ecap2_phase_caculate();
                    disp_ecap2_phase();
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
