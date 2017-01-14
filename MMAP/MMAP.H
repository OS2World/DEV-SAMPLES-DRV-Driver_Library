/*
  include file for memory-mapped driver
*/

#define  OUR_CAT  0x91               /* category for DosDevIOCtl */
#define  MEMSIZE  32800              /* 32 K bytes per adapter   */
#define  POS_BASE 0x100              /* MCA adapter base         */
#define  TARGET_ID 0x6CFD            /* adapter ID               */
#define  NUM_POS_BYTES 64
#define  MAX_NUM_SLOTS 8
#define  MAX_DEV_NUMS 8
#define  MAX_NUM_DSPS 5
#define  READY  0xFFFF               /* dsp read                 */
#define  POS_PORT 0x96
#define  POS_BASE 0x100
               
/* Constants used by ABIOS calls */

#define GET_LID_BLOCK_SIZE 0x01
#define POS_LID            0x10
#define READ_POS_REGS      0x0B
#define READ_POS_REGS_RAM  0x0B
#define READ_POS_REGS_CARD 0x0D

typedef struct _POS_STRUCT {
	USHORT	slot;
	USHORT   reg;
   USHORT   data;
	} POS_STRUCT;
typedef POS_STRUCT far *PPOS_STRUCT;

typedef struct _ADDR_STRUCT {
	void     far *mapped_addr;
	ULONG    board_addr;
	} ADDR_STRUCT;
typedef ADDR_STRUCT far *PADDR_STRUCT;

typedef struct function_parms_def {
   USHORT    req_blk_len;
   USHORT    LID;
   USHORT    unit;
   USHORT    function;
   USHORT    resvd1;
   USHORT    resvd2;
   USHORT    ret_code;
   USHORT    time_out;
   } function_parms_type;

typedef struct service_parms_def {
   UCHAR     slot_num;        /* 10h */
   UCHAR     resvd3;          /* 11h */
   USHORT    card_ID;         /* 12h */
   USHORT    resvd4;          /* 14h */
   UCHAR     far *pos_buf;    /* 16h */
   USHORT    resvd5;          /* 1Ah */
   USHORT    resvd6;          /* 1Ch */
   UCHAR     resvd7[40];      /* 1Eh */
   } service_parms_type;

typedef struct lid_service_parms_def {
   UCHAR     irpt_level;          /* 10h */
   UCHAR     arb_level;           /* 11h */
   USHORT    device_id;       /* 12h */
   USHORT    unit_count;      /* 14h */
   USHORT    flags;           /* 16h */
   USHORT    blk_size;        /* 18h */
   USHORT    secnd_id;        /* 1Ah */
   USHORT    resvd6;          /* 1Ch */
   USHORT    resvd7;          /* 1Eh */
   } lid_service_parms_type;

typedef struct req_block_def {
   function_parms_type f_parms;
   service_parms_type  s_parms;
   } REQBLK;

typedef struct lid_block_def {
   function_parms_type     f_parms;
   lid_service_parms_type  s_parms;
   } LIDBLK;

typedef struct card_def {
  USHORT     card_ID;        /* ID of the card in this slot              */
  UCHAR      pos_regs[NUM_POS_BYTES];
  } CARD;


