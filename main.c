#include"c6678.h"
#include <c6x.h>
#include <stdio.h>
#include <cslr_gpio.h>
#include <csl_bootcfgAux.h>
#include "KeyStone_common.h"
#include "gpio_z.h"
#include "hdlc_cmu.h"
#include "init.h"
/********************************************************************
 * 可修改的全局变量
 *******************************************************************/
#define DEBUG
//记录速度
CSL_Uint64 average_write_speed=0;
CSL_Uint64 average_read_speed=0;
//DMA测试宏变量
#define DEVICE_HZ         1000000000//800MHZ:800000000//1GHZ:1000000000;1.25GHZ:1250000000
#define MB   		      0x100000
#define TEST_LOOP_COUNT   1000
#define KB   		      1024
/********************************************************************
 * emif16_fpga部分
 *******************************************************************/
#define EMIF16FPGA_SEND_BUF_ADDR  0x0C1D0000
#define EMIF16FPGA_RECV_BUF_ADDR  0x0C1E0000
//#define GPIO_LOOP_BACK_TEST 	1
void main(void)
{
    init();
	int coreNum=0;
	int * src_buf= (int *)EMIF16FPGA_SEND_BUF_ADDR;
    #if GPIO_LOOP_BACK_TEST
        GPIO_loopback_test();
    #else
        int j;
        if(DEVICE==DEVICE1)
        {
            if (coreNum ==0)
            {
                while(1)
                {
                    /*for(j=0;j<512;j++)
                    {
                        //src_buf[j] = 255;
                        src_buf[j] = (j + 3)%256;
                    }
                    send_hdlc(500,src_buf);*/
                    //int * dst_buf= (int *)EMIF16FPGA_RECV_BUF_ADDR;
                    //unsigned short length;
                    //length = receive_hdlc(dst_buf);
                    C6678_TimeCounter_Delaycycles(10000000);
                }
            }
        }
    #endif
}

//Interrupt service routine for timer
void interrupt GPIO_ISR(void)
{
	Uint32 eventFlag;
	unsigned int i,error;
	unsigned short length;
	int * dst_buf= (int *)EMIF16FPGA_RECV_BUF_ADDR;
	int * src_buf= (int *)EMIF16FPGA_SEND_BUF_ADDR;

	/*read interrupt flag*/
	eventFlag= gpCGEM_regs->MEVTFLAG[2];

	/*clear interrupt flag*/
	gpCGEM_regs->EVTCLR[2]= eventFlag;

	//shift the interrupt register to match the GPIO data register
	#ifdef DEBUG
		printf("GPIO interrupt event flag on CorePac INTC = 0x%x\n",((eventFlag&(~(1<<(CSL_GEM_GPINTN-64))))>>(10))|((eventFlag>>(CSL_GEM_GPINTN-64))<<DNUM));
	#else
		;
	#endif
	
	length = receive_hdlc(dst_buf);
	error =0;
	/*for(i=0;i<length;i++)
	{
		if(*src_buf != *dst_buf)
			error = error + 1;
	}
	#ifdef DEBUG
		printf("error = %d\n",error);
	#else
		;
	#endif*/
}
