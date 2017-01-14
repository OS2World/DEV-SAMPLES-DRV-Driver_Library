/*   file pio.c  */
  
/****************************************************************/
/*  sample parallel port VDD init section                       */
/****************************************************************/

#include "mvdm.h"                       /* VDH services, etc.   */
#include "pio.h"                        /* PIO  data defines    */

#pragma entry (_PIOInit)

#pragma data_seg(CSWAP_DATA)

extern  SZ szProplpt1timeout;

#pragma alloc_text(CINIT_TEXT,_PIOInit,PIO_PDDProc)

/* init entry point called by system at load time */

BOOL EXPENTRY _PIOInit(psz)         /* PIO VDDInit               */
{

    /* Register a VDM termination handler entry point*/

    if ((VDHInstallUserHook((ULONG)VDM_TERMINATE,
                            (PUSERHOOK)PIOTerminate)) == 0)
       return 0;          /* return FALSE if VDH call failed */

    /* Register a VDM creation handler entry point */

    if ((VDHInstallUserHook((ULONG)VDM_CREATE,
                            (PUSERHOOK)PIOCreate)) == 0)
        return 0 ;        /* return FALSE if VDH call failed */

    /* Get the entry point to the PDD */

    PPIOPDDProc = VDHOpenPDD(PDD_NAME, PIO_PDDProc);

    return CTRUE;
}

/* entry point registered by VDHOpenPDD, called by the PDD            */

SBOOL VDDENTRY PIO_PDDProc(ulFunc,f16p1,f16p2)
ULONG ulFunc;
F16PVOID f16p1;
F16PVOID f16p2;
{
    return 0;
}
