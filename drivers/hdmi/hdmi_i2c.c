#include <config.h>

#define			I2C_SLAVE		(0x50)//(0xF0)

#define			I2C_BASE		HDMI_I2C_BASE//0x05190000)
#define			IC_CON			(I2C_BASE + 0)	
#define			IC_ENABLE		(I2C_BASE + 0x6c)
#define			IC_TAR			(I2C_BASE + 0x04)	
#define			IC_SAR			(I2C_BASE + 0x08)
#define			IC_DATA_CMD		(I2C_BASE + 0x10)
#define			IC_TX_TL		(I2C_BASE + 0x3C)
#define			IC_RAW_INTR_STAT 	(I2C_BASE + 0x34)
#define			IC_STATUS		(I2C_BASE + 0x70)
#define 		IC_RX_TL		(I2C_BASE + 0x38)
	
#define			IC_CLR_INTR		(I2C_BASE + 0x40)//Clear Combined and Individual Interrupts
#define			IC_CLR_RX_UNDER		(I2C_BASE + 0x44)
#define			IC_CLR_RX_OVER		(I2C_BASE + 0x48)
#define 		IC_CLR_TX_OVER		(I2C_BASE + 0x4c)
#define 		IC_CLR_RD_REQ		(I2C_BASE + 0x50)
#define 		IC_CLR_TX_ABRT		(I2C_BASE + 0x54)
#define 		IC_CLR_RD_DONE		(I2C_BASE + 0x58)
#define 		IC_CLR_ACTIVITY		(I2C_BASE + 0x5c)
#define 		IC_CLR_STOP_DET		(I2C_BASE + 0x60)
#define 		IC_CLR_START_DET	(I2C_BASE + 0x64)	


#define			TIMER_COUNT_CLK		(66000)  //50MHz
#define			TIMER_BASE_ADDR_	(0x05100000) 

#define			TIMERLOADCOUNT		(TIMER_BASE_ADDR_ + 0x14)
#define 		TIMERCTLREG		(TIMER_BASE_ADDR_ +0x1C)
#define			TIMEREOI		(TIMER_BASE_ADDR_ +0x20)
#define			TIMERINTSTATUS		(TIMER_BASE_ADDR_ +0x24)
#define			TIMERSINTSTATUS	 	(TIMER_BASE_ADDR_ +0xa0)

#define		I2C_STOP_DET		0x200
#define		I2C_START_DET		0x400

#define			REG_READ(addr)		*((volatile unsigned int *)addr)
#define			REG_WRITE(addr, data)	*((volatile unsigned int *)addr) = data


char rtc_dbuf[256];

unsigned char	sent_byte_counter = 0;
unsigned char	rcv_byte_counter = 0;
unsigned char	slv_rcv_stop;

void DelayMS(unsigned int ms)
{
	unsigned int count = ms * TIMER_COUNT_CLK;

	REG_WRITE(TIMERCTLREG, 0);
	REG_WRITE(TIMERLOADCOUNT, count);
	REG_WRITE(TIMERCTLREG, 3);
	
	while((REG_READ(TIMERSINTSTATUS) & 0x2) != 0x2);
	REG_READ(TIMEREOI);
	REG_WRITE(TIMERCTLREG, 0);	
}


void i2c_slave_init()
{
	REG_WRITE(IC_ENABLE, 0);
	REG_WRITE(IC_CON, 0);		//SLAVE MODE

	REG_WRITE(IC_SAR, I2C_SLAVE);

	REG_WRITE(IC_ENABLE, 1);
	sent_byte_counter = 0;
	rcv_byte_counter = 0;
	slv_rcv_stop = 0;

}

