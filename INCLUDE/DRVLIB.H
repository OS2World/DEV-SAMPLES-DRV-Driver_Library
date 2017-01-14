//  file drvlib.h
//   This header file contains definitions intended to go along with
//   DRVLIB.LIB, a C-callable subroutine library.
//
//   This file is for OS/2 2.0

typedef unsigned char   UCHAR;
typedef unsigned short  USHORT;
typedef unsigned short  BOOLEAN;
typedef unsigned long   ULONG;
typedef UCHAR near      *PUCHAR;
typedef UCHAR far       *FPUCHAR;
typedef USHORT near     *PUSHORT;
typedef USHORT far      *FPUSHORT;
typedef ULONG near      *PULONG;
typedef ULONG far       *FPULONG;
typedef char near       *PCHAR;
typedef short near      *PSHORT;
typedef long near       *PLONG; 
typedef void near       *POINTER;
typedef POINTER near    *PPOINTER;
typedef void far        *FARPOINTER;
typedef FARPOINTER near *PFARPOINTER;
typedef FARPOINTER far  *FPFARPOINTER;

typedef USHORT          ERRCODE;    // error code returned                 
typedef ERRCODE far     *PERRCODE;  // pointer to an error code            
typedef UCHAR           FLAG;       // 8-bit flag                          
typedef FLAG far        *PFLAG;     // pointer to 8-bit flag               
typedef USHORT          SEL;        // 16-bit selector                     
typedef SEL near        *PSEL;      // pointer to a selector               
typedef SEL far         *FPSEL;     // far pointer to selector             
typedef USHORT          SEG;        // 16-bit segment                      
typedef USHORT          OFF;        // 16-bit offset                       
typedef ULONG           LOFF;       // 32-bit offset                       
typedef USHORT          PID;        // Process ID                          
typedef USHORT          TID;        // Thread ID                           
typedef ULONG           PHYSADDR;   // 32-bit physical address             
typedef ULONG           LINADDR;    // 32-bit linear address               
typedef LINADDR  far    *PLINADDR;  // pointer to 32 bit linear address    
typedef PLINADDR far    *PPLINADDR; // pointer to linear address pointer   
typedef PHYSADDR far    *PPHYSADDR; // pointer to 32-bit physical address  
typedef char near       *PSTRING;   // pointer to character string         
typedef char far        *FPSTRING;// far pointer to string               
typedef USHORT          SHANDLE;    // short (16-bit) handle               
typedef SHANDLE far     *PSHANDLE;  // pointer to a short handle           
typedef ULONG           LHANDLE;    // long  (32-bit) handle               
typedef LHANDLE far     *PLHANDLE;  // pointer to a long handle            

//  pointers to functions 

typedef int (pascal near          *PFUNCTION) ();
typedef int (pascal near * near  *PPFUNCTION) ();
typedef int (pascal far          *FPFUNCTION) ();
typedef int (pascal far  * near *PFPFUNCTION) ();

// macros 

#define FALSE   0
#define TRUE    1

#define NP near pascal

// far pointer from selector-offset 

#define MAKEP(sel, off)     ( (void far *) MAKEULONG(off, sel) )

// get selector or offset from far pointer 

#define SELECTOROF(p)       ( ((USHORT far *) &(p)) [1])
#define OFFSETOF(p)         ( ((USHORT far *) &(p)) [0])

// Combine l(ow) & h(igh) to form a 32 bit quantity. 

#define MAKEULONG(l, h)  ((ULONG)(((USHORT)(l)) | ((ULONG)((USHORT)(h))) << 16))
#define MAKELONG(l, h)   ((LONG)MAKEULONG(l, h))
#define MAKEBIGOFFSETOF(p) ((ULONG) (OFFSETOF (p)))

// Combine l(ow) & h(igh) to form a 16 bit quantity. 

