#include <common.h>
#include <command.h>
#include "fastboot.h"
#include <bootimg.h>
#include <search.h>

#define GET_STATUS 0
#define SET_ADDRESS 5
#define GET_DESCRIPTOR 6
#define SET_CONFIGURATION 9

#define DESCRIPTOR_DEVICE 1
#define DESCRIPTOR_CONFIGURATION 2
#define DESCRIPTOR_STRING 3
#define CONFIG_FASTBOOT_GET_VAR_BASEBAND_STRING "NS115"


#define SETID_ADDR	0x05800000
#define SETID_MASK	0x00000fff

#define USB_BASE	(0x050a0000)
#define DL_BUF_ADDR	(0x91000000)
#define TX_BUF0		0x90000000
#define RX_BUF0		0x90001000
#define TX_BUF		0x90002000
#define RX_BUF 		0x90003000
#define app_write_single(reg,val)  (*(volatile uint32_t *)(reg) = (val))
#define app_read_single(reg)  (*(volatile uint32_t *)(reg))
#define usb_readl(offset)       (*(volatile uint32_t *)(USB_BASE + (offset)))
#define usb_writel(offset,val)  (*(volatile uint32_t *)(USB_BASE + (offset)) = (val))

#define USB_MPS 0x200
#define PCNT	0x1

static  unsigned char string_descriptor[256] __attribute__ ((aligned (4))) = {
	0x12, 0x03, 'f', 0x00, 'a', 0x00, 's', 0x00, 't', 0x00, 'b', 0x00, 'o', 0x00, 'o', 0x00, 't', 0x00,
};

volatile unsigned char usb_device_addr = 0;
/////////////////////////////////////////////
// optimize fastboot ;
#define FASTBOOT_MEMORY_OPTIMIZE

#if defined(FASTBOOT_MEMORY_OPTIMIZE)

#define FASTBOOT_PARTITION_FORCE

// dump fastboot optimize data;
//#define FASTBOOT_OPTIMIZE_DUMP

#if defined(FASTBOOT_OPTIMIZE_DUMP)
#   define optimize_dump(fmt, args...) printf(fmt, ##args)
#else
#   define optimize_dump(fmt, args...) 
#endif

//#define RX_BUF_CMD  (RX_BUF + 0x40)
#define RX_BUF_CMD  (RX_BUF)
volatile unsigned char* data_buffer = (unsigned char*)RX_BUF_CMD;


void* memcpy4(void* dest, const void* src, size_t count);
#define memcpy memcpy4
#endif
////////////////////////////////////////////

//#define  EN_DEBUG
#define  PERR(format,...)	printf("[%d:%s]:"format, \
                                	__LINE__,__func__ , ##__VA_ARGS__ )
#define  PLOG(format,...)	printf("[%d:%s]:"format, \
                                	__LINE__,__func__ , ##__VA_ARGS__ )
#define  PRINT(format,...)	printf(format, ##__VA_ARGS__ )                                                                                          

#ifdef   EN_DEBUG
#define  PDBG(format,...)	printf("[%d:%s]:"format, \
                                	__LINE__,__func__ , ##__VA_ARGS__ )
#define  DUMP_DATA(...)		dump_data(__VA_ARGS__)
#else   
#define  PDBG(format,...)		do{}while(0)
#define  DUMP_DATA(...)			do{}while(0)
#endif

#define  ENTER_FUNC()                        PDBG("enter func: %s\n",__func__);
#define  LEAVE_FUNC()                        PDBG("leave func: %s\n",__func__);

/*****************************/
static unsigned char rxdata[512] __attribute__((__aligned__(4)));
static unsigned char txdata[512] __attribute__((__aligned__(4)));
static void wait_intr(uint32_t offset, uint32_t val);
static void wait_cycle(int n);

static struct bootsel bootsel;

struct FastbootArgs {
	int b_quit_fastboot;
	int b_is_upload_partition_config;
};

static struct FastbootArgs  fastbootArgs;

void init_fastboot(struct FastbootArgs* args) {
	memset(args, sizeof(struct FastbootArgs), 0);
}
/********************************/

//#define FASTLOG(fmt, args...)  printf("fastboot host: " fmt "\n", ##args)
#define FASTLOG(fmt, args...)
#define MAX_PTN 20

#define RESERV_SIZE 	10	//10 Mbytes
#define SECTOR_SIZE	512	// bytes

#if 1
struct fastboot_ptentry {
	char name[16];              // partation name;
	unsigned int start;         // start offset of mmc block;
	unsigned long long length;         
	unsigned int flags; 
};

enum {
	P_BOOT = 0,
	P_SYSTEM,
	P_DATA,
	P_CACHE,
	P_MISC,
	P_RECOVERY,

	P_NUM,
};

typedef struct {
	unsigned int start;
	unsigned int length;
	unsigned int end;
}ptn;

ptn fastboot_ptn[P_NUM];

typedef struct fastboot_ptentry fastboot_ptentry;

static fastboot_ptentry ptable[MAX_PTN];
static unsigned int pcount = 0;

#ifndef CONFIG_NS115_TEST
extern unsigned int _xloader_boot_mode;

