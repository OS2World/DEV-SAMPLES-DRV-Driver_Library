//   OS/2 Device Driver for memory mapped I/O
//
//                Steve Mastrianni
//                15 Great Oak Lane
//                Unionville, CT 06085
//                (203) 693-0404 voice
//                (203) 693-9042 data
//                CI$ 71501,1652
//                BIX smastrianni
//                Internet 6099225@mcimail.com
//
//   This driver is loaded in the config.sys file with the DEVICE=
//   statement. For ISA configuration, the first parameter to the "DEVICE="
//   is the board base memory address in hex.
//
//   This driver also returns a boolean to the calling application to
//   inform it of the bus type (Micro Channel or ISA).
//
//   All numbers are in hex. For MCA configuration, the board address 
//   is read from the board POS regs. The POS regs data is specific for
//   each adapter, so the address calculations here may not work with 
//   your specific adapter. Refer to the hardware tech reference for the
//   particular adapter to determine where and how the address appears
//   in the POS registers.
//
//
//   This driver allows the application I/O to run in Ring 2 with IOPL.
//   The CONFIG.SYS files *must* contain the IOPL=YES statement.
//
//   This driver supports 4 IOCtls, Category 0x90.
//
//   IOCtl 0x01 test for MCA or ISA bus
//   IOCtl 0x02 gets and returns a selector to fabricated board memory
//   IOCtl 0x03 gets the value of a selected POS register
//   IOCtl 0x04 gets the board address that the driver found
//
//   The driver is made by using the make file mmap.mak.

#include "drvlib.h" 
#include "mmap.h"

extern void near STRATEGY();        // name of strat rout. in DDSTART      

DEVICEHDR devhdr = {
        (void far *) 0xFFFFFFFF,    // link                                 
        (DAW_CHR | DAW_OPN | DAW_LEVEL1),// attribute                      
        (OFF) STRATEGY,             // &strategy                        
        (OFF) 0,                    // &IDCroutine                        
        "MMAP$   "
};

FPFUNCTION  DevHlp=0;               // storage area for DevHlp calls       
LHANDLE     lock_seg_han;           // handle for locking appl. segment    
PHYSADDR    appl_buffer=0;          // address of caller's buffer          
PREQPACKET  p=0L;                   // pointer to request packet           
ERRCODE     err=0;                  // error return                        
void        far *ptr;               // temp far pointer                    
USHORT      i,j;                    // general counters                    
PHYSADDR    board_address;          // base board address                  
USHORT      opencount;              // count of DosOpens                   
USHORT      savepid;                // save the caller's PID               
USHORT      cntr = 0;               // misc counter                        
USHORT      bus = 0;                // default ISA bus                     
REQBLK      ABIOS_r_blk;            // ABIOS request block                 
LIDBLK      ABIOS_l_blk;            // ABIOS LID block                     
USHORT      lid_blk_size;           // size of LID block                   
CARD        card[MAX_NUM_SLOTS+1];  // array for IDs and POS reg values    
CARD        *pcard;                 // pointer to card array               
USHORT      matches = 0;            // match flag for card ID              
POS_STRUCT  pos_struct;             // struct to get POS reg               
ADDR_STRUCT addr_struct;            // struct for passing addresses        
USHORT      chunk1,chunk2;          // temp variables for address calc     

char     arguments[64]={0};         // save command line args in dgroup    
char     NoMatchMsg[]  = " no match for selected Micro Channel card ID found.\r\n";
char     MainMsgMCA[]  = "\r\nOS/2 Micro Channel memory-mapped driver installed.\r\n";
char     MainMsgISA[]  = "\r\nOS/2 ISA bus memory-mapped driver installed.\r\n";

// prototypes 

int      hex2bin(char c);
USHORT   get_POS();
UCHAR    get_pos_data();
UCHAR    nget_pos_data();

// common entry point for calls to Strategy routines 

