/*
 * Copyright (C) Freescale Semiconductor, Inc. 2006.
 * Author: Jason Jin<Jason.jin@freescale.com>
 *         Zhang Wei<wei.zhang@freescale.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 * with the reference on libata and ahci drvier in kernel
 *
 */
#include <common.h>

#include <command.h>
#include <pci.h>
#include <asm/processor.h>
#include <asm/errno.h>
#include <asm/io.h>
#include <malloc.h>
#include <scsi.h>
#include <ata.h>
#include <linux/ctype.h>
#include <ahci.h>

static inline int __ilog2(unsigned int x)
{
	int i=0;
	unsigned int tmp;

	for(i=0; i<32; i++)
	{
		tmp = (x<<i);
	
		if(0x80000000 & tmp)
		{
			return 31-i;
		}
	}
	return 0;
}

//800ms
#define CMD_WAIT_TIME		(1500)
#define NS2816_SATA_BASE	(0x050b0000)
#define SCM_BASE		(0x051c0000)
#define SCM_SATA_CS		((SCM_BASE) + 0x2c)

struct ahci_probe_ent *probe_ent = NULL;
hd_driveid_t *ataid[AHCI_MAX_PORTS];
u32 mem_alloc[(AHCI_PORT_PRIV_DMA_SZ + 2048)/4];

void scm_sata_control()
{
	u32 value;

	value = __raw_readl(SCM_SATA_CS);
	value &= ~(0x38); //[5:3] clear
	value |= 0x18; //[5:3] set 011
	__raw_writel(value, SCM_SATA_CS);

	value = __raw_readl(SCM_SATA_CS);
	printf("SATA Control and Status = 0x%08x\n", value);
}

static inline u32 ahci_port_base(u32 base, u32 port)
{
	return base + 0x100 + (port * 0x80);
}


static void ahci_setup_port(struct ahci_ioports *port, unsigned long base,
			    unsigned int port_idx)
{
	base = ahci_port_base(base, port_idx);

	port->cmd_addr = base;
	port->scr_addr = base + PORT_SCR;
}

