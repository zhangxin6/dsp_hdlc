#include"c6678.h"
#include <c6x.h>
#include <stdio.h>
#include <cslr_gpio.h>
#include <csl_bootcfgAux.h>
#include "KeyStone_common.h"
#include "gpio_z.h"
#include "hdlc_cmu.h"
#define ADDR_HDLC_START   0x0700001ff
#define ADDR_HDLC_RECEIVE 0x0700001ff

void send_hdlc(unsigned int length, int * src_buf)
{
	unsigned short src_16[512];
	unsigned short i,length_16;
	for(i=0;i<length/2;i++)
	{
		src_16[i] = src_buf[2*i] + (src_buf[2*i+1]<<8);
	}
	if(length%2 ==1)
	{
		src_16[i] = src_buf[2*i];
	}
	length_16 = (length/2) + (length%2);

	C6678_Cache_Wb((Uint32 *)src_16,length_16,CACHE_WAIT);
	C6678_Emif16_Fpga_WriteByte(0,length_16,src_16);

	*((unsigned short*) ADDR_HDLC_START ) = length;
}

unsigned short receive_hdlc(int * dst_buf)
{
	unsigned short dst_16[512];
	unsigned short i,length_16,length;
	length = *((unsigned short*) ADDR_HDLC_RECEIVE );
	length = (length <= 512) ? length:512;
	length_16 = (length/2) + (length%2);
	C6678_Emif16_Fpga_ReadByte(0,length_16,dst_16);
	for(i=0;i<length/2;i++)
	{
		dst_buf[2*i]   = dst_16[i] & 0x00ff;
		dst_buf[2*i+1] = (dst_16[i] & 0xff00)>>8;
	}
	if(length%2 ==1)
	{
		dst_buf[2*i] = dst_16[i] & 0x00ff;
	}
	return length; 
}