#define MAKEUSHORT(l, h) (((USHORT)(l)) | ((USHORT)(h)) << 8)
#define MAKESHORT(l, h)  ((SHORT)MAKEUSHORT(l, h))

// get high and low order parts of a 16 and 32 bit quantity 

#define LOBYTE(w)       LOUCHAR(w)
#define HIBYTE(w)       HIUCHAR(w)
#define LOUCHAR(w)      ((UCHAR)(w))
#define HIUCHAR(w)      (((USHORT)(w) >> 8) & 0xff)
#define LOUSHORT(l)     ((USHORT)(l))
#define HIUSHORT(l)     ((USHORT)(((ULONG)(l) >> 16) & 0xffff))

//  the driver device header 

typedef struct DeviceHdr {
   struct DeviceHdr far *DHnext;    // pointer to next header, or FFFF     
   USHORT DHattribute;              // device attribute word               
   OFF    DHstrategy;               // offset of strategy routine          
   OFF    DHidc;                    // offset of IDC routine               
   UCHAR  DHname[8];                // dev name (char) or #units (blk)     
   char   reserved[8];
   ULONG  bit_strip;                // bit 0 DevIOCtl2, bit 1 32 bit addr  
   } DEVICEHDR;
typedef DEVICEHDR near *PDEVICEHDR;

//  driver device attributes word 

#define DAW_CHR    0x8000           // 1=char, 0=block                     
#define DAW_IDC    0x4000           // 1=IDC available in this DD          
#define DAW_IBM    0x2000           // 1=non-IBM block format              
#define DAW_SHR    0x1000           // 1=supports shared device access     
#define DAW_OPN    0x0800           // 1=open/close, or removable media    
#define DAW_LEVEL1 0x0080           // level 1                             
#define DAW_LEVEL2 0x0100           // level 2 DosDevIOCtl2                
#define DAW_LEVEL3 0x0180           // level 3 bit strip                   
#define DAW_GIO    0x0040           // 1=generic IOCtl supported           
#define DAW_CLK    0x0008           // 1=CLOCK device                      
#define DAW_NUL    0x0004           // 1=NUL device                        
#define DAW_SCR    0x0002           // 1=STDOUT (screen)                   
#define DAW_KBD    0x0001           // 1=STDIN  (keyboard)                 

// capabilities bit strip 

#define CBS_SHD    0x0001           // 1=shutdown/DevIOCtl2                
#define CBS_HMEM   0x0002           // hign memory map for adapters        
#define CBS_PP     0x0004           // supports parallel ports         
#define CBS_ADD    0x0010           // driver is an ADD
#define CBS_INIT   010020           // driver receives InitComplete    

// SaveMessage structure 

typedef struct MessageTable {
   USHORT      id;
   USHORT      fill_in_item;
   FARPOINTER  item1;
   FARPOINTER  item2;
   FARPOINTER  item_last;
   } MESSAGETABLE;

// OS/2 circular character queues 

#define QUEUE_SIZE  512             //  size of queues                     
typedef struct CharQueue {
   USHORT   qsize;                  // number of bytes in queue            
   USHORT   qchrout;                // index of next char to put out       
   USHORT   qcount;                 // number of charactes in queue        
   UCHAR    qbuf[QUEUE_SIZE]; 
   } CHARQUEUE;
typedef CHARQUEUE near *PCHARQUEUE;

// AttachDD inter device driver communication data area 

typedef struct AttachArea {
   OFF realOFF;                     // real-mode offset of idc entry point 
   SEG realCS;                      // real-mode CS of IDC entry point     
   SEG realDS;                      // real-mode DS of IDC DD              
   OFF protOFF;                     // protect-mode offset of entry point  
   SEL protCS;                      // protect-mode CS of entry point      
   SEL protDS;                      // protect-mode DS of other DD         
   } ATTACHAREA;
typedef ATTACHAREA near *PATTACHAREA;

// driver request packet 

