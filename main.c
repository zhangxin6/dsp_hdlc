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
//#define DEBUG
//记录速度
CSL_Uint64 average_write_speed=0;
CSL_Uint64 average_read_speed=0;
unsigned int frame=0;
unsigned int hdlc_rec_flag =0;
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
    int * dst_buf= (int *)EMIF16FPGA_RECV_BUF_ADDR;
    unsigned int length,i;
	coreNum = CSL_chipReadReg (CSL_CHIP_DNUM);
    if(DEVICE==DEVICE1)
    {
        if (coreNum ==0)
        {
            while(1)
            {
                if(hdlc_rec_flag == 1)
                {
                    hdlc_rec_flag = 0;
                    frame=frame+1;
                    length = receive_hdlc(dst_buf);
                    if( (frame%3)==1 )
                    {
                        src_buf[0]=0x77;
                        src_buf[1]=0x22;
                        src_buf[2]=0x44;
                        //src_buf[2]=frame/3;
                        for(i=1;i<200;i++)
                        {
                            src_buf[i+2] = i+1 ;
                        }
                        send_hdlc(202,src_buf);
                    }
                }
                C6678_TimeCounter_Delaycycles(10000000);
            }
        }
    }
}

//Interrupt service routine for timer
void interrupt GPIO_ISR(void)
{
	Uint32 eventFlag;
	/*read interrupt flag*/
	eventFlag= gpCGEM_regs->MEVTFLAG[2];
	/*clear interrupt flag*/
	gpCGEM_regs->EVTCLR[2]= eventFlag;

	hdlc_rec_flag = 1;
}