void i2c_slave_poll()
{
	unsigned int raw_stat, i;
	for(i = 0; i< 256; i++)
		rtc_dbuf[i] = i + 1;

	while(1)
	{
		while((REG_READ(IC_RAW_INTR_STAT) & I2C_START_DET) != I2C_START_DET); //1: START condition has occurred, slave recevier stop

		while(1)
		{
#if 0
			if((REG_READ(IC_STATUS) & 0x08) == 0x08) //1: Receive FIFO is not empty, slave recevier
			{
				if(rcv_byte_counter < 256)
					rtc_dbuf[rcv_byte_counter++] = (char)REG_READ(IC_DATA_CMD);
			}
#endif
			
			raw_stat = 	REG_READ(IC_RAW_INTR_STAT);
			if(raw_stat & (1 << 5))
			{
				if(sent_byte_counter < 256)
					REG_WRITE(IC_DATA_CMD, rtc_dbuf[sent_byte_counter++]);
				else
					REG_WRITE(IC_DATA_CMD, rtc_dbuf[sent_byte_counter - 1]);
				REG_READ(IC_CLR_RD_REQ);
			}
			
#if 0		
			switch(REG_READ(IC_RAW_INTR_STAT) & 0xE0) //slave transmitter
			{
			case 0x80:
			case 0xc0:
			case 0xe0:
			case 0xa0:
				//RX_DONE : 1<<7, transmission is done.
				// the master does not acknowledge to last byte
				REG_READ(IC_CLR_INTR);
				REG_READ(IC_CLR_RD_DONE);
				REG_READ(IC_CLR_TX_ABRT);
				REG_READ(IC_CLR_RD_REQ);

				REG_READ(IC_CLR_ACTIVITY);
				REG_READ(IC_CLR_STOP_DET);
				REG_READ(IC_CLR_START_DET);
				REG_READ(IC_CLR_TX_OVER);
				sent_byte_counter = 0;
				rcv_byte_counter = 0;
				break;
			case 0x40:
				//TX_ABRT: 1<<6, I2C transmitter abort
				REG_READ(IC_CLR_TX_ABRT);
				REG_READ(IC_CLR_INTR);
				break;
			case 0x20:
			case 0x60:
			//RD_REQ: 1<<5, 
				if(sent_byte_counter < 256)
					REG_WRITE(IC_DATA_CMD, rtc_dbuf[sent_byte_counter++]);
				else
					REG_WRITE(IC_DATA_CMD, rtc_dbuf[sent_byte_counter - 1]);
		
				REG_READ(IC_CLR_TX_ABRT);
				REG_READ(IC_CLR_RD_REQ);
				REG_READ(IC_CLR_INTR);
				break;
			default:
				break;
			}
#endif		
			if((REG_READ(IC_RAW_INTR_STAT) & I2C_STOP_DET) == I2C_STOP_DET) //1: STOP condition has occurred, slave recevier stop
			{
				REG_READ(IC_CLR_STOP_DET);
				REG_READ(IC_CLR_START_DET);
				REG_READ(IC_CLR_INTR);
				REG_READ(IC_CLR_RD_DONE);
				REG_READ(IC_CLR_TX_ABRT);
				REG_READ(IC_CLR_RD_REQ);

				REG_READ(IC_CLR_ACTIVITY);
				REG_READ(IC_CLR_STOP_DET);
				REG_READ(IC_CLR_START_DET);
				REG_READ(IC_CLR_TX_OVER);
				slv_rcv_stop ++;
				break;
			}
		}
	}	
		
#if 0		
		switch(REG_READ(IC_RAW_INTR_STAT) & 0xE0) //slave transmitter
		{
		case 0x80:
		case 0xc0:
		case 0xe0:
		case 0xa0:
			//RX_DONE : 1<<7, transmission is done.
			// the master does not acknowledge to last byte
			REG_READ(IC_CLR_INTR);
			REG_READ(IC_CLR_RD_DONE);
			REG_READ(IC_CLR_TX_ABRT);
			REG_READ(IC_CLR_RD_REQ);

			REG_READ(IC_CLR_ACTIVITY);
			REG_READ(IC_CLR_STOP_DET);
			REG_READ(IC_CLR_START_DET);
			REG_READ(IC_CLR_TX_OVER);
			sent_byte_counter = 0;
			rcv_byte_counter = 0;
			break;
		case 0x40:
			//TX_ABRT: 1<<6, I2C transmitter abort
			REG_READ(IC_CLR_TX_ABRT);
			REG_READ(IC_CLR_INTR);
			break;
		case 0x20:
		case 0x60:
			//RD_REQ: 1<<5, 
			if(sent_byte_counter < 256)
				REG_WRITE(IC_DATA_CMD, rtc_dbuf[sent_byte_counter++]);
			else
				REG_WRITE(IC_DATA_CMD, rtc_dbuf[sent_byte_counter - 1]);
		
			REG_READ(IC_CLR_TX_ABRT);
			REG_READ(IC_CLR_RD_REQ);
			REG_READ(IC_CLR_INTR);
			break;
		default:
			break;
	
		}

		if((REG_READ(IC_STATUS) & 0x08) == 0x08) //1: Receive FIFO is not empty, slave recevier
		{
			if(rcv_byte_counter < 256)
				rtc_dbuf[rcv_byte_counter++] = (char)REG_READ(IC_DATA_CMD);
		}
		
#endif
#if 0
		if((REG_READ(IC_RAW_INTR_STAT) & 0x200) == 0x200) //1: STOP condition has occurred, slave recevier stop
		{
			while((REG_READ(IC_STATUS) & 0x08) == 0x08) //1: Receive FIFO is not empty, slave recevier
			{
				if(rcv_byte_counter < 256)
					str[rcv_byte_counter++] = (char)REG_READ(IC_DATA_CMD);
			}
			REG_READ(IC_CLR_ACTIVITY);
			REG_READ(IC_CLR_STOP_DET);
			REG_READ(IC_CLR_START_DET);
			
			REG_READ(IC_CLR_RX_UNDER);
			REG_READ(IC_CLR_RX_OVER);
			slv_rcv_stop ++;
		}
#endif
	
}