typedef struct ReqPacket {
   UCHAR RPlength;                  // request packet length               
   UCHAR   RPunit;                  // unit code for block DD only         
   UCHAR RPcommand;                 // command code                        
   USHORT  RPstatus;                // status word                         
   UCHAR   RPreserved[4];           // reserved bytes                      
   ULONG RPqlink;                   // queue linkage                       
   union {                          // command-specific data               
   UCHAR   avail[19];               
    struct {                        // init                                
      UCHAR      units;             // number of units                     
      FPFUNCTION DevHlp;            // &DevHlp                             
      char far   *args;             // &args                               
      UCHAR      drive;             // drive #                             
      }Init;                        
    struct { 
      UCHAR     units;              // same as input                       
      OFF       finalCS;            // final offset, 1st code segment      
      OFF       finalDS;            // final offset, 1st data segment      
      FARPOINTER BPBarray;          // &BPB                                
      } InitExit;

    struct {                        // read, write, write w/verify         
      UCHAR      media;             // media descriptor                    
      PHYSADDR   buffer;            // transfer address                    
      USHORT     count;             // bytes/sectors                       
      ULONG      startsector;       // starting sector#                    
      USHORT     reserved; 
      } ReadWrite;                  

    struct {                        // cached read, write, write w/verify  
      UCHAR      media;             // media descriptor                    
      PHYSADDR   buffer;            // transfer address                    
      USHORT     count;             // bytes/sectors                       
      ULONG      startsector;       // starting sector#                    
      USHORT     reserved; 
      } CReadWrite;
      
    struct {                        // system shutdown                     
      UCHAR      subcode;           // sub request code                    
      ULONG      reserved;
      } Shutdown;

    struct {                        // open/close                          
      USHORT     sysfilenum;        // system file number                  
      } OpenClose;

    struct {                        // IOCtl                               
      UCHAR      category;          // category code                       
      UCHAR      function;          // function code                       
      FARPOINTER parameters;        // &parameters                         
      FARPOINTER buffer;            // &buffer                             
      } IOCtl;                      

    struct {                        // read, no wait                       
      UCHAR      char_returned;     // char to return                      
      } ReadNoWait;                 

    struct {                        // media check                         
      UCHAR      media;             // media descriptor                    
      UCHAR      return_code;       // see #defines                        
      FARPOINTER prev_volume;       // &previous volume ID                 
      } MediaCheck;                 

    struct {                        // build BPB                           
      UCHAR      media;             // media descriptor                    
      FARPOINTER buffer;            // 1-sector buffer FAT                 
      FARPOINTER BPBarray;          // &BPB array                          
      UCHAR      drive;             // drive #                             
      } BuildBPB;                   
                                                                             
    struct {                        // query partitionalble fixed disks    
      UCHAR      count;             // # disks                             
      ULONG      reserved;
      } Partitionable;              
     
    struct {                        // fixed disk LU map                   
      ULONG      units;             // units supported                     
      ULONG      reserved;
      } GetFixedMap;  

    struct {                        // get driver capabilities             
      UCHAR      reserved[3];
      FARPOINTER capstruct;         // 16:16 pointer to DCS                
      FARPOINTER volcharstruct;     // 16:16 pointer to VCS                
      } GetDriverCaps;
              
   } s;                             // command info                        
} REQPACKET;

typedef REQPACKET far *PREQPACKET;
typedef PREQPACKET far *PPREQPACKET;
typedef PREQPACKET QHEAD;           // Queue Head is &ReqPacket            
typedef QHEAD near *PQHEAD;

// Global Info Seg 