static void calc_ptn_bound(struct bootsel *bootsel)
{
	int spm = 1024 * 1024 / SECTOR_SIZE;
	int offset;

	fastboot_ptn[P_BOOT].start = RESERV_SIZE * spm;
	fastboot_ptn[P_BOOT].length = bootsel->bootsize * spm;
	fastboot_ptn[P_BOOT].end = fastboot_ptn[P_BOOT].start + bootsel->bootsize * spm - 1;

	fastboot_ptn[P_SYSTEM].start = fastboot_ptn[P_BOOT].end + 1;
	fastboot_ptn[P_SYSTEM].end = fastboot_ptn[P_SYSTEM].start + bootsel->systemsize * spm - 1;

	//	ext_ptn->start = ptn[P_SYSTEM].end + 1;

	offset = fastboot_ptn[P_SYSTEM].end + (bootsel->datasize + 1) * spm;
	fastboot_ptn[P_DATA].start = fastboot_ptn[P_SYSTEM].end + 17;
	fastboot_ptn[P_DATA].end = offset;

	offset += (bootsel->cachesize + 1) * spm;
	fastboot_ptn[P_CACHE].start = fastboot_ptn[P_DATA].end + 17;
	fastboot_ptn[P_CACHE].end = offset;

	offset += (bootsel->miscsize + 1) * spm;
	fastboot_ptn[P_MISC].start = fastboot_ptn[P_CACHE].end + 17;
	fastboot_ptn[P_MISC].end = offset;

	offset += (bootsel->recoverysize + 1) * spm;
	fastboot_ptn[P_RECOVERY].start = fastboot_ptn[P_MISC].end + 17;
	fastboot_ptn[P_RECOVERY].end = offset;

	//	ext_ptn->end = offset;
}




void read_boot_ptn(unsigned int size)
{
	int ret,selboot = 0;
	unsigned int charge_flag = 0;
	struct hsearch_data env_htab;
	unsigned int readsize;

	char sep = '\n';
	const char *addr = 0x90003000;

	printf("size = %d\n",size);
	//if(_xloader_boot_mode == 0x02){
	if (fastbootArgs.b_is_upload_partition_config) {
		readsize = size;
	}else{
		//	struct bootsel bootsel;
		char *ext4_load[5]  = { "do_ext4_load","mmc","1:7","0x90003000","partition_config", };
		readsize = do_ext4_load(NULL,0,5,ext4_load);
		if (readsize == 1)
			return -1;
	}
	hread_buf_ptn(&env_htab,addr,readsize,sep,&bootsel);
	calc_ptn_bound(&bootsel);
	if (fastbootArgs.b_is_upload_partition_config){
		int sector_buf[512]; 

		sector_buf[0] = fastboot_ptn[P_BOOT].start;
		sector_buf[1] = fastboot_ptn[P_BOOT].end;

		sector_buf[2] = fastboot_ptn[P_SYSTEM].start;
		sector_buf[3] = fastboot_ptn[P_SYSTEM].end;

		sector_buf[4] = fastboot_ptn[P_DATA].start;
		sector_buf[5] = fastboot_ptn[P_DATA].end;

		sector_buf[6] = fastboot_ptn[P_CACHE].start;
		sector_buf[7] = fastboot_ptn[P_CACHE].end;

		sector_buf[8] = fastboot_ptn[P_MISC].start;
		sector_buf[9] = fastboot_ptn[P_MISC].end;

		sector_buf[10] = fastboot_ptn[P_RECOVERY].start;
		sector_buf[11] = fastboot_ptn[P_RECOVERY].end;

		int i ;
		for( i = 0; i < 15; i++)
			printf("sector_buf[%d] = %d\n",i,sector_buf[i]);

		int start_sector_no = 2051;
		mmc_write(1,sector_buf,start_sector_no,1);

		char *p = sector_buf;	
		for(i = 0; i < 8; i++)
			p[i] = string_descriptor[i*2 + 2];

		mmc_write(1,p,2050,1);
	}	
	return;
}

void fastboot_flash_reset_ptn(void)
{
	pcount = 0;

}
#endif

void fastboot_flash_add_ptn(fastboot_ptentry *ptn)
{
	if(pcount < MAX_PTN){
		memcpy(ptable + pcount, ptn, sizeof(*ptn));
		pcount++;
	}
}

fastboot_ptentry *fastboot_flash_find_ptn(const char *name)
{
	unsigned int n;

	for(n = 0; n < pcount; n++) {
		/* Make sure a substring is not accepted */
		if (strlen(name) == strlen(ptable[n].name))
		{
			if(0 == strcmp(ptable[n].name, name))
				return ptable + n;
		}
	}
	return 0;
}
fastboot_ptentry *fastboot_flash_get_ptn(unsigned int n)
{
	if(n < pcount) {
		return ptable + n;
	} else {
		return 0;
	}
}

unsigned int fastboot_flash_get_ptn_count(void)
{
	return pcount;
}

