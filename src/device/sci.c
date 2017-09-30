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

uint16_t scia_device_open = 0;

volatile int scia_status = 0;
volatile FILE *scia_fid;

int scia_open(const char * path, unsigned int flags, int llv_fd)
{
    if (scia_device_open) {
        return (-1);
    } else {
        EALLOW;
        CpuSysRegs.PCLKCR7.bit.SCI_A = 1;
        SciaRegs.SCIFFTX.all=0xE040;
        SciaRegs.SCIFFRX.all=0x2044;
        SciaRegs.SCIFFCT.all=0x0;
        SciaRegs.SCICCR.all =0x0007;   // 1 stop bit,  No loopback
                                       // No parity,8 char bits,
                                       // async mode, idle-line protocol
        SciaRegs.SCICTL1.all =0x0003;  // enable TX, RX, internal SCICLK,
                                       // Disable RX ERR, SLEEP, TXWAKE
        SciaRegs.SCICTL2.all =0x0003;
        SciaRegs.SCICTL2.bit.TXINTENA =1;
        SciaRegs.SCICTL2.bit.RXBKINTENA =1;

        SciaRegs.SCIHBAUD.bit.BAUD =0x0000;  // 9600 baud @LSPCLK = 10MHz
                                             //(40 MHz SYSCLK).
        SciaRegs.SCILBAUD.bit.BAUD =0x0081;

        SciaRegs.SCICTL1.all =0x0023;  // Relinquish SCI from Reset
        EDIS;

        scia_device_open = 1;
        return (1);
    }
}

int scia_close(int dev_fd)
{
    if ((dev_fd != 1) || (!scia_device_open)) {
        return (-1);
    } else {
        scia_device_open = 0;
        return (0);
    }
}

int scia_read(int dev_fd, char * buf, unsigned int count)
{
    uint16_t readCount = 0;
    uint16_t * bufPtr = (uint16_t *) buf;

    if (count == 0) {
        return (0);
    }

    while ((readCount < count) && SciaRegs.SCIRXST.bit.RXRDY) {
        *bufPtr = SciaRegs.SCIRXBUF.bit.SAR;
        readCount++;
        bufPtr++;
    }

    return (readCount);
}

int scia_write(int dev_fd, const char * buf, unsigned int count)
{
    uint16_t writeCount = 0;
    uint16_t * bufPtr = (uint16_t *) buf;

    if (count == 0) {
        return (0);
    }

    while (writeCount < count) {
        while (SciaRegs.SCICTL2.bit.TXRDY != 1);
        SciaRegs.SCITXBUF.bit.TXDT = *bufPtr;
        writeCount++;
        bufPtr++;
    }

    return (writeCount);
}

off_t scia_lseek(int dev_fd, off_t offset, int origin)
{
    return (0);
}

int scia_unlink(const char * path)
{
    return (0);
}

int scia_rename(const char * old_name, const char * new_name)
{
    return (0);
}

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
    scia_status = add_device("scia", _SSA, scia_open, scia_close, scia_read, scia_write,
                        scia_lseek, scia_unlink, scia_rename);
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
