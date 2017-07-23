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

#include <inc/system/fonts.h>
#include <inc/driver/ssd1351.h>

#define MENU_ITEM_NUM   4

uint16_t disp_menu_index  = 0;
uint16_t disp_menu_level = 0;
uint16_t disp_menu_refresh = 0;

void disp_adc1_vrms(void)
{

}

void disp_adc1_spectrum(void)
{
    extern double adc1_spectrum_amp[128];
    int i;

    ssd1351_display_num(80, 0, 0, 3, FONT_1616, Yellow, Blue);

    for (i=0; i<80; i++) {
        if ((uint32_t)(adc1_spectrum_amp[i]*1e3) > 128) {
            ssd1351_draw_column(i, 0, 128, Black, White);
        } else {
            ssd1351_draw_column(i, 0, 128-(uint32_t)(adc1_spectrum_amp[i]*1e3), Black, White);
        }
    }
    for (i=80; i<128; i++) {
        if ((uint32_t)(adc1_spectrum_amp[i]*1e3) > 128-16) {
            ssd1351_draw_column(i, 16, 128-16, Black, White);
        } else {
            ssd1351_draw_column(i, 16, 128-(uint32_t)(adc1_spectrum_amp[i]*1e3)-16, Black, White);
        }
    }
}

void disp_adc1_harmonic(void)
{
    extern double adc1_harmonic_amp[10];
    extern double adc1_harmonic_percent[10];
    extern uint16_t adc1_first_harmonic_index;

    int i;
    char str_disp[10][24] = {0};
    const int str_fore_color[10] = {Magenta, White, White, White, White, White, White, White, White, White};
    const int str_back_color[10] = {Black, Black, Black, Black, Black, Black, Black, Black, Black, Black};

    snprintf(str_disp[0], 24, "DC%7ld.%01lumV%5lu.%01lu%%", (int32_t)floor(adc1_harmonic_amp[0] * 1e3), (uint32_t)(floor(adc1_harmonic_amp[0] * 1e4 - floor(adc1_harmonic_amp[0] * 1e3) * 1e1)), (uint32_t)floor(adc1_harmonic_percent[0]), (uint32_t)(floor(adc1_harmonic_percent[0] * 1e1 - floor(adc1_harmonic_percent[0]) * 1e1)));
    ssd1351_display_string(0, 2, str_disp[0], FONT_1206, str_fore_color[0], str_back_color[0]);

    for (i=1; i<10; i++) {
        snprintf(str_disp[i], 24, "No.%d%5lu.%01lumV%5lu.%01lu%%", i, (uint32_t)floor(adc1_harmonic_amp[i] * 1e3), (uint32_t)(floor(adc1_harmonic_amp[i] * 1e4 - floor(adc1_harmonic_amp[i] * 1e3) * 1e1)), (uint32_t)floor(adc1_harmonic_percent[i]), (uint32_t)(floor(adc1_harmonic_percent[i] * 1e1 - floor(adc1_harmonic_percent[i]) * 1e1)));
        ssd1351_display_string(0, 16+(i-1)*12, str_disp[i], FONT_1206, str_fore_color[i], str_back_color[i]);
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
        snprintf(str_freq, 20, "  none  ");
        // below 50 mHz
        snprintf(str_duty, 20, "  none  ");
    } else if (ecap1_freq < 0.1000) {
        // 50.000 mHz(0.002%) ~ 99.999 mHz(0.001%)
        snprintf(str_freq, 20, "%4lu.%03lu", (uint32_t)floor(ecap1_freq * 1e3), (uint32_t)(floor(ecap1_freq * 1e6) - floor(ecap1_freq * 1e3) * 1e3));
        // 1.000 %(0.1%) ~ 99.999%(0.001%)
        snprintf(str_duty, 20, "%4lu.%03lu", (uint32_t)floor(ecap1_duty), (uint32_t)(floor(ecap1_duty * 1e3) - floor(ecap1_duty) * 1e3));
    } else if (ecap1_freq < 1.0000) {
        // 100.000 mHz(0.001%) ~ 999.999 mHz(0.0001%)
        snprintf(str_freq, 20, "%4lu.%03lu", (uint32_t)floor(ecap1_freq * 1e3), (uint32_t)(floor(ecap1_freq * 1e6) - floor(ecap1_freq * 1e3) * 1e3));
        // 1.000 %(0.1%) ~ 99.999%(0.001%)
        snprintf(str_duty, 20, "%4lu.%03lu", (uint32_t)floor(ecap1_duty), (uint32_t)(floor(ecap1_duty * 1e3) - floor(ecap1_duty) * 1e3));
    } else if (ecap1_freq < 10.000) {
        // 1.000000 Hz(0.0001%) ~ 9.999999 Hz(0.00001%)
        snprintf(str_freq, 20, "%1lu.%06lu", (uint32_t)floor(ecap1_freq), (uint32_t)(floor(ecap1_freq * 1e6) - floor(ecap1_freq) * 1e6));
        // 1.000 %(0.1%) ~ 99.999%(0.001%)
        snprintf(str_duty, 20, "%4lu.%03lu", (uint32_t)floor(ecap1_duty), (uint32_t)(floor(ecap1_duty * 1e3) - floor(ecap1_duty) * 1e3));
    } else if (ecap1_freq < 100.000) {
        // 10.0000 Hz(0.001%) ~ 99.9999 Hz(0.0001%)
        snprintf(str_freq, 20, "%3lu.%04lu", (uint32_t)floor(ecap1_freq), (uint32_t)(floor(ecap1_freq * 1e4) - floor(ecap1_freq) * 1e4));
        // 1.000 %(0.1%) ~ 99.999%(0.001%)
        snprintf(str_duty, 20, "%4lu.%03lu", (uint32_t)floor(ecap1_duty), (uint32_t)(floor(ecap1_duty * 1e3) - floor(ecap1_duty) * 1e3));
    } else if (ecap1_freq < 1000.00) {
        // 100.00 Hz(0.01%) ~ 999.99 Hz(0.001%)
        snprintf(str_freq, 20, "%5lu.%02lu", (uint32_t)floor(ecap1_freq), (uint32_t)(floor(ecap1_freq * 1e2) - floor(ecap1_freq) * 1e2));
        // 1.000 %(0.1%) ~ 99.999%(0.001%)
        snprintf(str_duty, 20, "%4lu.%03lu", (uint32_t)floor(ecap1_duty), (uint32_t)(floor(ecap1_duty * 1e3) - floor(ecap1_duty) * 1e3));
    } else if (ecap1_freq < 10000.0) {
        // 1.000 kHz(0.1%) ~ 9.999 kHz(0.01%)
        snprintf(str_freq, 20, "%4lu.%03lu", (uint32_t)floor(ecap1_freq * 1e-3), (uint32_t)(floor(ecap1_freq) - floor(ecap1_freq * 1e-3) * 1e3));
        // 1.00 %(1%) ~ 99.99%(0.01%)
        snprintf(str_duty, 20, "%5lu.%02lu", (uint32_t)floor(ecap1_duty), (uint32_t)(floor(ecap1_duty * 1e2) - floor(ecap1_duty) * 1e2));
    } else if (ecap1_freq < 100000.0) {
        // 10.0 kHz(1%) ~ 99.9 kHz(0.1%)
        snprintf(str_freq, 20, "%6lu.%01lu", (uint32_t)floor(ecap1_freq * 1e-3), (uint32_t)(floor(ecap1_freq * 1e-2) - floor(ecap1_freq * 1e-3) * 1e1));
        // 1.0 %(10%) ~ 99.9 %(0.1%)
        snprintf(str_duty, 20, "%6lu.%01lu", (uint32_t)floor(ecap1_duty), (uint32_t)(floor(ecap1_duty * 1e1) - floor(ecap1_duty) * 1e1));
    } else if (ecap1_freq < 1000000.0) {
        // 10x kHz(10%) ~ 99x kHz(1%)
        snprintf(str_freq, 20, "%8lu", (uint32_t)floor(ecap1_freq * 1e-3));
        // 1 %(100%) ~ 99 %(1%)
        snprintf(str_duty, 20, "%8lu", (uint32_t)floor(ecap1_duty));
    } else if (ecap1_freq < 5000000.0) {
        // 1.0 MHz(10%) ~ 5.0 MHz(2%)
        snprintf(str_freq, 20, "%6lu.%01lu", (uint32_t)floor(ecap1_freq * 1e-6), (uint32_t)(floor(ecap1_freq * 1e-5) - floor(ecap1_freq * 1e-6) * 1e1));
        // 1x %(100%) ~ 9x %(10%)
        snprintf(str_duty, 20, "%8lu", (uint32_t)floor(ecap1_duty));
    } else {
        // above 5 MHz
        snprintf(str_freq, 20, "  none  ");
        // above 5 MHz
        snprintf(str_duty, 20, "  none  ");
    }

    ssd1351_display_num(0, 0, ecap1IntCount, 8, FONT_3216, Black, White);
    ssd1351_display_string(0, 48, str_freq, FONT_3216, Black, Lime);
    ssd1351_display_string(0, 96, str_duty, FONT_3216, Black, Yellow);
}

