/* allocate space for a GDT selector during INIT */

 if (AllocGDTSelector (1, sel_array)) 	/* allocate a GDT sel  */
 {
	DosPutMessage(1, 8, devhdr.DHname);
	DosPutMessage(1,strlen(GDTFailMsg),GDTFailMsg);
	break;
 }

 /* map the board memory address to the GDT selector */

 else
 
 if (PhysToGDTSelector (board_address, (USHORT) MEMSIZE, 
     sel_array[0], &err))
 {
	DosPutMessage(1, 8, devhdr.DHname);
	DosPutMessage(1,strlen(SELFailMsg),SELFailMsg);
      break;
 }


