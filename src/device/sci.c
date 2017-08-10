/*
 * sci.c
 *
 *  Created on: 2017-7-30
 *      Author: redchenjs
 */
#include "driverlib.h"
#include "device.h"

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <file.h>

#include "F2837xD_device.h"
#include "F2837xD_sci_io.h"

volatile int scia_status = 0;
volatile FILE *scia_fid;

typedef struct sci_buff_def {
    uint16_t data;
    struct sci_buff_def *next;
} sci_buff_t;

//
// Send data for SCI-A
//
uint16_t sci2_tx_buff[64];
uint16_t sci2_tx_num = 0;
//
// Received data for SCI-A
//
#define RX_BUFF_NUM 64
sci_buff_t sci2_rx_buff[RX_BUFF_NUM];
sci_buff_t *sci2_rx_read_ptr;
sci_buff_t *sci2_rx_write_ptr;
uint16_t sci2_rx_num = 0;
//
// Function Prototypes
//
__interrupt void sci2_tx_fifo_isr(void);
__interrupt void sci2_rx_fifo_isr(void);

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

void sci2_init(void)
{
    //
    // GPIO19 is the SCI Rx pin.
    //
    GPIO_setMasterCore(19, GPIO_CORE_CPU1);
    GPIO_setPinConfig(GPIO_19_SCIRXDB);
    GPIO_setDirectionMode(19, GPIO_DIR_MODE_IN);
    GPIO_setPadConfig(19, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(19, GPIO_QUAL_ASYNC);

    //
    // GPIO18 is the SCI Tx pin.
    //
    GPIO_setMasterCore(18, GPIO_CORE_CPU1);
    GPIO_setPinConfig(GPIO_18_SCITXDB);
    GPIO_setDirectionMode(18, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(18, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(18, GPIO_QUAL_ASYNC);

    Interrupt_register(INT_SCIB_RX, sci2_rx_fifo_isr);
//    Interrupt_register(INT_SCIB_TX, sci2_tx_fifo_isr);

    //
    // 8 char bits, 1 stop bit, no parity. Baud rate is 115200.
    //
    SCI_setConfig(SCIB_BASE, DEVICE_LSPCLK_FREQ, 115200, (SCI_CONFIG_WLEN_8 |
                                                        SCI_CONFIG_STOP_ONE |
                                                        SCI_CONFIG_PAR_NONE));
    SCI_enableModule(SCIB_BASE);
    SCI_resetChannels(SCIB_BASE);
    SCI_enableFIFO(SCIB_BASE);

    //
    // RX and TX FIFO Interrupts Enabled
    //
    SCI_enableInterrupt(SCIB_BASE, SCI_INT_RXFF);
    SCI_disableInterrupt(SCIB_BASE, SCI_INT_RXERR);

    SCI_setFIFOInterruptLevel(SCIB_BASE, SCI_FIFO_TX0, SCI_FIFO_RX1);
    SCI_performSoftwareReset(SCIB_BASE);

    SCI_resetTxFIFO(SCIB_BASE);
    SCI_resetRxFIFO(SCIB_BASE);

    Interrupt_enable(INT_SCIB_RX);
//    Interrupt_enable(INT_SCIB_TX);

    uint16_t i;
    memset(sci2_rx_buff, 0x00, sizeof(sci2_rx_buff));
    for (i=0; i<RX_BUFF_NUM-1;i++) {
        sci2_rx_buff[i].next = &sci2_rx_buff[i+1];
    }
    sci2_rx_buff[RX_BUFF_NUM-1].next = &sci2_rx_buff[0];
    sci2_rx_read_ptr = &sci2_rx_buff[0];
    sci2_rx_write_ptr = &sci2_rx_buff[0];
    sci2_rx_num = 0;

    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);
}

__interrupt void sci2_tx_fifo_isr(void)
{
    SCI_clearInterruptStatus(SCIB_BASE, SCI_INT_TXFF);

    //
    // Issue PIE ACK
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);
}

__interrupt void sci2_rx_fifo_isr(void)
{
    sci2_rx_write_ptr->data = SCI_readCharBlockingFIFO(SCIB_BASE);
    sci2_rx_write_ptr = sci2_rx_write_ptr->next;
    sci2_rx_num++;

    if (sci2_rx_num >= RX_BUFF_NUM) {
        sci2_rx_num = 0;
    }

    SCI_clearOverflowStatus(SCIB_BASE);

    SCI_clearInterruptStatus(SCIB_BASE, SCI_INT_RXFF);

    //
    // Issue PIE ack
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);
}