static void printf_ahci_reg(u32 mmio)
{
	u32 port0_base = ahci_port_base(mmio, 0);
	
	printf("\nprintf_ahci_reg mmio=0x%08x port0_base=0x%08x\n", 
		mmio, port0_base); 
	
	printf("GCSR register map:\n");
	printf("0x051c002c: 0x%08x\n\n", __raw_readl(0x051c002c)); 
	printf("CAP	: 0x%08x\n", __raw_readl(mmio + 0x00)); 
	printf("GHC	: 0x%08x\n", __raw_readl(mmio + 0x04)); 
	printf("IS	: 0x%08x\n", __raw_readl(mmio + 0x08)); 
	printf("PI	: 0x%08x\n", __raw_readl(mmio + 0x0c)); 
	printf("VS	: 0x%08x\n", __raw_readl(mmio + 0x10)); 
	printf("CCC_CTL	: 0x%08x\n", __raw_readl(mmio + 0x14)); 
	printf("CCC_PORTS: 0x%08x\n", __raw_readl(mmio + 0x18)); 
	printf("HOST_EM_LOC: 0x%08x\n", __raw_readl(mmio + 0x1c)); 
	printf("HOST_EM_CTL: 0x%08x\n", __raw_readl(mmio + 0x20)); 
	printf("CAP2	: 0x%08x\n", __raw_readl(mmio + 0x24)); 
	printf("BISTAFR	: 0x%08x\n", __raw_readl(mmio + 0xa0)); 
	printf("BISTCR	: 0x%08x\n", __raw_readl(mmio + 0xa4)); 
	printf("BISTFCTR: 0x%08x\n", __raw_readl(mmio + 0xa8)); 
	printf("BISTSR	: 0x%08x\n", __raw_readl(mmio + 0xac)); 
	printf("BISTDECR: 0x%08x\n", __raw_readl(mmio + 0xb0)); 
	printf("0xb4	: 0x%08x\n", __raw_readl(mmio + 0xb4));
	printf("0xb8	: 0x%08x\n", __raw_readl(mmio + 0xb8));
	printf("OOBR	: 0x%08x\n", __raw_readl(mmio + 0xbc)); 
	printf("TIMER1MS: 0x%08x\n", __raw_readl(mmio + 0xe0)); 
	printf("GPARAM1R: 0x%08x\n", __raw_readl(mmio + 0xe8)); 
	printf("GPARAM2R: 0x%08x\n", __raw_readl(mmio + 0xec)); 
	printf("PPARAMR	: 0x%08x\n", __raw_readl(mmio + 0xf0)); 
	printf("TESTR	: 0x%08x\n", __raw_readl(mmio + 0xf4)); 
	printf("VERSIONR: 0x%08x\n", __raw_readl(mmio + 0xf8)); 
	printf("IDR	: 0x%08x\n", __raw_readl(mmio + 0xfc)); 

	printf("\nPCSR register port0 map:\n");
	printf("P#CLB	: 0x%08x\n", __raw_readl(port0_base + 0x00)); 
	printf("P#CLBU	: 0x%08x\n", __raw_readl(port0_base + 0x04)); 
	printf("P#FB	: 0x%08x\n", __raw_readl(port0_base + 0x08)); 
	printf("P#FBU	: 0x%08x\n", __raw_readl(port0_base + 0x0c)); 
	printf("P#IS	: 0x%08x\n", __raw_readl(port0_base + 0x10)); 
	printf("P#IE	: 0x%08x\n", __raw_readl(port0_base + 0x14)); 
	printf("P#CMD	: 0x%08x\n", __raw_readl(port0_base + 0x18)); 
	printf("P#TFD	: 0x%08x\n", __raw_readl(port0_base + 0x20)); 
	printf("P#SIG	: 0x%08x\n", __raw_readl(port0_base + 0x24)); 
	printf("P#SSTS	: 0x%08x\n", __raw_readl(port0_base + 0x28)); 
	printf("P#SCTL	: 0x%08x\n", __raw_readl(port0_base + 0x2c)); 
	printf("P#SERR	: 0x%08x\n", __raw_readl(port0_base + 0x30)); 
	printf("P#SACT	: 0x%08x\n", __raw_readl(port0_base + 0x34)); 
	printf("P#CI	: 0x%08x\n", __raw_readl(port0_base + 0x38)); 
	printf("P#SNTF	: 0x%08x\n", __raw_readl(port0_base + 0x3c)); 
	printf("P#DMACR	: 0x%08x\n", __raw_readl(port0_base + 0x70)); 
	printf("P#PHYCR	: 0x%08x\n", __raw_readl(port0_base + 0x78)); 
	printf("P#PHYSR	: 0x%08x\n", __raw_readl(port0_base + 0x7c)); 
}

void sata_clear_interrupt(u32 base_addr)
{
	u32 tmp;
	u32 port_mmio;

	port_mmio=ahci_port_base(base_addr, 0);

	tmp = __raw_readl(base_addr + HOST_IRQ_STAT);
	
	if(tmp)
		__raw_writel(tmp, base_addr + HOST_IRQ_STAT);

	tmp = __raw_readl(port_mmio + PORT_IRQ_STAT);

	if(tmp)
		__raw_writel(tmp, port_mmio + PORT_IRQ_STAT);
}

#define msleep(a) udelay(a * 1000)
#define ssleep(a) msleep(a * 1000)

/* we do bios initial at here */
static void ahci_bios_init(u32 base_addr)
{
	u32 tmp;

        __raw_writel(0x0, base_addr + HOST_CAP);
        msleep(500);
        tmp = __raw_readl(base_addr + HOST_CAP);
        debug("SATA CAP=0x%08x\n", tmp);

        __raw_writel(0x1, base_addr + HOST_PORTS_IMPL);
        msleep(100);

        tmp = __raw_readl(base_addr + HOST_PORTS_IMPL);
        debug("SATA HOST_PORTS_IMPL=0x%08x\n", tmp);
}

static int waiting_for_cmd_completed(u32 offset,
				     int timeout_msec,
				     u32 sign)
{
	int i;
	u32 status;

	for (i = 0; ((status = __raw_readl(offset)) & sign) && i < timeout_msec; i++)
		msleep(1);

//	if(i>=timeout_msec)
//		printf_ahci_reg(NS2816_SATA_BASE);	
	return (i < timeout_msec) ? 0 : -1;
}

