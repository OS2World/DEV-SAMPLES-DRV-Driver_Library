/* pioout.c	*/

#include "mvdm.h"                       /* VDH services, etc.   */
#include "pio.h"                        /* PIO specific         */

#pragma data_seg(CSWAP_DATA)

extern IOH Ioh;

#pragma alloc_text(CSWAP_TEXT,PIODataOut)

/* this routine is the data out trap entry point */

VOID HOOKENTRY PIODataOut(BYTE chartowrite,ULONG portaddr,PCRF pcrf)
{
	 RequestDirect();

	 /* disable port trapping */

	 VDHSetIOHookState(current_VDM,DIGIO_BASE,3,&Ioh,0);
 
    outp(portaddr,chartowrite);      /*  write the char */     
    return;
}

