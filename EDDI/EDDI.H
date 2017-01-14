typedef struct _DRIVCAPSTRUCT {
   USHORT     reserved;
   UCHAR      VerMajor;    /* major version, should be 01 */
   UCHAR      VerMinor;    /* minor version, should be 01 */
   ULONG      Capabilities;/* capabilities bits           */
   PFUNCTION  Strategy2;   /* 16:16 pointer to STRAT2     */
   PFUNCTION  SetFSDInfo;  /* 16:16 pointer to SetFSDInfo */
   PFUNCTION  ChgPriority; /* 16:16 pointer to ChgPriority*/
   PFUNCTION  SetRestPos;  /* 16:16 pointer to RestPos    */
   PFUNCTION  GetBoundary; /* 16:16 pointer to GetBoundary*/
   } DRIVCAPSTRUCT;

typedef struct _VOLCHARSTRUCT {
   USHORT VolDescriptor;
   USHORT AvgSeekTime;
   USHORT AvgLatency;
   USHORT TrackMinBlocks;
   USHORT TrackMaxBlocks;
   USHORT Heads per cylinder
   } VOLCHARSTRUCT;

typedef struct _REQUESTLISTHEADER {
  USHORT     ReqListCount;
  USHORT     Reserved;
  FARPOINTER ListNotifyAddress;
  USHORT     ListRequestControl;
  UCHAR      BlkDevUnit;
  UCHAR      ListStatus;
  ULONG      Reserved1
  ULONG      Reserved2;
  } REQUESTLISTHEADER;

typedef struct _REQUESTHEADER {
   USHORT      ReqLength;
   UCHAR       CmdPrefix;
   UCHAR       CmdCode;
   ULONG       HeaderOffset;
   UCHAR       RequestCtl;
   UCHAR       Priority;
   UCHAR       Status;
   UCHAR       ErrorCode;
   FARPOINTER  NotifyAddress; 
   FARPOINTER  HintPointer;
   ULONG       Reserved1;
   ULONG       Reserved2;
   ULONG       Reserved3;
   } REQUESTHEADER;

typedef struct _SGD {
   PHYSADDR BufferPtr;
   ULONG    BufferSize;
   } SGD;

typedef struct _READWRITE {
   REQUESTHEADER ReadWriteHeader;
   ULONG         StartBlock;
   ULONG         BlockCount;
   ULONG         BlocksXferred;
   USHORT        Flags;
   USHORT        SGDescrCount
   ULONG         Reserved;
   SGD           Sgd[SGDescrCount];
   } READWRITE;

typedef struct _READPREFETCH {
  REQUESTHEADER ReadPreHdr;
  ULONG         StartBlock;
  ULONG         BlockCount;
  ULONG         BlocksXferred;
  USHORT        Flags;
  USHORT        Reserved;
  } READPREFETCH;

typedef struct _FSDInfo {
  ULONG       Reserved1;   /* reserved, must be 0          */
  FARPOINTER  EndOfInit;   /* pointer to FSD's EOI         */
  ULONG       Reserved2;   /* reserved, must be 0          */
  FARPOINTER  AccValidate; /* pointer to FSD's AccValidate */
  } FSDInfo;


