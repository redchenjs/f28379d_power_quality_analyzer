/*
 * task_status.c
 *
 *  Created on: 2017年8月10日
 *      Author: redchenjs
 */
#include <driverlib.h>
#include <device.h>

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "inc/system/fonts.h"
#include "inc/driver/ssd1351.h"
#include "inc/tasks/task_menu.h"
#include "inc/tasks/task_status.h"

power_param_t power_param;
harmonic_param_t harmonic_param;
elec_param_t elec_param[8];

uint16_t func_flag = 0;
uint16_t learn_flag = 0;

void status_update(void)
{
    extern double adc1_voltage_rms;
    extern double adc2_current_rms;
    extern double adc1_2_apparent_power;
    extern double adc1_2_active_power;
    extern double adc1_2_reactive_power;
    extern double adc1_2_power_factor;
    extern double ecap1_freq;
    extern double ecap1_duty;
    extern double ecap2_phase_degree;
    extern double ecap2_phase_radian;
    extern double adc2_harmonic_abs[10];
    uint16_t i;

    power_param.voltage = adc1_voltage_rms;
    power_param.current = adc2_current_rms;
    power_param.phase = ecap2_phase_degree;
    power_param.frequency = ecap1_freq;
    power_param.duty = ecap1_duty;
    power_param.apparent_power = adc1_2_apparent_power;
    power_param.active_power = adc1_2_active_power;
    power_param.reactive_power = adc1_2_reactive_power;
    power_param.power_factor = adc1_2_power_factor;

//    elec_param[0].current = 0.007;
//    elec_param[1].current = 0.002;
//    elec_param[2].current = 0.036;
//    elec_param[3].current = 0.017;
//    elec_param[4].current = 0.013;
//    elec_param[5].current = 0.012;
//    elec_param[6].current = 0.025;
//    elec_param[7].current = 8;

    for (i=1; i<8; i++) {
        if ((elec_param[i].status == 0) && ((adc2_current_rms) <=  (elec_param[i].current + 0.001)) && ((adc2_current_rms >=  elec_param[i].current - 0.001))) {
            elec_param[i].count_1++;
            if (elec_param[i].count_1 == 1) {
                elec_param[i].count_1 = 0;
                elec_param[i].status = 1;
            }
        }
        if ((elec_param[i].status == 1) && ((adc2_current_rms) >=  (elec_param[i].current + 0.001)) || ((adc2_current_rms <=  elec_param[i].current - 0.001))) {
            elec_param[i].count_2++;
            if (elec_param[i].count_2 == 1) {
                elec_param[i].count_2 = 0;
                elec_param[i].status = 0;
            }
        }
    }
//    for (i=1; i<8; i++) {
//        if ((elec_param[i].status == 0) && (adc2_harmonic_abs[1] <=  (elec_param[i].current + 0.0005)) && (adc2_harmonic_abs[1] >=  (elec_param[i].current - 0.0005))) {
//            elec_param[i].count_1++;
//            if (elec_param[i].count_1 == 1) {
//                elec_param[i].count_1 = 0;
//                elec_param[i].status = 1;
//            }
//        }
//        if ((elec_param[i].status == 1) && (adc2_harmonic_abs[1] >=  (elec_param[i].current + 0.0005)) || (adc2_harmonic_abs[1] <=  (elec_param[i].current - 0.0005))) {
//            elec_param[i].count_2++;
//            if (elec_param[i].count_2 == 1) {
//                elec_param[i].count_2 = 0;
//                elec_param[i].status = 0;
//            }
//        }
//    }
}