int main(PREQPACKET rp )
{
    void far *ptr;
    int far *pptr;
    PLINFOSEG liptr;                // pointer to local info seg           
    int i;
    ULONG addr;
    USHORT in_data;

    switch(rp->RPcommand)
    {
    case RPINIT:                    // 0x00                                

        // init called by kernel in protected mode ring 3 with IOPL 

        return Init(rp);

    case RPOPEN:                    // 0x0d                                

        // get current processes id 

        if (GetDOSVar(2,&ptr))
            return (RPDONE | RPERR | ERROR_BAD_COMMAND);

        // get process info 

        liptr = *((PLINFOSEG far *) ptr);

        // if this device never opened, can be opened by any process       

        if (opencount == 0)    // first time this device opened       
        {
            opencount=1;                 // set open counter                    
            savepid = liptr->pidCurrent; // save current process id   
        }
        else
            {
            if (savepid != liptr->pidCurrent) // another proc tried to open 
                return (RPDONE | RPERR | RPBUSY ); // so return error      
            ++opencount;            // bump counter, same pid              
        }
        return (RPDONE);

    case RPCLOSE:                   // 0x0e                                

        // get process info of caller 

        if (GetDOSVar(2,&ptr))
            return (RPDONE | RPERR | ERROR_BAD_COMMAND); // no info        

        // get process info from os/2 

        liptr= *((PLINFOSEG far *) ptr); // ptr to process info seg        

        // 
        // make sure that process attempting to close this device 
        // one that originally opened it and the device was open in
        // first place.
        //

        if (savepid != liptr->pidCurrent || opencount == 0)  
            return (RPDONE | RPERR | ERROR_BAD_COMMAND);

        // if an LDT selector was allocated, free it 

        PhysToUVirt(board_address,0x8000,2,&addr_struct.mapped_addr);
 
        --opencount;                // close counts down open counter      
        return (RPDONE);            // return 'done' status to caller      

    case RPREAD:                    // 0x04                                

        return(RPDONE);

    case RPWRITE:                   // 0x08                                

        return (RPDONE);

    case RPIOCTL:                   // 0x10                                

        if (rp->s.IOCtl.category != OUR_CAT) // only our category          
            return (RPDONE);

        switch (rp->s.IOCtl.function)
        {

		  // this IOCtl returns the bus type. If the type is Micro Channel
		  // the return is 0xff01. If ISA, the return is ff00

        case 0x01:                  // check if MCA or ISA
            return (RPDONE | RPERR | bus);

		  // this IOCtl maps an adapter memory to an LDT selector:offset,
		  // and sends it to the application for direct application reads
		  // and writes

        case 0x02:                  // send memory-mapped addr to app   

            // verify caller owns this buffer area 

            if(VerifyAccess(
            SELECTOROF(rp->s.IOCtl.buffer), // selector                    
            OFFSETOF(rp->s.IOCtl.buffer),   // offset                      
            8,                              // 8 bytes                     
            1) )                            // read write                  
                return (RPDONE | RPERR | ERROR_GEN_FAILURE);

            // lock the segment down temp 

            if(LockSeg(
            SELECTOROF(rp->s.IOCtl.buffer), // selector                    
            0,                              // lock < 2 sec                
            0,                              // wait for seg lock           
            (PLHANDLE) &lock_seg_han))      // handle returned             
                return (RPDONE | RPERR | ERROR_GEN_FAILURE);

           // map the board address to an LDT entry

           if ( PhysToUVirt(board_address,0x8000,1,&addr_struct.mapped_addr)) 
               return (RPDONE | RPERR | ERROR_GEN_FAILURE);

           // move data to users buffer 

           if(MoveBytes(
           &addr_struct,                   // source
           rp->s.IOCtl.buffer,             // dest                      
           8))                             // 8 bytes                     
               return (RPDONE | RPERR | ERROR_GEN_FAILURE);

           // unlock segment 

           if(UnLockSeg(lock_seg_han))
               return(RPDONE | RPERR | ERROR_GEN_FAILURE);

           return (RPDONE);

		  // this IOCtl demonstrates how an application program can get the
		  // contents of a Micro Channel Adapter's POS registers

        case 0x03:                  // get pos reg data                    

            // verify caller owns this buffer area 

            if(VerifyAccess(
            SELECTOROF(rp->s.IOCtl.buffer), // selector                    
            OFFSETOF(rp->s.IOCtl.buffer),   // offset                      
            6,                              // 6 bytes                     
            1) )                            // read write                  
                return (RPDONE | RPERR | ERROR_GEN_FAILURE);

            // lock the segment down temp 

            if(LockSeg(
            SELECTOROF(rp->s.IOCtl.buffer), // selector                    
            0,                              // lock < 2 sec                
            0,                              // wait for seg lock           
            (PLHANDLE) &lock_seg_han))      // handle returned             
                return (RPDONE | RPERR | ERROR_GEN_FAILURE);

            // move slot data to driver buffer 

            if(MoveBytes(
            (FARPOINTER) appl_buffer,       // source                      
            &pos_struct,                    // for pos data                
            6))                             // 6 bytes                     
                return (RPDONE | RPERR | ERROR_GEN_FAILURE);

            pos_struct.data = get_pos_data(pos_struct.slot,pos_struct.reg);

            // move POS reg data to users buffer 

            if(MoveBytes(
            &pos_struct,                    // for pos data                
            (FARPOINTER) appl_buffer,       // source                      
            6))                             // 6 bytes                     
                return (RPDONE | RPERR | ERROR_GEN_FAILURE);

            // unlock segment 

            if(UnLockSeg(lock_seg_han))

                return(RPDONE | RPERR | ERROR_GEN_FAILURE);

            return (RPDONE);

		  // this IOCtl is essentially the same as 0x02, except the
		  // user virtual address is mapped to a linear address in the
		  // process address range and then sent to the application. This
		  // save the SelToFlat and FlatToSel each time the pointer is
		  // referenced.

        case 0x04:                  // 32-bit memory-mapped addr to app   

            // verify caller owns this buffer area 

            if(VerifyAccess(
            SELECTOROF(rp->s.IOCtl.buffer), // selector                    
            OFFSETOF(rp->s.IOCtl.buffer),   // offset                      
            8,                              // 8 bytes                     
            1) )                            // read write                  
                return (RPDONE | RPERR | ERROR_GEN_FAILURE);

            // lock the segment down temp 

            if(LockSeg(
            SELECTOROF(rp->s.IOCtl.buffer), // selector                    
            0,                              // lock < 2 sec                
            0,                              // wait for seg lock           
            (PLHANDLE) &lock_seg_han))      // handle returned             
                return (RPDONE | RPERR | ERROR_GEN_FAILURE);

           // map the board address to an LDT entry

           if ( PhysToUVirt(board_address,0x8000,1,&addr_struct.mapped_addr)) 
               return (RPDONE | RPERR | ERROR_GEN_FAILURE);

			  // now convert it to a linear address

			  if (VirtToLin((FARPOINTER)addr_struct.mapped_addr,
								(PLINADDR)&addr_struct.mapped_addr))
               return (RPDONE | RPERR | ERROR_GEN_FAILURE);

           // move data to users buffer 

           if(MoveBytes(
           &addr_struct,                   // source
           rp->s.IOCtl.buffer,             // dest                      
           8))                             // 8 bytes                     
               return (RPDONE | RPERR | ERROR_GEN_FAILURE);

           // unlock segment 

           if(UnLockSeg(lock_seg_han))
               return(RPDONE | RPERR | ERROR_GEN_FAILURE);

           return (RPDONE);

        } // switch (rp->s.IOCtl.function 

    case RPDEINSTALL:               // 0x14                            

        return(RPDONE | RPERR | ERROR_BAD_COMMAND);

        // all other commands are ignored 

    default:
        return(RPDONE);

    }
}

