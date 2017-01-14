Driver Library Version 2.2 01 May 1993 (c) Personal Systems, Inc.

To all registered users:

1) Please note our mailing address and phone numbers:

   Personal Systems Software, Inc.
   15 Great Oak Lane
   Unionville, CT 06085

   How to contact us:

   Voice    (203) 693-0404
   FAX      (203) 693-9042
   CI$      71501,1652
   BIX      smastrianni
   Internet smastrianni@bix.com

2) As always, tech support can be had by calling our voice number, 9-5 EST.
   However, we prefer CompuServe so everyone else can benefit by a posting
   in the device Device Driver section of the OS2DF1 conference (GO OS2DF1).

   Bug fixes are always free. Please don't call with 
  
   1) questions on C programming (yes, we get tons of these) 

   2) how to run the compiler and/or linker

   3) how to set up the keyboard debugger, this is explained in the IBM
      toolkit documentation. Also, don't call us with questions on how
      to wire the cable. Use a simple cable with transmit, receive and
      ground, with *no* handshake pins wired.

   4) why you can't write your drivers with C Set/2, or why IBM does not
      supply an assembler.

3) Please let me know if you come across any bugs, or suggestions on how
   we can improve the package.

4) This release fixed a several bugs since version 1.7. The PhysToUVirt 
   DevHlp caused a fatal error if the selector was released in the Init
   routine. ES was pointing to the memory, and once released, popping 
   ES caused a GP fault. You probably never encountered this, as once
   an LDT entry is created, its usually not released until a close.

5) The companion disk now includes a C Set/2 device monitor program, a kernel
   printf function, a complete memory-mapped device driver for ISA and Micro
   Channel, and IOPL routines with 16-bit and 32-bit calling programs. 

6) The VirtToLin function was rewritten. All you have to pass it is the
   virtual address, not a separate selector and offset. The offset is
   moved properly and zero extended.

7) MoveBytes would fail if the status of the request packet was accessed
   just following a MoveBytes operation. The SI register was not being
   saved and restored correctly. This is now fixed.

8) The capabilities bit strip info in the DRVLIB.H has been updated to
   reflect the new CmdInit Complete strategy command.

   Thanks.


   Steve Mastrianni

=============================================================================


The following is errata for Writing OS/2 2.0 Device Drivers in C as
of 29 May, 1992.


Page 9, last paragraph, line 2, replace "Figure 3-2" with "Figure
2-3"

Page 16, first paragraph, replace entire paragraph with:

"Processors are generally referred to by the size of the data which
 can be handled at any one given time. The 8088 is referred to as a
 16-bit processor, although technically it can only handle 8 bits at
 a time (The 8088 performs 16-bit operations by transparently
 multiplexing two 8-bit operations). The 80286, for example, is
 called a 16-bit processor because it can handle data 16 bits at a
 time. The 80386 and 80486 are referred to as 32-bit processors
 because they can directly manipulate data 32 bits at a time."

Page 17, first paragraph, line 2, replace "eight-bit" with "16-bit"

Page 18, last paragraph, first sentence, replace "30" with "25".

Page 19, first paragraph, line 2, replace "25" with "33".

Page 19, last paragraph, third sentence which begins "The processor
speed", delete entire sentence.

Page 49, last paragraph, second sentence, replace "Figure 5-3" with
"Figure 5-5".

Pages 106, 107, 108, 109, 358, 360, 361, 362, in the MoveBytes 
function call, be sure to cast the "address of" pointer with
FARPOINTER. For example, on page 107, the example shows a call to
Movebytes that looks like this:

  if(MoveBytes(&input_char,rp->s.IOCtl.buffer,1))....

Change it to look like:

  if(MoveBytes((FARPOINTER)&input_char,rp->s.IOCtl.buffer,1))....

Do this for all eight pages listed above.

Page 308, Delete last paragraph, beginning with "Note", in its
entirety.

Page 310, SetIRQ Device Helper, IRQNumber should be a UCHAR, not
a USHORT.

Page 315, UnSetIRQ Device Helper, IRQNum should be a UCHAR, not
a USHORT

Page 318, VMAlloc Device Helper, the calling sequence should be:

if (VMAlloc(ULONG Physaddr,ULONG Size,ULONG Flags,PULONG Linaddr)) error;

Page 399, Listings, change "#define WRITE_POS_REG_CARD 0x0D" with
"#define WRITE_POS_REG_CARD 0x0E"

