/* kprintf, kernel printf function */
/*
   This code is released to the public domain. You are free to modify
	and use it as you wish, but you may not charge anything for it.
*/


/* for kprintf(), define COLOR_SYSTEM if you are using a color adapter. If
   you are using a monochrome adapter, comment out the next #define.
   N.B. OS/2 2.0 can only run with a COLOR_SYSTEM, and thus this constant
   should always be defined. */
#define COLOR_SYSTEM

//#define DEBUG1		/* init */

/* most of the time when DEBUGx is defined, the debugging method is mainly
   print message to screen using kprintf(). In order to use kprintf(), 
   KERNEL_PRINTF must be defined. Note: if the DEBUGx does not use kprintf(),
   there is no need to include it in the following OR'ed list.
*/
#if defined(DEBUG1) || defined(DEBUG2) || defined(DEBUG3) || defined(DEBUG4)
#define KERNEL_PRINTF
#endif

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <conio.h>
#include <ctype.h>
#include "drvlib.h"

/* for kprintf */
#ifdef COLOR_SYSTEM
#define SCR_BASE	0xB8000L
#else
#define SCR_BASE	0xB0000L		/* monochrome */
#endif

/*******************************
*
	WARNING: Device Header must be the first data structure in the data
	segment. Make sure there is no variable declaration placed before
	this structure declaration. In addition, please make sure no auto-string
	initialization in this program. These string will be placed in the beginning
	of the data segment by the MS C compiler.

	All followings examples are on the allocation of "this is a test" in DS.
	The unacceptable forms are:
		char *message = "this is a test";
		string_pointer = "this is a test";

	Ok forms are:
		char message[] = "this is a test"
		kprintf("this is a test");

*
********************************/

/* devhdr here */

/*************************************
	Free to declare globals after this point.
*************************************/

FPFUNCTION DevHlp;
#ifdef KERNEL_PRINTF
SEL GDTsel;
char far *scr_ptr;
ULONG SCR_TOP, SCR_BOTTOM;
#endif

/* the following two variables are basically used by kprintf(). But other can
   use the buffer too, but have to keep in mind to block interrupt if necessary
*/
char buffer[120];
char digitbuf[16];

#ifdef KERNEL_PRINTF
void kprintf(char *, ...);
#endif

/*
	The C compiler will put any constant string in the first routine in the
	source file to the beginning of the data segment. This will displace the
	DEVHDR data structure from being placed in the very beginning of the
	data segment. For example, if main() is the first routine in the source
	file, the statement 'kprintf("this is a test");' will have the constant
	string being placed at ORG 0 of data segment by the compiler. On contrary,
	if ioctl() is the next routine, the same statement will have a very
	different behavior: the constant string will be placed following all the
	global defines, which is the way we want. The reason to define a dummy
	routine in here is to allow kprintf to be used in any routines without
	worrying the DEVHDR business.
*/
xxxxxxx(){}

main (PREQPACKET rp, int lindex)
{
	int rc;

	switch(rp->RPcommand) {
		case RPINIT:
			return(init(rp, lindex));

		default:
			return(RPDONE | RPERR | ERROR_BAD_COMMAND);

	} /* switch request packet */
} /* main */