static void ahci_enable_ahci(u32 mmio)
{
	int i;
	u32 tmp;

	/* turn on AHCI_EN */
	tmp = __raw_readl(mmio + HOST_CTL);

	if (tmp & HOST_AHCI_EN)
		return;

	/* Some controllers need AHCI_EN to be written multiple times.
	 * Try a few times before giving up.
	 */
	for (i = 0; i < 5; i++) {
		tmp |= HOST_AHCI_EN;
		__raw_writel(tmp, mmio + HOST_CTL);
		tmp = __raw_readl(mmio + HOST_CTL);	/* flush && sanity check */
		if (tmp & HOST_AHCI_EN)
			return;
		msleep(10);
	}

	debug("ahci_enable_ahci CTL=0x%08x\n", tmp);
}

static int ahci_host_init(struct ahci_probe_ent *probe_ent)
{
	u32 mmio = probe_ent->mmio_base;
	u32 tmp, cap_save;
	int i, j;
	u32 port_mmio;

	scm_sata_control();
	/* we do bios initial at here */
	ahci_bios_init(mmio);

	//ahci enable 
	ahci_enable_ahci(mmio);

	/* global controller reset */
	tmp = __raw_readl(mmio + HOST_CTL);
	if ((tmp & HOST_RESET) == 0)
		__raw_writel(tmp | HOST_RESET, mmio + HOST_CTL);

	/* reset must complete within 1 second, or
	 * the hardware should be considered fried.
	 */
	ssleep(1);

	tmp = __raw_readl(mmio + HOST_CTL);
	if (tmp & HOST_RESET) {
		printf("controller reset failed (0x%x)\n", tmp);
		return -1;
	}

	//ahci enable 
	ahci_enable_ahci(mmio);

	probe_ent->cap = __raw_readl(mmio + HOST_CAP);
	probe_ent->port_map = __raw_readl(mmio + HOST_PORTS_IMPL);
	probe_ent->n_ports = (probe_ent->cap & 0x1f) + 1;

	debug("cap 0x%x  port_map 0x%x  n_ports %d\n",
	      probe_ent->cap, probe_ent->port_map, probe_ent->n_ports);

	for (i = 0; i < probe_ent->n_ports; i++) {
		probe_ent->port[i].port_mmio = ahci_port_base(mmio, i);
		port_mmio = probe_ent->port[i].port_mmio;
		ahci_setup_port(&probe_ent->port[i], mmio, i);

		/* make sure port is not active */
		tmp = __raw_readl(port_mmio + PORT_CMD);
		if (tmp & (PORT_CMD_LIST_ON | PORT_CMD_FIS_ON |
			   PORT_CMD_FIS_RX | PORT_CMD_START)) {
			tmp &= ~(PORT_CMD_LIST_ON | PORT_CMD_FIS_ON |
				 PORT_CMD_FIS_RX | PORT_CMD_START);
			__raw_writel(tmp, port_mmio + PORT_CMD);

			/* spec says 500 msecs for each bit, so
			 * this is slightly incorrect.
			 */
			msleep(500);
		}

		/* clear SError */
		while(j<10)
		{
			tmp = __raw_readl(port_mmio + PORT_SCR_ERR);
			debug("PORT_SCR_ERR 0x%x\n", tmp);
			if(0x04050002 == tmp)
			{
				__raw_writel(tmp, port_mmio + PORT_SCR_ERR);
				printf("success polled the DIAG_X bit!\n");
				break;
			}
			j++;
			msleep(50);
		}

		//wait SSTS 
		j = 0;
		while (j < 100) {
                        msleep(10);
                        tmp = readl(port_mmio + PORT_SCR_STAT);
                        if ((tmp & 0xf) == 0x3)
			{
				printf("SSTS poll OK, SSTS=0x%08x\n", tmp);
                                break;
			}
                        j++;
                }


		j = 0;

		/* ack any pending irq events for this port */
		tmp = __raw_readl(port_mmio + PORT_IRQ_STAT);
		debug("PORT_IRQ_STAT 0x%x\n", tmp);
		if (tmp)
			__raw_writel(tmp, port_mmio + PORT_IRQ_STAT);

		__raw_writel(1 << i, mmio + HOST_IRQ_STAT);

		/* set irq mask (enables interrupts) */
		__raw_writel(DEF_PORT_IRQ, port_mmio + PORT_IRQ_MASK);

		/*register linkup ports */
		tmp = __raw_readl(port_mmio + PORT_SCR_STAT);
		printf("Port %d status: 0x%x\n", i, tmp);
		if ((tmp & 0xf) == 0x03)
			probe_ent->link_port_map |= (0x01 << i);
	}

	tmp = __raw_readl(mmio + HOST_CTL);
	__raw_writel(tmp | HOST_IRQ_EN, mmio + HOST_CTL);
	tmp = __raw_readl(mmio + HOST_CTL);
	printf("HOST_CTL 0x%x\n", tmp);

	return 0;
}


