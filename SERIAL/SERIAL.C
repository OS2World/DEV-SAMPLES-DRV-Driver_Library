/* file sample.c
   sample OS/2 serial device driver
*/

#include "drvlib.h"
#include "uart.h"
#include "serial.h"

extern void near STRAT(); /* name of strat rout.*/
extern void near TIM_HNDLR(); /* timer handler  */
extern int  near INT_HNDLR(); /* interrupt hand */

DEVICEHDR devhdr = {
 (void far *) 0xFFFFFFFF,  /* link              */
 (DAW_CHR | DAW_OPN | DAW_LEVEL1),/* attribute  */
 (OFF) STRAT,              /* &strategy         */
 (OFF) 0,                  /* &IDCroutine       */
 "DEVICE1 "
  };

CHARQUEUE   rx_queue;      /* receiver queue    */
CHARQUEUE   tx_queue;      /* transmitter queue */
FPFUNCTION  DevHlp=0;      /* for DevHlp calls  */
LHANDLE     lock_seg_han;  /* handle for locking*/
PHYSADDR    appl_buffer=0; /* address of caller */
PREQPACKET  p=0L;          /* Request Packet ptr*/
ERRCODE     err=0;         /* error return      */
void        far *ptr;      /* temp far pointer  */
DEVICEHDR   *hptr;         /* pointer to Device */
USHORT      i;             /* general counter   */
UARTREGS    uart_regs;     /* uart registers    */
ULONG       WriteID=0L;    /* ID for write Block*/
ULONG       ReadID=0L;     /* ID for read Block */
PREQPACKET  ThisReadRP=0L; /* for read Request  */
PREQPACKET  ThisWriteRP=0L;/* for write Request */
char        inchar,outchar;/* temp chars        */
USHORT      baud_rate;     /* current baud rate */
unsigned    int savepid;   /* PID of driver own */
UCHAR       opencount;     /* number of times   */
ULONG       tickcount;     /* for timeouts      */
unsigned    int com_error_word; /* UART status  */
USHORT      port;          /* port variable     */
USHORT      temp_bank;     /* holds UART bank   */
QUEUE       rqueue;        /* receive queue info*/

void near init();
void near enable_write();
void near disable_write();
void near set_dlab();
void near reset_dlab();
void near config_82050();

char   IntFailMsg[] = " interrupt handler failed to install.\r\n";
char   MainMsg[] = " OS/2 Serial Device Driver V1.0 installed.\r\n";

/* common entry point to strat routines */