void board_mmc_init(unsigned int size)
{
#ifndef CONFIG_NS115_TEST
	read_boot_ptn(size);
	fastboot_flash_reset_ptn();
#endif
	/* Partitons on EMMC preasent on OMAP4SDP required for Fastboot*/
	struct fastboot_ptentry ptn[MAX_PTN] = {
		{
			.name   = "mbr",
			.start  = 0, /*Sector Start */
			.length = 512, /*512B */
			.flags  = 0,
		},
		{
			.name   = "xloader",
			.start  = 4096, /*Sector Start */
			.length = 128*1024, /*128KB */
			.flags  = 0,
		},

		//        {
		//            .name   = "environment",
		//            .start  = 4592,  /* Sector Start */
		//            .length = 8*1024, /*8KB */
		//            .flags  = 0,//FASTBOOT_PTENTRY_FLAGS_WRITE_ENV,
		//        },

		{
			.name   = "ept1",
			//            .start  = 11354240,  /* Sector Start */
			.start  = fastboot_ptn[P_BOOT].start - 16,  /* Sector Start */
			.length = 8*1024, /*8KB */
			.flags  = 0,//FASTBOOT_PTENTRY_FLAGS_WRITE_ENV,
		},
		{
			.name   = "ept2",
			//            .start  = 14500032,  /* Sector Start */
			.start  = fastboot_ptn[P_CACHE].start - 16,  /* Sector Start */
			.length = 8*1024, /*8KB */
			.flags  = 0,//FASTBOOT_PTENTRY_FLAGS_WRITE_ENV,
		},
		{
			.name   = "ept3",
			//            .start  = 14602496,  /* Sector Start */
			.start  = fastboot_ptn[P_MISC].start - 16,  /* Sector Start */
			.length = 8*1024, /*8KB */
			.flags  = 0,//FASTBOOT_PTENTRY_FLAGS_WRITE_ENV,
		},
		{
			.name   = "ept4",
			//            .start  = 14623040,  /* Sector Start */
			.start  = fastboot_ptn[P_RECOVERY].start - 16,  /* Sector Start */
			.length = 8*1024, /*8KB */
			.flags  = 0,//FASTBOOT_PTENTRY_FLAGS_WRITE_ENV,
		},
		{
			.name   = "uboot",
			.start  = 4608, /*Sector Start */
			.length = 301*1024, /*301KB */
			.flags  = 0,
		},
		{
			.name   = "logo32",
			.start  = 5210,  /* Sector Start */
			.length = 750*1024, /*750K */
			.flags  = 0,
		},
		{
			.name   = "fastbootlogo16",
			.start  = 7000,  /* Sector Start */
			.length = 500*1024, /*500K */
			.flags  = 0,
		},
		{
			.name   = "logo16",
			.start  = 8800,  /* Sector Start */
			.length = 500*1024, /*500K */
			.flags  = 0,
		},
		{
			//			.name   = "sdcard",
			//			.start  = fastboot_ptn.sdcardstart,  /* Sector Start */
			//			.length = 10768447 * 512, 
			//			.flags  = 0,
		},
		{
			.name   = "boot",
			.start  = fastboot_ptn[P_BOOT].start,  /*11,403,264 Sector Start */
			//.length = 20*1024*1024, /*20MB */
			.length = 41023 * 512, 
			.flags  = 0,
		},
		{
			.name   = "system",
			.start  = fastboot_ptn[P_SYSTEM].start,  /*11,444,288 Sector Start */
			.length = 350*1024*1024, /*256MB */
			//.length = 524351 * 512, 
			.flags  = 0,
		},
		{
			.name   = "data",
			.start  = fastboot_ptn[P_DATA].start,  /*11,968,656 Sector Start */
			//.length = 1536*1024*1024*1024, /*1.5GB */
			.length = 3145775 * 512, 
			.flags  = 0,
		},
		{
			.name   = "cache",
			.start  = fastboot_ptn[P_CACHE].start,  /* Sector Start */
			//.length = 50*1024*1024, /*50MB */
			.length = 102447 * 512, 
			.flags  = 0,
		},
		{
			.name   = "misc",
			.start  = fastboot_ptn[P_MISC].start,  /* Sector Start */
			//.length = 10*1024*1024, /*10MB */
			.length = 20527 * 512, 
			.flags  = 0,
		},
		{
			.name   = "recovery",
			.start  = fastboot_ptn[P_RECOVERY].start, /*Sector Start */
			//.length = 128*1024*1024, /*14MB */
			.length = 262191 * 512, 
			.flags  = 0,
		},
		/* Rest of the RAW Partitions can start from Sector start 0x27B622 */
	};
	int i;
	const int n = sizeof(ptn)/sizeof(struct fastboot_ptentry);
	for (i = 0; i < n; i++){
		fastboot_flash_add_ptn(&ptn[i]);
		printf("ptn[%d].start = %d\n",i,ptn[i].start);
	}
	FASTLOG("ptn count %d\n", n);
}
#else
#define board_mmc_init() 
#endif
/********************************/


#ifdef EN_DEBUG
static void dump_data(unsigned char * data, int len, int prntAll)
{
#define COL             16
#define ROW             5
	int	i,
		j,
		row,
		index;

	if( prntAll )
	{
		row = 10000;
	}
	else
	{
		row = ROW;
	}

	PRINT("=======<DUMP_DATA>=======\n");
	PRINT("           0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F"
			"    0123456789ABCDEF\n");
	for(i = 0, index = 0 ; i < row ; i++)
	{
		PRINT("0x%04x:    ",i*COL);
		for(j = 0 ; j < COL ; j++)
		{
			if( index + j >= len )
			{
				break;
			}

			PRINT("%02x ",(unsigned char)data[index + j]);
		}

		for(; j < COL ; j++)
		{
			PRINT("-- ");
		}
		PRINT(" |");
		for(j = 0 ; j < COL ; j++)
		{
			if( index >= len )
			{
				break;
			}

			if( data[index] >= 32 && data[index] <= 126 )
			{
				PRINT("%c",(unsigned char)data[index]);
			}
			else
			{
				PRINT(".");
			}

			index ++;
		}
		PRINT("\n");    

		if( index >= len )
		{
			break;
		}
	}
	PRINT("=========================\n");
#undef COL
#undef ROW
}
#endif

//------------------------------------------------------------//

static const unsigned char device_config[20] __attribute__ ((aligned (4)))=
{
	0x12,
	0x01,
	0x00,//0x01,
	0x02,

	0x00,
	0x00,
	0x00,
	0x40,

	0xB4,
	0x0B,
	0xFF,
	0x0F,

	0x00,
	0x01,
	0x01,
	0x01,

	0x01,
	0x01,
	0xAA,
	0x55,
};