static void ahci_print_info(struct ahci_probe_ent *probe_ent)
{
	//volatile u8 *mmio = (volatile u8 *)probe_ent->mmio_base;
	u32 mmio = probe_ent->mmio_base;
	u32 vers, cap, impl, speed;
	const char *speed_s;
	u16 cc;
	const char *scc_s;

	vers = __raw_readl(mmio + HOST_VERSION);
	cap = probe_ent->cap;
	impl = probe_ent->port_map;

	speed = (cap >> 20) & 0xf;
	if (speed == 1)
		speed_s = "1.5";
	else if (speed == 2)
		speed_s = "3";
	else
		speed_s = "?";

	scc_s = "SATA";
	printf("AHCI %02x%02x.%02x%02x "
	       "%u slots %u ports %s Gbps 0x%x impl %s mode\n",
	       (vers >> 24) & 0xff,
	       (vers >> 16) & 0xff,
	       (vers >> 8) & 0xff,
	       vers & 0xff,
	       ((cap >> 8) & 0x1f) + 1, (cap & 0x1f) + 1, speed_s, impl, scc_s);

	printf("cap = 0x%08x flags: "
	       "%s%s%s%s%s%s"
	       "%s%s%s%s%s%s%s\n",
	       cap,
	       cap & (1 << 31) ? "64bit " : "",
	       cap & (1 << 30) ? "ncq " : "",
	       cap & (1 << 28) ? "ilck " : "",
	       cap & (1 << 27) ? "stag " : "",
	       cap & (1 << 26) ? "pm " : "",
	       cap & (1 << 25) ? "led " : "",
	       cap & (1 << 24) ? "clo " : "",
	       cap & (1 << 19) ? "nz " : "",
	       cap & (1 << 18) ? "only " : "",
	       cap & (1 << 17) ? "pmp " : "",
	       cap & (1 << 15) ? "pio " : "",
	       cap & (1 << 14) ? "slum " : "",
	       cap & (1 << 13) ? "part " : "");
	
}

static int ahci_init_one()
{
	int rc;

	memset((void *)ataid, 0, sizeof(hd_driveid_t *) * AHCI_MAX_PORTS);

	probe_ent = malloc(sizeof(struct ahci_probe_ent));
	memset(probe_ent, 0, sizeof(struct ahci_probe_ent));

	probe_ent->host_flags = ATA_FLAG_SATA
				| ATA_FLAG_NO_LEGACY
				| ATA_FLAG_MMIO
				| ATA_FLAG_PIO_DMA
				| ATA_FLAG_NO_ATAPI
				| ATA_FLAG_AN;
	probe_ent->pio_mask = 0x1f;
	probe_ent->udma_mask = 0x7f;	/*Fixme,assume to support UDMA6 */

	probe_ent->mmio_base = NS2816_SATA_BASE; 

	/* Take from kernel:
	 * JMicron-specific fixup:
	 * make sure we're in AHCI mode
	 */

	/* initialize adapter */
	rc = ahci_host_init(probe_ent);
	if (rc)
		goto err_out;

	ahci_print_info(probe_ent);

	return 0;

      err_out:
	return rc;
}


#define MAX_DATA_BYTE_COUNT  (4*1024*1024)

