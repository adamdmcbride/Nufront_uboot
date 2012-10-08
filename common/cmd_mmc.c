/*
 * (C) Copyright 2003
 * Kyle Harris, kharris@nexus-tech.net
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
 */

#include <common.h>
#include <command.h>
//#include <mmc.h>
#include <asm/arch/mmc.h>

#ifndef CONFIG_GENERIC_MMC
static int curr_device = -1;

int mem_cmp(unsigned int *src, unsigned int *dst, unsigned int count)
{
	unsigned int addr1 = src;
	unsigned int addr2 = dst;
	unsigned int ngood = 0;
	unsigned int size = 4;
    	unsigned int i = count;
	while (i-- > 0) {
		unsigned int word1 = *(unsigned int *)addr1;
		unsigned int word2 = *(unsigned int *)addr2;
		if (word1 != word2) {
			printf("word at 0x%08lx (0x%08lx) "
				"!= word at 0x%08lx (0x%08lx)\n",
				addr1, word1, addr2, word2);
				break;
		}
	ngood++;
	addr1 += size;
	addr2 += size;
	}

	printf("Total %ld of %ld were the same.\n", ngood, count);
	return 1;
}

int do_mmc (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int dev;
	unsigned int n = 0;
//	int slot_no = 0;

	if (argc < 2) {
		cmd_usage(cmdtp);
		return 1;
	}

	if (strcmp(argv[1], "init") == 0) {
		if (argc == 2) {
			if (curr_device < 0)
				dev = 0;
			else
				dev = curr_device;
		} else if (argc == 3) {
			dev = (int)simple_strtoul(argv[2], NULL, 10);
		} else {
			cmd_usage(cmdtp);
			return 1;
		}

		if (mmc_init(dev) != 0) {
			puts("No MMC card found\n");
			return 1;
		}

		curr_device = dev;
		printf("mmc%d is available\n", curr_device);
		return 1;
	} else if (strcmp(argv[1], "device") == 0) {
		if (argc == 2) {
			if (curr_device < 0) {
				puts("No MMC device available\n");
				return 1;
			}
		} else if (argc == 3) {
			dev = (int)simple_strtoul(argv[2], NULL, 10);

#ifdef CONFIG_SYS_MMC_SET_DEV
			if (mmc_set_dev(dev) != 0)
				return 1;
#endif
			curr_device = dev;
                } else {
                        cmd_usage(cmdtp);
                  	return 1;
                }

                printf("mmc%d is current device\n", curr_device);


	} else  if (strcmp(argv[1],"read") == 0) {
			//if (argc ==2){
			//	if (curr_device <0){
			//		puts("No MMC device available\n");
			//		return 1;
			//	}
		//	} else if (argc ==3 | argc ==4 |argv == 5) {
			int dev =simple_strtoul (argv[2],NULL,10);
			void *addr = (void *)simple_strtoul (argv[3],NULL,16);
			u32 blkcnt = simple_strtoul(argv[5],NULL,10);
			u32 blknr = simple_strtoul(argv[4],NULL,10);
			printf("\nMMC read: dev # %d, block # %d, count %d ...\n", dev, blknr, blkcnt);
			mmc_read(dev, blknr, addr, blkcnt);
		//	flush_cache((ulong)addr, cnt * 512);

	//		printf("%d blocks read: %s\n",n, (n==cnt) ? "OK" : "ERROR");
	//		return (n == cnt) ? 0 : 1;
		//		}
	} else  if (strcmp(argv[1],"write") == 0) {
		//	if (argc ==2){
		//		if (curr_device <0){
		//			puts("No MMC device available\n");
		//			return 1;
		//		}
		//	} else if (argc ==3 | argc ==4 |argv == 5) {
			int dev =simple_strtoul (argv[2],NULL,10);
			void *addr = (void *)simple_strtoul (argv[3],NULL,16);
			u32 blkcnt = simple_strtoul(argv[5],NULL,10);
			u32 blknr = simple_strtoul(argv[4],NULL,10);
//			printf("\nMMC write: dev # %d, block # %d, count %d ...\n", dev, blknr, blkcnt);
			mmc_write( dev, addr, blknr, blkcnt);
	//		printf("%d blocks written: %s\n",n, (n == cnt) ? "OK" : "ERROR");
	//		return (n == cnt)? 0 : 1;
		//		}
	} else if (strcmp(argv[1], "erase") == 0) {
//		if (argc != 5){
//			goto cmd_usage;
//		} else {
			int dev =simple_strtoul (argv[2],NULL,10);
			unsigned int src_addr = simple_strtoul(argv[3], NULL, 16);
			int size = simple_strtoul(argv[4], NULL, 16);
			mmc_erase(dev, src_addr, size);
//		}
	} else if (strcmp(argv[1], "rwtest") == 0) {

#define RWTES_START_ADDR	0x80100000
#define MMC_START_SECTOR	5000
#define RWTEST_SIZE		100	/* in MBytes */

#define MMC_RWTEST_SIZE		(RWTEST_SIZE*1024*1024/512)
#define MMC_END_SECTOR		(MMC_START_SECTOR + MMC_RWTEST_SIZE)

		/* mmc rwtest  0 0x80100000 5000 100 */
		int dev = simple_strtoul(argv[2], NULL, 10);
		void *addr = (void *)simple_strtoul(argv[3], NULL, 16);
		int start_sect = simple_strtoul(argv[4], NULL, 10);
	//	int size = simple_strtoul(argv[5], NULL, 10);
	//	int nr_sectors = size*1024*1024/512;
		int nr_sectors = simple_strtoul(argv[5], NULL, 10);
		int end_sect   = start_sect + nr_sectors;

		int i;

		/* specify your test pattern */
#if 0
		int pattern[] = { 0x0001, 0x0002, 0x0004, 0x0008,
				  0x0010, 0x0020, 0x0040, 0x0080,
				  0x0100, 0x0200, 0x0400, 0x0800,
				  0x1000, 0x2000, 0x4000, 0x8000};
#endif
		int pattern[] = { 0x6162, 0x6364, 0x6566, 0x6768,
				  0x6970, 0x7172, 0x7374, 0x7576,
				  0x7778, 0x797a, 0x7b7c, 0x7d7e,
				  0x7f80, 0x8182, 0x8384, 0x8586};
		/* poison DDR area */
		printf("Now prepare source & destination ram area\n");
		for(i = 0; i <= nr_sectors*512/4; i++) {
			(*(((unsigned int *)addr) + i)) = pattern[i % 0x10];
			if(!(i % 0x2000)/* 8KBytes*/)
				printf("@");
		}

		for(i = 0; i <= nr_sectors*512/4; i++) {
			(*(((unsigned int *)addr) + +nr_sectors*512/4 + i)) = ~ pattern[i % 0x10];
			if(!(i % 0x2000)/* 8KBytes*/)
				printf("#");
		}

		printf("\nStarting SD/MMC read/write test now\n");
		printf("\t start sector -- %d\n",start_sect);
		printf("\t end sector 	-- %d\n", end_sect);
		printf("\t total sector -- %d \n", nr_sectors);
	//	printf("\t total size	-- %d MBytes\n", size);

		mmc_write(dev, addr, start_sect, nr_sectors);
		mmc_read(dev, start_sect, (((unsigned int *)addr)+nr_sectors*512/4), nr_sectors);

		printf("\nFinished SD/MMC read/write test\n");
//		printf("please run command \n");
		mem_cmp(addr, (((unsigned int *)addr)+nr_sectors*512/4),nr_sectors*512/4);
//		printf("\t\tcmp.l 0x%08x 0x%08x %x\n", addr, addr+(nr_sectors*512/32), nr_sectors*512/32);
//		printf("to checkout the read/write test result\n");

	} else {
		cmd_usage(cmdtp);
		return 1;
	}
	return 0;
}

