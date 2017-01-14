/*
  ABIOS specific includes
*/

#define  POS_BASE 0x100             /* MCA adapter base       */
#define  NUM_POS_BYTES 64           /* maximum num POS bytes  */
#define  MAX_NUM_SLOTS 8            /* model 80 8 slots       */
#define  POS_PORT 0x96              /* use this to enable POS */
#define  POS_BASE 0x100             /* all POS regs start here*/
               
/* Constants used by ABIOS calls */

#define GET_LID_BLOCK_SIZE  0x01    /* ABIOS command          */
#define POS_LID             0x10    /* get POS LID from ABIOS */
#define READ_POS_REGS_RAM   0x0B    /* read POS from NVRAM    */
#define WRITE_POS_REGS_RAM  0x0C    /* write NVRAM POS data   */
#define READ_POS_REGS_CARD  0x0D    /* read POS data from card*/
#define WRITE_POS_REGS_CARD 0x0E    /* write POS data to card */

/* ABIOS request function parameters */

typedef struct function_parms_def {
   USHORT    req_blk_len;           /* length, must be init.  */
   USHORT    LID;                   /* the LID                */
   USHORT    unit;                  /* unit within a LID      */
   USHORT    function;              /* category of request    */
   USHORT    resvd1;                /* reserved               */
   USHORT    resvd2;                /* reserved               */
   USHORT    ret_code;              /* return code            */
   USHORT    time_out;              /* timeout in seconds     */
   } function_parms_type;

typedef struct service_parms_def {
   UCHAR     slot_num;              /* 10h slot number        */
   UCHAR     resvd3;                /* 11h reserved           */
   USHORT    card_ID;               /* 12h card ID            */
   USHORT    resvd4;                /* 14h reserved           */
   UCHAR     far *pos_buf;          /* 16h address of buffer  */
   USHORT    resvd5;                /* 1Ah reserved           */
   USHORT    resvd6;                /* 1Ch reserved           */
   UCHAR     resvd7[40];            /* 1Eh work area          */
   } service_parms_type;

/* LID request parameters */

typedef struct lid_service_parms_def {
   UCHAR     irpt_level;            /* 10h interrupt level    */
   UCHAR     arb_level;             /* 11h arbitration level  */
   USHORT    device_id;             /* 12h device ID          */
   USHORT    unit_count;            /* 14h count of units     */
   USHORT    flags;                 /* 16h LID flags          */
   USHORT    blk_size;              /* 18h req blk length     */
   USHORT    secnd_id;              /* 1Ah secondary dev ID   */
   USHORT    resvd6;                /* 1Ch reserved           */
   USHORT    resvd7;                /* 1Eh reserved           */
   } lid_service_parms_type;

/* complete request block */

typedef struct req_block_def {
   function_parms_type f_parms;
   service_parms_type  s_parms;
   } REQBLK;

/* complete LID block */

typedef struct lid_block_def {
   function_parms_type     f_parms;
   lid_service_parms_type  s_parms;
   } LIDBLK;

/* card struct, contains ID and POS reg data */

typedef struct card_def {
  USHORT     card_ID;               /* ID of the card slot    */
  UCHAR      pos_regs[NUM_POS_BYTES];
  } CARD;