static int ahci_fill_sg(u8 port, unsigned char *buf, int buf_len)
{
	struct ahci_ioports *pp = &(probe_ent->port[port]);
	struct ahci_sg *ahci_sg = pp->cmd_tbl_sg;
	u32 sg_count;
	int i;

	sg_count = ((buf_len - 1) / MAX_DATA_BYTE_COUNT) + 1;
	
	debug("buf=0x%08x buf_len=%d sg_count is %d\n", buf, buf_len, sg_count);

	if (sg_count > AHCI_MAX_SG) {
		printf("Error:Too much sg!\n");
		return -1;
	}

	for (i = 0; i < sg_count; i++) {

		ahci_sg->addr =
		    cpu_to_le32((u32) buf + i * MAX_DATA_BYTE_COUNT);
	
		ahci_sg->addr_hi = 0;
		//ahci_sg->flags_size = cpu_to_le32(0x3fffff &
/*		ahci_sg->flags_size = (0x3fffff &
					  (buf_len < MAX_DATA_BYTE_COUNT
					   ? (buf_len - 1)
					   : (MAX_DATA_BYTE_COUNT - 1)));

*/
		ahci_sg->flags_size=(buf_len-1);

		ahci_sg++;
		buf_len -= MAX_DATA_BYTE_COUNT;
		
	}

	return sg_count;
}


static void ahci_fill_cmd_slot(struct ahci_ioports *pp, u32 opts)
{
	pp->cmd_slot->opts = cpu_to_le32(opts);
	pp->cmd_slot->status = 0;
	pp->cmd_slot->tbl_addr = cpu_to_le32(pp->cmd_tbl & 0xffffffff);
	pp->cmd_slot->tbl_addr_hi = 0;
}


static void ahci_set_feature(u8 port)
{
	struct ahci_ioports *pp = &(probe_ent->port[port]);
	u32 port_mmio = pp->port_mmio;
	u32 cmd_fis_len = 5;	/* five dwords */
	u8 fis[20];

	//clear interrupt
	sata_clear_interrupt(NS2816_SATA_BASE);

	/*set feature */
	memset(fis, 0, 20);
	fis[0] = 0x27;
	fis[1] = 1 << 7;
	//fis[1] = 0xf;
	fis[2] = ATA_CMD_SETF;

	/*
	 * SETFEATURES_XFER=0x03, which means set transfer mode based on value in
	 * Sector Count register. now Sector Count regiseter value is in fis[12] 
	 */
	fis[3] = SETFEATURES_XFER;	

	/*
	 * set probe_ent->udma_mask=0x7f,
	 * so fis[12]=0x46[01000 110b], which means Ultra DMA mode , mode = 110b
	 */
	fis[12] = __ilog2(probe_ent->udma_mask + 1) + 0x40 - 0x01;	//tf->nsect

	//fis[7] = ATA_DEVICE_OBS;
	fis[7] = (ATA_DEVICE_OBS| ATA_LBA);

	memcpy((unsigned char *)pp->cmd_tbl, fis, 20);
	ahci_fill_cmd_slot(pp, cmd_fis_len);
	__raw_writel(1, port_mmio + PORT_CMD_ISSUE);
	__raw_readl(port_mmio + PORT_CMD_ISSUE);

	if (waiting_for_cmd_completed(port_mmio + PORT_CMD_ISSUE, 150, 0x1)) {
		printf("set feature error!\n");
	}
}


static int ahci_port_start(u8 port)
{
	struct ahci_ioports *pp = &(probe_ent->port[port]);
	u32 port_mmio = pp->port_mmio;
	u32 port_status;
	u32 mem;
	int j=0;
	u32 tfd;

	debug("Enter start port: %d\n", port);
	port_status = __raw_readl(port_mmio + PORT_SCR_STAT);
	debug("Port %d status: %x\n", port, port_status);
	if ((port_status & 0xf) != 0x03) {
		printf("No Link on this port!\n");
		return -1;
	}

	mem = ((u32)mem_alloc + 0x800) & (~0x7ff);	/* Aligned to 2048-bytes */
	memset((u8 *) mem, 0, AHCI_PORT_PRIV_DMA_SZ);

	/*
	 * First item in chunk of DMA memory: 32-slot command table,
	 * 32 bytes each in size
	 */
	pp->cmd_slot = (struct ahci_cmd_hdr *)mem;
	debug("cmd_slot = 0x%x\n", pp->cmd_slot);
	mem += (AHCI_CMD_SLOT_SZ + 224);

	/*
	 * Second item: Received-FIS area
	 */
	pp->rx_fis = mem;
	mem += AHCI_RX_FIS_SZ;

	/*
	 * Third item: data area for storing a single command
	 * and its scatter-gather table
	 */
	pp->cmd_tbl = mem;
	debug("cmd_tbl_dma = 0x%x\n", pp->cmd_tbl);

	mem += AHCI_CMD_TBL_HDR;
	pp->cmd_tbl_sg = (struct ahci_sg *)mem;

	__raw_writel((u32) pp->cmd_slot, port_mmio + PORT_LST_ADDR);

	__raw_writel(pp->rx_fis, port_mmio + PORT_FIS_ADDR);

	__raw_writel(PORT_CMD_ICC_ACTIVE | PORT_CMD_FIS_RX |
			  PORT_CMD_POWER_ON | PORT_CMD_SPIN_UP |
			  PORT_CMD_START, port_mmio + PORT_CMD);

	//wait TFD status
	j = 0;
	while (j < 200) {
		tfd = __raw_readl(port_mmio + PORT_TFDATA);
		if (tfd == 0x00000150)
		{
			printf("TFD poll OK, TFD=0x%08x\n", tfd);
			break;
		}
		j++;
		msleep(10);
	}

	debug("Exit start port %d\n", port);

	return 0;
}