void status_view(void)
{
    extern double adc1_voltage_rms;
    extern double adc2_current_rms;
    extern double adc1_2_apparent_power;
    extern double adc1_2_active_power;
    extern double adc1_2_reactive_power;
    extern double adc1_2_power_factor;
    extern double ecap1_freq;
    extern double ecap1_duty;
    extern double ecap2_phase_degree;
    extern double ecap2_phase_radian;
    extern double adc2_harmonic_abs[10];
    uint16_t num;
    extern elec_param_t elec_param[8];
    char str_temp[24] = {0};
    extern uint32_t eqep1_position;

    num = (uint16_t)eqep1_position % 7 + 1;

    snprintf(str_temp, 32, "%d", num);
    ssd1351_display_string(0, 0, str_temp, FONT_3216, Yellow, Blue);

    snprintf(str_temp, 32, "Current:%9.5lf", round(elec_param[num].current * 1e3) / 1e3);
    ssd1351_display_string(0, 32, str_temp, FONT_1206, Lime, Black);

    snprintf(str_temp, 32, "Harmonic1:%9.3lf", round(elec_param[num].harmonic_1 * 1e3) / 1e3);
    ssd1351_display_string(0, 44, str_temp, FONT_1206, Lime, Black);

    snprintf(str_temp, 32, "Harmonic2:%9.3lf", round(elec_param[num].harmonic_2 * 1e3) / 1e3);
    ssd1351_display_string(0, 56, str_temp, FONT_1206, Lime, Black);

    snprintf(str_temp, 32, "Harmonic3:%9.3lf", round(elec_param[num].harmonic_3 * 1e3) / 1e3);
    ssd1351_display_string(0, 68, str_temp, FONT_1206, Lime, Black);
}

void status_learn(void)
{
    extern double adc1_voltage_rms;
    extern double adc2_current_rms;
    extern double adc1_2_apparent_power;
    extern double adc1_2_active_power;
    extern double adc1_2_reactive_power;
    extern double adc1_2_power_factor;
    extern double ecap1_freq;
    extern double ecap1_duty;
    extern double ecap2_phase_degree;
    extern double ecap2_phase_radian;
    extern double adc2_harmonic_abs[10];
    uint16_t num;
    extern elec_param_t elec_param[8];
    char str_temp[24] = {0};
    extern uint32_t eqep1_position;

    num = (uint16_t)eqep1_position % 7 + 1;

    snprintf(str_temp, 32, "%d", num);
    ssd1351_display_string(0, 0, str_temp, FONT_3216, Yellow, Blue);

    if (learn_flag != 0) {
        learn_flag = 0;
        ssd1351_display_string(32, 64, "Wait...", FONT_1608, Yellow, Blue);
        DEVICE_DELAY_US(1000000);
        elec_param[num].voltage = adc1_voltage_rms;
        elec_param[num].current = adc2_current_rms;
        elec_param[num].phase = ecap2_phase_degree;
        elec_param[num].frequency = ecap1_freq;
        elec_param[num].duty = ecap1_duty;
        elec_param[num].harmonic_1 = adc2_harmonic_abs[1];
        elec_param[num].harmonic_2 = adc2_harmonic_abs[2];
        elec_param[num].harmonic_3 = adc2_harmonic_abs[3];
        elec_param[num].harmonic_4 = adc2_harmonic_abs[4];
        elec_param[num].harmonic_5 = adc2_harmonic_abs[5];
        elec_param[num].harmonic_6 = adc2_harmonic_abs[6];
        elec_param[num].harmonic_7 = adc2_harmonic_abs[7];
        elec_param[num].harmonic_8 = adc2_harmonic_abs[8];
        elec_param[num].harmonic_9 = adc2_harmonic_abs[9];
        ssd1351_display_string(32, 64, "   OK  ", FONT_1608, Yellow, Blue);
    }
    func_flag = 0;
}

void status_clear(void)
{
    extern uint16_t disp_refresh_flag;
    extern uint16_t menu_item;
    extern uint16_t menu_level;
    extern uint16_t menu_refresh;

    if (menu_level == MENU_LEVEL_2) {
        menu_level = MENU_LEVEL_1;
        disp_refresh_flag = 1;
    }

    memset(&power_param, 0x00, sizeof(power_param));
    memset(&harmonic_param, 0x00, sizeof(harmonic_param));
    memset(elec_param, 0x00, sizeof(elec_param));

    ssd1351_display_string(32, 64, "   OK  ", FONT_1608, Yellow, Blue);
    DEVICE_DELAY_US(1000000);
}