void i2c_master_init(unsigned char slv_addr)
{
	REG_WRITE(IC_ENABLE, 0);

#define MASTER_MODE		(1 << 0)
#define STD_SPEED		(1 << 1)
#define IC_SLAVE_DISABLE	(1 << 6) 	
#define IC_RESTART_EN		(1 << 5) 
	
	REG_WRITE(IC_CON, MASTER_MODE | STD_SPEED | IC_SLAVE_DISABLE|IC_RESTART_EN);

	if(slv_addr == 0)
		REG_WRITE(IC_TAR, I2C_SLAVE);
	else
		REG_WRITE(IC_TAR, slv_addr);		

//	REG_WRITE(IC_TX_TL, 0);

//	REG_WRITE(IC_RX_TL,0x0100);
	
	REG_WRITE(IC_ENABLE, 1);

	REG_READ(I2C_BASE + 0x40);
}


void i2c_send_bytes(unsigned char * out_buf, unsigned int len)
{
	unsigned int i = 1;
	REG_WRITE(IC_DATA_CMD, out_buf[0]);	
	while((REG_READ(IC_RAW_INTR_STAT) & I2C_START_DET) != I2C_START_DET);

	while(i < len && (REG_READ(IC_RAW_INTR_STAT) & I2C_STOP_DET) != I2C_STOP_DET)
	{
		REG_WRITE(IC_DATA_CMD, out_buf[i]);
		while((REG_READ(IC_STATUS) & 0x02) == 0x0) //0: Transmit FIFO is full
		{
				if((REG_READ(IC_RAW_INTR_STAT) & I2C_STOP_DET) == I2C_STOP_DET)
					break;
		}
		i++;
	}
	//while((REG_READ(IC_RAW_INTR_STAT) & 0x10) != 0x10);
	while((REG_READ(IC_RAW_INTR_STAT) & I2C_STOP_DET) != I2C_STOP_DET);
	REG_READ(IC_CLR_STOP_DET);
	REG_READ(IC_CLR_TX_ABRT);
}

void i2c_rcv_bytes(unsigned char * in_buf, unsigned int len)
{
	unsigned int i = 0, j = 1;

	REG_WRITE(IC_DATA_CMD, 0x100);
	while(i < len && (REG_READ(IC_RAW_INTR_STAT) & I2C_STOP_DET) != I2C_STOP_DET)
	{
		if(j < len && (REG_READ(IC_STATUS) & 0x02) == 0x02) //1: Transmit FIFO is not full
		{
			REG_WRITE(IC_DATA_CMD, 0x100);
			j++;
		}
		if((REG_READ(IC_STATUS) & 0x08) == 0x08) //1: Receive FIFO is not empty
			in_buf[i++] = (unsigned char)REG_READ(IC_DATA_CMD);
	}
	while((REG_READ(IC_RAW_INTR_STAT) & I2C_STOP_DET) != I2C_STOP_DET);
	REG_READ(IC_CLR_STOP_DET);
	REG_READ(IC_CLR_TX_ABRT);

}
#define HDMI_ADDR 0x39
void I2C_ReadBlock(unsigned char TPI_BASE_ADDR, unsigned char TPI_Offset, unsigned char * pData, int NBytes)
{
	unsigned char buf[2];
        buf[0] = TPI_Offset;
        i2c_send_bytes(buf, 1); //S+DEVICE+W A RegAddr A        
        i2c_rcv_bytes(pData, NBytes);  //S+ DEVICE + R Data Byte High A Data Byte Low          
}

