USHORT  SetupDMA(USHORT channel)
    {
    if(DMAChannelBusy(channel))
       return (DMA_CHANNEL_BUSY);
    MaskDMA(channel);
    SetDMAMode(channel,DMA_SINGLE | DMA_READ);
    InitDMA(channel,(UCHAR) DMACh.PageSelect,
       (USHORT) DMACh.BaseAddress,
       (USHORT) DMACh.WordCount);
    UnmaskDMA(channel);
    return (DMA_COMPLETE);
    }


void MaskDMA(USHORT channel)
{
UCHAR channel_mask;

/* output a channel specific value to mask a DMA channel */

switch (channel) {

    case 5:
      channel_mask = 5;
      break;

    case 6:
      channel_mask = 6;
      break;

    case 7:
      channel_mask = 7;
      break;
      }
  out8reg(DMA_MASK_REGISTER,channel_mask);
}


void SetDMAMode(USHORT channel,UCHAR mode)
{
unsigned char mode_byte;

/* output a channel specific value to unmask a DMA channel */

switch (channel) {

   case 5:
       mode_byte = mode | 0x01;
       break;

   case 6:
       mode_byte = mode | 0x02;
       break;

   case 7:
       mode_byte = mode | 0x03;
       break;
      }
   out8reg(DMA_MODE_REGISTER,mode_byte);
}


void InitDMA(USHORT channel,UCHAR page,USHORT address,
            USHORT count)
{
/* set up page select, addr, and cnt for specified channel */

switch (channel) {

   case 5:
      out8reg(DMA_PAGE_SELECT_5,page);
      out16reg(DMA_BASE_ADDRESS_5,address);
      out16reg(DMA_WORD_COUNT_5,count);
      break;

   case 6:
      out8reg(DMA_PAGE_SELECT_6,page);
      out16reg(DMA_BASE_ADDRESS_6,address);
      out16reg(DMA_WORD_COUNT_6,count);
      break;

   case 7:
      out8reg(DMA_PAGE_SELECT_7,page);
      out16reg(DMA_BASE_ADDRESS_7,address);
      out16reg(DMA_WORD_COUNT_7,count);
      break;
      }
}


void UnmaskDMA(USHORT channel)
{
unsigned char unmask_byte;

/* output a channel specific value to unmask a DMA channel */

switch (channel) {

case 5:
    unmask_byte = 1;
    break;

case 6:
    unmask_byte = 2;
    break;

case 7:
    unmask_byte = 3;
    break;
    }
 out8reg(DMA_MASK_REGISTER,unmask_byte);
}


USHORT DMAChannelBusy(USHORT ch)
{

  UCHAR ch_status;
  USHORT rc;

/* returns 0 if not busy, 1 if busy */

   ch_status = inp (DMA_STATUS_REG47)
   rc = 0;
   switch(ch) {
 
      case 5:
         if (ch_status & 0x20)
         rc = 1;
         break;

      case 6:
         if (ch_status & 0x40)
         rc = 1;
         break;

      case 7:
         if (ch_status & 0x80)
         rc = 1;
         break
      }
   return (rc);
}


