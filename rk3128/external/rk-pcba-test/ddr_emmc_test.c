#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

#include "math.h"
#include "test_case.h"
#include "language.h"
#include "ddr_emmc_test.h"

#ifdef RK3399_PCBA

#define EMMCPATH "/sys/bus/mmc/devices/mmc1:0001/block/mmcblk0/size"
#define READ_DDR_COMMAND "cat /proc/zoneinfo | busybox grep present | \
				busybox awk '{print $2}'"			
#else
	
#define EMMCPATH "/sys/bus/mmc/devices/mmc0:0001/block/mmcblk0/size"
#define READ_DDR_COMMAND "cat /proc/zoneinfo | busybox grep present | \
				busybox awk 'BEGIN{a=0}{a+=$2}END{print a}'"
#endif



/* for ddr  */
int ddr_exec(const char *cmd, char *tmp, unsigned int length)
{
	FILE *pp = popen(cmd, "r");

	if (!pp)
		return -1;
	if (fgets(tmp, length, pp) == NULL) {
		printf("popen read from cmd is NULL!\n");
		pclose(pp);
		return -1;
	}
	pclose(pp);
	return 0;
}

/* for emmc  */
static int readFromFile(const char *path, char *buf, size_t size)
{
	if (!path)
		return -1;
	int fd = open(path, O_RDONLY, 0);

	if (fd == -1) {
		printf("Could not open '%s'", path);
		return -1;
	}
	ssize_t count = read(fd, buf, size);

	if (count > 0) {
		while (count > 0 && buf[count-1] == '\n')
			count--;
			buf[count] = '\0';
	} else {
		buf[0] = '\0';
	}
	close(fd);
	return count;
}

int get_emmc_size(char *size_data)
{
	char i;
	double size = (double)(atoi(size_data))/2/1024/1024;

	if (size > 0 && size <= 1)  /*1 GB */
		return 1;
	for (i = 0; i < 10; i++) {
		if (size > pow(2, i) && size <= pow(2, i+1))  /*2 - 512 GB*/
		return pow(2, i+1);
	}
	return -1;
}

void *ddr_emmc_test(void *argv)
{
	struct testcase_info *tc_info = (struct testcase_info *)argv;

	int emmc_ret = 0;
	char emmcsize_char[20];
	int emmc_size = 0;

	int ddr_ret = 0;
	char ddrsize_char[20];
	int ddr_size = 0;

	if (tc_info->y <= 0)
		tc_info->y = get_cur_print_y();

	ui_print_xy_rgba(0, tc_info->y, 255, 255, 0, 255, "%s:[%s..]\n",
			 PCBA_DDR_EMMC, PCBA_TESTING);

	/* For ddr */
	memset(ddrsize_char, 0, sizeof(ddrsize_char));
	ddr_ret = ddr_exec(READ_DDR_COMMAND,
		ddrsize_char, sizeof(ddrsize_char));
	if (ddr_ret >= 0) {
		ddr_size = (int)(atoi(ddrsize_char)*4/1024/1024);
		/* printf("=========== ddr_zize is : %dGB
			==========\n",ddr_size);*/
	}

	/* For emmc */
	memset(emmcsize_char, 0, sizeof(emmcsize_char));
	emmc_ret = readFromFile(EMMCPATH, emmcsize_char, sizeof(emmcsize_char));
	if (emmc_ret >= 0) {  /*read back normal*/
		emmc_size = get_emmc_size(emmcsize_char);
		if (emmc_size < 0)
			emmc_ret = -1;
		/* printf("=======  emmc_size is: %d  ========\n",emmc_size);*/
	}

	if (ddr_ret < 0 && emmc_ret < 0) {
		ui_print_xy_rgba(0, tc_info->y, 255, 0, 0, 255,
			"%s:[%s] { %s:%s,%s:%s }\n",
				PCBA_DDR_EMMC, PCBA_FAILED,
				PCBA_DDR, PCBA_FAILED,
				PCBA_EMMC, PCBA_FAILED);
	} else if (ddr_ret < 0 && emmc_ret >= 0) {
		ui_print_xy_rgba(0, tc_info->y, 255, 0, 0, 255,
			"%s:[%s] { %s:%s,%s:%dGB }\n",
			PCBA_DDR_EMMC, PCBA_FAILED,
			PCBA_DDR, PCBA_FAILED,
			PCBA_EMMC, emmc_size);
	} else if (ddr_ret >= 0 && emmc_ret < 0) {
		ui_print_xy_rgba(0, tc_info->y, 255, 0, 0, 255,
			"%s:[%s] { %s:%dGB,%s:%s }\n",
			PCBA_DDR_EMMC, PCBA_FAILED,
			PCBA_DDR, ddr_size,
			PCBA_EMMC, PCBA_FAILED);
	} else {
		ui_print_xy_rgba(0, tc_info->y, 0, 255, 0, 255,
			"%s:[%s] { %s:%dGB,%s:%dGB }\n",
			PCBA_DDR_EMMC, PCBA_SECCESS,
			PCBA_DDR, ddr_size,
			PCBA_EMMC, emmc_size);

		tc_info->result = 1;
		return argv;
	}

	tc_info->result = -1;
	return argv;
}
