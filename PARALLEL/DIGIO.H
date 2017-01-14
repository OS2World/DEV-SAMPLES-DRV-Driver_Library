/*
 digio.h memory map for os/2 device driver
*/

#define  DIGIO_CAT    0x91      /* category for DosDevIOCtl     */
#define  DIGIO_BASE   0x2c0     /* board address                */
#define  DIGIO_OUTPUT DIGIO_BASE /* output port                 */
#define  DIGIO_INPUT  DIGIO_BASE+1 /* input port                */
#define  DIGIO_CONFIG DIGIO_BASE+3 /* initialization port       */

