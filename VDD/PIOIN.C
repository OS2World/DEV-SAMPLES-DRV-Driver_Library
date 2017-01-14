/*  pioin.c */

#include "mvdm.h"                       /* VDH services, etc.   */
#include "pio.h"      
#include "basemid.h"
   
/* PIO specific         */

#pragma alloc_text(CSWAP_TEXT,PIODataIn,RequestDirect)

extern IOH Ioh;

/* entry from data input trap in VDM */

BYTE HOOKENTRY PIODataIn(ULONG portaddr, PCRF pcrf)
{
    BYTE dataread;                     /* set up byte to return */

	 VdhInt3();

	 RequestDirect();

	 /* disable I/O trap */

	 VDHSetIOHookState(current_VDM,DIGIO_BASE,3,&Ioh,0);

    dataread = inp(portaddr);	
    return(dataread);                  /* return data read      */
}

BOOL HOOKENTRY RequestDirect(void)
{
	if (owner_VDM != current_VDM)
	{
	  if (owner_VDM !=0)
	  {
	     VDHPopup(0,0,MSG_DEVICE_IN_USE,&Resp,ABORT,0);
		  if (Resp != ABORT)
		  {
		     VDHKillVDM(current_VDM);
			  owner_VDM = current_VDM;
		  }
	  }
	  else
	  owner_VDM = current_VDM;
	}
}