static const unsigned char config_descriptor[32] __attribute__ ((aligned (4)))=
{
	/* Configuration Descriptor */
	0x09, 0x02, 0x20, 0x00, 0x01, 0x01, 0x01, 0x80, 0x80,

	/* Interface Descriptor */
	0x09, 0x04, 0x00, 0x00, 0x02, 0xFF, 0x42, 0x03, 0x00,

	/* Endpoint Descriptor */
	0x07, 0x05, 0x82, 0x02, 0x00, 0x02, 0x00,
	0x07, 0x05, 0x01, 0x02, 0x00, 0x02, 0x01,

};

static const unsigned char zero[8] __attribute__ ((aligned (4))) = 
{
	0x00, 0x00, 0x00, 0x00,
};

static const unsigned char language_descriptor[8] __attribute__ ((aligned (4)))=
{
	0x04, 0x03, 0x09, 0x04,
};

static void usb_dev_init(void)
{
	int i;

	// Init Step 1 : Programming the Core (6.1)
	// 1.1 Read hardware configuration
	//TRACE(KERN_DEBUG,"GSNPSID = 0x%08X\n", usb_readl(GSNPSID));
	//TRACE(KERN_DEBUG,"GHWCFG1 = 0x%08X\n", usb_readl(GHWCFG1));
	//TRACE(KERN_DEBUG,"GHWCFG2 = 0x%08X\n", usb_readl(GHWCFG2));
	//TRACE(KERN_DEBUG,"GHWCFG3 = 0x%08X\n", usb_readl(GHWCFG3));
	//TRACE(KERN_DEBUG,"GHWCFG4 = 0x%08X\n", usb_readl(GHWCFG4));
	//TRACE(KERN_DEBUG,"GLPMCFG = 0x%08X\n", usb_readl(GLPMCFG));

	// 1.2 Set AHB configuration
	// Core in slave Mode
	// Unmask the interrupt assertion to the application
	// Non-Periodic TxFIFO Empty Level: completely empty
	// Periodic TxFIFO Empty Level: completely empty
	usb_writel(GAHBCFG, 0x1a3);

	// 1.3 Enable interrupts:
	// GINTMSK.RxFLvlMsk = 1'b0
	// usb_writel(GINTMSK, 0);  //default is 0

	// 1.4 Set GUSBCFG:
	//  480-MHz Internal PLL clock
	//  The MAC interface is 16-bit UTMI+
	//  HNP capability is not enabled
	//  SRP capability is not enabled
	//  Single Data Rate ULPI Interface, with 8-bit-wide data bus
	//  USB 2.0 high-speed UTMI+ or ULPI PHY
	//  6-pin unidirectional full-speed serial interface
	//  UTMI+ Interface
	//  PHY Interface: 16-bits
	usb_writel(GUSBCFG, 0x1409);

	// 1.5 Enable interrupts:
	// OTG Interrupt Mask
	// Mode Mismatch Interrupt Mask
	usb_writel(GINTMSK, 0x6);  

	// Init step 2: Device Init (6.3.3)
	// 2.1 Set DCFG:
	//  Resume Validation Period: 2
	//  IN Endpoint Mismatch Count: 4
	// + Periodic Frame Interval: 1 (85%)
	//usb_writel(DCFG, 0x08100801); //full speed
	usb_writel(DCFG, 0x08100800);  //high speed
	wait_cycle(100);
	// 2.2 Enable thresholding
	// 

	// 2.3 Enable interrupts:
	//  OTG Interrupt
	//  Mode Mismatch Interrupt
	//  Start of (micro)Frame Mask 
	//  Early Suspend Mask
	//  USB Suspend Mask
	//  USB Reset Mask
	//  Enumeration Done Mask
	usb_writel(GINTMSK, 0x3c0e);

	// + Data line pulsing using utmi_termsel 
	//usb_writel(GUSBCFG, 0x401408);
	// clear soft disconnect
	usb_writel(DCTL, 0x0);
	wait_cycle(10);


	usb_writel(GRSTCTL, (0x10<<6)|(1<<5)|(1<<4));
	// 2.4 wait for USB reset
	wait_intr(GINTSTS, 0x1000);
	PDBG("usb reset ...\n");

	// Initialization on USB Reset (6.6.1.1)

	// 2.4.1 Set the NAK bit for all OUT endpoints
	//usb_writel(DOEPCTL0, 1<<27);		// uncomment this.

	// 2.4.2Enable interrupts for IN/OUT EP 0
	usb_writel(DAINTMSK, 0x10001);
	// Enable Device OUT Endpoint Common Interrupts:
	//  SETUP Phase Done
	//  Endpoint Disabled Interrupt
	//  Transfer Completed Interrupt
	usb_writel(DOEPMSK, 0xb);
	// Enable Device IN Endpoint Common Interrupts:
	//  IN Endpoint NAK Effective
	//  Timeout Condition
	//  Endpoint Disabled Interrupt Mask
	//  Transfer Completed Interrupt Mask
	usb_writel(DIEPMSK, 0x4b);

	// 2.4.3 Set for receive data 
	// mask GINTMSK.NPTxFEmpMsk, and GINTMSK.RxFLvlMsk
	//usb_writel(GINTMSK, 0x3c3e);
	usb_writel(GINTMSK, 0x3c8e);  // comment this

	// 2.4.4 Program the GRXFSIZ Register, to be able to receive control OUT data and setup data.

	// 2.4.5 
	usb_writel(DOEPTSIZ0, 0x00180018);
	// 2.5 
	wait_intr(GINTSTS, 0x2000);
	PDBG("Enum...\n");

	i = usb_readl(DSTS);
	PDBG("DSTS:%x\n", i);

	usb_writel(DIEPCTL0, 0);
	usb_writel(DOEPCTL0, 0x80000000);


	//Reset FIFOs
	// Waiting for Enumeration Done


	usb_writel(GINTMSK,/* 0xfb0c3c0c*/ 0xfb0c2c06);
	usb_writel(GINTSTS, 0xffffffff);

	usb_writel(DIEPMSK,  0xffffffff);
	usb_writel(DOEPMSK,  0xffffffff);
	usb_writel(DAINTMSK, 0xffffffff);

	// Device OUT Endpoint 0 Transfer Size Register
	//  Packet Count: 3
	//  Transfer Size: 24
	//usb_writel(DOEPTSIZ0, 0x0);
	usb_writel(DOEPTSIZ0, 0x60180018);
	usb_writel(DOEPDMA0, RX_BUF0);
	usb_writel(DOEPCTL0, 0x84000000);
	//TRACE(KERN_DEBUG,"Waiting for Enumeration Done ");
}


