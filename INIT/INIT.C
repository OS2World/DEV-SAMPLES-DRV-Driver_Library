/* Ex.INIT section, combination ISA and MicroChannel bus driver
   
   This driver is loaded in the config.sys file with the DEVICE=
   statement. For ISA configuration, the first parameter to the
   "DEVICE=" is the base port address. The next parameter is the
   board base address. All numbers are in hex. For Micro Channel
   configuration, the board address and port address are read
   from the board POS regs.
*/

PHYSADDR    board_address;     /* base board address            */
USHORT      port_address;      /* base port address             */
USHORT      bus = 0;           /* default ISA bus               */
REQBLK      ABIOS_r_blk;       /* ABIOS request block           */
LIDBLK      ABIOS_l_blk;       /* ABIOS LID block               */
USHORT      lid_blk_size;      /* size of LID block             */
CARD        card[MAX_NUM_SLOTS+1];/* array for IDs and POS reg  */
CARD        *pcard;            /* pointer to card array         */
USHORT      matches = 0;       /* match flag for card ID        */
USHORT      port1,port2;       /* temp variables for addr calc  */

char     NoMatchMsg[]  = " no match for DESIRED card ID found.\r\n";
char     MainMsgMCA[]  = "\r\nOS/2 Micro Channel (tm) Device
Driver installed.\r\n";
char     MainMsg[] = "\r\nOS/2 ISA Device Driver installed.\r\n";

/* prototypes */

int      hex2bin(char);
USHORT   get_POS();
UCHAR    get_pos_data();
.
.
* Device Driver Strategy Section Here *
.
.
int  hex2bin(char c)
{
	if(c < 0x3a)
		return (c - 48);
	else
		return (( c & 0xdf) - 55);
}

USHORT get_POS(USHORT slot_num,USHORT far *card_ID,
       UCHAR far *pos_regs)
{
USHORT rc, i, lid;
    
    if (GetLIDEntry(0x10, 0, 1, &lid))	/* get LID for POS   */
        return (1);

    /* Get the size of the LID request block */

    ABIOS_l_blk.f_parms.req_blk_len = sizeof(struct lid_block_def);
    ABIOS_l_blk.f_parms.LID = lid;
    ABIOS_l_blk.f_parms.unit = 0;;
    ABIOS_l_blk.f_parms.function = GET_LID_BLOCK_SIZE;
    ABIOS_l_blk.f_parms.ret_code = 0x5a5a;
    ABIOS_l_blk.f_parms.time_out = 0;

    if (ABIOSCall(lid,0,(void far *)&ABIOS_l_blk))
        return (1);

    lid_blk_size = ABIOS_l_blk.s_parms.blk_size; /* Get blk siz*/

    /* Fill POS regs and card ID with FF in case               */

    *card_ID = 0xFFFF;
    for (i=0; i<NUM_POS_BYTES; i++) { pos_regs[i] = 0x00; }; 

    /* Get the POS registers and card ID for the commanded slot*/

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
     else {                                       /* Else      */
       *card_ID = ABIOS_r_blk.s_parms.card_ID;   /* Setcard    */
       rc = 0;
      }
    FreeLIDEntry(lid);
    return(rc);
    
}

UCHAR get_pos_data (int slot, int reg)
{
   UCHAR pos;
   CARD *cptr;

   cptr = &card[slot-1];            /* set ptr to beg of array */
   if (reg == 0)                    /* card ID                 */
      pos = LOUSHORT(cptr->card_ID);
   else
     if ( reg == 1)
      pos = HIUSHORT(cptr->card_ID);
   else
      pos = cptr->pos_regs[reg-2];  /* POS data register       */
   return (pos);
}

/* Device Initialization Routine */

int Init(PREQPACKET rp)
{
    USHORT lid;

    register char far *p;

    /* store DevHlp entry point */

    DevHlp = rp->s.Init.DevHlp;/* save DevHlp entry point      */

    if (!(GetLIDEntry(0x10, 0, 1, &lid))){/* get LID for POS   */
      FreeLIDEntry(lid);

      /* Micro Channel (tm) setup section */

      bus = 1;                      /* Micro Channel bus       */

      /*    Get the POS data and card ID for each of 8 slots   */

      for (i=0;i <= MAX_NUM_SLOTS; i++) 
         get_POS(i+1,(FARPOINTER)&card[i].card_ID,
            (FARPOINTER)card[i].pos_regs);

      matches = 0;
      for (i=0, pcard = card; i <= MAX_NUM_SLOTS; i++, pcard++){
         if (pcard->card_ID == DESIRED_ID) { 
            matches = 1;
            break;
            }
         }

      if (matches == 0) {           /* no matches found        */
         DosPutMessage(1, 8, devhdr.DHname);
         DosPutMessage(1,strlen(NoMatchMsg),NoMatchMsg);
         rp->s.InitExit.finalCS = (OFF) 0;
         rp->s.InitExit.finalDS = (OFF) 0;
         return (RPDONE | RPERR | ERROR_BAD_COMMAND);
         }

      /* calculate the board address from the POS regs */

      board_address = ((unsigned long) get_pos_data(i+1, 4)
      << 16) | ((unsigned long)(get_pos_data(i+1, 3) & 1) << 15);

      /* calculate the port address from the POS regs data */

      port1 = (get_pos_data(i+1, 3) << 8) & 0xf800;
      port2 = (get_pos_data(i+1, 2) << 3) & 0x07e0;
      port_address = (port1 | port2);

      }
   else
      {
      /* ISA bus setup */
      bus = 0;                      /* ISA bus                 */

    /* get parameters, port addr and base mem addr */

      for (p = rp->s.Init.args; *p && *p != ' ';++p);/* sk name*/
      for (; *p == ' '; ++p);       /* skip blanks after name  */
      if (*p)
       {
       port_address = 0;
       board_address=0;           /* i/o port address          */
       for (; *p != ' '; ++p)     /* get port address          */
       port_address = (port_address << 4) + (hex2bin(*p));
       for (; *p == ' '; ++p);    /* skip blanks after address */
       for (; *p != '\0'; ++p)    /* get board address         */
       board_address = (board_address << 4) + (hex2bin(*p));
       }
    }

    if (bus)
       DosPutMessage(1,strlen(MainMsgMCA),MainMsgMCA);
    else
       DosPutMessage(1,strlen(MainMsg),MainMsg);

       /* send back our end values to os/2 */
        
    if (SegLimit(HIUSHORT((void far *) Init),
        &rp->s.InitExit.finalCS) ||
        SegLimit(HIUSHORT((void far *) MainMsg),
        &rp->s.InitExit.finalDS))
        Abort();

    return (RPDONE);
}


