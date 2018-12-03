-heap  0x40000
-stack 0x1000

MEMORY
{
	/* Local L2, 0.5~1MB*/
	VECTORS: 	    o = 0x00800000  l = 0x00000200
	LL2_RW_DATA: 	o = 0x00800200  l = 0x7FDFF               //0x0003FE00

	/* Shared L3 2~4MB*/
	SL3: 		    o = 0x0C010000  l = 0x001f0000
	
	TemL:			o = 0x0C2C0000  l = 0x00010000

	/* External DDR3, upto 2GB per core */
	DDR3_CODE: 	    o = 0x80000000  l = 0x01000000   /*set memory protection attribitue as execution only*/
	DDR3_R_DATA: 	o = 0x81000000  l = 0x01000000 	 /*set memory protection attribitue as read only*/
	DDR3_RW_DATA: 	o = 0x82000000  l = 0x06000000   /*set memory protection attribitue as read/write*/
	DDR:			o = 0x88000000  l = 0x08000000
}

SECTIONS
{
	vecs       	    >    VECTORS
	.sysmem         >    LL2_RW_DATA
	.cinit          >    LL2_RW_DATA
	.const          >    LL2_RW_DATA
	.switch         >    LL2_RW_DATA
	.stack          >    LL2_RW_DATA
	GROUP
	{
		.neardata
		.rodata
		.bss
	} 		        >    LL2_RW_DATA
	.far            >    LL2_RW_DATA
	.fardata        >    LL2_RW_DATA
	.cio            >    LL2_RW_DATA
	.text           >    SL3
	QMSS_Data:Descriptor_DDR 	> 	SL3
	.netbuf			>    SL3
	.BUFTEMP            >   TemL
	
}


