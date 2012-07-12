/*
 * Copyright (C) 2010 Nufront Beijing, Inc. All rights reserved.
 *
 * Author: Shuai Yang <shuai.yang@nufront.com>, Mar, 2010
 *
 * Description:
 * Synopsys GMAC driver.
 * *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of  the GNU General  Public License as published by
 * the Free Software Foundation;  either version 2 of the  License, or
 * (at your option) any later version.
 */

#include <common.h>
#include <malloc.h>
#include <net.h>
#include <netdev.h>
//#include <asm/io.h>

/*macro/enum define*/
#define IOBASE	0x050A0000
#if defined(CONFIG_ARCH_REALVIEW_PB)
#define FPGA_BASE 0x70000000
#endif

#define FPGA_BASE 0x00000000

#define DESC_COUNT 0x10
#define MAX_PACKET_SIZE 1514
#define MAX_FRAME_LENGTH 1514 + 34   //must be multiple of the 32bit
#define TDES0_DEFAULT	0x00000000
#define TDES1_DEFAULT	0xF9000000
#define TDES1_DEFAULT_LAST	0xFB000000
#define RDES0_DEFAULT	0x80000000
#define RDES1_DEFAULT	0x01000000 + MAX_FRAME_LENGTH
#define RDES1_DEFAULT_LAST	0x03000000 + MAX_FRAME_LENGTH

#define outl(v,p)	(*(volatile unsigned int *)(p)=(v))
#define inl(a)	(*(volatile unsigned int *)(a))

enum emac_offsets{
	DR0 = 0x1000,DR1 = 0x1004,DR2 = 0x1008,DR3 = 0x100C,DR4 = 0x1010,DR6 = 0x1018,GR0 = 0x0000,
	GR1 = 0x0004,GR4 = 0x0010,GR5 = 0x0014,GR16 = 0x0040,GR17 = 0x0044
};

enum emac_DR6_bits{
	DR6_SR = 0x2, DR6_ST = 0x2000

};

enum emac_GR0_bits{
	GR0_DM = 0x800,GR0_TE = 0x8,GR0_RE = 0x4
};

enum emac_GR4_bits{
	GR4_CR = 0x10, GR4_GW=0x2, GR4_GB=0x1 
};

/*structure declaration*/
struct emac_board_info
{
	unsigned int ioaddr;
	unsigned int phy_addr;
	unsigned int duplex;
	struct txdesc *tx_first_desc;
	struct rxdesc *rx_first_desc;

	unsigned int tx_packet_count;
	struct txdesc *tx_insert_desc;
	struct txdesc *tx_remove_desc;

	struct rxdesc *rx_ready_desc;
	

};

struct txdesc{
	unsigned int tdes0;
	unsigned int tdes1;
	unsigned char *buffer;
	struct txdesc *ndescriptor;
};

struct rxdesc{
	unsigned int rdes0;
	unsigned int rdes1;
	unsigned char *buffer;
	struct rxdesc *ndescriptor;
};

/*Global variable declaration*/
static struct txdesc tx_desc_array[DESC_COUNT]
	__attribute__ ((aligned(32)));

static unsigned char tx_buf_array[DESC_COUNT][MAX_FRAME_LENGTH];

static struct rxdesc rx_desc_array[DESC_COUNT]
	__attribute__ ((aligned(32)));

static unsigned char rx_buf_array[DESC_COUNT][MAX_FRAME_LENGTH];

/*fuction declaration */
int emac_initialize(bd_t *bis);

static int emac_init_one(struct eth_device *dev, bd_t *bis);
static int emac_start_xmit(struct eth_device *dev, volatile void *packet, int length);
static int emac_rx_packet(struct eth_device *dev);
static int emac_disable(struct eth_device *dev);

static int phy_find(unsigned int *phy_addr);
static void emac_descriptor_init(struct eth_device *dev);
static struct txdesc* txdesc_init(unsigned int num);
static struct rxdesc* rxdesc_init(unsigned int num);
static void phy_read_macaddr(struct eth_device *dev);/*48bit OUI id*/
static void phy_set_register(struct eth_device *dev);

/*emac routine*/
int emac_initialize(bd_t *bis)
{
	//printf("nufront:emac initialize\n");
	unsigned int card_number = 0;
	unsigned int phy_addr = 0;
	struct eth_device *dev;
	struct emac_board_info *db;
		
	if(phy_find(& phy_addr))
	{
		//printf("nufront:phy is found:%d\n",phy_addr);
		
		dev = (struct eth_device *)malloc(sizeof(struct eth_device));
		sprintf(dev->name, "emac:%d", card_number);
		db = (struct emac_board_info *)malloc(sizeof(struct emac_board_info));

		dev->priv = db;
		dev->iobase = cpu_to_le32(IOBASE);

		dev->init = emac_init_one;
		dev->halt = emac_disable;
		dev->send = emac_start_xmit;
		dev->recv = emac_rx_packet;

		/* init db */
		db->ioaddr = dev->iobase;
		db->phy_addr = cpu_to_le32(phy_addr);

		eth_register(dev);
		card_number++;
		
		udelay(10 * 1000);
	}

	return card_number;	

}