typedef struct _GINFOSEG {      
    ULONG   time;
    ULONG   msecs;
    UCHAR   hour;
    UCHAR   minutes;
    UCHAR   seconds;
    UCHAR   hundredths;
    USHORT  timezone;
    USHORT  cusecTimerInterval;
    UCHAR   day;
    UCHAR   month;
    USHORT  year;
    UCHAR   weekday;
    UCHAR   uchMajorVersion;
    UCHAR   uchMinorVersion;
    UCHAR   chRevisionLetter;
    UCHAR   sgCurrent;
    UCHAR   sgMax;
    UCHAR   cHugeShift;
    UCHAR   fProtectModeOnly;
    USHORT  pidForeground;
    UCHAR   fDynamicSched;
    UCHAR   csecMaxWait;
    USHORT  cmsecMinSlice;
    USHORT  cmsecMaxSlice;
    USHORT  bootdrive;
    UCHAR   amecRAS[32];
    UCHAR   csgWindowableVioMax;
    UCHAR   csgPMMax;
} GINFOSEG;
typedef GINFOSEG far *PGINFOSEG;

// local info seg 

typedef struct _LINFOSEG {      
    PID     pidCurrent;
    PID     pidParent;
    USHORT  prtyCurrent;
    TID     tidCurrent;
    USHORT  sgCurrent;
    UCHAR   rfProcStatus;
    UCHAR   dummy1;
    USHORT  fForeground;
    UCHAR   typeProcess;
    UCHAR   dummy2;
    SEL     selEnvironment;
    USHORT  offCmdLine;
    USHORT  cbDataSegment;
    USHORT  cbStack;
    USHORT  cbHeap;
    USHORT  hmod;
    SEL     selDS;
} LINFOSEG;

typedef LINFOSEG far *PLINFOSEG;

typedef struct _REGSTACK {          // stack usgae structure					
    USHORT  usStruct;					// set to 14 before using              
	 USHORT  usFlags;						// 0x01 means that the interrupt proc  
	                                 // enables interrupts. All others resvd
	 USHORT  usIRQ;						// IRQ of interrupt handler            
	 USHORT  usStackCLI;             // # of stack bytes with interrupts off
	 USHORT  usStackSTI;             // # of stack bytes with interrupts on 
	 USHORT  usStackEOI;					// number of bytes needed after EOI    
	 USHORT  usNest;						// max number of nested levels         
	 } REGSTACK;

typedef REGSTACK near *PREGSTACK;

// page list struct 

typedef struct _PAGELIST {
    ULONG pl_Physaddr;
	 ULONG pl_cb;
	 } PAGELIST;
typedef PAGELIST far *PPAGELIST;

// RPstatus bit values 

#define RPERR   0x8000              //  error occurred, err in RPstatus    
#define RPDEV   0x4000              //  error code defined by driver       
#define RPBUSY  0x0200              //  device is busy                     
#define RPDONE  0x0100              //  driver done with request packet    

// error codes returned in RPstatus 

#define ERROR_WRITE_PROTECT         0x0000
#define ERROR_BAD_UNIT              0x0001
#define ERROR_NOT_READY             0x0002
#define ERROR_BAD_COMMAND           0x0003
#define ERROR_CRC                   0x0004
#define ERROR_BAD_LENGTH            0x0005    
#define ERROR_SEEK                  0x0006
#define ERROR_NOT_DOS_DISK          0x0007
#define ERROR_SECTOR_NOT_FOUND      0x0008
#define ERROR_OUT_OF_PAPER          0x0009
#define ERROR_WRITE_FAULT           0x000A
#define ERROR_READ_FAULT            0x000B
#define ERROR_GEN_FAILURE           0x000C
#define ERROR_DISK_CHANGE           0x000D
#define ERROR_WRONG_DISK            0x000F
#define ERROR_UNCERTAIN_MEDIA       0x0010
#define ERROR_CHAR_CALL_INTERRUPTED 0x0011
#define ERROR_NO_MONITOR_SUPPORT    0x0012
#define ERROR_INVALID_PARAMETER     0x0013
#define ERROR_DEVICE_IN_USE         0x0014

// driver request codes  B=block, C=character 