int main(PREQPACKET rp, int dev )
{
   void far *ptr;
   int far *pptr;
   PLINFOSEG liptr;    /* pointer to local info */
   int i;
   ULONG addr;

   switch(rp->RPcommand)
    {
    case RPINIT:       /* 0x00                  */

        /* init called by kernel in prot mode */

        return Init(rp,dev);

    case RPOPEN:       /* 0x0d                  */

        /* get current processes id */

        if (GetDOSVar(2,&ptr))
            return (RPDONE|RPERR|ERROR_BAD_COMMAND);

        /* get process info */

        liptr = *((PLINFOSEG far *) ptr);

        /* if this device never opened */

        if (opencount == 0) /* 1st time dev op'd*/
        {
            ThisReadRP=0L;
            ThisWriteRP=0L;
            opencount=1;  /* set open counter   */
            savepid = liptr->pidCurrent; /* PID */
            QueueInit(&rx_queue);/* init driver */
            QueueInit(&tx_queue);
        }
        else
            {
            if (savepid != liptr->pidCurrent)
                return (RPDONE | RPERR | RPBUSY );
            ++opencount;       /* bump counter  */
        }
        return (RPDONE);

    case RPCLOSE:        /* 0x0e                */

        /* get process info of caller */

        if (GetDOSVar(2,&ptr))
            return (RPDONE|RPERR|ERROR_BAD_COMMAND); /* no info  */

        /* get process info from os/2 */

        liptr= *((PLINFOSEG far *) ptr); /* PID */
        if (savepid != liptr->pidCurrent ||
           opencount == 0)
        return (RPDONE|RPERR|ERROR_BAD_COMMAND);
        --opencount;   /* close counts down open*/

        if (ThisReadRP !=0 && opencount == 0) {
            Run((ULONG) ThisReadRP); /* dangling*/
            ThisReadRP=0L;
        }
        return (RPDONE);     /* return 'done'   */

    case RPREAD:             /* 0x04            */

        /*  Try to read a character */

        ThisReadRP = rp;
        if (opencount == 0)/* drvr was closed   */
        {
            rp->s.ReadWrite.count = 0;  /* EOF  */
            return(RPDONE);
        }
        com_error_word=0;/* start off no errors */
        ReadID = (ULONG) rp;
        if (Block(ReadID, -1L, 0, &err))
            if (err == 2)       /* interrupted  */
               return(RPDONE|RPERR|ERROR_CHAR_CALL_INTERRUPTED);

        if (rx_queue.qcount == 0) {
           rp->s.ReadWrite.count=0;
           return (RPDONE|RPERR|ERROR_NOT_READY);
           }

        i=0;
        do {
          if (MoveData((FARPOINTER)&inchar,
          (FARPOINTER) (rp->s.ReadWrite.buffer+i),
          1,
			 MOVE_VIRTTOPHYS))
            return(RPDONE|RPERR|ERROR_GEN_FAILURE);
            }
        while (++i < rp->s.ReadWrite.count
            && !QueueRead(&rx_queue,&inchar));
        rp->s.ReadWrite.count = i;
        QueueInit(&rx_queue);
        return(rp->RPstatus);

    case RPWRITE:        /* 0x08                */

        ThisWriteRP = rp;

        /* transfer characters from user buffer */

        addr=rp->s.ReadWrite.buffer;/* get addr */
        for (i = rp->s.ReadWrite.count; i; --i,++addr)
        {
          if (MoveData((FARPOINTER)addr,
             (FARPOINTER)&outchar,
				 1,
				 MOVE_PHYSTOVIRT))
                return (RPDONE|RPERR|ERROR_GEN_FAILURE);

          if (QueueWrite(&tx_queue,outchar))
              return (RPDONE|RPERR|ERROR_GEN_FAILURE);
          }
        WriteID = (ULONG) rp;
        enable_write();

        if (Block(WriteID, -1L, 0, &err))
            if (err == 2)   /* interrupted      */
                return(RPDONE|RPERR|ERROR_CHAR_CALL_INTERRUPTED);

        tickcount=MIN_TIMEOUT; /* reset timeout */
        QueueInit(&tx_queue);
        return (rp->RPstatus);

    case RPINPUT_FLUSH:      /* 0x07            */

        QueueFlush(&rx_queue);
        return (RPDONE);

    case RPOUTPUT_FLUSH:     /* 0x0b            */

        QueueFlush(&tx_queue);
        return (RPDONE);

    case RPIOCTL:            /* 0x10            */

        if (!((rp->s.IOCtl.category == SAMPLE_CAT)
           || (rp->s.IOCtl.category == 0x01)))
              return (RPDONE);

        switch (rp->s.IOCtl.function)
        {
        case 0x41:    /* set baud rate          */
        /* set baud rate to 1.2, 2.4, 9.6, 19.2 */
        /* verify caller owns the buffer area   */

        if(VerifyAccess(
         SELECTOROF(rp->s.IOCtl.parameters),
         OFFSETOF(rp->s.IOCtl.parameters),
         2,             /* two bytes            */
         1) )           /* read/write           */
                return (RPDONE|RPERR|ERROR_GEN_FAILURE);

         /* lock the segment down temp */

         if(LockSeg(
         SELECTOROF(rp->s.IOCtl.parameters),
         0,          /* lock for < 2 sec      */
         0,          /* wait for seg lock     */
         (PLHANDLE) &lock_seg_han)) /* handle */
             return (RPDONE|RPERR|ERROR_GEN_FAILURE);

         /* get physical address of buffer */
         if (VirtToPhys(
         (FARPOINTER) rp->s.IOCtl.parameters,
         (FARPOINTER) &appl_buffer))
             return (RPDONE|RPERR|ERROR_GEN_FAILURE);

         /* move data to local driver buffer */

         if(MoveData(
         (FARPOINTER) appl_buffer,  /* source             */
         (FARPOINTER)&baud_rate,    /* destination        */
         2,                         /* 2 bytes            */
			MOVE_PHYSTOVIRT))
                return (RPDONE|RPERR|ERROR_GEN_FAILURE);

         if (UnPhysToVirt()) /* release selector*/
                return(RPDONE|RPERR|ERROR_GEN_FAILURE);

         /* unlock segment */

         if(UnLockSeg(lock_seg_han))
              return(RPDONE|RPERR|ERROR_GEN_FAILURE);

         switch (baud_rate)
            {
            case 1200:

                uart_regs.Bal=0xe0;
                uart_regs.Bah=0x01;
                break;

            case 2400:

                uart_regs.Bal=0xf0;
                uart_regs.Bah=0x00;
                break;

            case 9600:

                uart_regs.Bal=0x3c;
                uart_regs.Bah=0x00;
                break;

            case 19200:

                uart_regs.Bal=0x1e;
                uart_regs.Bah=0x00;
                break;

            case 38400:

                uart_regs.Bal=0x0f;
                uart_regs.Bah=0x00;
                break;

error:
            return (RPDONE|RPERR|ERROR_BAD_COMMAND);

            }
            init();      /* reconfigure uart    */
            return (RPDONE);

        case 0x68:       /* get number of chars */

            /* verify caller owns the buffer    */

            if(VerifyAccess(
            SELECTOROF(rp->s.IOCtl.buffer),
            OFFSETOF(rp->s.IOCtl.buffer),
            4,            /* 4 bytes            */
            1) )          /* read/write         */
                return (RPDONE|RPERR|ERROR_GEN_FAILURE);

            /* lock the segment down temp */

            if(LockSeg(
            SELECTOROF(rp->s.IOCtl.buffer),
            0,           /* lock for < 2 sec    */
            0,           /* wait for seg lock   */
            (PLHANDLE) &lock_seg_han)) /* handle*/
                return (RPDONE|RPERR|ERROR_GEN_FAILURE);

            /* get physical address of buffer */

            if (VirtToPhys(
            (FARPOINTER) rp->s.IOCtl.buffer,
            (FARPOINTER) &appl_buffer))
                return (RPDONE|RPERR|ERROR_GEN_FAILURE);

            rqueue.cch=rx_queue.qcount;
            rqueue.cb=rx_queue.qsize;

            /* move data to local driver buffer */

            if(MoveData(
            (FARPOINTER)&rx_queue,  /* source   */
            (FARPOINTER) appl_buffer, /* dest   */
            4,           /* 4 bytes             */		 
				MOVE_PHYSTOVIRT))
                return (RPDONE|RPERR|ERROR_GEN_FAILURE);

            if (UnPhysToVirt())
                return(RPDONE|RPERR|ERROR_GEN_FAILURE);

            /* unlock segment */

            if(UnLockSeg(lock_seg_han))
                return(RPDONE|RPERR|ERROR_GEN_FAILURE);

            return (RPDONE);

        case 0x6d:    /* get COM error info     */

            /* verify caller owns the buffer    */

            if(VerifyAccess(
            SELECTOROF(rp->s.IOCtl.buffer),
            OFFSETOF(rp->s.IOCtl.buffer),
            2,        /* two bytes              */
            1) )      /* read/write             */
                return (RPDONE|RPERR|ERROR_GEN_FAILURE);

            /* lock the segment down temp */

            if(LockSeg(
            SELECTOROF(rp->s.IOCtl.buffer),
            0,        /* lock for < 2 sec       */
            0,        /* wait for seg lock      */
            (PLHANDLE) &lock_seg_han)) /* handle*/
                return (RPDONE|RPERR|ERROR_GEN_FAILURE);

            /* get physical address of buffer */

            if (VirtToPhys(
            (FARPOINTER) rp->s.IOCtl.buffer,
            (FARPOINTER) &appl_buffer))
               return (RPDONE|RPERR|ERROR_GEN_FAILURE);

            /* move data to application buffer */

            if(MoveData(
            (FARPOINTER)&com_error_word, /* source */
            (FARPOINTER) appl_buffer,    /* dest   */
            2,            /* 2 bytes               */
				MOVE_VIRTTOPHYS))
                return (RPDONE|RPERR|ERROR_GEN_FAILURE);

            if (UnPhysToVirt())
                return(RPDONE|RPERR|ERROR_GEN_FAILURE);

            /* unlock segment */

            if(UnLockSeg(lock_seg_han))
                return(RPDONE|RPERR|ERROR_GEN_FAILURE);

            return (RPDONE);

        default:
            return(RPDONE|RPERR|ERROR_GEN_FAILURE);
        }

        /* don't allow deinstall */

    case RPDEINSTALL:  /* 0x14                  */
        return(RPDONE|RPERR|ERROR_BAD_COMMAND);

    /* all other commands are ignored */

    default:
        return(RPDONE);

    }
}

