/*  file piouser.c */

#include "mvdm.h"                         /* VDH services, etc.   */
#include "pio.h"                          /* PIO specific         */
#include "basemid.h"

#pragma data_seg(CSWAP_DATA)

/* our routines are for 8-bit ports */

IOH Ioh = {PIODataIn,PIODataOut,0,0,0};

#pragma alloc_text(CSWAP_TEXT,PIOCreate,PIOTerminate)

/*----------------------------------------------------------------

PIOCreate, entered when the VDM is created 

----------------------------------------------------------------*/

BOOL HOOKENTRY PIOCreate(hvdm)
HVDM hvdm;
{
    current_VDM = hvdm;				/* save our vdm handle */

	 /* install I/O hooks for our three 8-bit ports */

    if ((VDHInstallIOHook(hvdm,
                          DIGIO_BASE,
                          3,
                          (PIOH)&Ioh,
                          !VDH_ASM_HOOK)) == 0) {
        PIOTerminate(hvdm);
        return 0;             /* return FALSE               */
    } /* endif */

    return CTRUE;
}

/*----------------------------------------------------------------
 
PIOTerminate, called when the VDM terminates. This code is 
optional, as the User and IO hooks are removed automatically by 
the system when the VDM terminates. It is shown for example.

----------------------------------------------------------------*/

BOOL HOOKENTRY PIOTerminate(hvdm)
HVDM hvdm;
{

	 owner_VDM = 0;

    VDHRemoveIOHook(hvdm,         /* remove the IO hooks        */
                    DIGIO_BASE,
                    3,
                    (PIOH)&Ioh);

    return CTRUE;
}