static void Setup_Tx(unsigned int *data, unsigned int len)
{
	unsigned int val;

	PDBG("len:%d\n", len);
	DUMP_DATA(data, len, 0);
	memcpy(TX_BUF0, data, len);
	usb_writel(DIEPTSIZ0, 0x80000 | len);
	usb_writel(DIEPDMA0, TX_BUF0);
	usb_writel(DIEPCTL0, 0x84000000 | (0x02 << 11));

	wait_cycle(100);
	do{
		val = usb_readl(DIEPINT0);
		if(val & 0x1) //xfer complete
		{
			usb_writel(DIEPINT0,0x1);
			break;		
		}
		udelay(100);
	}		
	while(1);

	return;
}

static unsigned int PID_DATA;
#if !defined(FASTBOOT_MEMORY_OPTIMIZE)
void active_bulk_data(int pkcnt)
#else
void active_bulk_data(unsigned char** data_buffer, int pkcnt)
#endif
{
	int size = pkcnt * USB_MPS;

	if(PID_DATA == 1) {
		PID_DATA = 2;
	} else {
		PID_DATA = 1;
	}

	usb_writel(DOEPTSIZ1, pkcnt << 19 | size);
	// Device OUT Endpoint 1 DMA Address Register
#if !defined(FASTBOOT_MEMORY_OPTIMIZE)
	usb_writel(DOEPDMA1, RX_BUF);
#else
	usb_writel(DOEPDMA1, data_buffer != NULL ? *data_buffer : RX_BUF_CMD);
#endif

	// Device Control OUT Endpoint 1 Control Register
	//   Maximum Packet Size: USB_MPS
	//   Endpoint Type: BULK
	//   Clear NAK
	//   Set DATA0 PID
	//   Endpoint Enable
	usb_writel(DOEPCTL1, 0x94088000|(PID_DATA<<28)|USB_MPS);

	usb_writel(DIEPCTL2, 0x08088000|USB_MPS);
}

static void bulk_in(unsigned int *data, unsigned int len)
{
	unsigned int val;

	PDBG("len:%d\n", len);
	DUMP_DATA(data, len, 0);
	memcpy(TX_BUF, data, len);
	usb_writel(DIEPTSIZ2, 0x80000|len);
	usb_writel(DIEPDMA2, TX_BUF);

	val = usb_readl(DIEPINT2);
	usb_writel(DIEPINT2, val);

	val = usb_readl(DIEPCTL2);
	usb_writel(DIEPCTL2, val | 0x84000000);

	do{
		val = usb_readl(DIEPINT2);
		if(val & 0x1) //xfer complete
		{
			usb_writel(DIEPINT2,0x1);
			break;		
		}
		udelay(100);
	}		
	while(1);

	return;
}

static void fastboot_ack(const char *code, const char *reason)
{	
	char * response = (char*)txdata;

	if (reason == NULL)		
		reason = "";	

	sprintf(response, "%s%s", code, reason);	
	bulk_in(response, strlen(response));
}

static void fastboot_fail(const char *reason)
{	
	fastboot_ack("FAIL", reason);
}

static void fastboot_okay(const char *info)
{	
	fastboot_ack("OKAY", info);
}

static void fastboot_out(const char* msg) {
	fastboot_ack("", msg);
}


static int b_quit_fastboot = 0;

static void fastboot_mmc_init() {
	char* mmc_init[]  = {"mmc", "init", "1"};
	do_mmc(NULL, 0, 3, mmc_init);
}

static int fastboot_mmc_write(unsigned char* buffer,
		unsigned int base_block,
		unsigned int block_offset,
		unsigned int block_count
		) {
	char* mmc_write[] = {"mmc", "write", "1", NULL, NULL, NULL};

	char strAddr[12] = "";
	char strStartBlk[20] = "";
	char strBlks[20] = "";

	sprintf(strAddr, "0x%08x", buffer);
	sprintf(strStartBlk, "%d", base_block + block_offset);
	sprintf(strBlks, "%d",     block_count);

	mmc_write[3] = strAddr;
	mmc_write[4] = strStartBlk;
	mmc_write[5] = strBlks;

	return do_mmc(NULL, 0, 6, mmc_write);
}

static void handle_setup(unsigned char *rev_buf);

static void fastboot_response_setup() {
	unsigned int int0 = usb_readl(DOEPINT0);
	unsigned int size;

	if(int0 & (0x1 << 3))   //setup                                 
	{
		size = usb_readl(DOEPTSIZ0);
		PDBG("[setup]doeptsiz0:%x\n", size);

		handle_setup((unsigned char *)RX_BUF0);
		usb_writel(DOEPTSIZ0, 0x60180018);
		usb_writel(DOEPDMA0, RX_BUF0);
		usb_writel(DOEPCTL0, 0x84000000);
	}
}


