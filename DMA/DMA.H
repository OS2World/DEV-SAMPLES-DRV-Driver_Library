/* mmap.h DMA Channel data structure */

typedef struct _DMACh {
   UCHAR  Filler;	      /* force all fields aligned	*/
			      /* boundaries			*/
   UCHAR  PageSelect;         /* page select		        */
   USHORT BaseAddress;        /* base address	            	*/
   USHORT WordCount;          /* word count	              	*/
    } DMACh;

/* DMA Channel 5 */

#define DMA_PAGE_SELECT_5	   0x8B
#define DMA_BASE_ADDRESS_5	   0xC4
#define DMA_WORD_COUNT_5	   0xC6

/* DMA Channel 6 */

#define DMA_PAGE_SELECT_6	   0x89
#define DMA_BASE_ADDRESS_6	   0xC8
#define DMA_WORD_COUNT_6	   0xCA

/* DMA Channel 7 */
#define DMA_PAGE_SELECT_7	   0x8A
#define DMA_BASE_ADDRESS_7	   0xCC
#define DMA_WORD_COUNT_7	   0xCE

/* Other DMA Registers */

#define DMA_REFRESH_CHANNEL	   0x8F
#define DMA_MASK_REGISTER	   0xD4
#define DMA_MODE_REGISTER	   0xD6
#define DMA_BYTE_POINTER_FLIPFLOP  0xD8
#define DMA_MASTER_RESET	   0xDA
#define DMA_RESET_MASK_REGISTER    0xDC

/* DMA Mode Flag Bit Definitions */

#define DMA_WRITE     0x04    /* write transfer          */
#define DMA_READ	      0x08    /* read transfer	           */
#define DMA_AUTOINIT  0x10    /* autoinit enabled        */
#define DMA_DECREMENT	 0x20    /* address dec selected    */
#define DMA_SINGLE    0x40    /* SINGLE mode selected    */
#define DMA_BLOCK     0x80    /* BLOCK mode selected     */
#define DMA_CASCADE   0xC0    /* CASCADE mode selected   */
 
