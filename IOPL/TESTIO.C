//
//  testio.c - test IOPL functions 
//

#define INCL_DOS
#include <os2.h>

#define INPUT_PORT  0x2f8
#define OUTPUT_PORT 0x2f8
#define TEST_DATA   0x41

extern far pascal in_port();
extern far pascal out_port();

int main()
{
	 USHORT in_stuff;

	 in_stuff = in_port (INPUT_PORT);
	 out_port (OUTPUT_PORT,TEST_DATA);

}