#if !defined(FASTBOOT_MEMORY_OPTIMIZE) 
static void handle_fastboot(unsigned char * buf, unsigned int len)
#else
static void handle_fastboot(unsigned char ** data_buff, unsigned int len)
#endif
{
	static int dlflag = 0;
	static int dlsize = 0;
	static int dllen = 0;
	static char * dlbuf = NULL;

	static char outbuffer[256] = "";


#if defined(FASTBOOT_MEMORY_OPTIMIZE)
	volatile unsigned char* buf;
	buf = *data_buff;
#endif
	//	ENTER_FUNC();
	//	DUMP_DATA(buf, len, 1);

	if(dlflag == 1)
	{
		static int i = 0;

		if(i++ > 2000){
			printf("#");
			i = 0;
		}

#if !defined(FASTBOOT_MEMORY_OPTIMIZE)
		memcpy(dlbuf + dllen, buf, len);
		dllen += len;
#else
		//int ni = 100;
		//while(ni--);


		*data_buff += len;
		dllen += len;
		if (dlsize == dllen) {
			//*data_buff = (unsigned char*)RX_BUF_CMD;     // finish download;
			if (((unsigned int)*data_buff) % 4) {
				*data_buff += 4 - ((unsigned int)*data_buff) % 4;
			}
		}
#endif
		if(dlsize == dllen) //data download finish
		{
			FASTLOG("download size %d finish.", dllen);
			dlflag = 0;
			fastboot_okay(NULL);
#if defined(FASTBOOT_OPTIMIZE_DUMP) 
			// dump download data;          
			unsigned char* buf = (unsigned char*) RX_BUF_CMD; 
			int ti;
			printf("%08x: ", buf);
			for(ti = 0 ;ti<dllen;ti++) {
				printf("%02x ", buf[ti]);
				if (ti && ti % 16 == 15) {
					printf("\n%08x: ", buf + ti);
				}
			}
			printf("\n");
#endif
		}

	}
	else if(!strncmp(buf,"getvar:",7))
	{
		//FASTLOG("getvar:");
		buf[len] = 0;
		if (!memcmp(buf + 7, "version-bootloader", 18)) {
			printf("get var: version-bootloader\n");
			extern char version_string[];
			int  version_string_len = strlen(version_string);
			char version_str[64] = "";
			char* p1 = version_string;
			char* p2 = version_str;
			while(memcmp(p1++, "*** ", 4) && (p1 - version_string) < version_string);
			for(; 
					memcmp(p1, " ***", 4); 
					(p2 - version_str < sizeof(version_str)) && (*p2++ = *p1++)) ;
			fastboot_okay(version_str);
		}
		else if (!memcmp(buf + 7, "version-baseband", 16)) {
			printf("get var: version-baseband\n");
			char ver[60] = "";
			strcpy(ver, CONFIG_FASTBOOT_GET_VAR_BASEBAND_STRING); 
			fastboot_okay(ver);
		}
		else if (!memcmp(buf + 7, "serialno", 8)) {
			printf("get var: serialno");
			fastboot_okay("");
		}
		else if (!memcmp(buf + 7, "usbaddr", 7)) {
			char strbuff[8];
			memset(strbuff, 0, sizeof(strbuff));
			sprintf(strbuff, "%d ", (unsigned int)usb_device_addr);
			//fastboot_okay(strbuff);
			//ADUMP_DATA(strbuff, strlen(strbuff), 1);
			fastboot_okay(strbuff);
			//printf("usbaddr, %s,%p", strbuff, strbuff);
		}
		else if(!strncmp(buf+7,"version",7))
		{
			printf("get var: version.\n");
			fastboot_okay("NS2815");
		}
		else if(!strncmp(buf+7,"0x",2))
		{
			FASTLOG("----> 0x");
			unsigned int * addr = simple_strtoul(buf+7,NULL,16);
			char tmp[12];

			sprintf(tmp,"%8x", *addr);
			fastboot_okay(tmp);
		}
		else {
			char tcmdbuff[100] = "";
			sprintf(tcmdbuff, "not finish %s", buf + 7);
			//fastboot_okay(tcmdbuff);
			fastboot_okay("");
		}
		return;
	}
	else if(!strncmp(buf,"download:",9))
	{
		char tmp[12];

		buf += 7;
		buf[0] = '0';
		buf[1] = 'x';
		buf[10]= NULL;

		dlsize = simple_strtoul(buf,NULL,16);
		dllen = 0;
		dlflag = 1;
#if defined(FASTBOOT_MEMORY_OPTIMIZE)
		*data_buff = (unsigned char*)RX_BUF_CMD;//DL_BUF_ADDR;
		dlbuf = (char *)RX_BUF_CMD;
		optimize_dump("\nfastboot, download:%08x\n", *data_buff);
#else
		dlbuf = (char *)DL_BUF_ADDR;
#endif
		FASTLOG("download: %08x", dlsize);

		sprintf(tmp, "%8x", dlsize);
		fastboot_ack("DATA", tmp);
	}
	else if(!strncmp(buf,"partition",9)){
		fastbootArgs.b_is_upload_partition_config = 1;
		//	char *buf_part = (char *)TX_BUF;
		//	memcpy(buf_part,data_buff,dlsize);
		//	optimize_dump("\nfastboot, download:%08x\n", *buf_part);
		board_mmc_init(dlsize);
		fastboot_okay(NULL);
	}
	else if(!strncmp(buf,"flash",5))
	{
		char * ptn = "null";

		buf[len] = 0;
		if(buf[5] == ':') {
			ptn = &buf[6];
		}

		//flash dlbuf[dlsize] to emmc

		FASTLOG("execute flash: %s.", buf);
		PDBG("flash dlbuf[%d] to partition:%s\n", dlsize, ptn);
		DUMP_DATA(dlbuf, dlsize, 0);
		//DUMP_DATA(dlbuf, dlsize, 0);
		//do_flash(const char* ptn, char* buff, int size);

		if (dlsize) {
			struct fastboot_ptentry* bootptn;
			bootptn = fastboot_flash_find_ptn(ptn);
			if (bootptn == NULL) {
				printf("Patition: '%s' not exists.", ptn);
				sprintf(outbuffer, "FAIL'%s' partition not exists.", ptn);
			}
			else {
				printf("writing to partition '%s'\n", bootptn->name);

				if (dlsize > bootptn->length) {
					sprintf(outbuffer, "FAILfile too large.");
					goto done;
				}

				FASTLOG("will exec 'mmc write 1 %s %s %s;", strAddr, strStartBlk, strBlks);

				printf("Initializing '%s'\n", bootptn->name);

				fastboot_mmc_init();

				const unsigned int transfer_blk_unit = 4 * 1024 * 1024 / 512 / 128;
				unsigned char* pBuffer = dlbuf;
				unsigned int   base_block = bootptn->start;
				unsigned int   block_offset = 0;
				unsigned int   tblks = dlsize % 512 ? dlsize / 512 + 1 : dlsize / 512;
				int            bFlashOK = 1;
				int            nc = 0;

				do {
					if (block_offset + transfer_blk_unit < tblks) {
						if (fastboot_mmc_write(pBuffer, base_block, block_offset, transfer_blk_unit)) {
							bFlashOK = 0;
							break;
						}
						pBuffer += transfer_blk_unit * 512;
						block_offset += transfer_blk_unit;
					}
					else {
						if (fastboot_mmc_write(pBuffer, base_block, 
									block_offset, tblks-block_offset)) {
							bFlashOK = 0;
							break;
						}
						break;
					}
					fastboot_response_setup();
					nc++;
					if (nc && nc % 32 == 0)
						printf("#");
				} while(block_offset < tblks);

				if (!bFlashOK) {
					printf("Writing '%s' FAILED!\n", bootptn->name);
					sprintf(outbuffer, "FAIL: write partition failed.");
				}
				else {
					printf("Writing '%s' DONE!\n", bootptn->name);
					sprintf(outbuffer, "OKAYWriting '%s' DONE.", bootptn->name);
				}
			}
			dlsize = 0;
		}
		else {
			FASTLOG("failed: data size is 0;");
			sprintf(outbuffer, "FAILfile size is 0.");
		}
		fastboot_out(outbuffer);
		//fastboot_okay(NULL);
	}
	else if (!memcmp(buf, "boot", 4)) {
		FASTLOG("boot, buf: %s\n", buf);   
		char buff[20];
#if defined(FASTBOOT_MEMORY_OPTIMIZE)
		char* dlbuff = (char*) RX_BUF_CMD;
#else
		char* dlbuff  = (char*) DL_BUF_ADDR;
#endif
		memcpy(buff, dlbuff, 19);
		boot_img_hdr* h = (boot_img_hdr*) dlbuff;
		printf("boot args: %s\n", h->cmdline);
		
		load_addr = 0x90003000;
		// call setenv;
		{
			char* setenv_args[] = {"setenv", "bootargs", h->cmdline};
			int ret;
			ret = do_setenv(NULL, 0, 3, setenv_args);
			printf("setenv bootargs return %d\n", ret);

			setenv_args[1] = "bootcmd";
			setenv_args[2] = "bootm";
			ret = do_setenv(NULL, 0, 3, setenv_args);
			printf("setenv bootcmd return %d\n", ret);
		}

		// move memory;
		char* target = (char*)(0x90003000);
		dlbuff += h->page_size;
		memcpy(target, dlbuff, h->kernel_size);

		fastboot_okay(NULL);
		// call bootm;
		{
			char* run_args[] = {"run ", "bootcmd"};
			int ret;
			ret = do_run(NULL, 0, 2, run_args);
			FASTLOG("run return %d\n", ret);
		}
		//	fastboot_okay(NULL);
	}
	else if (!strncmp(buf, "reboot", 6) ||
			!memcmp(buf, "reboot-bootloader", 17)) {
		printf("will reset ...\n");
		fastboot_okay("will reboot ...");
		do_reset(NULL, 0, 0, NULL);  
	}
	else if (!strncmp(buf, "erase", 5)) {
		static const char* const mmc_init[] = {"mmc", "init", "1"};
		//static const char* const mmc_erase[] = {"mmc", "write", "1", "0x80007fc0", "4096", "1212" };
		static const char* const mmc_erase[] = {"mmc", "write", "1", "0x80007fc0", "0", "20480" };
		static const char* const reset_cmd[] = {"reset"};
		do_mmc(NULL, 0, 3, mmc_init);
		do_mmc(NULL, 0, 6, mmc_erase);
		fastboot_okay("erase completed");
		do_reset(NULL, 0, 1, reset_cmd);
	}
	else if (!strncmp(buf, "exit", 4)) {
		fastboot_okay("will quit.");
		fastbootArgs.b_quit_fastboot = 1;
		return 0;
	}
	else
	{
		printf("'%s' Invalid command!", buf);
		fastboot_fail(" Invalid command!");
	} 
	return;
done:
	fastboot_out(outbuffer);
	dlsize = 0;
}

