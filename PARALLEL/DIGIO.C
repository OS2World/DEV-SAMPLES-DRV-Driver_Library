/*
   This driver supports DosOpen, DosClose, DosRead, DosWrite
   and IOCtl 0x91 codes 1, 2 and 3. All other driver calls and
   IOCtls are ignored (returns ERROR_BAD_COMMAND).

   The driver also uses these #defs

   #define  DIGIO_CAT    0x91         driver category       
   #define  DIGIO_BASE   0x2c0        base port address
   #define  DIGIO_OUTPUT DIGIO_BASE   output port
   #define  DIGIO_INPUT  DIGIO_BASE+1 input port
   #define  DIGIO_CONFIG DIGIO_BASE+3 initialization port

   1. Open the driver with:

      if ((RetCode=DosOpen("DIGIO$",
         &digio_handle,
         &ActionTaken,
         FileSize,
         FileAttribute,
         FILE_OPEN,
         OPEN_SHARE_DENYNONE | OPEN_FLAGS_FAIL_ON_ERROR
         | OPEN_ACCESS_READWRITE,Reserved)) !=0) 
            printf("\nopen error = %d",RetCode);

   2. Output byte to the output port (base +0) with this IOCtl:
         
      DosDevIOCtl(NULL,&char,1,0x91,digio_handle);

      or with this standard request:

      DosWrite(digio_handle,&char,1,&bytes_written;

   3. Read data from the input port (base + 1) with this IOCtl.
      The driver will block until the bit in specified in the
      mask is set:

      DosDevIOCtl(&char,NULL,2,0x91,digio_handle);

   4. Read data from the input port (base + 1) with this IOCtl.
      This IOCtl returns immediately with the status:

      DosDevIOCtl(&char,NULL,3,0x91,digio_handle);

      or with this standard driver request:

      DosRead(digio_handle,&char,1,&bytes_read;
*/

#include "drvlib.h"
#include "digio.h"

extern void STRATEGY();       /* name of strat rout. in drvstart*/
extern void TIMER_HANDLER();  /* timer handler in drvstart      */

DEVICEHDR devhdr = {
    (void far *) 0xFFFFFFFF,  /* link                           */
    (DAW_CHR | DAW_OPN | DAW_LEVEL1),/* attribute word          */
    (OFF) STRATEGY,           /* &strategy                      */
    (OFF) 0,                  /* &IDC routine                   */
    "DIGIO$  "                /* name/#units                    */
};

FPFUNCTION  DevHlp=0;         /* pointer to DevHlp entry point  */
UCHAR       opencount = 0;    /* keeps track of open's          */
USHORT      savepid=0;        /* save thread pid                */
LHANDLE     lock_seg_han;     /* handle for locking appl. seg   */
PHYSADDR    appl_buffer=0;    /* address of caller's buffer     */
ERRCODE     err=0;            /* error return                   */
ULONG       ReadID=0L;        /* current read pointer           */
USHORT      num_rupts=0;      /* count of interrupts            */
USHORT      temp_char;        /* temp character for in-out      */
void        far *ptr;         /* temp far pointer               */
FARPOINTER  appl_ptr=0;       /* pointer to application buffer  */
char        input_char,output_char; /* temp character storage   */
char        input_mask;       /* mask for input byte            */

/* messages */

char     CrLf[]= "\r\n"; 
char     InitMessage1[] = " 8 bit Digital I/O ";
char     InitMessage2[] = " driver installed\r\n";
char     FailMessage[]  = " driver failed to install.\r\n";

/* common entry point for calls to Strategy routines */