static int emac_init_one(struct eth_device *dev, bd_t *bis)
{
	//printf("nufront:emac init one\n");
	unsigned int rdata=0;
	unsigned int wdata=0;
	struct emac_board_info *db;
	db = dev->priv;

	//init descriptor
	emac_descriptor_init(dev);

	//init db
	db->tx_packet_count = 0;
	db->tx_insert_desc = db->tx_first_desc;
	db->tx_remove_desc = db->tx_first_desc;
	db->rx_ready_desc = db->rx_first_desc;

	//reset mac register
	wdata=cpu_to_le32(1);
	outl(wdata,IOBASE + DR0);
	do{
		rdata = inl(IOBASE + DR0);
	}while(rdata & cpu_to_le32(0x00000001));
	
	//set phy register
	phy_set_register(dev);	

	//read 48bit mac addr
	phy_read_macaddr(dev);		

	//set dma register
	outl((unsigned int)db->rx_first_desc,IOBASE + DR3);
	outl((unsigned int)db->tx_first_desc,IOBASE + DR4);
	outl(0x400,IOBASE + GR1);// DA filter
	outl(inl(IOBASE + GR0) | (GR0_DM & ((db->duplex==0x5 || db->duplex==0x6)?0xFFF:0x0)) 
			| GR0_TE | GR0_RE,IOBASE + GR0);

//	printf("the value:%x\n",(db->duplex==0x5 || db->duplex==0x6)?0x1:0x0);

	//starting dma
	rdata = inl(IOBASE + DR6);
	rdata |= cpu_to_le32(DR6_SR | DR6_ST);
	outl(rdata,IOBASE + DR6);

			
}

static int emac_start_xmit(struct eth_device *dev, volatile void *packet, int length)
{
	//printf("nufront:emac start xmit\n");
	struct emac_board_info *db = dev->priv;
       	struct txdesc *txptr;
	unsigned int tdes0;
       	unsigned int len = length;
       	/* Too large packet check */
       	if (len > MAX_PACKET_SIZE) {
		printf(": big packet = %d\n", len);
                return 0;
       	}
	
	/* No Tx resource check, it never happen normally */
        if (db->tx_packet_count >= (DESC_COUNT -2)) {
                printf("No Tx resource %d\n", db->tx_packet_count);
                return 0;
        }

        /* transmit this packet */
        txptr = db->tx_insert_desc;
        memcpy((char *)txptr->buffer, (char *)packet, (int)length);
        txptr->tdes1 = cpu_to_le32(TDES1_DEFAULT | length);

        /* Point to next transmit free descriptor */
        db->tx_insert_desc = txptr->ndescriptor;

        /* Transmit Packet Process */
        if ((db->tx_packet_count < DESC_COUNT)) {
                txptr->tdes0 = cpu_to_le32(TDES0_DEFAULT | 0x80000000); /* Set owner bit */
                db->tx_packet_count++;                    /* Ready to send */
                outl(0x1, IOBASE + DR1);  /* Issue Tx polling */
		//printf("nufront:add a packet to send\n");
		
        }
    	/* Got emac status */
        //db->cr5_data = inl(db->ioaddr + DCR5);
        //outl(db->cr5_data, db->ioaddr + DCR5);


        txptr = db->tx_remove_desc;
        while (db->tx_packet_count){
                tdes0 = le32_to_cpu(txptr->tdes0);
                /* printf(DRV_NAME ": tdes0=%x\n", tdes0); */
                if (tdes0 & 0x80000000)
                        break;

                /* A packet sent completed */
		//printf("nufront:send a packet\n");
                db->tx_packet_count--;

/*                if (tdes0 != 0x7fffffff) {
                        if (tdes0 & TDES0_ERR_MASK) {
                                if (tdes0 & 0x0002) {  // UnderRun 
                                        if (!(db->cr6_data & CR6_SFT)) {
                                                db->cr6_data = db->cr6_data |
                                                        CR6_SFT;
                                                update_cr6(db->cr6_data,
                                                        db->ioaddr);
                                        }
                                }
                        }
                }
*/
                txptr = txptr->ndescriptor;
        }/* End of while */

        /* Update TX remove pointer to next */
        db->tx_remove_desc = txptr;

        return length;
	
}
static int emac_rx_packet(struct eth_device *dev)
{
       	struct emac_board_info *db = dev->priv;
        struct rxdesc *rxptr;
        unsigned int rxlen = 0;
        unsigned int rdes0;

        rxptr = db->rx_ready_desc;

        rdes0 = le32_to_cpu(rxptr->rdes0);

        if (!(rdes0 & 0x80000000)) {    /* packet owner check */
                if ((rdes0 & 0x300) != 0x300) {
                        /* A packet without First/Last flag */
                        /* reuse this buf */
                        printf("A packet without First/Last flag");
                        rxptr->rdes0 = cpu_to_le32(RDES0_DEFAULT);
                } else {
                        /* A packet with First/Last flag */
                        rxlen = ((rdes0 >> 16) & 0x3fff) - 4;//drop the CRC

                        /* error summary bit check */
                        if (rdes0 & 0x8000) {
                                /* This is a error packet */
                                printf("Error: rdes0: %x\n", rdes0);
                        	rxptr->rdes0 = cpu_to_le32(RDES0_DEFAULT);
                        }

                        if (!(rdes0 & 0x8000)) {
				//printf("nufront:receive a packet\n");

                                NetReceive((uchar *)rxptr->buffer, rxlen);
                        	rxptr->rdes0 = cpu_to_le32(RDES0_DEFAULT);
                        }
                  }

                rxptr = rxptr->ndescriptor;
        }

        db->rx_ready_desc = rxptr;
        
	outl(0x1, IOBASE + DR2);  /* Issue Rx polling */

        return rxlen;

}