void enable_write()

/* enable write interrupts on uart */

{
    int  port;
    int  reg_val;

    port=UART_PORT_ADDRESS;
    reg_val=inp(port+2) & 0x60;
    set_bank(00);
    outp((port+1),inp(port+1) | 0x12);
    outp((port+2),reg_val);

}
void disable_write()

/* turn off write interrupts on uart */

{
    int  port;
    int  reg_val;

    port=UART_PORT_ADDRESS;
    reg_val=inp(port+2) & 0x60;
    set_bank(00);
    outp((port+1),inp(port+1) & 0xed);
    outp((port+2),reg_val);

}

void init ()

/* intializes software and configures 82050 */

{
    config_82050 ();    /* Configure 82050      */
    set_bank(01);
}

void config_82050()

/*  Configure the 82050      */

{
    int  port;
    int inval;

    Disable();                          /* disable interrupts    */
    port=UART_PORT_ADDRESS;

    /* set stick bit */

    set_bank(01);                       /* stick bit            */
    outp((port+7),0x10);                /* reset port           */
    outp ((port+1), uart_regs.Txf);     /* stick bit            */

    set_bank (02);                      /* general config       */
    outp ((port + 4), uart_regs.Imd);   /*auto rupt             */
    outp ((port + 7), uart_regs.Rmd);
    outp ((port + 5), uart_regs.Acr1);  /* cntl-z               */
    outp ((port + 3), uart_regs.Tmd);   /* no 9 bit             */
    outp ((port + 1), uart_regs.Fmd);   /* rx fifo              */
    outp ((port + 6), uart_regs.Rie);   /* enable               */

    set_bank (03);                      /* modemconfiguration   */

    outp ((port + 0), uart_regs.Clcf);  /* clock                */
    set_dlab (03);                      /*                      */
    outp ((port + 0), uart_regs.Bbl);   /* BRGB lsb             */
    outp ((port + 1), uart_regs.Bbh);   /* BRGB msb             */
    reset_dlab (03);                    /*                      */
    outp ((port + 3), uart_regs.Bbcf);  /* BRGB                 */
    outp ((port + 6), uart_regs.Tmie);  /* timer b              */

    set_bank (00);                      /* general cfg          */
    outp ((port + 1), uart_regs.Ger);   /* enable               */
    outp ((port + 3), uart_regs.Lcr);   /* 8 bit                */
    outp ((port + 7), uart_regs.Acr0);  /* CR                   */
    outp ((port + 4), uart_regs.Mcr_0); /* no DTR               */
    set_dlab (00);                      /*                      */
    outp ((port + 0), uart_regs.Bal);   /* BRGA lsb             */
    outp ((port + 1), uart_regs.Bah);   /* BRGA msb             */
    reset_dlab (00);
    set_bank(01);

    Enable();                           /* turn on              */
}