#define RPINIT          0x00        //  BC                                 
#define RPMEDIA_CHECK   0x01        //  B                                  
#define RPBUILD_BPB     0x02        //  B                                  
#define RPREAD          0x04        //  BC                                 
#define RPREAD_NO_WAIT  0x05        //   C                                 
#define RPINPUT_STATUS  0x06        //   C                                 
#define RPINPUT_FLUSH   0x07        //   C                                 
#define RPWRITE         0x08        //  BC                                 
#define RPWRITE_VERIFY  0x09        //  BC                                 
#define RPOUTPUT_STATUS 0x0a        //   C                                 
#define RPOUTPUT_FLUSH  0x0b        //   C                                 
#define RPOPEN          0x0d        //  BC                                 
#define RPCLOSE         0x0e        //  BC                                 
#define RPREMOVABLE     0x0f        //  B                                  
#define RPIOCTL         0x10        //  BC                                 
#define RPRESET         0x11        //  B                                  
#define RPGET_DRIVE_MAP 0x12        //  B                                  
#define RPSET_DRIVE_MAP 0x13        //  B                                  
#define RPDEINSTALL     0x14        //   C                                 
#define RPPARTITIONABLE 0x16        //  B                                  
#define RPGET_FIXED_MAP 0x17        //  B                                  
#define RPSHUTDOWN      0x1c        //  BC                                 
#define RPGET_DRIVER_CAPS 0x1d      //  B                                  

// check for monitor call in DosOpen/DosClose 

#define MON_OPEN_STATUS   0x08      // open from DosMonOpen                
#define MON_CLOSE_STATUS  0x08      // close from DosMonClose              

// media descriptor byte 

#define MDB_REMOVABLE     0x04      //  1=removable                        
#define MDB_EIGHT_SECTORS 0x02      //  1=8 sectors per track              
#define MDB_DOUBLE_SIDED  0x01      //  1=double-sided media               

// return codes from MediaCheck 

#define MC_MEDIA_UNCHANGED 0x01
#define MC_MEDIA_CHANGED   0xFF
#define MC_MEDIA_UNSURE    0x00

// event numbers for SendEvent 

#define EVENT_SM_MOUSE   0x00       // session switch via mouse            
#define EVENT_CTRLBRK    0x01       // control break                       
#define EVENT_CTRLC      0x02       // control C                           
#define EVENT_CTRLNUMLK  0x03       // control num lock                    
#define EVENT_CTRLPRTSC  0x04       // control printscreen                 
#define EVENT_SHFTPRTSC  0x05       // shift printscreen                   
#define EVENT_SM_KBD     0x06       // session switch hot key              

// defines for 1.x movedata function 

#define   MOVE_PHYSTOPHYS   0       // move bytes from phys to phys memory 
#define   MOVE_PHYSTOVIRT   1       // move bytes from phys to virt memory 
#define   MOVE_VIRTTOPHYS   2       // move bytes from virt to phys memory 
#define   MOVE_VIRTTOVIRT   3       // move bytes from virt to virt memory 

// Micro Channel specific 

int NP GetLIDEntry (USHORT, USHORT, USHORT, FPUSHORT);
int NP FreeLIDEntry (USHORT);
int NP ABIOSCall (USHORT, USHORT, FARPOINTER);
int NP ABIOSComm (USHORT, FARPOINTER);
int NP GetDeviceBlock(USHORT, FARPOINTER);

// special routines 

void NP INT3  (void);
void NP Enable  (void);
void NP Disable  (void);
void NP Abort  (void);
int  NP SegLimit (SEL, OFF far *);
int  NP MoveBytes (FARPOINTER,FARPOINTER,FLAG);
int  NP MoveData (FARPOINTER, FARPOINTER, USHORT, USHORT);

// system services and misc. 