static int get_ahci_device_data(u8 port, u8 *fis, int fis_len, u8 *buf,
				int buf_len)
{

	struct ahci_ioports *pp = &(probe_ent->port[port]);
	u32 port_mmio = pp->port_mmio;
	u32 opts;
	u32 port_status;
	int sg_count;

	if(0 == buf_len)
	{
		debug("read data len is equal to 0, ignore!\n");
		return 0;
	}

	debug("Enter get_ahci_device_data: for port %d\n", port);

	//clear interrupt
	sata_clear_interrupt(NS2816_SATA_BASE);

	if (port >= probe_ent->n_ports) {
		printf("Invaild port number %d\n", port);
		return -1;
	}

	port_status = __raw_readl(port_mmio + PORT_SCR_STAT);
	if ((port_status & 0xf) != 0x03) {
		printf("No Link on port %d!\n", port);
		return -1;
	}

	memcpy((unsigned char *)pp->cmd_tbl, fis, fis_len);

	sg_count = ahci_fill_sg(port, buf, buf_len);
	opts = (fis_len >> 2) | (sg_count << 16);

	ahci_fill_cmd_slot(pp, opts);

	__raw_writel(1, port_mmio + PORT_CMD_ISSUE);

	if (waiting_for_cmd_completed(port_mmio + PORT_CMD_ISSUE, CMD_WAIT_TIME, 0x1)) {
		printf("timeout exit!\n");
		return -1;
	}
	debug("get_ahci_device_data: %d byte transferred.\n",
	      pp->cmd_slot->status);

	return 0;
}


static char *ata_id_strcpy(u16 *target, u16 *src, int len)
{
	int i;
	for (i = 0; i < len / 2; i++)
		target[i] = le16_to_cpu(src[i]);
	return (char *)target;
}


static void dump_ataid(hd_driveid_t *ataid)
{
	debug("(49)ataid->capability = 0x%x\n", ataid->capability);
	debug("(53)ataid->field_valid =0x%x\n", ataid->field_valid);
	debug("(63)ataid->dma_mword = 0x%x\n", ataid->dma_mword);
	debug("(64)ataid->eide_pio_modes = 0x%x\n", ataid->eide_pio_modes);
	debug("(75)ataid->queue_depth = 0x%x\n", ataid->queue_depth);
	debug("(80)ataid->major_rev_num = 0x%x\n", ataid->major_rev_num);
	debug("(81)ataid->minor_rev_num = 0x%x\n", ataid->minor_rev_num);
	debug("(82)ataid->command_set_1 = 0x%x\n", ataid->command_set_1);
	debug("(83)ataid->command_set_2 = 0x%x\n", ataid->command_set_2);
	debug("(84)ataid->cfsse = 0x%x\n", ataid->cfsse);
	debug("(85)ataid->cfs_enable_1 = 0x%x\n", ataid->cfs_enable_1);
	debug("(86)ataid->cfs_enable_2 = 0x%x\n", ataid->cfs_enable_2);
	debug("(87)ataid->csf_default = 0x%x\n", ataid->csf_default);
	debug("(88)ataid->dma_ultra = 0x%x\n", ataid->dma_ultra);
	debug("(93)ataid->hw_config = 0x%x\n", ataid->hw_config);
}