void set_dlab (bank)

/*  Set DLAB bit to allow access to divisior registers  */

int bank;
{
    int  inval;
    int  port;

    port=UART_PORT_ADDRESS;
    set_bank (00);
    inval=inp(port +3);
    inval =inval | 0x80;                 /* set dlab in LCR     */
    outp ((port+3),inval);
    set_bank (bank);
}

getsrc()

{
    int   v,src;
    int   port;

    port=UART_PORT_ADDRESS;             /* get base address     */
    v=inp(port+2);                      /* get data             */
    src=v & 0x0e;                       /* mask bits            */
    src=src/2;                          /* divide by 2          */
    return(src);                        /* and pass it back     */
}

set_bank(bank_num)

/* set bank of 82050 uart */

int  bank_num;

{
    int reg_val;
    int   port;

    reg_val=bank_num*0x20;              /* select bank numb */
    port=UART_PORT_ADDRESS;             /* get real port    */
    outp(port+gir_addr,reg_val);        /* output      */
}

void reset_dlab (bank)

/*  Reset DLAB bit of LCR   */

int bank;

{
    int  inval;
    int  port;

    port=UART_PORT_ADDRESS;
    set_bank (00);
    inval=inp (port +3);
    inval = (inval & 0x7f);             /* dlab = 0 in LCR      */
    outp ((port+3),inval);
    set_bank (bank);
}

