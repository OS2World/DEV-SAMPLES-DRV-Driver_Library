#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INCL_BASE
#define	INCL_DOS
#define INCL_DOSDEVIOCTL
#define INCL_DOSMONITORS
#include <os2.h>

typedef SHORT HFILE16;
typedef HFILE16 *PHFILE16;

#define	PLACE_DONT_CARE		0
#define	PLACE_BEGIN_CHAIN	1
#define	PLACE_END_CHAIN		2

#define	PARALLEL_DATA_CHAIN	1
#define	PARALLEL_CODE_CHAIN	2

#define	WAIT_ENABLE			0
#define	WAIT_DISABLE		1

#define	MONFLAG_JOB_TITLE	0x2000
#define	MONFLAG_RESERVED	0x1000
#define	MONFLAG_STATUS		0x0800
#define	MONFLAG_CODE_PAGE	0x0400
#define	MONFLAG_BUFCMD		0x0200
#define	MONFLAG_FONT		0x0100
#define	MONFLAG_FLUSH		0x0004
#define	MONFLAG_CLOSE		0x0002
#define	MONFLAG_OPEN		0x0001

typedef struct
{
	USHORT	usMonitorFlag;
	USHORT	usProcessId;
	UCHAR	uchMonitorData[4096];
} MONITORPACKET, *PMONITORPACKET;

UCHAR	szPipeFileName[128];
UCHAR	szPrintFileName[128];
UCHAR	szSpoolFileName[128];
UCHAR	uchChainInput[4096];
UCHAR	uchChainOutput[4096];
UCHAR	uchSpoolReply[128];
ULONG	ulSpoolFileId;
ULONG	ulVerbose;

MONITORPACKET MonitorPacket;

APIRET16 APIENTRY16 Dos16MonOpen(PSZ,PHFILE16);
APIRET16 APIENTRY16 Dos16MonClose(HFILE16);
APIRET16 APIENTRY16 Dos16MonReg(HFILE16,PUCHAR,PUCHAR,USHORT,USHORT);
APIRET16 APIENTRY16 Dos16MonRead(PUCHAR,USHORT,PMONITORPACKET,PUSHORT);
APIRET16 APIENTRY16 Dos16MonWrite(PUCHAR,PMONITORPACKET,PUSHORT);

