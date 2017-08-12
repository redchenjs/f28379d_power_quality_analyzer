/*
 * task_sci.c
 *
 *  Created on: 2017年8月10日
 *      Author: redchenjs
 */
#include <driverlib.h>
#include <device.h>

#include <stdio.h>
#include <string.h>

#include "inc/tasks/task_status.h"

extern power_param_t power_param;
extern harmonic_param_t harmonic_param;
extern elec_param_t elec_param[8];

void sci2_transmit_data(void)
{
    char str_temp[1024];

    memset(str_temp, 0x00, sizeof(str_temp));
    snprintf(str_temp, 1024, "A=%16.12lfB=%16.12lfC=%16.10lfD=%16.12lfE=%16.12lfF=%16.10lfG=%16.10lfH=%16.10lfI=%16.14lfJ=%uK=%uL=%uM=%uN=%uO=%uP=%u",\
                             power_param.voltage,\
                             power_param.current,\
                             power_param.phase,\
                             power_param.frequency,\
                             power_param.duty,\
                             power_param.apparent_power,\
                             power_param.active_power,\
                             power_param.reactive_power,\
                             power_param.power_factor,\
                             elec_param[1].status,\
                             elec_param[2].status,\
                             elec_param[3].status,\
                             elec_param[4].status,\
                             elec_param[5].status,\
                             elec_param[6].status,\
                             elec_param[7].status\
                             );
    SCI_writeCharArray(SCIB_BASE, (uint16_t *)"\xff", 1);
    SCI_writeCharArray(SCIB_BASE, (uint16_t *)str_temp, strlen(str_temp));
    SCI_writeCharArray(SCIB_BASE, (uint16_t *)"\xfe", 1);
//    power_param.voltage += 1;
//
//    memset(str_temp, 0x00, sizeof(str_temp));
//    snprintf(str_temp, 32, "", );
//    SCI_writeCharArray(SCIB_BASE, (uint16_t *)str_temp, sizeof(str_temp) / 2);
//    power_param.current += 1;
//
//    memset(str_temp, 0x00, sizeof(str_temp));
//    snprintf(str_temp, 32, "Phase=%lf", power_param.phase);
//    SCI_writeCharArray(SCIB_BASE, (uint16_t *)str_temp, sizeof(str_temp) / 2);
//    power_param.phase += 1;
//
//    memset(str_temp, 0x00, sizeof(str_temp));
//    snprintf(str_temp, 32, "", power_param.current);
//    SCI_writeCharArray(SCIB_BASE, (uint16_t *)str_temp, sizeof(str_temp) / 2);
//    power_param.frequency += 1;
//
//    memset(str_temp, 0x00, sizeof(str_temp));
//    snprintf(str_temp, 32, "", power_param.apparent_power);
//    SCI_writeCharArray(SCIB_BASE, (uint16_t *)str_temp, sizeof(str_temp) / 2);
//    power_param.apparent_power += 1;
//
//    memset(str_temp, 0x00, sizeof(str_temp));
//    snprintf(str_temp, 32, "", power_param.active_power);
//    SCI_writeCharArray(SCIB_BASE, (uint16_t *)str_temp, sizeof(str_temp) / 2);
//    power_param.active_power += 1;
//
//    snprintf(str_temp, 32, "", power_param.reactive_power);
//    SCI_writeCharArray(SCIB_BASE, (uint16_t *)str_temp, sizeof(str_temp) / 2);
//    power_param.reactive_power += 1;
//
//    snprintf(str_temp, 32, "", power_param.power_factor);
//    SCI_writeCharArray(SCIB_BASE, (uint16_t *)str_temp, sizeof(str_temp) / 2);
//    power_param.power_factor += 1;
//
//    snprintf(str_temp, 32, "", elec_param[0].status);
//    SCI_writeCharArray(SCIB_BASE, (uint16_t *)str_temp, sizeof(str_temp) / 2);
//    elec_param[0].status += 1;
//
//    snprintf(str_temp, 32, "Status 1=%u", elec_param[1].status);
//    SCI_writeCharArray(SCIB_BASE, (uint16_t *)str_temp, sizeof(str_temp) / 2);
//    elec_param[1].status += 1;
//
//    snprintf(str_temp, 32, "Status 2=%u", elec_param[2].status);
//    SCI_writeCharArray(SCIB_BASE, (uint16_t *)str_temp, sizeof(str_temp) / 2);
//    elec_param[2].status += 1;
//
//    snprintf(str_temp, 32, "Status 3=%u", elec_param[3].status);
//    SCI_writeCharArray(SCIB_BASE, (uint16_t *)str_temp, sizeof(str_temp) / 2);
//    elec_param[3].status += 1;
//
//    snprintf(str_temp, 32, "Status 4=%u", elec_param[4].status);
//    SCI_writeCharArray(SCIB_BASE, (uint16_t *)str_temp, sizeof(str_temp) / 2);
//    elec_param[4].status += 1;
//
//    snprintf(str_temp, 32, "Status 5=%u", elec_param[5].status);
//    SCI_writeCharArray(SCIB_BASE, (uint16_t *)str_temp, sizeof(str_temp) / 2);
//    elec_param[5].status += 1;
//
//    snprintf(str_temp, 32, "Status 6=%u", elec_param[6].status);
//    SCI_writeCharArray(SCIB_BASE, (uint16_t *)str_temp, sizeof(str_temp) / 2);
//    elec_param[6].status += 1;
//
//    snprintf(str_temp, 32, "Status 7=%u", elec_param[7].status);
//    SCI_writeCharArray(SCIB_BASE, (uint16_t *)str_temp, sizeof(str_temp) / 2);
//    elec_param[7].status += 1;

//    DEVICE_DELAY_US(1000000);
}
