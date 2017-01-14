/* serial.h
   include file for sample driver
*/

#define  SAMPLE_CAT  0x90           /* category for DosDevIOCtl          */
#define  CR 0x0d
#define  MIN_TIMEOUT 15000L		   /* ~5 second timeout                 */
#define  UART_PORT_ADDRESS 0x220

/* driver utilized structures */

typedef struct _DCBINFO {           /* DCB storage for this driver       */
    USHORT usWriteTimeout;          /* timeouts for Block calls          */
    USHORT usReadTimeout;           /* real Block call timeout           */
    UCHAR  fbCtlHndShake;           /* OS/2 std data follows             */
    UCHAR  fbFlowReplace;
    UCHAR  fbTimeout;
    UCHAR  bErrorReplacementChar;
    UCHAR  bBreakReplacementChar;
    UCHAR  bXONChar;
    UCHAR  bXOFFChar;
} 
DCBINFO;

typedef struct _MODEMSTATUS {
    UCHAR fbModemOn;
    UCHAR fbModemOff;
} 
MODEMSTATUS;

/* standard OS/2 structure */

typedef struct _LINECONTROL {
    UCHAR bDataBits;
    UCHAR bParity;
    UCHAR bStopBits;
    UCHAR fbTransBreak;
} LINECONTROL;

/* queue info returned by driver */

typedef struct _QUEUE {
    USHORT cch;
    USHORT cb;
} 
QUEUE;

/* this structure holds the register values for the 82510 uart */

typedef struct _UARTREGS {

/* bank 0, 8250/16450 compatibility bank */

    UCHAR Txd_0;                    /* transmit/read data FIFO             */
    UCHAR Ger;                      /* general enable register             */
    UCHAR Bal;                      /* baud rate generator A low           */
    UCHAR Bah;                      /* baud rate generator A high          */
    UCHAR Bank_0;                   /* select bank 0                       */
    UCHAR Lcr;                      /* line control register               */
    UCHAR Mcr_0;                    /* modem control register              */
    UCHAR Lsr;                      /* line status register                */
    UCHAR Msr;                      /* modem status register               */
    UCHAR Acr0;                     /* character match value 0             */

/* bank 1, general work bank  */

    UCHAR Txd_1;                    /* transmit/read data FIFO             */
    UCHAR Txf;                      /* 9th bit transmit register           */
    UCHAR Bank_1;                   /* select bank 1                       */
    UCHAR Tmcr;                     /* start timer A/B command register    */
    UCHAR Mcr_1;                    /* modem control register for bank 1   */
    UCHAR Rcm;                      /* receiver command register           */
    UCHAR Tcm;                      /* transmit command register           */
    UCHAR Icm;                      /* internal command register           */

/* bank 2, general configuration bank */

    UCHAR Fmd;                      /* FIFO threshold register             */
    UCHAR Bank_2;                   /* select bank 2                       */
    UCHAR Tmd;                      /* transmit mode register              */
    UCHAR Imd;                      /* internal mode register              */
    UCHAR Acr1;                     /* character match value 1             */
    UCHAR Rie;                      /* receiver interrupt enable register  */
    UCHAR Rmd;                      /* receive mode register               */

/* bank 3, modem configuration bank */

    UCHAR Clcf;                     /* clocks configure register           */
    UCHAR Bacf;                     /* baud rate generator A config. reg.  */
    UCHAR Bbl;                      /* baud rate generator B low value     */
    UCHAR Bbh;                      /* baud rate generator B high value    */
    UCHAR Bank_3;                   /* selct bank 3                        */
    UCHAR Bbcf;                     /* baud rate generator B config. reg.  */
    UCHAR Pmd;                      /* I/O mode pin register               */
    UCHAR Mie;                      /* modem interrupt enable register     */
    UCHAR Tmie;                     /* timer interrupt enable register     */

} UARTREGS;
/* this structure holds the start and end message times for an adam message*/ 

typedef struct _DATETIME {         /* date, adam driver/server specific   */
    UCHAR   hours;
    UCHAR   minutes;
    UCHAR   seconds;
    UCHAR   hundredths;
    UCHAR   day;
    UCHAR   month;
    USHORT  year;
    short   timezone;
    UCHAR   weekday;

} DATETIME;
typedef DATETIME far *PDATETIME;

/* this structure holds all the driver and uart specific data for each
of the four or eight 82510 uarts.
*/   

typedef struct _UART {
   UARTREGS     uart_regs;          /* 82510 register structures           */
   LINECONTROL  line_control;       /* OS/2 line control struct            */
   MODEMSTATUS  modem_status;       /* OS/2 modem status struct            */
   DCBINFO      dcb_info;           /* our special DCBINFO struct          */
   UCHAR        opencount;          /* number of opens on a device         */
   unsigned int savepid;            /* saved PID of app that opened device */
   PREQPACKET   ReadQHead;          /* head pointer for linked req. queue  */
   PREQPACKET   WriteQHead;         /* head pointer for linked req. queue  */
   PREQPACKET   ThisReadRP;         /* pointer to current read req. pkt.   */
   PREQPACKET   ThisWriteRP;        /* pointer to current write req. pkt.  */
   ULONG        ReadID;             /* used for thread ID for read Block   */
   ULONG        WriteID;            /* used for thread ID for write Block  */
   USHORT       baud_rate;          /* current baud rate for port          */
   USHORT       first_flag;         /* for timeout logic                   */
   ULONG        tickcount;          /* used for driver Block call timeout  */
   UCHAR        inchar;             /* current char read from uart FIFO    */
   UCHAR        outchar;            /* current output character            */
   UCHAR        savechar;           /* last char transmitted for bus cont. */
   UCHAR        tempchar;           /* temp character storage              */
   ULONG        timeout;            /* current driver timeout value        */
   int          temp;               /* misc temp storage                   */
   int          temp_port;          /* used to hold port temporarily       */
   int          com_error_word;     /* OS/2-like, holds last comm error    */
   int          com_event_word;     /* what happened last                  */
   char         com_status_word;    /* what's happening now                */
   USHORT       transmit_data_status; /* xmit status                       */
   char         modem_input_signals; /* status of modem signals            */
   char         modem_output_signals;/* set modem signals                  */
   int          port;                /* current port                       */
   QUEUE        receive_queue;       /* receive queue data returned to app */
   QUEUE        transmit_queue;      /* xmit queue data returned to app    */
   CHARQUEUE    rxadam_queue;        /* OS/2 circular receive queue        */
   CHARQUEUE    txadam_queue;        /* OS/2 circular transmit queue       */

  } UART;


