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
#define EMIF16FPGA_SEND_BUF_ADDR  0x0C1D0000
#define EMIF16FPGA_RECV_BUF_ADDR  0x0C1E0000
void init(void)
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
    #ifdef DEBUG
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
	#endif
	
	int coreNum=0;
	/* Get the core number. */
	coreNum = CSL_chipReadReg (CSL_CHIP_DNUM);
	if(coreNum==0)
	{   //初始化PLL
		if (C6678_Pll_Init(PLATFORM_PLL1_PLLM_val)!= TRUE)
		{
			#ifdef DEBUG
				printf("PLL failed to initialize!!!!!!!!!!!!!!!!!!!!!!!!! \n" );
			#else
				;
			#endif
		}	
	    else
		{
			#ifdef DEBUG
				printf("PLL success to initialize\n" );
			#else
				;
			#endif	
		}	    
	    C6678_Power_UpDomains();
	    C6678_Ecc_Enable();
	}
	//使能TIMER
	C6678_TimeCounter_Enable();
	if(coreNum==0)
	{	//初始化EMIF16_FPGA
	    if (C6678_Emif16_Fpga_Init()!= TRUE)
		{
			#ifdef DEBUG
				printf("EMIF16_FPGA failed to initialize!!!!!!!!!!!!!!!!!!!!!!!!! \n" );
			#else
				;
			#endif
		}	
	    else
		{
			#ifdef DEBUG
				printf("EMIF16_FPGA success to initialize! \n" );
			#else
				;
			#endif
		}
	}
	
	GPIO_init();
	GPIO_Interrupts_Init();
}
