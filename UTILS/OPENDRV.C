if ((RetCode=DosOpen("DIGIO$",
    &digio_handle,
    &ActionTaken,
    FileSize,
    FileAttribute,
    FILE_OPEN,
    OPEN_SHARE_DENYNONE | OPEN_FLAGS_FAIL_ON_ERROR
    | OPEN_ACCESS_READWRITE,Reserved)) !=0) 
    printf("\nopen error = %d",RetCode);

 