/*
 * SCSI INQUIRY command operation.
 */
static int ata_scsiop_inquiry(ccb *pccb)
{
	u8 hdr[] = {
		0,
		0,
		0x5,		/* claim SPC-3 version compatibility */
		2,
		95 - 4,
	};
	u8 fis[20];
	u16 *tmpid;
	u8 port;
	int i=0;

	/* Clean ccb data buffer */
	memset(pccb->pdata, 0, pccb->datalen);

	memcpy(pccb->pdata, hdr, sizeof(hdr));

	if (pccb->datalen <= 35)
		return 0;

	memset(fis, 0, 20);
	/* Construct the FIS */
	fis[0] = 0x27;		/* Host to device FIS. */
	fis[1] = 1 << 7;	/* Command FIS. */
	fis[2] = ATA_CMD_IDENT;	/* Command byte. */

	fis[7] = ATA_DEVICE_OBS;

	/* Read id from sata */
	port = pccb->target;
	if (!(tmpid = malloc(sizeof(hd_driveid_t))))
		return -ENOMEM;

	memset(tmpid, 0, sizeof(hd_driveid_t));

	if (get_ahci_device_data(port, (u8 *) & fis, 20,
				 tmpid, sizeof(hd_driveid_t))) {
		printf("scsi_ahci: SCSI inquiry command failure.\n");
		return -EIO;
	}

	if (ataid[port])
		free(ataid[port]);
	ataid[port] = (hd_driveid_t *) tmpid;

	if(tmpid[83] & 0x0400)
	{
		printf("support LBA48!\n");
		//support LBA48
		ataid[pccb->target]->lba_capacity = ( (((u32)tmpid[101])<<16)|
							(((u32)tmpid[100])));

	}else{
		printf("support LBA28!\n");
		ataid[pccb->target]->lba_capacity = ((hd_driveid_t *)tmpid)->lba_capacity;
	}
	
	debug("total capacity is %d  0x%08x\n", 
		ataid[pccb->target]->lba_capacity, 
		ataid[pccb->target]->lba_capacity);

	memcpy(&pccb->pdata[8], "ATA     ", 8);
	ata_id_strcpy((u16 *) &pccb->pdata[16], (u16 *)ataid[port]->model, 16);
	ata_id_strcpy((u16 *) &pccb->pdata[32], (u16 *)ataid[port]->fw_rev, 4);

	dump_ataid(ataid[port]);
	return 0;
}


/*
 * SCSI READ10 command operation.
 */
static int ata_scsiop_read10(ccb * pccb)
{
	u32 len = 0;
	u8 fis[20];
	u32 cnt;
	int i;
	u8 *pdata;
	u32 datalen=0;
	u32 start;

	memset(fis, 0, 20);

	/* Construct the FIS */
	fis[0] = 0x27;		/* Host to device FIS. */
	fis[1] = 1 << 7;	/* Command FIS. */
	fis[2] = ATA_CMD_RD_DMA;	/* tf->command Command byte. */
	//fis[3] = 31; 			//tf->feature

	fis[4] = pccb->cmd[5];	//tf->lbal
	fis[5] = pccb->cmd[4];	//tf->lbam
	fis[6] = pccb->cmd[3];	//tf->lbah
	/* Sector Count */
	fis[12] = pccb->cmd[8];	//tf->nsect
	fis[13] = pccb->cmd[7]; //tf->hob_nsect
/*
#ifdef CONFIG_LBA48
	//fis[7] = (ATA_DEVICE_OBS);	//tf->device
	fis[7] = ((pccb->cmd[2] & 0x0f) | ATA_DEVICE_OBS);

	fis[8] = pccb->cmd[2];	//tf->hob_lbal
	fis[9] = 0;		//tf->hob_lbam
	fis[10] = 0;		//tf->hob_lbah
//	fis[11] = 31;		//tf.hob_feature
#else
*/
	fis[7] = ((pccb->cmd[2] & 0x0f) | 0xe0); //tf->device

//#endif

	cnt=fis[13];
	start=(((u32)pccb->cmd[5]) | (((u32)pccb->cmd[4])<<8) | 
		(((u32)pccb->cmd[3])<<16) |(((u32)pccb->cmd[2])<<24));
	pdata=pccb->pdata;
	datalen = (pccb->datalen - cnt*256*512);

	/* Read from ahci */
	if (get_ahci_device_data(pccb->target, (u8 *) & fis, 20, pdata, datalen)) 
	{
		printf("scsi_ahci: SCSI READ10 command failure.\n");
		return -EIO;
	}

	if(0!=cnt)
	{

		pdata += datalen;
		datalen = 512*256;
		start += fis[12];

		fis[12]=0x00;	//00h specifies that 256 sectors are to be transfered

		for(i=0; i<cnt; i++)
		{
			pccb->cmd[2]=((unsigned char) (start>>24))&0xff;
			pccb->cmd[3]=((unsigned char) (start>>16))&0xff;
			pccb->cmd[4]=((unsigned char) (start>>8))&0xff;
			pccb->cmd[5]=((unsigned char) (start))&0xff;

			fis[4] = pccb->cmd[5];	//tf->lbal
			fis[5] = pccb->cmd[4];	//tf->lbam
			fis[6] = pccb->cmd[3];	//tf->lbah

			fis[7] = ((pccb->cmd[2] & 0x0f) | 0xe0); //tf->device
			fis[13] = 0;
	
			/* Read from ahci */
			if (get_ahci_device_data(pccb->target, (u8 *) & fis, 20,
				 pdata, datalen)) 
			{
				printf("scsi_ahci: SCSI READ10 command failure.\n");
				return -EIO;
			}

			pdata += datalen;	
			start += 256;
		}
	}
	
	return 0;
}