static int emac_disable(struct eth_device *dev)
{
	//printf("nufront:emac disable\n");
        struct emac_board_info *db = dev->priv;
	unsigned int rdata=0;
	unsigned int wdata=0;
	//reset mac register
	wdata=cpu_to_le32(1);
	outl(wdata,IOBASE + DR0);
	do{
		rdata = inl(IOBASE + DR0);
	}while(rdata & cpu_to_le32(0x00000001));
	
	//stop dma
	rdata = inl(IOBASE + DR6);
	rdata &= cpu_to_le32(~(DR6_SR | DR6_ST));
	outl(rdata,IOBASE + DR6);

}

static int phy_find(unsigned int *phy_addr)
{
	unsigned int ret=0;
	unsigned int GR4_PA=0;
	unsigned int GR4_GR=0;

	for(GR4_PA=0;GR4_PA<32;GR4_PA++)
	{
		outl((GR4_PA << 11) | (GR4_GR << 6) | GR4_CR | GR4_GB , IOBASE + GR4);

		udelay(5);

		while(inl(IOBASE + GR4) & cpu_to_le32(0x00000001))
			udelay(10);	
				
		if(inl(IOBASE + GR5) != cpu_to_le32(0xFFFF))
		{
			ret=1;
			*phy_addr=GR4_PA;
			break;
		}

	}

	return ret;
}

static void emac_descriptor_init(struct eth_device *dev)
{
	struct emac_board_info *db;
	db = dev->priv;
	db->tx_first_desc = txdesc_init(DESC_COUNT);
	db->rx_first_desc = rxdesc_init(DESC_COUNT);
	//printf("nufront:tx_first_desc%x\n",(unsigned int)db->tx_first_desc);
	//printf("nufront:rx_first_desc%x\n",(unsigned int)db->rx_first_desc);
}

static struct txdesc* txdesc_init(unsigned int num)
{
        struct txdesc *ret=0;
        struct txdesc *prev=0;
        struct txdesc *next=0;
        unsigned int i;

        for(i=0;i<num;i++){
                if(i==0){
                        prev=(struct txdesc *)((unsigned int)(&tx_desc_array[i]) | FPGA_BASE);
                        ret=prev;
                        prev->buffer=(unsigned char*)((unsigned int)&tx_buf_array[i][0] | FPGA_BASE);
                }
                else{
                        next=(struct txdesc *)((unsigned int)(&tx_desc_array[i]) | FPGA_BASE);
                        prev->ndescriptor=next;
                        prev=next;
                        prev->buffer=(unsigned char*)((unsigned int)(&tx_buf_array[i][0]) | FPGA_BASE);
                }

                prev->tdes0 = cpu_to_le32(TDES0_DEFAULT);
		prev->tdes1 = cpu_to_le32(TDES1_DEFAULT);
        }

        prev->tdes1 = cpu_to_le32(TDES1_DEFAULT_LAST);
        prev->ndescriptor = (struct txdesc *)((unsigned int)(&tx_desc_array[0]) | FPGA_BASE);

        return ret;
}

