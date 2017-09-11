/*
 * task_adc.c
 *
 *  Created on: 2017-6-27
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

#include "inc/tasks/task_status.h"

uint16_t disp_refresh_flag = 0;

void disp_adc1_voltage(void)
{
//    extern double adc1_voltage_peak;
//    extern double adc1_voltage_ave;
    extern double adc1_voltage_rms;

    char str_rms[32] = {0};
//    ssd1351_display_num(16, 0, (long)(adc1_voltage_peak*1e3), 5, FONT_3216, Magenta, Black);
//    ssd1351_display_num(16, 32, (long)(adc1_voltage_ave*1e3), 5, FONT_3216, Lime, Black);
//    snprintf(str_rms, 32, "Voltage:%7.3f mV", round(adc1_voltage_rms * 1e6) / 1e3);
    snprintf(str_rms, 32, "Voltage:%9.3lf V", round(adc1_voltage_rms * 1e3) / 1e3);
    ssd1351_display_string(0, 0, str_rms, FONT_1206, Yellow, Black);
}

void disp_adc1_spectrum(void)
{
    extern double adc1_spectrum_abs[512];
    uint16_t i;

    if (adc1_spectrum_abs[0] < 0) {
        ssd1351_draw_column(0, 0, 128, Black, White);
    } else if ((uint32_t)(adc1_spectrum_abs[0]*1e3) > 128) {
        ssd1351_draw_column(0, 0, 0, Black, White);
    } else {
        ssd1351_draw_column(0, 0, 128-(uint32_t)(adc1_spectrum_abs[0]*1e3), Black, White);
    }

    for (i=1; i<128; i++) {
        if ((uint32_t)(adc1_spectrum_abs[i]*1e3) > 128) {
            ssd1351_draw_column(i, 0, 0, Black, White);
        } else {
            ssd1351_draw_column(i, 0, 128-(uint32_t)(adc1_spectrum_abs[i]*1e3), Black, White);
        }
    }
}

void disp_adc1_harmonic(void)
{
    extern double adc1_harmonic_rms[10];
    extern double adc1_harmonic_percent[10];
    extern uint16_t adc1_first_harmonic_index;

    int i;
    char str_disp[24] = {0};
    const int str_fore_color[10] = {Magenta, Lime, White, Lime, White, Lime, White, Lime, White, Lime};
    const int str_back_color[10] = {Black, Black, Black, Black, Black, Black, Black, Black, Black, Black};

    snprintf(str_disp, 24, "DC%9.1fmV%7.1f%%", round(adc1_harmonic_rms[0] * 1e7) / 1e1, round(adc1_harmonic_percent[0] * 1e1) / 1e1);
    ssd1351_display_string(0, 2, str_disp, FONT_1206, str_fore_color[0], str_back_color[0]);

    for (i=1; i<10; i++) {
        snprintf(str_disp, 24, "No.%d%7.1fmV%7.1f%%", i, round(adc1_harmonic_rms[i] * 1e7) / 1e1, round(adc1_harmonic_percent[i] * 1e1) / 1e1);
        ssd1351_display_string(0, 16+(i-1)*12, str_disp, FONT_1206, str_fore_color[i], str_back_color[i]);
    }
}

void disp_adc2_current(void)
{
//    extern double adc2_current_peak;
//    extern double adc2_current_ave;
    extern double adc2_current_rms;

    char str_rms[32] = {0};
//    ssd1351_display_num(16, 0, (long)(adc2_current_peak*1e3), 5, FONT_3216, Magenta, Black);
//    ssd1351_display_num(16, 32, (long)(adc2_current_ave*1e3), 5, FONT_3216, Lime, Black);
//    snprintf(str_rms, 32, "Current:%7.3f mV", round(adc2_current_rms * 1e6) / 1e3);
    snprintf(str_rms, 32, "Current:%9.3lf A", adc2_current_rms);
    ssd1351_display_string(0, 12, str_rms, FONT_1206, Lime, Black);
}

void disp_adc2_spectrum(void)
{
    extern double adc2_spectrum_abs[512];
    uint16_t i;

    if (adc2_spectrum_abs[0] < 0) {
        ssd1351_draw_column(0, 0, 128, Black, White);
    } else if ((uint32_t)(adc2_spectrum_abs[0]*1e3) > 128) {
        ssd1351_draw_column(0, 0, 0, Black, White);
    } else {
        ssd1351_draw_column(0, 0, 128-(uint32_t)(adc2_spectrum_abs[0]*1e3), Black, White);
    }

    for (i=1; i<128; i++) {
        if ((uint32_t)(adc2_spectrum_abs[i]*1e3) > 128) {
            ssd1351_draw_column(i, 0, 0, Black, White);
        } else {
            ssd1351_draw_column(i, 0, 128-(uint32_t)(adc2_spectrum_abs[i]*1e3), Black, White);
        }
    }
}

void disp_adc2_harmonic(void)
{
    extern double adc2_harmonic_rms[10];
    extern double adc2_harmonic_percent[10];
    extern uint16_t adc2_first_harmonic_index;

    int i;
    char str_disp[24] = {0};
    const int str_fore_color[10] = {Magenta, Lime, White, Lime, White, Lime, White, Lime, White, Lime};
    const int str_back_color[10] = {Black, Black, Black, Black, Black, Black, Black, Black, Black, Black};

    snprintf(str_disp, 24, "DC%9.1fmV%7.1f%%", round(adc2_harmonic_rms[0] * 1e7) / 1e1, round(adc2_harmonic_percent[0] * 1e1) / 1e1);
    ssd1351_display_string(0, 2, str_disp, FONT_1206, str_fore_color[0], str_back_color[0]);

    for (i=1; i<10; i++) {
        snprintf(str_disp, 24, "No.%d%7.1fmV%7.1f%%", i, round(adc2_harmonic_rms[i] * 1e7) / 1e1, round(adc2_harmonic_percent[i] * 1e1) / 1e1);
        ssd1351_display_string(0, 16+(i-1)*12, str_disp, FONT_1206, str_fore_color[i], str_back_color[i]);
    }
}

void disp_ecap1_frequency(void)
{
    extern uint32_t ecap1IntCount;
    extern double ecap1_freq;
    extern double ecap1_duty;

    char str_freq[32];
    char str_duty[32];

    if (ecap1_freq < 0.050) {
        // below 50 mHz
        snprintf(str_freq, 20, "< 50.0 mHz");
        // below 50 mHz
        snprintf(str_duty, 20, "--------");
    } else if (ecap1_freq < 0.1000) {
        // 50.000 mHz(0.002%) ~ 99.999 mHz(0.001%)
        snprintf(str_freq, 20, "%8.3lf mHz", round(ecap1_freq * 1e6) / 1e3);
        // 1.000 %(0.1%) ~ 99.999%(0.001%)
        snprintf(str_duty, 20, "%8.2lf %%", round(ecap1_duty * 1e2) / 1e2);
    } else if (ecap1_freq < 1.0000) {
        // 100.000 mHz(0.001%) ~ 999.999 mHz(0.0001%)
        snprintf(str_freq, 20, "%8.3lf mHz", round(ecap1_freq * 1e6) / 1e3);
        // 1.000 %(0.1%) ~ 99.999%(0.001%)
        snprintf(str_duty, 20, "%8.2lf %%", round(ecap1_duty * 1e2) / 1e2);
    } else if (ecap1_freq < 10.000) {
        // 1.000000 Hz(0.0001%) ~ 9.999999 Hz(0.00001%)
        snprintf(str_freq, 20, "%9.6lf Hz", round(ecap1_freq * 1e6) / 1e6);
        // 1.000 %(0.1%) ~ 99.999%(0.001%)
        snprintf(str_duty, 20, "%8.2lf %%", round(ecap1_duty * 1e2) / 1e2);
    } else if (ecap1_freq < 100.000) {
        // 10.0000 Hz(0.001%) ~ 99.9999 Hz(0.0001%)
        snprintf(str_freq, 20, "%9.5lf Hz", round(ecap1_freq * 1e5) / 1e5);
        // 1.000 %(0.1%) ~ 99.999%(0.001%)
        snprintf(str_duty, 20, "%8.2lf %%", round(ecap1_duty * 1e2) / 1e2);
    } else if (ecap1_freq < 1000.00) {
        // 100.00 Hz(0.01%) ~ 999.99 Hz(0.001%)
        snprintf(str_freq, 20, "%9.3lf Hz", round(ecap1_freq * 1e3) / 1e3);
        // 1.000 %(0.1%) ~ 99.999%(0.001%)
        snprintf(str_duty, 20, "%8.2lf %%", round(ecap1_duty * 1e2) / 1e2);
    } else if (ecap1_freq < 10000.0) {
        // 1.000 kHz(0.1%) ~ 9.999 kHz(0.01%)
        snprintf(str_freq, 20, "%8.3lf kHz", round(ecap1_freq) / 1e3);
        // 1.00 %(1%) ~ 99.99%(0.01%)
        snprintf(str_duty, 20, "%8.2lf %%", round(ecap1_duty * 1e2) / 1e2);
    } else {
        // above 1 MHz
        snprintf(str_freq, 20, "> 10.0 kHz");
        // above 1 MHz
        snprintf(str_duty, 20, "--------");
    }

    char str_temp[32];
    strcpy(str_temp, str_freq);
    //    ssd1351_display_num(0, 0, ecap1IntCount, 8, FONT_3216, Black, White);
    snprintf(str_freq, 32, "Freq:%15s", str_temp);
    ssd1351_display_string(0, 24, str_freq, FONT_1206, Lime, Black);
    strcpy(str_temp, str_duty);
    snprintf(str_duty, 32, "Duty:%14s", str_temp);
    ssd1351_display_string(0, 36, str_duty, FONT_1206, Yellow, Black);
}

void disp_ecap2_phase(void)
{
    extern uint32_t ecap1IntCount;
    extern uint32_t ecap2IntCount;
    extern double ecap2_phase_degree;

    char str_phase[20];

//    snprintf(str_phase, 32, "Phase:%7.2f deg", round(ecap2_phase_degree * 1e2) / 1e2);
    snprintf(str_phase, 32, "Phase:%11.2lf deg", round(ecap2_phase_degree * 1e2) / 1e2);
//    ssd1351_display_num(0, 0, ecap1IntCount, 8, FONT_3216, Black, White);
//    ssd1351_display_num(0, 48, ecap2IntCount, 8, FONT_3216, Black, Lime);
    ssd1351_display_string(0, 48, str_phase, FONT_1206, Yellow, Black);
}

void disp_eqep3_frequency(void)
{
    extern uint32_t eqep3IntCount;
    extern double eqep3_freq;

    char str_freq[20];

    if (eqep3_freq < 1.000) {
        // below 1 Hz
        snprintf(str_freq, 20, "  none  ");
    } else if (eqep3_freq < 10.0) {
        // 1 Hz(100%) ~ 9 Hz(10%)
        snprintf(str_freq, 20, "%8lu", (uint32_t)round(eqep3_freq));
    } else if (eqep3_freq < 100.0) {
        // 10 Hz(10%) ~ 99 Hz(1%)
        snprintf(str_freq, 20, "%8lu", (uint32_t)round(eqep3_freq));
    } else if (eqep3_freq < 1000.0) {
        // 100 Hz(1%) ~ 999 Hz(0.1%)
        snprintf(str_freq, 20, "%8lu", (uint32_t)round(eqep3_freq));
    } else if (eqep3_freq < 10000.0) {
        // 1.000 kHz(0.01%) ~ 9.999 kHz(0.001%)
        snprintf(str_freq, 20, "%4lu.%03lu", (uint32_t)floor(eqep3_freq * 1e-3), (uint32_t)(round(eqep3_freq) - floor(eqep3_freq * 1e-3) * 1e3));
    } else if (eqep3_freq < 100000.0) {
        // 10.000 kHz(0.001%) ~ 99.999 kHz(0.0001%)
        snprintf(str_freq, 20, "%4lu.%03lu", (uint32_t)floor(eqep3_freq * 1e-3), (uint32_t)(round(eqep3_freq) - floor(eqep3_freq * 1e-3) * 1e3));
    } else if (eqep3_freq < 1000000.0) {
        // 100.000 kHz(0.0001%) ~ 999.999 kHz(0.00001%)
        snprintf(str_freq, 20, "%4lu.%03lu", (uint32_t)floor(eqep3_freq * 1e-3), (uint32_t)(round(eqep3_freq) - floor(eqep3_freq * 1e-3) * 1e3));
    } else if (eqep3_freq < 10000000.0) {
        // 1.000000 MHz(0.00001%) ~ 9.999999 MHz(0.000001%)
        snprintf(str_freq, 20, "%1lu.%06lu", (uint32_t)floor(eqep3_freq * 1e-6), (uint32_t)(round(eqep3_freq) - floor(eqep3_freq * 1e-6) * 1e6));
    } else if (eqep3_freq < 100000000.0) {
        // 10.000000 MHz(0.000001%) ~ 99.999999 MHz(0.0000001%)
        snprintf(str_freq, 20, "%2lu.%05lu", (uint32_t)floor(eqep3_freq * 1e-6), (uint32_t)((round(eqep3_freq) - floor(eqep3_freq * 1e-6) * 1e6) * 1e-1));
    } else {
        // above 100 MHz
        snprintf(str_freq, 20, "  none  ");
    }

    ssd1351_display_num(0, 0, eqep3IntCount, 8, FONT_3216, Black, White);
    ssd1351_display_string(0, 96, str_freq, FONT_3216, Black, Lime);
}

void disp_adc1_2_power(void)
{
    extern double adc1_2_apparent_power;
    extern double adc1_2_active_power;
    extern double adc1_2_reactive_power;
    extern double adc1_2_power_factor;

    char str_factor[20];

    snprintf(str_factor, 32, "PF:%14.3lf", round(adc1_2_power_factor * 1e3) / 1e3);

//    ssd1351_display_num(0, 0, ecap1IntCount, 8, FONT_3216, Black, White);
//    ssd1351_display_num(0, 48, ecap2IntCount, 8, FONT_3216, Black, Lime);
    ssd1351_display_string(0, 60, str_factor, FONT_1206, Yellow, Black);
}

void disp_status(void)
{
    extern elec_param_t elec_param[8];
    char str_temp[24] = {0};
    uint16_t i, j;

    i = 8;
    for (j=1; j<5; j++) {
        if (elec_param[j].status != 0) {
            snprintf(str_temp, 32, "%d", j);
            ssd1351_display_string(i, 84, str_temp, FONT_1616, Black, Lime);
        } else {
            snprintf(str_temp, 32, "%d", j);
            ssd1351_display_string(i, 84, str_temp, FONT_1616, Black, Red);
        }
        i += 32;
    }

    i = 24;
    for (j=5; j<8; j++) {
        if (elec_param[j].status != 0) {
            snprintf(str_temp, 32, "%d", j);
            ssd1351_display_string(i, 112, str_temp, FONT_1616, Black, Lime);
        } else {
            snprintf(str_temp, 32, "%d", j);
            ssd1351_display_string(i, 112, str_temp, FONT_1616, Black, Red);
        }
        i += 32;
    }
}

void disp_time(void)
{
    extern uint16_t cpuTimer0IntCount;
    char str_time[24] = {0};

    snprintf(str_time, 20, "%02u:%02u:%02u", (cpuTimer0IntCount/60/60)%24, (cpuTimer0IntCount/60)%60, cpuTimer0IntCount%60);
    ssd1351_display_string(0, 112, str_time, FONT_1616, Lime, Black);
}

void disp_menu(void)
{
    extern const uint16_t menu_item_num;

    extern const char menu_item_string[][24];
    extern const int  menu_item_fore_color[];
    extern const int  menu_item_back_color[];

    extern uint16_t menu_item;
    extern uint16_t menu_level;

    char str_menu_item[24] = {0};

    uint16_t i;

    for (i=0; i<menu_item_num; i++) {
        snprintf(str_menu_item, 17, "%2u.%s", i+1, menu_item_string[i]);
        if (i == menu_item) {
            ssd1351_display_string(0, i*16, str_menu_item, FONT_1608, menu_item_back_color[i], menu_item_fore_color[i]);
        } else {
            ssd1351_display_string(0, i*16, str_menu_item, FONT_1608, menu_item_fore_color[i], menu_item_back_color[i]);
        }
    }
}

void disp_refresh(void)
{
    extern uint16_t menu_item;
    extern uint16_t menu_level;
    extern uint16_t menu_refresh;

    if (disp_refresh_flag == 1) {
        disp_refresh_flag = 0;
        ssd1351_clear_gram();
        printf("menu_level : %u menu_item : %u\n", menu_level, menu_item);
        SCI_writeCharArray(SCIB_BASE, (uint16_t *)"01234567890\r\n", sizeof("01234567890\r\n")-1);
    }
}
