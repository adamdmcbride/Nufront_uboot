/*
 * (C) Copyright 2012 Nufront Software, Inc
 *
 * Bai Nianfu, nianfu.bai@nufront.com
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 */


#include <common.h>
#include <command.h>
#include <nusmart_i2c.h>

#ifdef CONFIG_CMD_BRINGUP

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

#define BRINGUP_CASE(pname, pfunc) \
	{.name = pname, .func = pfunc}


extern int do_mem_mtest (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);

struct bringup_case_args
{
	char* name;
	void (*func)();
};


static void printf_separator()
{
    printf("***************************************\n");
}


static void printf_blankline(int number)
{
    int i;

    for(i=0;i<number;i++)
        printf("\n");
}


static void bringup_ec()
{
	unsigned char wbuf = 0x00;
    unsigned char rbuf = 0x00;
    int ret;


    wbuf = 0x5A;
    ret = io373x_smbus_write_reg(NS115_I2C0_BASE, SLV_ADDR_IO373X, IO373X_EVENT_ADDR, &wbuf, 1);
    if(ret){
        printf("ERROR: ec test failed, write ec register error\n");
        return;
    }

    ret = io373x_smbus_read_reg(NS115_I2C0_BASE, SLV_ADDR_IO373X, IO373X_EVENT_ADDR, &rbuf, 1);
    if(ret){
        printf("ERROR: ec test failed, read ec register error\n");
        return;
    }

    if(rbuf != 0x5A)
        printf("ERROR: write data does not match read data\n");

	printf("EC: test passed\n");

}

static void bringup_g_sensor()
{
	unsigned char rbuf[4];
    unsigned char wbuf[4];
    int ret = 0;

    rbuf[0] = 0x00;
    wbuf[0] = 0x0F;
    dw_i2c_master_init(NS115_I2C0_BASE, SLV_ADDR_G_SENSOR);
    ret = dw_i2c_smbus_read(NS115_I2C0_BASE, wbuf, 1, rbuf, 1);
    if(ret){
        printf("ERROR: read g-sensor register failed\r\n");
        return;
    }

    if(rbuf[0] == 0x07)
        printf("g-sensor test passed\r\n");
    else{
        printf("ERROR: read data does not match fixed value, return value is %d\n", rbuf[0]);
    }	
}


/* ecompass ami306 chipset bringup */
static int ami306_chipset_reset(void)
{	
    unsigned char wbuf[4];
    int ret = 0;

    wbuf[0] = 0x1D;
	wbuf[1] = 0x80;
    dw_i2c_master_init(NS115_I2C0_BASE, SLV_ADDR_E_SENSOR);
    ret = dw_i2c_send_bytes(NS115_I2C0_BASE, wbuf, 2);
    if(ret)
	{
        printf("ERROR: read ecompass-sensor register failed\r\n");
        return -1;
    }
	
	return 0;
}


static int ami306_chipset_identify(void)
{
	unsigned char rbuf[4];
    unsigned char wbuf[4];
    int ret = 0;

    rbuf[0] = 0x00;
    wbuf[0] = 0x0F;
    dw_i2c_master_init(NS115_I2C0_BASE, SLV_ADDR_E_SENSOR);
    ret = dw_i2c_smbus_read(NS115_I2C0_BASE, wbuf, 1, rbuf, 1);
    if(ret){
        printf("ERROR: read ecompass-sensor register failed\r\n");
        return -1;
    }

    if(rbuf[0] == 0x46)
       return 0;
    else{
	   printf("ERROR: read AMI306 IDdata does not match fixed value(0x46), return value is %d\n", rbuf[0]);	
       return -1;
    }	

}

static void bringup_e_sensor()
{
	int ret;

	ret = ami306_chipset_reset();
	if (ret)
	{
		printf("AMI306 reset failed\r\n");
		return;
	}
    udelay(1000);
	
	ret = ami306_chipset_identify();
	if (ret)
	{
		printf("ERROR:Ecompass ami306 test failed, can't probe it\r\n");
	}
	else
	{	
		printf("ecompass-sensor ami306 test passed\r\n");
	}

}



static struct bringup_case_args bringup_case[] = 
{
	BRINGUP_CASE("EC", (void*)bringup_ec),
	BRINGUP_CASE("g-sensor", (void*)bringup_g_sensor),
	BRINGUP_CASE("e-sensor", (void*)bringup_e_sensor),
	BRINGUP_CASE("memory test", (void*)do_mem_mtest),

};


static void bringup_options()
{
	int i;

	printf_separator();
	printf("Choose action:\n");
	printf("0 : exit bringup mode\n");

	for(i=0; i<ARRAY_SIZE(bringup_case); i++){
		if(i < 9){
			printf("%d : %s test\n", i+1, bringup_case[i].name);
		}
		else{
			printf("%c : %s test\n", i+88, bringup_case[i].name);
		}	
	}	
}

static void bringup_process(char key)
{

	if((key > 48) && (key < 58)){
		printf_blankline(2);
		printf_separator();
		printf("Test name: %s\n", bringup_case[key-49].name);
		printf("Test result: \n");
		bringup_case[key-49].func();
	}
	else if((key > 96) && (key < 123)){
		printf_blankline(2);
		printf_separator();
		printf("Test name: %s\n", bringup_case[key-88].name);
		printf("Test result: \n");
		bringup_case[key-88].func();
	}
	else{
		printf("ERROR: invalid case number\n");
	}
} 

static int do_bringup(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	char key;
	char semi_cmd[256] = {0};
	int ret;
	int semi_mode = 0;

	while(1){
		bringup_options();
		key = serial_getc();

		//48 is the ascii value of '0'
		if(key == 48)
			return;

		bringup_process(key);
	}

	return 0;
}


U_BOOT_CMD(
	bringup, 3, 1, do_bringup,
	"hardware detect\n",
	"enter bringup command to terminal and press enter key"
);

#endif
