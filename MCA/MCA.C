USHORT get_POS(USHORT slot_num,USHORT far *card_ID,UCHAR far *pos_regs)
{
USHORT rc, i, lid;

if (GetLIDEntry(0x10, 0, 1, &lid))  /* POS LID */
    return (1);

/* Get the size of the LID request block */

ABIOS_l_blk.f_parms.req_blk_len=sizeof(struct
   lid_block_def);
ABIOS_l_blk.f_parms.LID = lid;
ABIOS_l_blk.f_parms.unit = 0;;
ABIOS_l_blk.f_parms.function = GET_LID_BLOCK_SIZE;
ABIOS_l_blk.f_parms.ret_code = 0x5a5a;
ABIOS_l_blk.f_parms.time_out = 0;

if (ABIOSCall(lid,0,(void far *)&ABIOS_l_blk))
    return (1);

lid_blk_size = ABIOS_l_blk.s_parms.blk_size; 

/* Fill POS regs and card ID */

*card_ID = 0xFFFF;
for (i=0; i<NUM_POS_BYTES; i++) { pos_regs[i] =
    0x00; };

/* Get the POS registers and card ID for slot */

ABIOS_r_blk.f_parms.req_blk_len = lid_blk_size;
ABIOS_r_blk.f_parms.LID = lid;
ABIOS_r_blk.f_parms.unit = 0;;
ABIOS_r_blk.f_parms.function = READ_POS_REGS_CARD;
ABIOS_r_blk.f_parms.ret_code = 0x5a5a;
ABIOS_r_blk.f_parms.time_out = 0;

ABIOS_r_blk.s_parms.slot_num = (UCHAR)slot_num & 0x0F;
ABIOS_r_blk.s_parms.pos_buf = (void far * ) pos_regs;
ABIOS_r_blk.s_parms.card_ID = 0xFFFF;
if (ABIOSCall(lid,0,(void far *)&ABIOS_r_blk))
     rc = 1;
else {
     *card_ID = ABIOS_r_blk.s_parms.card_ID; 
     rc = 0;
     }
FreeLIDEntry(lid);
return(rc);
}