main(int argc,char **argv)
{
	HFILE		hSpool;
	HFILE16		hMonitor16;
	PUCHAR		pMonitorData;
	APIRET		rcStatus;
	APIRET16	rcStatus16;
	ULONG		ulAction;
	ULONG		ulTemp;
	USHORT		usBytesRead;

	/* process command line arguments */

	for (ulTemp = 1; ulTemp < argc; ulTemp++)
		switch (argv[ulTemp][0])
		{
		case '-' :
			switch (argv[ulTemp][1])
			{
			case 'v' :
				sscanf(&argv[ulTemp][2],"%lu",&ulVerbose);
				break;
			default :
				fputs("Usage : devmon16 [-v#] [input port] [output pipe]\n",stderr);
				DosExit(EXIT_PROCESS,1);
				break;
			}
			break;

		default :
			if (szPrintFileName[0])
				strcpy(szPipeFileName,argv[ulTemp]);
			else
				strcpy(szPrintFileName,argv[ulTemp]);
			break;
		}

	/* copy default arguments */

	if (!szPipeFileName[0])
		strcpy(szPipeFileName,"\\PIPE\\SPOOL.PIP");

	if (!szPrintFileName[0])
		strcpy(szPrintFileName,"LPT1");

	/* open device monitor */

	rcStatus16 = Dos16MonOpen(szPrintFileName,&hMonitor16);

	if (rcStatus16)
	{
		printf("DosMonOpen failure rcStatus16 %u szFileName %s\n",rcStatus16,szPrintFileName);
		DosExit(EXIT_PROCESS,1);
	}

	if (ulVerbose)
		printf("DosMonOpen success hMonitor16 %lu\n",hMonitor16);

	/* register device monitor */

	*(USHORT *)uchChainInput = sizeof(uchChainInput);
	*(USHORT *)uchChainOutput = sizeof(uchChainOutput);

	rcStatus16 = Dos16MonReg(hMonitor16,uchChainInput,uchChainOutput,PLACE_END_CHAIN,PARALLEL_DATA_CHAIN);

	if (rcStatus16)
	{
		printf("DosMonReg failure rcStatus16 %u\n",rcStatus16);
		DosExit(EXIT_PROCESS,1);
	}

	if (ulVerbose)
		printf("DosMonReg success\n");

	while (1)
	{
		/* read device monitor */

		usBytesRead = sizeof(MONITORPACKET);

		rcStatus16 = Dos16MonRead(uchChainInput,WAIT_ENABLE,&MonitorPacket,&usBytesRead);

		if (rcStatus16)
		{
			printf("DosMonRead failure rcStatus16 %u\n",rcStatus16);
			DosExit(EXIT_PROCESS,1);
		}

		if (ulVerbose)
			printf("DosMonRead success usBytesRead %u usMonitorFlag %X usProcessId %u\n",
				usBytesRead,
				MonitorPacket.usMonitorFlag,
				MonitorPacket.usProcessId);

		/* if monitor open packet */

		if (MonitorPacket.usMonitorFlag & MONFLAG_OPEN)
		{
			if (hSpool)
			{
				DosClose(hSpool);
				hSpool = 0;
			}

			sprintf(szSpoolFileName,"\\SPOOL\\FAX\\%lu.tmp",ulSpoolFileId++);

			rcStatus = DosOpen(
						szSpoolFileName,			/* pszFileName			*/
						&hSpool,					/* ppFileHandle			*/
						&ulAction,					/* pActionTaken			*/
						0,							/* ulFileSize			*/
						FILE_NORMAL,				/* ulFileAttribute		*/
						OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_REPLACE_IF_EXISTS,
													/* ulOpenFlag			*/
						OPEN_ACCESS_WRITEONLY | OPEN_SHARE_DENYWRITE,
													/* ulOpenMode			*/
						NULL);						/* ppEABuf				*/

			if (rcStatus)
			{
				printf("DosOpen failure rcStatus %lu szFileName %s\n",rcStatus,szSpoolFileName);
				DosExit(EXIT_PROCESS,1);
			}

			if (ulVerbose)
				printf("Spool begin %s\n",szSpoolFileName);
		}

		/* if monitor close packet */

		if (MonitorPacket.usMonitorFlag & MONFLAG_CLOSE)
		{
			if (hSpool)
			{
				DosClose(hSpool);
				hSpool = 0;
			}

			if (ulVerbose)
				printf("Spool complete %s\n",szSpoolFileName);

			rcStatus = DosCallNPipe(
						szPipeFileName,					/* pszFileName		*/
						(PVOID)szSpoolFileName,			/* pInBuffer		*/
						sizeof(szSpoolFileName),		/* ulInBufferLen	*/
						(PVOID)uchSpoolReply,			/* pOutBuffer		*/
						sizeof(uchSpoolReply),			/* ulOutBufferLen	*/
						&ulTemp,						/* pBytesOut		*/
						180*1000);						/* ulTimeOut		*/

			if (rcStatus)
				printf("DosCallNPipe failure rcStatus %lu szFileName %s\n",rcStatus,szPipeFileName);
		}

		/* if monitor data packet */

		if (MonitorPacket.usMonitorFlag == 0 && usBytesRead > 4)
		{
			if (ulVerbose > 1)
			{
				for (ulTemp = 4, pMonitorData = MonitorPacket.uchMonitorData; ulTemp < usBytesRead; ulTemp++, pMonitorData++)
					printf("%02X ",*pMonitorData);
				putchar('\n');
			}

			if (hSpool)
			{
				rcStatus = DosWrite(hSpool,MonitorPacket.uchMonitorData,usBytesRead-4,&ulTemp);

				if (rcStatus)
				{
					printf("DosWrite failure rcStatus %lu ulBufferLength %lu ulBytesWritten %lu\n",rcStatus,usBytesRead-4,ulTemp);
					DosExit(EXIT_PROCESS,1);
				}
			}
		}
	}

	/* close device monitor */

	rcStatus16 = Dos16MonClose(hMonitor16);

	if (rcStatus16)
	{
		printf("DosMonClose failure rcStatus16 %u\n",rcStatus16);
		DosExit(EXIT_PROCESS,1);
	}

	if (ulVerbose)
		printf("DosMonClose success\n");
}