init (PREQPACKET rp, int device)
{
	int rc;

	DevHlp = rp->s.Init.DevHlp;
	
#ifdef KERNEL_PRINTF
	/* map screen to LDT, as we can't use GDT at init time */
	PhysToUVirt(SCR_BASE, 0x1000, 1, &scr_ptr);
	SCR_TOP = (ULONG) scr_ptr;
	SCR_BOTTOM = (ULONG) scr_ptr + (80 * 22 * 2);
	AllocGDTSelector(1, &GDTsel);
	/* and now we can use kprintf() in INIT time */
#endif

	/* drvr ... */
	rc = DosPutMessage(1, sizeof(init_str1) - 1, init_str1);
	if (rc)
		init_abort(rp, errmsg1);

#ifdef DEBUG1
	kprintf("rc = %d, semaphore = %x:%x ", rc, SELECTOROF(IR_LS_semaphore),
		OFFSETOF(IR_LS_semaphore));
#endif

	/* ready to quit INIT */

#ifdef KERNEL_PRINTF
	/* free the screen pointer for kprintf() from LDT entry */
	PhysToUVirt((ULONG) scr_ptr & 0xFFFF0000UL, 0x1000, 2, &scr_ptr);
	/* then remap the same pointer to use GDT */
	/* the screen size is 80 * 25 then times 2 (char and attr) */
	PhysToGDTSelector(SCR_BASE, 4000, GDTsel, &error);
	scr_ptr = MAKEP(GDTsel, 80 << 1);
	SCR_TOP = (ULONG) scr_ptr;		/* brk up the statement just to avoid 
				C compiler warning. */
	SCR_BOTTOM = (ULONG) scr_ptr + (80 * 22 * 2);
#endif

	rp->s.InitExit.units = 0;		/* must be 0 for character device */
	SegLimit(HIUSHORT((void far *) init), &rp->s.InitExit.finalCS);
	SegLimit(HIUSHORT((void far *) init_str1), &rp->s.InitExit.finalDS);
	rp->s.InitExit.BPBarray = NULL;
	return(RPDONE);

} /* init */


init_abort (PREQPACKET rp, char *errmsg)
{
	int i;

	Beep(500,200);
	DosPutMessage(1, strlen(errmsg), errmsg);
	DosPutMessage(1, sizeof(init_err_final) - 1, init_err_final);

#ifdef KERNEL_PRINTF
	/* free the screen pointer for kprintf() from LDT entry */
	PhysToUVirt((ULONG) scr_ptr & 0xFFFF0000UL, 0x1000, 2, &scr_ptr);
	/* free GDT */
	FreeGDTSelector(GDTsel);
#endif
	rp->s.InitExit.finalCS = (OFF) 0;
	rp->s.InitExit.finalDS = (OFF) 0;

	RHT_INTR_INTV = 0;		/* this will make sure all sequence device's
			INIT will also return error and so the driver will not be
			installed. */
	return(RPDONE | RPERR | ERROR_GEN_FAILURE);

} /* init_abort */


#ifdef KERNEL_PRINTF
/**************************************

	kprintf() allow DD to print stuff on the screen while in kernel mode.
	It can't scroll screen. When pointer is almost at the bottom, output
	will be wrap back to the beginning of the screen.

	kprintf() takes only the %x, %d and %s formatter. Please make sure to
	use the right case (lower case). Note: since the DD can't assume SS
	as DGROUP, you cannot print %s from stack's value. In other words, 
	this routine will not print right if the string is on the stack.
	The string must be in the DS.

	Note: output starts from the second line on the screen as OS/2 will
	overprint the top line when cmd.exe takes control.

***************************************/
void kprintf (char *str, ...)
{
	char *ptr, *xptr;
	int far *argumentp;

	buffer[0] = 0;
	argumentp = (int far *) (((ULONG) (int far *) &str) + sizeof(int));
	ptr = str;
	while ((xptr = strchr(ptr, '%')) != (char *) NULL) {
		*xptr = 0;
		strcat(buffer, ptr);
		*xptr++ = '%';
		if (*xptr == 'x') {
			itoa(*argumentp++, digitbuf, 16);
			strcat(buffer, digitbuf);
		} else if (*xptr == 'd') {
			itoa(*argumentp++, digitbuf, 10);
			strcat(buffer, digitbuf);
		} else if (*xptr == 's') {
			strcat(buffer, (char *) *argumentp++);
		} else {
			strcat(buffer, "%");
			if (*xptr == 0) {	/* str ends in % with nothing follows */
				ptr = xptr;		/* so the strcat will cat nothing */
				break;
			}
		}
		ptr = xptr + 1;
	}
	strcat(buffer, ptr);

	/* print it out */
	/* disable intr while modifying global data */
	spl5();		/* cli */
	ptr = buffer;
	while (*ptr) {
		*scr_ptr = *ptr++;
		scr_ptr += 2;
		if ((ULONG) scr_ptr > SCR_BOTTOM)
			scr_ptr = (char far *) SCR_TOP;
	}
	splx();		/* sti */

} /* kprint */
#endif		/* KERNEL_PRINTF */