/* 82050 interrupt handler */

void interrupt_handler ()
{
    int  rupt_dev;
    int  source;
    int  cmd_b;
    int  st_b;
    int  port;
    int  temp;
    int  rxlevel;


    port=UART_PORT_ADDRESS;
    outp((port+2),0x20);                /* switch to bank 1     */
    source = getsrc ();                 /* get vector           */
    switch (source)
    {

    /* optional timer service routine */

    case timer :

        st_b=inp (port+3);              /* dec transmit count   */
        if ( ThisReadRP == 0)           /* nobody waiting       */
            break;
        ThisReadRP->RPstatus=(RPDONE|RPERR|ERROR_NOT_READY);
        Run ((ULONG)  ThisWriteRP);     /* run thread           */
        ThisWriteRP=0;
        break;

    case txm   :
    case txf   :

        /* spurious write interrupt */

        if ( ThisWriteRP == 0) {
            temp=inp(port+2);
            break;
        }

        /* keep transmitting until no data left */

        if  (!(QueueRead(&tx_queue,&outchar)))
        {
             outp((port), outchar);
             tickcount=MIN_TIMEOUT;
             break;
        }

        /* done writing, run blocked thread     */

        tickcount=MIN_TIMEOUT;
        disable_write();
        ThisWriteRP->RPstatus = (RPDONE);
        Run ((ULONG)  ThisWriteRP);
        ThisWriteRP=0;
        break;

    case ccr   :

        /* control character, treat as normal   */

        inchar=inp(port+5);

    case rxf   :

        /* rx fifo service routine */

        if ( ThisReadRP == 0)
            inchar=inp (port); /* get character */
        else
        {
        temp=inp(port+4);
        rxlevel=(temp & 0x70) / 0x10;

         /* empty out chip FIFO */

         while (rxlevel !=0) {

           inchar=inp (port); /* get character */
           rxlevel--;
           tickcount=MIN_TIMEOUT;

           /* write input data to queue */

           if(QueueWrite(&rx_queue,inchar))

             /* error, queue must be full */

             {
             ThisReadRP->RPstatus=(RPDONE|RPERR|ERROR_GEN_FAILURE);
             Run ((ULONG) ThisReadRP);
             ThisReadRP=0;
             break;
             }
           com_error_word |= inp(port+5);

        } /* while rxlevel */
     } /* else */
  } /* switch (source) */
  EOI(5);
}
void timer_handler()
{
  if (ThisReadRP == 0)
        return;

  tickcount--;
  if(tickcount == 0)  {
    ThisReadRP->RPstatus=(RPDONE);
    Run ((ULONG) ThisReadRP);
    ThisReadRP=0L;
    tickcount=MIN_TIMEOUT;
    }
}

/* Device Initialization Routine */

int Init(PREQPACKET rp, int dev)
{
    register char far *p;

    /* store DevHlp entry point */

    DevHlp = rp->s.Init.DevHlp;

    /* install interrupt hook in vector */

    if (SetTimer((PFUNCTION)TIM_HNDLR))
                goto fail;

    rx_queue.qsize=QUEUE_SIZE;
    tx_queue.qsize=QUEUE_SIZE; /* init queue    */
    init();                    /* init the port */
    tickcount=MIN_TIMEOUT;     /* set timeout   */

    if(SetIRQ(5,(PFUNCTION)INT_HNDLR,0)) {

     /* if we failed, deinstall driver cs+ds=0 */
fail:
     DosPutMessage(1, 8, devhdr.DHname);
     DosPutMessage (1,strlen(IntFailMsg),IntFailMsg);
     rp->s.InitExit.finalCS = (OFF) 0;
     rp->s.InitExit.finalDS = (OFF) 0;
     return (RPDONE | RPERR | ERROR_BAD_COMMAND);
     }

/* output initialization message */

DosPutMessage(1, 8, devhdr.DHname);
DosPutMessage(1, strlen(MainMsg), MainMsg);

/* send back our cs and ds values to os/2 */

if (SegLimit(HIUSHORT((void far *) Init),&rp->s.InitExit.finalCS)
    || SegLimit(HIUSHORT((void far *) MainMsg),
    &rp->s.InitExit.finalDS))
     Abort();
   return(RPDONE);
}