int main(PREQPACKET rp)
{
    void far *ptr;     
    PLINFOSEG liptr;             /* pointer to global info seg  */
    int i;
    
    switch(rp->RPcommand)
    {
    case RPINIT:                 /* 0x00                        */

        /* init called by kernel in protected mode */

        return Init(rp);

    case RPREAD:                 /* 0x04                        */

        rp->s.ReadWrite.count = 0; /* in case we fail           */
        
        input_char = inp(DIGIO_INPUT);/* get data               */

        if (PhysToVirt( (ULONG) rp->s.ReadWrite.buffer,
           1,0,&appl_ptr))
             return (RPDONE | RPERR | ERROR_GEN_FAILURE);

        if (MoveBytes((FARPOINTER)&input_char,appl_ptr,1)) /* move one byte */
             return (RPDONE | RPERR | ERROR_GEN_FAILURE);

        rp->s.ReadWrite.count = 1;  /* one byte read           */
        return (RPDONE);

    case RPWRITE:                /* 0x08                       */

        rp->s.ReadWrite.count = 0;

        if (PhysToVirt( (ULONG) rp->s.ReadWrite.buffer,
           1,0,&appl_ptr))
             return (RPDONE | RPERR | ERROR_GEN_FAILURE);

        if (MoveBytes(appl_ptr,(FARPOINTER)&output_char,1)) /* move 1 byte */
             return (RPDONE | RPERR | ERROR_GEN_FAILURE);

        outp (DIGIO_OUTPUT,output_char); /* send byte          */

        rp->s.ReadWrite.count = 1; /* one byte written         */
        return (RPDONE);

    case RPOPEN:                 /* 0x0d open driver           */

        /* get current process id */

        if (GetDOSVar(2,&ptr))
            return (RPDONE | RPERR | ERROR_BAD_COMMAND);

        /* get process info */

        liptr = *((PLINFOSEG far *) ptr);

        /* if this device never opened, can be opened by anyone*/

        if (opencount == 0)      /* first time this dev opened */
        {
            opencount=1;         /* bump open counter          */
            savepid = liptr->pidCurrent; /* save current PID   */
        }
        else
            {
            if (savepid != liptr->pidCurrent) /* another proc  */
                return (RPDONE | RPERR | ERROR_NOT_READY);/*err*/
            ++opencount;         /* bump counter, same pid     */
        }
        return (RPDONE);

    case RPCLOSE:                /* 0x0e DosClose,ctl-C, kill  */

        /* get process info of caller */

        if (GetDOSVar(2,&ptr))
            return (RPDONE | RPERR | ERROR_BAD_COMMAND); 

        /* get process info from os/2 */

        liptr= *((PLINFOSEG far *) ptr); /* ptr to linfoseg    */

        /* 
        make sure that process attempting to close this device
        is the one that originally opened it and the device was
        open in the first place.
        */

        if (savepid != liptr->pidCurrent || opencount == 0)  
            return (RPDONE | RPERR | ERROR_BAD_COMMAND);

        --opencount;             /* close counts down open cntr*/
         return (RPDONE);        /* return 'done' status       */

    case RPIOCTL:                /* 0x10                       */

        /*
        The function code in an IOCtl packet has the high bit set
        for the DIGIO$ board. We return all others with the done
        bit set so we don't have to handle things like the 5-48
        code page IOCtl
        */

        if (rp->s.IOCtl.category != DIGIO_CAT)/* other IOCtls  */
            return (RPDONE | RPERR | ERROR_BAD_COMMAND);

        switch (rp->s.IOCtl.function)
        {

        case 0x01:               /* write byte to digio port   */

            /* verify caller owns this buffer area */

            if(VerifyAccess(
            SELECTOROF(rp->s.IOCtl.parameters), /* selector    */
            OFFSETOF(rp->s.IOCtl.parameters),   /* offset      */
            1,                                  /* 1 byte      */
            0) )                                /* read only   */
                return (RPDONE | RPERR | ERROR_GEN_FAILURE);

            if(MoveBytes(rp->s.IOCtl.parameters,(FARPOINTER)&output_char,1))
                return (RPDONE | RPERR | ERROR_GEN_FAILURE);

            outp(DIGIO_OUTPUT,output_char);     /*send to digio*/
            return (RPDONE);

        case 0x02:               /* read byte w/wait from port */

            /* verify caller owns this buffer area */

            if(VerifyAccess(
            SELECTOROF(rp->s.IOCtl.buffer), /* selector        */
            OFFSETOF(rp->s.IOCtl.buffer),   /* offset          */
            1,                              /* 1 bytes)        */
            0))                             /* read only       */
               return (RPDONE | RPERR | ERROR_GEN_FAILURE);

            /* lock the segment down temp */

            if(LockSeg(
            SELECTOROF(rp->s.IOCtl.buffer), /* selector        */
            1,                              /* lock forever    */
            0,                              /* wait for seg loc*/
            (PLHANDLE) &lock_seg_han))      /* handle returned */
               return (RPDONE | RPERR | ERROR_GEN_FAILURE);

            if(MoveBytes(rp->s.IOCtl.parameters,(FARPOINTER)&input_mask,1))
                return (RPDONE | RPERR | ERROR_GEN_FAILURE);

            /* wait for switch to be pressed */
 
            ReadID = (ULONG)rp;             /* block ID        */
            if (Block(ReadID,-1L,0,&err))
              if (err == 2)
                return(RPDONE | RPERR
                       | ERROR_CHAR_CALL_INTERRUPTED);
            
            /* move data to users buffer */
 
            if(MoveBytes((FARPOINTER)&input_char,rp->s.IOCtl.buffer,1))
                return(RPDONE | RPERR | ERROR_GEN_FAILURE);

            /* unlock segment */

            if(UnLockSeg(lock_seg_han))
                return(RPDONE | RPERR | ERROR_GEN_FAILURE);
 
            return (RPDONE);

        case 0x03:               /* read byte immed digio port */

            /* verify caller owns this buffer area */

            if(VerifyAccess(
            SELECTOROF(rp->s.IOCtl.buffer), /* selector        */
            OFFSETOF(rp->s.IOCtl.buffer),   /* offset          */
            4,                              /* 4 bytes         */
            0))                             /* read only       */
                return (RPDONE | RPERR | ERROR_GEN_FAILURE);

            input_char = inp(DIGIO_INPUT);  /* get data        */

            if(MoveBytes((FARPOINTER)&input_char,rp->s.IOCtl.buffer,1))
                return(RPDONE | RPERR | ERROR_GEN_FAILURE);

            return (RPDONE);

        default:
            return(RPDONE | RPERR | ERROR_GEN_FAILURE);
        }

        /* don't allow deinstall */

    case RPDEINSTALL:            /* 0x14                       */
        return(RPDONE | RPERR | ERROR_BAD_COMMAND);

        /* all other commands are flagged as bad */

    default:
        return(RPDONE | RPERR | ERROR_BAD_COMMAND);

    }
}

