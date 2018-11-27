#include"c6678.h"
#include <c6x.h>
#include <stdio.h>
#include <cslr_gpio.h>
#include <csl_bootcfgAux.h>
#include "KeyStone_common.h"
#include "gpio_z.h"
#include "hdlc_cmu.h"
/********************************************************************
 * 可修改的全局变量
 *******************************************************************/
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
	KeyStone_common_CPU_init();
	/*print device information. 
	Enable memory protection interrupts, EDC for MSMC RAM*/
	KeyStone_common_device_init();

	//enable exception handling
	//KeyStone_Exception_cfg(TRUE);

    CACHE_setL1PSize(CACHE_L1_32KCACHE);
    CACHE_setL1DSize(CACHE_L1_32KCACHE);
    CACHE_setL2Size(CACHE_0KCACHE);

	if(C6678_EVM==gDSP_board_type)
	{
		//DSP core speed: 100*10/1=1000MHz
		KeyStone_main_PLL_init(100, 10, 1); 
	}
	else if(TCI6614_EVM==gDSP_board_type
		||DUAL_NYQUIST_EVM==gDSP_board_type
		||C6670_EVM==gDSP_board_type)
	{
		//DSP core speed: 122.88*236/29= 999.9889655MHz
		KeyStone_main_PLL_init(122.88, 236, 29);
	}
	else
	{
		puts("Unknown DSP board type!");
		return;
	}
	
	int coreNum=0;
	int * src_buf= (int *)EMIF16FPGA_SEND_BUF_ADDR;
	/* Get the core number. */
	coreNum = CSL_chipReadReg (CSL_CHIP_DNUM);
	if(coreNum==0)
	{   //初始化PLL
		if (C6678_Pll_Init(PLATFORM_PLL1_PLLM_val)!= TRUE)
			printf("PLL failed to initialize!!!!!!!!!!!!!!!!!!!!!!!!! \n" );
	    else
			printf("PLL success to initialize\n" );
	    
	    C6678_Power_UpDomains();
	    C6678_Ecc_Enable();
	}
	//使能TIMER
	C6678_TimeCounter_Enable();
	if(coreNum==0)
	{	//初始化EMIF16_FPGA
	    if (C6678_Emif16_Fpga_Init()!= TRUE)
	    	printf("EMIF16_FPGA failed to initialize!!!!!!!!!!!!!!!!!!!!!!!!! \n" );
	    else
	    	printf("EMIF16_FPGA success to initialize! \n" );
	}
	
	GPIO_init();

	GPIO_Interrupts_Init();

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
				for(j=0;j<512;j++)
				{
					src_buf[j] = (j + 3)%256;
				}
				send_hdlc(512,src_buf);
				C6678_TimeCounter_Delaycycles(10000000);
			}
		}
	}
#endif
	puts("Test complete.");
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
	printf("GPIO interrupt event flag on CorePac INTC = 0x%x\n",
		((eventFlag&(~(1<<(CSL_GEM_GPINTN-64))))>>(10))
		|((eventFlag>>(CSL_GEM_GPINTN-64))<<DNUM));

	length = receive_hdlc(dst_buf);
	error =0;
	for(i=0;i<length;i++)
	{
		if(*src_buf != *dst_buf)
			error = error + 1;
	}
	printf("error = %d\n",error);

}