static void handle_setup(unsigned char *rev_buf)
{
	PDBG("recv SETUP:\n");
	//	read_setid();
	DUMP_DATA(rev_buf, 8, 0);

	switch (rev_buf[1]) {
		case GET_DESCRIPTOR:
			switch (rev_buf[3]) {
				case DESCRIPTOR_DEVICE:
					PDBG("IN Dev desc\n");
					Setup_Tx((unsigned int*)device_config, 18);
					break;
				case DESCRIPTOR_CONFIGURATION:
					PDBG("In Cfg desc\n");
					if (rev_buf[6] == 0x09) {
						Setup_Tx((unsigned int*)config_descriptor, 0x09);
					} else {
						Setup_Tx((unsigned int*)config_descriptor, 0x20);
					}
					break;
				case DESCRIPTOR_STRING:
					PDBG("In lang desc\n");
					if (rev_buf[2] == 0x00) {
						Setup_Tx((unsigned int*)language_descriptor, 0x04);
					} else {
						Setup_Tx((unsigned int*)string_descriptor, string_descriptor[0]);
					}
					break;
				default:
					PDBG("other desc %d\n", rev_buf[3]);
					break;
			}
			break;
		case SET_ADDRESS:
			PDBG("Set Address %d\n", rev_buf[2]);
			usb_device_addr = rev_buf[2];
			usb_writel(DCFG, 0x08100800|(rev_buf[2]<<4));
			Setup_Tx((unsigned int*)zero,0);
			break;
		case SET_CONFIGURATION:
			PDBG("Set Configuration\n");
			Setup_Tx((unsigned int*)zero,0);
#if !defined(FASTBOOT_MEMORY_OPTIMIZE)
			active_bulk_data(PCNT);
#else
			active_bulk_data(NULL, PCNT);
#endif
			break;
		case GET_STATUS:
			PDBG("Get Status\n");
			Setup_Tx((unsigned int*)zero, 0x02);
			break;
		default:
			PDBG("Unknown Setup packet %d\n", rev_buf[1]);
			break;
	}
}