U_BOOT_CMD(
	mmc, 6, 1, do_mmc,
	"MMC sub-system",
	"init [dev] - init MMC sub system\n"
	"mmc device [dev] - show or set current device\n"
	"mmc write <device num> addr blk# cnt\n"
	"mmc read <device num> addr blk# cnt\n"
	"mmc erase <device num> start_addr size#\n"
	"mmc rwtest <dev> addr start_blk# cnt\n");
#else /* !CONFIG_GENERIC_MMC */

static void print_mmcinfo(struct mmc *mmc)
{
	printf("Device: %s\n", mmc->name);
	printf("Manufacturer ID: %x\n", mmc->cid[0] >> 24);
	printf("OEM: %x\n", (mmc->cid[0] >> 8) & 0xffff);
	printf("Name: %c%c%c%c%c \n", mmc->cid[0] & 0xff,
			(mmc->cid[1] >> 24), (mmc->cid[1] >> 16) & 0xff,
			(mmc->cid[1] >> 8) & 0xff, mmc->cid[1] & 0xff);

	printf("Tran Speed: %d\n", mmc->tran_speed);
	printf("Rd Block Len: %d\n", mmc->read_bl_len);

	printf("%s version %d.%d\n", IS_SD(mmc) ? "SD" : "MMC",
			(mmc->version >> 4) & 0xf, mmc->version & 0xf);

	printf("High Capacity: %s\n", mmc->high_capacity ? "Yes" : "No");
	printf("Capacity: %lld\n", mmc->capacity);

	printf("Bus Width: %d-bit\n", mmc->bus_width);
}

