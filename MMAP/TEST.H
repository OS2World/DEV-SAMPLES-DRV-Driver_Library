//  include file for test.c 

#define  OUR_CAT  0x91               // category for DosDevIOCtl 
#define  DRIVER_BASE 0xD8000         // board address            
#define  BASE_LENGTH 0x1000			 // length of memory map     

typedef struct _ADDR_STRUCT {
	void     far *mapped_addr;
	ULONG    board_addr;
	} ADDR_STRUCT;
typedef ADDR_STRUCT far *PADDR_STRUCT;

