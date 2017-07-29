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
#include <stdlib.h>

#include "inc/system/fonts.h"
#include "inc/driver/ssd1351.h"

uint16_t disp_refresh_flag = 0;

void disp_adc1_voltage(void)
{
    extern double adc1_voltage_peak;
    extern double adc1_voltage_ave;
    extern double adc1_voltage_rms;

    ssd1351_display_num(16, 0, (long)(adc1_voltage_peak*1e3), 5, FONT_3216, Magenta, Black);
    ssd1351_display_num(16, 32, (long)(adc1_voltage_ave*1e3), 5, FONT_3216, Lime, Black);
    ssd1351_display_num(16, 64, (long)(adc1_voltage_rms*1e3), 5, FONT_3216, Yellow, Black);
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

    snprintf(str_disp, 24, "DC%7ld.%01lumV%5lu.%01lu%%", (int32_t)floor(adc1_harmonic_rms[0] * 1e6), (uint32_t)(floor(adc1_harmonic_rms[0] * 1e7 - floor(adc1_harmonic_rms[0] * 1e6) * 1e1)), (uint32_t)floor(adc1_harmonic_percent[0]), (uint32_t)(floor(adc1_harmonic_percent[0] * 1e1 - floor(adc1_harmonic_percent[0]) * 1e1)));
    ssd1351_display_string(0, 2, str_disp, FONT_1206, str_fore_color[0], str_back_color[0]);

    for (i=1; i<10; i++) {
        snprintf(str_disp, 24, "No.%d%5lu.%01lumV%5lu.%01lu%%", i, (uint32_t)floor(adc1_harmonic_rms[i] * 1e6), (uint32_t)(floor(adc1_harmonic_rms[i] * 1e7 - floor(adc1_harmonic_rms[i] * 1e6) * 1e1)), (uint32_t)floor(adc1_harmonic_percent[i]), (uint32_t)(floor(adc1_harmonic_percent[i] * 1e1 - floor(adc1_harmonic_percent[i]) * 1e1)));
        ssd1351_display_string(0, 16+(i-1)*12, str_disp, FONT_1206, str_fore_color[i], str_back_color[i]);
    }
}

void disp_ecap1_frequency(void)
{
    extern uint32_t ecap1IntCount;
    extern double ecap1_freq;
    extern double ecap1_duty;

    char str_freq[20];
    char str_duty[20];

    if (ecap1_freq < 0.050) {
        // below 50 mHz
        snprintf(str_freq, 20, "  <50.0 mHz ");
        // below 50 mHz
        snprintf(str_duty, 20, "----------");
    } else if (ecap1_freq < 0.1000) {
        // 50.000 mHz(0.002%) ~ 99.999 mHz(0.001%)
        snprintf(str_freq, 20, "%8.3f mHz", round(ecap1_freq * 1e6) / 1e3);
        // 1.000 %(0.1%) ~ 99.999%(0.001%)
        snprintf(str_duty, 20, "%8.2f %%", round(ecap1_duty * 1e2) / 1e2);
    } else if (ecap1_freq < 1.0000) {
        // 100.000 mHz(0.001%) ~ 999.999 mHz(0.0001%)
        snprintf(str_freq, 20, "%8.3f mHz", round(ecap1_freq * 1e6) / 1e3);
        // 1.000 %(0.1%) ~ 99.999%(0.001%)
        snprintf(str_duty, 20, "%8.2f %%", round(ecap1_duty * 1e2) / 1e2);
    } else if (ecap1_freq < 10.000) {
        // 1.000000 Hz(0.0001%) ~ 9.999999 Hz(0.00001%)
        snprintf(str_freq, 20, "%9.6f Hz", round(ecap1_freq * 1e6) / 1e6);
        // 1.000 %(0.1%) ~ 99.999%(0.001%)
        snprintf(str_duty, 20, "%8.2f %%", round(ecap1_duty * 1e2) / 1e2);
    } else if (ecap1_freq < 100.000) {
        // 10.0000 Hz(0.001%) ~ 99.9999 Hz(0.0001%)
        snprintf(str_freq, 20, "%9.5f Hz", round(ecap1_freq * 1e5) / 1e5);
        // 1.000 %(0.1%) ~ 99.999%(0.001%)
        snprintf(str_duty, 20, "%8.2f %%", round(ecap1_duty * 1e2) / 1e2);
    } else if (ecap1_freq < 1000.00) {
        // 100.00 Hz(0.01%) ~ 999.99 Hz(0.001%)
        snprintf(str_freq, 20, "%9.3f Hz", round(ecap1_freq * 1e3) / 1e3);
        // 1.000 %(0.1%) ~ 99.999%(0.001%)
        snprintf(str_duty, 20, "%8.2f %%", round(ecap1_duty * 1e2) / 1e2);
    } else if (ecap1_freq < 10000.0) {
        // 1.000 kHz(0.1%) ~ 9.999 kHz(0.01%)
        snprintf(str_freq, 20, "%8.3f kHz", round(ecap1_freq) / 1e3);
        // 1.00 %(1%) ~ 99.99%(0.01%)
        snprintf(str_duty, 20, "%8.2f %%", round(ecap1_duty * 1e2) / 1e2);
    } else {
        // above 1 MHz
        snprintf(str_freq, 20, "  >10.0 kHz ");
        // above 1 MHz
        snprintf(str_duty, 20, "----------");
    }

    ssd1351_display_num(0, 0, ecap1IntCount, 8, FONT_3216, Black, White);
    ssd1351_display_string(0, 48, str_freq, FONT_1608, Lime, Black);
    ssd1351_display_string(0, 96, str_duty, FONT_1608, Yellow, Black);
}

void disp_ecap2_phase(void)
{
    extern uint32_t ecap1IntCount;
    extern uint32_t ecap2IntCount;
    extern double ecap2_phase_degree;

    char str_phase[20];

    snprintf(str_phase, 20, "%7.2f deg", round(ecap2_phase_degree * 1e2) / 1e2);

    ssd1351_display_num(0, 0, ecap1IntCount, 8, FONT_3216, Black, White);
    ssd1351_display_num(0, 48, ecap2IntCount, 8, FONT_3216, Black, Lime);
    ssd1351_display_string(0, 96, str_phase, FONT_1608, Yellow, Black);
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
    ssd1351_display_string(0, 48, str_freq, FONT_3216, Black, Lime);
}

void disp_time(void)
{
    extern uint16_t cpuTimer0IntCount;
    char str_time[24] = {0};

    snprintf(str_time, 20, "%02u:%02u:%02u", (cpuTimer0IntCount/60/60)%24, (cpuTimer0IntCount/60)%60, cpuTimer0IntCount%60);
    ssd1351_display_string(0, 96, str_time, FONT_3216, Lime, Black);
}

void disp_menu(void)
{
    extern const uint16_t menu_item_num;

    extern const char menu_item_string[][24];
    extern const int  menu_item_fore_color[];
    extern const int  menu_item_back_color[];

    extern uint16_t menu_item;;
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
    extern uint16_t menu_refresh;

    if (disp_refresh_flag == 1) {
        disp_refresh_flag = 0;
        ssd1351_clear_gram();
    }
}
