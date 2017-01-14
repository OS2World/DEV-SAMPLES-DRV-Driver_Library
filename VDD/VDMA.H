/***	VDMA.H
 *
 *	MVDM Virtual DMA Exported Definitions
 *
 *	For include file hierarchy, see MVDM.H
 *
 */

/* VDMA defines */

#define VDD_DMA_MASKOFF     0	    /* Mask off notification to VDD */
#define VDD_DMA_MASK	    1	    /* Mask on	notification to VDD */

typedef BOOL (*HOOKENTRY PFNDMA)(HVDM,ULONG);

/* VDMA function prototypes */

BOOL VDHENTRY VDHRegisterDMAChannel(ULONG,PFNDMA);
VOID VDHENTRY VDHCallOutDMA(VOID);
