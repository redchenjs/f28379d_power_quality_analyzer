/*
 * sci.c
 *
 *  Created on: 2017-7-30
 *      Author: redchenjs
 */
#include "driverlib.h"
#include "device.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <file.h>

#include "F2837xD_device.h"
#include "F2837xD_sci_io.h"

volatile int scia_status = 0;
volatile FILE *scia_fid;

void sci1_init(void)
{
    EALLOW;
    GpioCtrlRegs.GPBMUX1.bit.GPIO42 = 3;
    GpioCtrlRegs.GPBMUX1.bit.GPIO43 = 3;
    GpioCtrlRegs.GPBGMUX1.bit.GPIO42 = 3;
    GpioCtrlRegs.GPBGMUX1.bit.GPIO43 = 3;
    EDIS;

    //
    // Redirect STDOUT to SCI
    //
    scia_status = add_device("scia", _SSA, SCI_open, SCI_close, SCI_read, SCI_write,
                        SCI_lseek, SCI_unlink, SCI_rename);
    scia_fid = fopen("scia","w");
    freopen("scia:", "w", stdout);
    setvbuf(stdout, NULL, _IONBF, 0);
}
