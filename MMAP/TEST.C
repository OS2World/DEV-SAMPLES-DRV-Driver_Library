#define      INCL_DOSFILEMGR
#define      INCL_DOS
#define      INCL_DOSDEVICES
#define      INCL_DOSDEVIOCTL
#include     <os2.h>
#include     <stdio.h>
#include     "test.h"
HFILE 	    driver_handle=0; 
USHORT       err;
UCHAR        far *myptr=0;
USHORT       ActionTaken;
USHORT       rc;
ULONG        FileSize=0;
USHORT       FileAttribute;
ULONG        Reserved=0L;
UCHAR        Data1[8]={0};
UCHAR        Data2=0;
PADDR_STRUCT paddr_ptr;

void main()
{

		  // open the driver

        if ((rc = DosOpen("MMAP$   ",
        &driver_handle,
        &ActionTaken,
        FileSize,
        FileAttribute,
        FILE_OPEN,
        OPEN_SHARE_DENYNONE | OPEN_FLAGS_FAIL_ON_ERROR | OPEN_ACCESS_READWRITE,
        Reserved)) !=0) 
		  {
            printf("\nDosOpen failed, error = %d",rc);
				DosExit(EXIT_PROCESS,0);
		  }
 
		  printf ("Bus Type              = ");

        rc = DosDevIOCtl(&Data1,&Data2,0x01,OUR_CAT,driver_handle);
		  
		  if (rc & 0x01)
		     printf ("Micro Channel (tm)\n");
		  else
		     printf ("ISA\n");

        if (rc = DosDevIOCtl(&Data1,&Data2,0x02,OUR_CAT,driver_handle))
		  {
		     printf ("DevIOCtl failed, error code = %d\n",rc);
			  DosExit(EXIT_PROCESS,0);
		  }

		  // pointer to data buffer

		  paddr_ptr = (PADDR_STRUCT) Data1;

		  printf ("Memory Mapped Address = %p\nPhysical Address      = %lx\n",
		         paddr_ptr->mapped_addr,paddr_ptr->board_addr);

		  myptr = (void far *) paddr_ptr->mapped_addr;

		  printf ("First Byte Of Adapter = %x\n",*myptr);

		  // close driver

		  DosClose(driver_handle);
}