int  hex2bin(char c)
{
 if(c < 0x3a)
  return (c - 48);
 else
  return (( c & 0xdf) - 55);
}

USHORT get_POS(USHORT slot_num,USHORT far *card_ID,UCHAR far *pos_regs)
{
USHORT rc, i, lid;
    
    if (GetLIDEntry(0x10, 0, 1, &lid)) // get LID for POS   
        return (1);

    // Get the size of the LID request block 

    ABIOS_l_blk.f_parms.req_blk_len = sizeof(struct lid_block_def);
    ABIOS_l_blk.f_parms.LID = lid;
    ABIOS_l_blk.f_parms.unit = 0;;
    ABIOS_l_blk.f_parms.function = GET_LID_BLOCK_SIZE;
    ABIOS_l_blk.f_parms.ret_code = 0x5a5a;
    ABIOS_l_blk.f_parms.time_out = 0;

    if (ABIOSCall(lid,0,(void far *)&ABIOS_l_blk))
        return (1);

    lid_blk_size = ABIOS_l_blk.s_parms.blk_size; // Get the block size  

    // Fill POS regs and card ID with FF in case this does not work          

    *card_ID = 0xFFFF;
    for (i=0; i<NUM_POS_BYTES; i++) { pos_regs[i] = 0x00; }; 

    // Get the POS registers and card ID for the commanded slot 

    ABIOS_r_blk.f_parms.req_blk_len = lid_blk_size;
    ABIOS_r_blk.f_parms.LID = lid;
    ABIOS_r_blk.f_parms.unit = 0;;
    ABIOS_r_blk.f_parms.function = READ_POS_REGS_CARD;
    ABIOS_r_blk.f_parms.ret_code = 0x5a5a;
    ABIOS_r_blk.f_parms.time_out = 0;
    
    ABIOS_r_blk.s_parms.slot_num = (UCHAR)slot_num & 0x0F;
    ABIOS_r_blk.s_parms.pos_buf = (void far *)pos_regs;
    ABIOS_r_blk.s_parms.card_ID = 0xFFFF;
    
    if (ABIOSCall(lid,0,(void far *)&ABIOS_r_blk))
       rc = 1;
     else {                                       // Else                 
       *card_ID = ABIOS_r_blk.s_parms.card_ID;   //    Set the card ID value     
       rc = 0;
      }
    FreeLIDEntry(lid);
    return(rc);
    
}