int  NP GetDOSVar (USHORT, FPFARPOINTER);
int  NP SendEvent (USHORT, USHORT);
void NP SchedClockAddr (PFARPOINTER);
int  NP AttachDD (PSTRING, PATTACHAREA);
int  NP InternalError(PSTRING,USHORT);
int  NP SaveMessage(FPSTRING);
int  NP ProtToReal(void);
int  NP RealToProt(void);
int  NP SetROMVector(USHORT,PFUNCTION,PFUNCTION,FARPOINTER);

// process mgmt 

void NP Yield  (void);
void NP TCYield  (void);
int  NP Block  (ULONG, ULONG, USHORT, FARPOINTER);
void NP Run  (ULONG);
void NP DevDone  (PREQPACKET);
int  NP VideoPause(USHORT);

// memory management 

int  NP AllocPhys (ULONG, USHORT, PPHYSADDR);
int  NP FreePhys (PHYSADDR);
int  NP VerifyAccess (SEL, OFF, USHORT, USHORT);
int  NP LockSeg  (SEL, USHORT, USHORT, PLHANDLE);
int  NP UnLockSeg (LHANDLE);

// address conversion 

int  NP AllocGDTSelector(USHORT, FARPOINTER);
int  NP PhysToGDTSelector(PHYSADDR, USHORT, SEL, PERRCODE);
int  NP VirtToPhys (FARPOINTER, PPHYSADDR);
int  NP PhysToUVirt (PHYSADDR, USHORT, USHORT, FPFARPOINTER);
int  NP PhysToVirt (PHYSADDR, USHORT, USHORT, FARPOINTER);
int  NP UnPhysToVirt (void);

// request packet queue stuff 

int  NP AllocReqPacket (USHORT, PPREQPACKET);
void NP FreeReqPacket (PREQPACKET);
void NP PushReqPacket (PQHEAD, PREQPACKET);
void NP SortReqPacket (PQHEAD, PREQPACKET);
int  NP PullReqPacket (PQHEAD, PPREQPACKET);
int  NP PullParticular  (PQHEAD, PREQPACKET);

// driver semaphores 

int  NP SemHandle (LHANDLE, FLAG, PLHANDLE);
int  NP SemRequest (LHANDLE, ULONG, PERRCODE);
void NP SemClear (LHANDLE);

// circular character queues 

void NP QueueInit (PCHARQUEUE);
void NP QueueFlush (PCHARQUEUE);
int  NP QueueWrite (PCHARQUEUE, UCHAR);
int  NP QueueRead (PCHARQUEUE, FPUCHAR);

// interrupt stuff 

int  NP SetIRQ  (USHORT, PFUNCTION, USHORT);
int  NP UnSetIRQ (USHORT);
int  NP EOI  (USHORT);
void NP ClaimInterrupt(void);
void NP RefuseInterrupt(void);
int  NP RegisterStackUsage(PREGSTACK);

// timer stuff 

int  NP SetTimer (PFUNCTION);
int  NP ResetTimer (PFUNCTION);
int  NP TickCount (PFUNCTION, USHORT);

// device monitors 

int  NP MonCreate (PSHANDLE, FARPOINTER, FARPOINTER, PERRCODE);
int  NP Register (SHANDLE, USHORT, PID, FARPOINTER, OFF, PERRCODE);
int  NP MonWrite (SHANDLE, POINTER, USHORT, USHORT, ULONG, PERRCODE);
int  NP MonFlush (SHANDLE, PERRCODE);
int  NP DeRegister (SHANDLE, PID, PERRCODE);

// 2.0  specfic 

