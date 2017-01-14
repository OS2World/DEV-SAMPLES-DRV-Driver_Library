#define INCL_DOS
#include <os2.h>

#define  EABUF       0L
#define  OUR_CAT  0x91L
#define  BUS_TYPE 0x01L
#define  GET_PTR  0x02L
#define  GET_POS  0x03L
#define  GET_LIN  0x04L

typedef struct _ADDR_STRUCT {
	void     *mapped_addr;
	ULONG    board_addr;
	} ADDR_STRUCT;
typedef ADDR_STRUCT *PADDR_STRUCT;


char     buf[100] = {0};
USHORT   BytesRead;
ULONG    ActionTaken;               /* for file opens                      */
APIRET   rc;                        /* return code for driver open         */
ULONG    FileSize=0;                /* NULL file size                      */
ULONG    FileAttribute;             /* attribute bits                      */
HFILE    handle=0;
UCHAR    parmbuf [20];
UCHAR    databuf[20];
ULONG    plength,dlength;
PADDR_STRUCT paddr_ptr;
UCHAR    *myptr;

main()
{
    rc = DosOpen("MMAP$   ",
    &handle,
    &ActionTaken,
    FileSize,
    FileAttribute,
    OPEN_ACTION_OPEN_IF_EXISTS,
    OPEN_ACCESS_READWRITE | OPEN_SHARE_DENYNONE | OPEN_FLAGS_NOINHERIT, 
    EABUF);
      if (rc) {
        printf("\nDosOpen failed, error = %ld",rc);
        DosExit(EXIT_PROCESS,0);    /* exit gracefully                     */
	   }

	 printf ("Bus Type              = ");

	 rc = DosDevIOCtl(handle,OUR_CAT,BUS_TYPE,0,0L,&plength,databuf,8L,&dlength);

	 if (rc & 0x01)
	    printf ("Micro Channel (tm)\n");
	 else
	    printf ("ISA\n");

	 rc = DosDevIOCtl(handle,OUR_CAT,GET_LIN,0,0L,&plength,databuf,8L,&dlength); 

    if (rc)
	 {
      printf ("DevIOCtl failed, error code = %ld\n",rc);
		DosExit(EXIT_PROCESS,0);
	 }	

    paddr_ptr = (PADDR_STRUCT) databuf;

	 printf ("Memory Mapped Address = %p\nPhysical Address      = %lx\n",
	        paddr_ptr->mapped_addr,paddr_ptr->board_addr);

	 myptr = paddr_ptr->mapped_addr; 

	 printf ("First Byte Of Adapter = %x\n",*myptr);

    DosClose(handle);

} 