unsigned char I2C_WriteBlock(unsigned char deviceID, unsigned char offset, unsigned char  *buffer,int length)
{
	int i;	

        unsigned char   out_buf[256];
        out_buf[0] = offset;
        for(i=0; i<length; i++)
		out_buf[i+1] = buffer[i];

        i2c_send_bytes(out_buf, length+1);

}
unsigned char I2C_ReadSegmentBlock(unsigned char deviceID, unsigned char segment, unsigned char offset, unsigned char *buffer, int length)
{
	return 0;

}



void I2C_WriteByte(unsigned char TPI_BASE_ADDR, unsigned char addr,unsigned char value)
{
	unsigned char   out_buf[2];
        out_buf[0] = addr;      
        out_buf[1] = value;
        i2c_send_bytes(out_buf, 2);
}
unsigned char I2C_ReadByte(unsigned char base, unsigned char addr)
{
 	unsigned char buf[2];
        buf[0] = addr;
        i2c_send_bytes(buf, 1); //S+DEVICE+W A RegAddr A        
        i2c_rcv_bytes(buf, 1);  //S+ DEVICE + R Data Byte High A Data Byte Low          
        return buf[0];
	
}
void hdmi_regwrite(unsigned char addr, unsigned int value)
{	
	unsigned char	out_buf[2];
  	out_buf[0] = addr;  	
	out_buf[1] = value;
  	i2c_send_bytes(out_buf, 2);
}

unsigned short hdmi_regread(unsigned char addr)
{  	
	unsigned char buf[2];
	buf[0] = addr;   	
	i2c_send_bytes(buf, 1); //S+DEVICE+W A RegAddr A 	
	i2c_rcv_bytes(buf, 1);  //S+ DEVICE + R Data Byte High A Data Byte Low		
	return buf[0];
}

unsigned int value = 0;
void nusmart_hdmi_config()
{
	int i=0;
	unsigned int buffer[2];
	
	i2c_master_init(HDMI_ADDR);

//initialization
	hdmi_regwrite(0xC7,0);
	hdmi_regwrite(0xBC,1);
	hdmi_regwrite(0xBD,0x82);
	value = hdmi_regread(0xBE);
	value |= 0x1;
	hdmi_regwrite(0xBE,value);
	

	//Identification
	while(1)
	value = hdmi_regread(0x1B);
	value = hdmi_regread(0x1C);
	value = hdmi_regread(0x1D);

	//power up
	hdmi_regwrite(0x1A,0x11);
	hdmi_regwrite(0x1E,0);
	
	//input bus and pixel
	hdmi_regwrite(0x08,0x60);
		
	//YC input mode
	hdmi_regwrite(0x0B,0x0);


//hot plug event

	//video format and resolution
	//pixel clock
	value = 25000000/10000;
	hdmi_regwrite(0x0,value & 0xFF);
	hdmi_regwrite(0x1,(value >> 8) & 0xFF);

	//vertical freq	
	value = 60;
	hdmi_regwrite(0x2,value & 0xFF);
	hdmi_regwrite(0x3,(value >> 8) & 0xFF);

	//pixel in line
	value = 640;
	hdmi_regwrite(0x4,value & 0xFF);
	hdmi_regwrite(0x5,(value >> 8) & 0xFF);
	
	//lines
	value = 480;	
	hdmi_regwrite(0x6,value & 0xFF);
	hdmi_regwrite(0x7,(value >> 8) & 0xFF);


	hdmi_regwrite(0x1A,0x1);	
}