static struct rxdesc* rxdesc_init(unsigned int num)
{
        struct rxdesc *ret=0;
        struct rxdesc *prev=0;
        struct rxdesc *next=0;
        unsigned int i;

        for(i=0;i<num;i++){
                if(i==0){
                        prev=(struct rxdesc *)((unsigned int)(&rx_desc_array[i]) | FPGA_BASE);
                        ret=prev;
                        prev->buffer=(unsigned char*)((unsigned int)(&rx_buf_array[i][0]) | FPGA_BASE);
                }
                else{
                        next=(struct rxdesc *)((unsigned int)(&rx_desc_array[i]) | FPGA_BASE);
                        prev->ndescriptor=next;
                        prev=next;
                        prev->buffer=(unsigned char*)((unsigned int)(&rx_buf_array[i][0]) | FPGA_BASE);
                }

                prev->rdes0 = cpu_to_le32(RDES0_DEFAULT);
		prev->rdes1 = cpu_to_le32(RDES1_DEFAULT);
        }

        prev->rdes1 = cpu_to_le32(RDES1_DEFAULT_LAST);
        prev->ndescriptor = (struct rxdesc *)((unsigned int)(&rx_desc_array[0]) | FPGA_BASE);

        return ret;
}

static void phy_read_macaddr(struct eth_device *dev)
{
	dev->enetaddr[0] = cpu_to_le32(0x00);	
	dev->enetaddr[1] = cpu_to_le32(0x01);	
	dev->enetaddr[2] = cpu_to_le32(0x02);	
	dev->enetaddr[3] = cpu_to_le32(0x03);	
	dev->enetaddr[4] = cpu_to_le32(0x04);	
	dev->enetaddr[5] = cpu_to_le32(0x05);	

	outl(0x80000504,IOBASE + GR16);
	outl(0x03020100,IOBASE + GR17);	

}

static void phy_set_register(struct eth_device *dev)
{
	struct emac_board_info *db;
	db = dev->priv;
	
	unsigned int rdata=0;
	unsigned int wdata=0;
	unsigned int GR4_GR=0;

	//read the phy register18
	GR4_GR=18;
	
	wdata = db->phy_addr << 11 | GR4_CR | GR4_GB | GR4_GR << 6;

	outl(wdata,IOBASE + GR4);

	while(inl(IOBASE + GR4) & cpu_to_le32(0x00000001))
		udelay(10);
	
	rdata = inl(IOBASE + GR5);
//	printf("phy register18:%x\n",rdata);

	//setting phy register18 for auto-negotiation
	wdata = rdata | 0xE0;

	outl(wdata, IOBASE + GR5);

	GR4_GR=18;
	
	wdata = db->phy_addr << 11 | GR4_CR | GR4_GB | GR4_GW | GR4_GR << 6;

	outl(wdata,IOBASE + GR4);

	while(inl(IOBASE + GR4) & cpu_to_le32(0x00000001))
		udelay(10);
	//read the phy register0
	GR4_GR=0;
	
	wdata = db->phy_addr << 11 | GR4_CR | GR4_GB | GR4_GR << 6;
	
	outl(wdata,IOBASE + GR4);

	while(inl(IOBASE + GR4) & cpu_to_le32(0x00000001))
		udelay(10);
	
	rdata = inl(IOBASE + GR5);

//	printf("phy register0:%x\n",rdata);

	//setting phy register0 for restart auto-negotiation
/*	wdata = rdata | 0x200;

	outl(wdata, IOBASE + GR5);

	GR4_GR=0;
	
	wdata = db->phy_addr << 11 | GR4_CR | GR4_GB | GR4_GW | GR4_GR << 6;

	outl(wdata,IOBASE + GR4);

	while(inl(IOBASE + GR4) & cpu_to_le32(0x00000001))
		udelay(10);
*/
	//read the phy register31
	GR4_GR=31;
	
	wdata = db->phy_addr << 11 | GR4_CR | GR4_GB | GR4_GR << 6;
	
	outl(wdata,IOBASE + GR4);

	while(inl(IOBASE + GR4) & cpu_to_le32(0x00000001))
		udelay(10);
	
	rdata = inl(IOBASE + GR5);

//	printf("phy register31:%x\n",rdata);

	db->duplex = cpu_to_le32((rdata >> 2) & 0x0000000F);// duplex mode 
	
	//setting to full duplex

/*	wdata = rdata | 0x00000100;

	outl(wdata,IOBASE + GR5);

	wdata = db->phy_addr << 11 | GR4_CR | GR4_GB | GR4_GW;

	outl(wdata,IOBASE + GR4);

	while(inl(IOBASE + GR4) & cpu_to_le32(0x00000001))
		udelay(10);
*/
}