int  NP RegisterPDD(FPUCHAR,FPFUNCTION);       
int  NP RegisterBeep(FPFUNCTION);      
int  NP Beep(USHORT,USHORT);              
int  NP FreeGDTSelector(USHORT);   
int  NP PhysToGDTSel(PHYSADDR,ULONG,SEL,USHORT,FPUSHORT);      
int  NP VMLock(LINADDR,ULONG,LINADDR,LINADDR,ULONG,FPULONG);            
int  NP VMUnlock(LHANDLE);          
int  NP VMAlloc(PLINADDR,ULONG,ULONG,PLINADDR);           
int  NP VMFree(PHYSADDR);            
int  NP VMProcessToGlobal(LINADDR,ULONG,ULONG,PLINADDR); 
int  NP VMGlobalToProcess(LINADDR,ULONG,ULONG,PLINADDR); 
int  NP VirtToLin(FARPOINTER,PLINADDR);         
int  NP LinToGDTSelector(SEL,LINADDR,ULONG);  
int  NP GetDescInfo(SEL,FPUSHORT,FPULONG,FPULONG);       
int  NP LinToPageList(LINADDR,ULONG,LINADDR,FPULONG);     
int  NP PageListToLin(ULONG,LINADDR,PLINADDR);     
int  NP PageListToGDTSelector(SEL,ULONG,LINADDR,USHORT,FPUSHORT);
int  NP RegisterTmrDD(FPFUNCTION,FPFARPOINTER,FPFARPOINTER);     
int  NP AllocateCtxHook(OFF,ULONG,PLHANDLE);   
int  NP FreeCtxHook(LHANDLE);       
int  NP ArmCtxHook(ULONG,LHANDLE,ULONG);        
int  NP VMSetMem(LINADDR,ULONG,ULONG);          
int  NP OpenEventSem(LHANDLE);      
int  NP CloseEventSem(LHANDLE);     
int  NP PostEventSem(LHANDLE);      
int  NP ResetEventSem(LHANDLE,FPULONG);     
int  NP DynamicAPI(FARPOINTER,USHORT,USHORT,FPUSHORT);

// these are the only API's available to the driver at Init time 

#define APIENTRY far pascal

USHORT APIENTRY DosBeep(USHORT, USHORT);
USHORT APIENTRY DosCaseMap(USHORT, FARPOINTER, FARPOINTER);
USHORT APIENTRY DosChgFilePtr(SHANDLE, long, USHORT, FARPOINTER);
USHORT APIENTRY DosClose(SHANDLE);
USHORT APIENTRY DosDelete(FARPOINTER, ULONG);
USHORT APIENTRY DosDevConfig(FARPOINTER, USHORT, USHORT);
USHORT APIENTRY DosDevIOCtl(FARPOINTER, FARPOINTER, USHORT, USHORT, USHORT);
USHORT APIENTRY DosFindClose(SHANDLE);
USHORT APIENTRY DosFindFirst(FARPOINTER, FARPOINTER, USHORT, FARPOINTER, 
        USHORT, FARPOINTER, ULONG);
USHORT APIENTRY DosFindNext(SHANDLE, FARPOINTER, USHORT, FARPOINTER);
USHORT APIENTRY DosGetEnv(FARPOINTER, FARPOINTER);
USHORT APIENTRY DosGetMessage(FARPOINTER, USHORT, FARPOINTER, USHORT, 
         USHORT, FARPOINTER, FARPOINTER);
USHORT APIENTRY DosOpen(FARPOINTER, FARPOINTER, FARPOINTER, ULONG, 
   USHORT, USHORT, USHORT, ULONG);
USHORT APIENTRY DosPutMessage(SHANDLE, USHORT, FARPOINTER);
USHORT APIENTRY DosQCurDir(USHORT, FARPOINTER, FARPOINTER);
USHORT APIENTRY DosQCurDisk(FARPOINTER, FARPOINTER);
USHORT APIENTRY DosQFileInfo(SHANDLE, USHORT, FARPOINTER, USHORT);
USHORT APIENTRY DosQFileMode(FARPOINTER, FARPOINTER, ULONG);
USHORT APIENTRY DosRead(SHANDLE, FARPOINTER, USHORT, FARPOINTER);
USHORT APIENTRY DosWrite(SHANDLE, FARPOINTER, USHORT, FARPOINTER);

// end of DRVLIB.H 