void disp_ecap2_phase(void)
{
    extern uint32_t ecap1IntCount;
    extern uint32_t ecap2IntCount;
    extern double ecap2_phase;

    char str_phase[20];

    if (ecap2_phase > 0.0) {
        snprintf(str_phase, 20, "%8lu", (uint32_t)(ecap2_phase * 1e6));
    } else {
        snprintf(str_phase, 20, "-%7lu", (uint32_t)(ecap2_phase * 1e6));
    }

    ssd1351_display_num(0, 0, ecap1IntCount, 8, FONT_3216, Black, White);
    ssd1351_display_num(0, 48, ecap2IntCount, 8, FONT_3216, Black, Lime);
    ssd1351_display_string(0, 96, str_phase, FONT_3216, Black, Yellow);
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
        snprintf(str_freq, 20, "%8lu", (uint32_t)floor(eqep3_freq));
    } else if (eqep3_freq < 100.0) {
        // 10 Hz(10%) ~ 99 Hz(1%)
        snprintf(str_freq, 20, "%8lu", (uint32_t)floor(eqep3_freq));
    } else if (eqep3_freq < 1000.0) {
        // 100 Hz(1%) ~ 999 Hz(0.1%)
        snprintf(str_freq, 20, "%8lu", (uint32_t)floor(eqep3_freq));
    } else if (eqep3_freq < 10000.0) {
        // 1.000 kHz(0.01%) ~ 9.999 kHz(0.001%)
        snprintf(str_freq, 20, "%4lu.%03lu", (uint32_t)floor(eqep3_freq * 1e-3), (uint32_t)(floor(eqep3_freq) - floor(eqep3_freq * 1e-3) * 1e3));
    } else if (eqep3_freq < 100000.0) {
        // 10.000 kHz(0.001%) ~ 99.999 kHz(0.0001%)
        snprintf(str_freq, 20, "%4lu.%03lu", (uint32_t)floor(eqep3_freq * 1e-3), (uint32_t)(floor(eqep3_freq) - floor(eqep3_freq * 1e-3) * 1e3));
    } else if (eqep3_freq < 1000000.0) {
        // 100.000 kHz(0.0001%) ~ 999.999 kHz(0.00001%)
        snprintf(str_freq, 20, "%4lu.%03lu", (uint32_t)floor(eqep3_freq * 1e-3), (uint32_t)(floor(eqep3_freq) - floor(eqep3_freq * 1e-3) * 1e3));
    } else if (eqep3_freq < 10000000.0) {
        // 1.000000 MHz(0.00001%) ~ 9.999999 MHz(0.000001%)
        snprintf(str_freq, 20, "%1lu.%06lu", (uint32_t)floor(eqep3_freq * 1e-6), (uint32_t)(floor(eqep3_freq) - floor(eqep3_freq * 1e-6) * 1e6));
    } else if (eqep3_freq < 100000000.0) {
        // 10.000000 MHz(0.000001%) ~ 99.999999 MHz(0.0000001%)
        snprintf(str_freq, 20, "%2lu.%05lu", (uint32_t)floor(eqep3_freq * 1e-6), (uint32_t)((floor(eqep3_freq) - floor(eqep3_freq * 1e-6) * 1e6) * 1e-1));
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
    extern uint32_t eqep1_position;
    extern uint32_t eqep2_position;

    enum menu_item {
        MENU_ITEM_VOLTAGE   = 0x0,
        MENU_ITEM_SPECTRUM  = 0x1,
        MENU_ITEM_HARMONIC  = 0x2,
        MENU_ITEM_FREQUENCY = 0x3
    };

    const char str_menu_id[MENU_ITEM_NUM][24] = {
                                       "Voltage      ",
                                       "Spectrum     ",
                                       "Harmonic     ",
                                       "Frequency    "
    };

    const int str_fore_color[MENU_ITEM_NUM] = {White, White, White, White};
    const int str_back_color[MENU_ITEM_NUM] = {Black, Black, Black, Black};

    char str_menu_item[24] = {0};

    uint16_t i;

    if (disp_menu_refresh == 1) {
        disp_menu_refresh = 0;
        ssd1351_clear_gram();
    }

    switch (disp_menu_level) {
        case 0:
            disp_menu_index = (uint16_t)eqep1_position % MENU_ITEM_NUM;
            for (i=0; i<MENU_ITEM_NUM; i++) {
                snprintf(str_menu_item, 17, "%2u.%s", i+1, str_menu_id[i]);
                if (i == disp_menu_index) {
                    ssd1351_display_string(0, i*16, str_menu_item, FONT_1608, str_back_color[i], str_fore_color[i]);
                } else {
                    ssd1351_display_string(0, i*16, str_menu_item, FONT_1608, str_fore_color[i], str_back_color[i]);
                }
            }
            disp_time();
            break;
        case 1:
            switch (disp_menu_index) {
                case MENU_ITEM_VOLTAGE:
                    disp_adc1_vrms();
                    break;
                case MENU_ITEM_SPECTRUM:
                    disp_adc1_spectrum();
                    break;
                case MENU_ITEM_HARMONIC:
                    disp_adc1_harmonic();
                    break;
                case MENU_ITEM_FREQUENCY:
                    disp_ecap1_frequency();
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
    if (disp_menu_level == 0) {
        disp_menu_level = 1;
        disp_menu_refresh = 1;
    }

    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}

interrupt void xint2_isr(void)
{
    if (disp_menu_level == 1) {
        disp_menu_level = 0;
        disp_menu_refresh = 1;
    }

    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}

void task_disp(void)
{
//    disp_adc1_fft();
//    disp_adc1_harmonic();
    disp_menu();
}