int do_mmcinfo (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	struct mmc *mmc;
	int dev_num;

	if (argc < 2)
		dev_num = 0;
	else
		dev_num = simple_strtoul(argv[1], NULL, 0);

	mmc = find_mmc_device(dev_num);

	if (mmc) {
		mmc_init(mmc);

		print_mmcinfo(mmc);
	}

	return 0;
}

U_BOOT_CMD(mmcinfo, 2, 0, do_mmcinfo,
	"mmcinfo <dev num>-- display MMC info\n",
	""
);

int do_mmcops(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int rc = 0;

	switch (argc) {
	case 3:
		if (strcmp(argv[1], "rescan") == 0) {
			int dev = simple_strtoul(argv[2], NULL, 10);
			struct mmc *mmc = find_mmc_device(dev);

			if (!mmc)
				return 1;

			mmc_init(mmc);

			return 0;
		}

	case 0:
	case 1:
	case 4:
		printf("Usage:\n%s\n", cmdtp->usage);
		return 1;

	case 2:
		if (!strcmp(argv[1], "list")) {
			print_mmc_devices('\n');
			return 0;
		}
		return 1;
	default: /* at least 5 args */
		if (strcmp(argv[1], "read") == 0) {
			int dev = simple_strtoul(argv[2], NULL, 10);
			void *addr = (void *)simple_strtoul(argv[3], NULL, 16);
			u32 cnt = simple_strtoul(argv[5], NULL, 16);
			u32 n;
			u32 blk = simple_strtoul(argv[4], NULL, 16);
			struct mmc *mmc = find_mmc_device(dev);

			if (!mmc)
				return 1;

			printf("\nMMC read: dev # %d, block # %d, count %d ... ",
				dev, blk, cnt);

			mmc_init(mmc);

			n = mmc->block_dev.block_read(dev, blk, cnt, addr);

			/* flush cache after read */
			flush_cache((ulong)addr, cnt * 512); /* FIXME */

			printf("%d blocks read: %s\n",
				n, (n==cnt) ? "OK" : "ERROR");
			return (n == cnt) ? 0 : 1;
		} else if (strcmp(argv[1], "write") == 0) {
			int dev = simple_strtoul(argv[2], NULL, 10);
			void *addr = (void *)simple_strtoul(argv[3], NULL, 16);
			u32 cnt = simple_strtoul(argv[5], NULL, 16);
			u32 n;
			struct mmc *mmc = find_mmc_device(dev);

			int blk = simple_strtoul(argv[4], NULL, 16);

			if (!mmc)
				return 1;

			printf("\nMMC write: dev # %d, block # %d, count %d ... ",
				dev, blk, cnt);

			mmc_init(mmc);

			n = mmc->block_dev.block_write(dev, blk, cnt, addr);

			printf("%d blocks written: %s\n",
				n, (n == cnt) ? "OK" : "ERROR");
			return (n == cnt) ? 0 : 1;
		} else {
			printf("Usage:\n%s\n", cmdtp->usage);
			rc = 1;
		}

		return rc;
	}
}


U_BOOT_CMD(
	mmc, 6, 1, do_mmcops,
	"MMC sub system",
	"read <device num> addr blk# cnt\n"
	"mmc write <device num> addr blk# cnt\n"
	"mmc rescan <device num>\n"
	"mmc list - lists available devices");
#endif