/*
 * SCSI READ CAPACITY10 command operation.
 */
static int ata_scsiop_read_capacity10(ccb *pccb)
{
	u32 cap;

	if (!ataid[pccb->target]) {
		printf("scsi_ahci: SCSI READ CAPACITY10 command failure. "
		       "\tNo ATA info!\n"
		       "\tPlease run SCSI commmand INQUIRY firstly!\n");
		return -EPERM;
	}

	//cap = le32_to_cpu(ataid[pccb->target]->lba_capacity);
	cap = (ataid[pccb->target]->lba_capacity);
	debug("device %d , capacity=%d\n", pccb->target, ataid[pccb->target]->lba_capacity);

	memcpy(pccb->pdata, &cap, sizeof(cap));

	pccb->pdata[4] = pccb->pdata[5] = 0;
	pccb->pdata[6] = 512 >> 8;
	pccb->pdata[7] = 512 & 0xff;

	return 0;
}


/*
 * SCSI TEST UNIT READY command operation.
 */
static int ata_scsiop_test_unit_ready(ccb *pccb)
{
	return (ataid[pccb->target]) ? 0 : -EPERM;
}


int scsi_exec(ccb *pccb)
{
	int ret;

	switch (pccb->cmd[0]) {
	case SCSI_READ10:
		ret = ata_scsiop_read10(pccb);
		break;
	case SCSI_RD_CAPAC:
		ret = ata_scsiop_read_capacity10(pccb);
		break;
	case SCSI_TST_U_RDY:
		ret = ata_scsiop_test_unit_ready(pccb);
		break;
	case SCSI_INQUIRY:
		ret = ata_scsiop_inquiry(pccb);
		break;
	default:
		printf("Unsupport SCSI command 0x%02x\n", pccb->cmd[0]);
		return FALSE;
	}

	if (ret) {
		printf("SCSI command 0x%02x ret errno %d\n", pccb->cmd[0], ret);
		return FALSE;
	}
	return TRUE;

}


void scsi_low_level_init(int busdevfunc)
{
	int i;
	u32 linkmap;

	ahci_init_one();

	linkmap = probe_ent->link_port_map;

	for (i = 0; i < CONFIG_SYS_SCSI_MAX_SCSI_ID; i++) {
		if (((linkmap >> i) & 0x01)) {
			if (ahci_port_start((u8) i)) {
				printf("Can not start port %d\n", i);
				continue;
			}
//			ahci_set_feature((u8) i);
		}
	}
}


void scsi_bus_reset(void)
{
	/*Not implement*/
}


void scsi_print_error(ccb * pccb)
{
	/*The ahci error info can be read in the ahci driver*/
}