UCHAR get_pos_data (int slot, int reg)
{
   UCHAR pos;
   CARD *cptr;

   cptr = &card[slot-1];            // set pointer to beg of card array    
   if (reg == 0)                    // card ID                             
      pos = LOUSHORT(cptr->card_ID);
   else
     if ( reg == 1)
      pos = HIUSHORT(cptr->card_ID);
   else
      pos = cptr->pos_regs[reg-2];  // POS data register                   
   return (pos);
}

// Device Initialization Routine 

int Init(PREQPACKET rp)
{
    USHORT lid;

    register char far *p;

    // store DevHlp entry point 

    DevHlp = rp->s.Init.DevHlp;  // save DevHlp entry point             

    if (!(GetLIDEntry(0x10, 0, 1, &lid))) { // get LID for POS   
       FreeLIDEntry(lid);

  // Micro Channel (tm) setup section 

  bus = 1;                      // MCA bus                             

      //    Get the POS data and card ID for each of 8 possible slots 

      for (i=0;i <= MAX_NUM_SLOTS; i++) 
         get_POS(i+1,(FARPOINTER)&card[i].card_ID,(FARPOINTER)card[i].pos_regs);

      matches = 0;
      for (i=0, pcard = card; i <= MAX_NUM_SLOTS; i++, pcard++) {
         if (pcard->card_ID == TARGET_ID) { 
            matches = 1;
            break;
            }
         }

      if (matches == 0) {           // at least one board found          
   DosPutMessage(1, 8, devhdr.DHname);
   DosPutMessage(1,strlen(NoMatchMsg),NoMatchMsg);
     rp->s.InitExit.finalCS = (OFF) 0;
   rp->s.InitExit.finalDS = (OFF) 0;
   return (RPDONE | RPERR | ERROR_BAD_COMMAND);
   }

      // calculate the board address from the POS regs 

    board_address = ((unsigned long) get_pos_data(i+1, 4) << 16) |
    ((unsigned long)(get_pos_data(i+1, 3) & 1) << 15);
  }

  else

  // ISA bus setup 

  {
  bus = 0;                      // ISA bus                             

  // get parameters, IRQ (not used yet), port addr and base mem addr 
  
  for (p = rp->s.Init.args; *p && *p != ' ';++p);// skip driver name 
  for (; *p == ' '; ++p);       // skip blanks following driver name   
  if (*p)
   {
         board_address=0;           // i/o port address                    
   for (; *p != '\0'; ++p)    // get board address                   
    board_address = (board_address << 4) + (hex2bin(*p));
   addr_struct.board_addr = board_address;
   }
  }

  if (bus)
         DosPutMessage(1,strlen(MainMsgMCA),MainMsgMCA);
      else
         DosPutMessage(1,strlen(MainMsgISA),MainMsgISA);

  // send back our cs and ds end values to os/2 
        
  if (SegLimit(HIUSHORT((void far *) Init), &rp->s.InitExit.finalCS) ||
     SegLimit(HIUSHORT((void far *) MainMsgISA), &rp->s.InitExit.finalDS))
       Abort();
  
  Beep(200,500);
  Beep(200,500);
  Beep(250,500);
  Beep(300,500);
  Beep(250,500);
  Beep(300,500);

  return (RPDONE);
  
}