timr_handler()
{

   if (ReadID != 0) {
      
      /* read data from port */
      
      input_char = inp(DIGIO_INPUT );/* get data               */

      if ((input_char && input_mask) !=0) {
         Run (ReadID);
         ReadID=0L;
         }
    }
}

/* Device Initialization Routine */

int Init(PREQPACKET rp)
{
    /* store DevHlp entry point */

    DevHlp = rp->s.Init.DevHlp;

    /* install timer handler */

    if(SetTimer((PFUNCTION)TIMER_HANDLER)) {

      /* if we failed, effectively deinstall driver with cs+ds=0 */

      DosPutMessage(1, 8, devhdr.DHname);
      DosPutMessage(1,strlen(FailMessage),FailMessage);
      rp->s.InitExit.finalCS = (OFF) 0;
      rp->s.InitExit.finalDS = (OFF) 0;
      return (RPDONE | RPERR | ERROR_GEN_FAILURE);
      }

    /* configure 8255 parallel chip */

    outp (DIGIO_CONFIG,0x91);

    /* output initialization message */

    DosPutMessage(1, 2, CrLf);
    DosPutMessage(1, 8, devhdr.DHname);
    DosPutMessage(1, strlen(InitMessage1), InitMessage1);
    DosPutMessage(1, strlen(InitMessage2), InitMessage2);

    /* send back our code and data end values to os/2 */

    if (SegLimit(HIUSHORT((void far *) Init),
      &rp->s.InitExit.finalCS) || SegLimit(HIUSHORT((void far *)
      InitMessage2), &rp->s.InitExit.finalDS))
        Abort();
    return(RPDONE);
}
 
