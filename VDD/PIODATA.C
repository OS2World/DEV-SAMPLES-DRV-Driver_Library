/* piodata.c */

#include "mvdm.h"                       /* VDH services, etc.   */
#include "pio.h"                        /* PIO specific         */

#pragma data_seg(SWAPINSTDATA)

HVDM owner_VDM = 0;                      /*  actual VDM handle   */
HVDM current_VDM;
ULONG Resp = 0;

#pragma data_seg(CSWAP_DATA)

FPFNPDD PPIOPDDProc = (FPFNPDD)0;       /* addr of PDD entry pt */