static void init_serial_no() {
	extern unsigned char _nufront_serial_no;
	unsigned char* pSerialNo = &_nufront_serial_no;
	if (pSerialNo[0] == 0) {
		printf("serial id is fastboot\n");
		return;
	}

	int n = *((char*)pSerialNo) + 4;
	if (n < 256) 
		n /= 4;
	else 
		n = 256 / 4;

	unsigned int* pNO = string_descriptor;
	unsigned int* pNO1 = pSerialNo;

	while(n-- > 0) 
		*pNO++ = *pNO1++; 
}

int do_fastboot (cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
#ifdef CONFIG_NS115_PAD_PROTOTYPE
#define GPIO_USB_SWITCH (32+5)
       gpio_pinmux_config(GPIO_USB_SWITCH);
       nufront_set_gpio_value(GPIO_USB_SWITCH,0);      /* 0: select usb slave */
#endif

	init_fastboot(&fastbootArgs);

	int partitionsize = 0;
	board_mmc_init(partitionsize);    
	init_serial_no();

	int i;
	unsigned int int0 = 0;
	unsigned int int1 = 0;
	unsigned int size = 0;
	unsigned int msk0 = 1 << 0 | 0x01 << 3;
	unsigned int msk1 = 1 << 0;

	PDBG("RESET USB ... ");
	usb_writel(DCTL, 0x02);
	udelay(100000);
	//	wait_cycle(10);
	usb_writel(GRSTCTL, 0x01);
	while( (usb_readl(GRSTCTL) & 0x01) != 0x00 )
	{
		wait_cycle(1);
	}
	PDBG("done!\n");

	PDBG("USB INIT ...\n");
	usb_dev_init();

	while(!fastbootArgs.b_quit_fastboot) {
		while(!fastbootArgs.b_quit_fastboot) {
			int0 = usb_readl(DOEPINT0);
			int1 = usb_readl(DOEPINT1);
			usb_writel(DOEPINT0,int0);  //Clear
			usb_writel(DOEPINT1,int1);  //Clear
			if ((int0 & msk0) || (int1 & msk1 )){
				break;
			}
		}

		//		PDBG("int0:0x%x, int1:0x%x\n", int0, int1);
		if(int0 & 0x1)   //xfer compl
		{
			size = usb_readl(DOEPTSIZ0);
			PDBG("[ep0 xfer compl]DOEPTSIZ0:%X\n", size);

			usb_writel(DOEPTSIZ0, 0x60180018);
			usb_writel(DOEPDMA0, RX_BUF0);
			usb_writel(DOEPCTL0, 0x84000000);
		}

		if(int0 & (0x1 << 3))   //setup
		{
			size = usb_readl(DOEPTSIZ0);
			PDBG("[setup]DOEPTSIZ0:%X\n", size);

			handle_setup((unsigned char *)RX_BUF0);
			usb_writel(DOEPTSIZ0, 0x60180018);
			usb_writel(DOEPDMA0, RX_BUF0);
			usb_writel(DOEPCTL0, 0x84000000);
		}

		if(int1 & 0x01)  //xfer compl OUT packet on EP 1
		{
			size = usb_readl(DOEPTSIZ1);
			//			PDBG("[ep1 xfer compl]DOEPTSIZ0:%X\n", size);
			size = PCNT * USB_MPS - (size & 0x7ffff);
			//			PDBG("[ep1]size:%d\n", size);

#if defined(FASTBOOT_MEMORY_OPTIMIZE)
			handle_fastboot(&data_buffer, size);   
			active_bulk_data(&data_buffer, PCNT);	
#else
			handle_fastboot(RX_BUF, size);

			active_bulk_data(PCNT);	
#endif
		}
	}
#ifdef CONFIG_NS115_PAD_PROTOTYPE
	nufront_set_gpio_value(GPIO_USB_SWITCH,1);      /* 1: select usb host */
#endif
	return 0;
}

static void wait_intr(uint32_t offset, uint32_t val)
{
	while( (usb_readl(offset) & val) != val )
	{
		wait_cycle(1);
	}
}

static void wait_cycle(int n)
{
	while(n-- > 0);
}

U_BOOT_CMD(fastboot, 1, 0, do_fastboot,
		"fastboot",
		" enter fastboot mode"
	  );


