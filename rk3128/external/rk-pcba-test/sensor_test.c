/*
 * Copyright (C) 2012 The Android Open-Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <hardware/sensors.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include <math.h>
#include <poll.h>
#include <pthread.h>
#include <sys/cdefs.h>
#include <sys/types.h>

#include <linux/input.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "sensor_test.h"
#include "common.h"
#include "test_case.h"
#include "language.h"

struct sensor_info {
	int type;
	const char *name;
	struct display_info *pdi;
	sensors_event_t *data;
};

enum {
	ID_GSENSOR = 0,
	ID_GYRO,
	ID_COMPASS,
	ID_LSENSOR,
	ID_PSENSOR,
	ID_GSENSOR_CALIBRATE,
	MAX_SENSOR,
};

static const char *SensorName[MAX_SENSOR] = {
	PCBA_GSENSOR,
	PCBA_GYRO,
	PCBA_COMPASS,
	PCBA_LSENSOR,
	PCBA_PSENSOR,
	PCBA_GSENSOR_CALIBRATE,
};

static struct display_info s_di[MAX_SENSOR];

static struct sensor_info s_sensor_info[MAX_SENSOR];

#define SENSOR_TYPE_MASK(x)	(1<<x)

#define SENSOR_FIFO_NAME	"/dev/sensor_fifo"

#define NSEC_PER_SEC            1000000000
static inline int64_t calcdiff_ns(struct timespec t1, struct timespec t2)
{
	int64_t diff;

	diff = NSEC_PER_SEC * (int64_t)((int) t1.tv_sec
		- (int) t2.tv_sec);
	diff += ((int) t1.tv_nsec - (int) t2.tv_nsec);
	return diff;
}

static inline int id_to_type(int id)
{
	switch (id) {
	case ID_GSENSOR:
		return SENSOR_TYPE_ACCELEROMETER;
	case ID_GYRO:
		return SENSOR_TYPE_GYROSCOPE;
	case ID_COMPASS:
		return SENSOR_TYPE_MAGNETIC_FIELD;
	case ID_LSENSOR:
		return SENSOR_TYPE_LIGHT;
	case ID_PSENSOR:
		return SENSOR_TYPE_PRESSURE;
	case ID_GSENSOR_CALIBRATE:
	default:
		return -1;
	}
	return -1;
}

uint64_t get_android_sensor_type_mask(int sensor_type)
{
	uint64_t res = 0;

	if (sensor_type & SENSOR_TYPE_MASK(ID_GSENSOR))
		res |= SENSOR_TYPE_MASK(SENSOR_TYPE_ACCELEROMETER);
	if (sensor_type & SENSOR_TYPE_MASK(ID_GYRO))
		res |= SENSOR_TYPE_MASK(SENSOR_TYPE_GYROSCOPE);
	if (sensor_type & SENSOR_TYPE_MASK(ID_COMPASS))
		res |= SENSOR_TYPE_MASK(SENSOR_TYPE_MAGNETIC_FIELD);
	if (sensor_type & SENSOR_TYPE_MASK(ID_LSENSOR))
		res |= SENSOR_TYPE_MASK(SENSOR_TYPE_LIGHT);
	if (sensor_type & SENSOR_TYPE_MASK(ID_PSENSOR))
		res |= SENSOR_TYPE_MASK(SENSOR_TYPE_PRESSURE);

	return res;
}

typedef		unsigned short	    uint16;
typedef		unsigned long	    uint32;
typedef		unsigned char	    uint8;

#define VENDOR_SECTOR_OP_TAG        0x444E4556 /*"VEND"*/
#define RKNAND_GET_VENDOR_SECTOR1       _IOW('v', 18, unsigned int)
#define RKNAND_STORE_VENDOR_SECTOR1     _IOW('v', 19, unsigned int)
#define RKNAND_GET_VENDOR_SECTOR0       _IOW('v', 16, unsigned int)
#define RKNAND_STORE_VENDOR_SECTOR0     _IOW('v', 17, unsigned int)

#define RKNAND_SYS_STORGAE_DATA_LEN 512

typedef struct tagRKNAND_SYS_STORGAE {
	uint32  tag;
	uint32  len;
	uint8   data[RKNAND_SYS_STORGAE_DATA_LEN];
} RKNAND_SYS_STORGAE;

void rknand_print_hex_data(uint8 *s, uint32 *buf, uint32 len)
{
	uint32 i, j, count;

	printf("%s\n", s);
	for (i = 0; i < len; i += 4)
		printf("%x %x %x %x\n", buf[i], buf[i+1], buf[i+2], buf[i+3]);
}

struct calibrate_data {
	char tag[8];
	float v[3];
};

int gsensor_calibrate_load(float *v)
{
	uint32 i;
	int ret;
	RKNAND_SYS_STORGAE sysData;

	int sys_fd = open("/dev/rknand_sys_storage", O_RDWR, 0);

	if (sys_fd < 0) {
		printf("rknand_sys_storage open fail\n");
		return -1;
	}

	sysData.tag = VENDOR_SECTOR_OP_TAG;
	sysData.len = RKNAND_SYS_STORGAE_DATA_LEN-8;
	memset(sysData.data, 0, RKNAND_SYS_STORGAE_DATA_LEN);

	ret = ioctl(sys_fd, RKNAND_GET_VENDOR_SECTOR1, &sysData);
	/*rknand_print_hex_data("vendor_sector load:",
		(uint32*)sysData.data,32);*/
	if (ret) {
		printf("get vendor_sector error\n");
		return -1;
	}

	struct calibrate_data *pcd = (struct calibrate_data *)sysData.data;

	if (!strcmp(pcd->tag, "GSENCAL")) {
		v[0] = pcd->v[0];
		v[1] = pcd->v[1];
		v[2] = pcd->v[2];
		printf("LOAD Calibrate: %+.4f %+.4f %+.4f\n", v[0], v[1], v[2]);
	} else {
		printf("No calibrate!\n");
	}

	return 0;
}

/*
    GSCAL:%+.4f %+.4f %+.4f
 */
int gsensor_calibrate_store(float *v)
{
	uint32 i;
	int ret;
	RKNAND_SYS_STORGAE sysData;

	int sys_fd = open("/dev/rknand_sys_storage", O_RDWR, 0);

	if (sys_fd < 0) {
		fprintf(stderr, "rknand_sys_storage open fail\n");
		return -1;
	}
	sysData.tag = VENDOR_SECTOR_OP_TAG;
	sysData.len = RKNAND_SYS_STORGAE_DATA_LEN - 8;
	memset(sysData.data, 0, RKNAND_SYS_STORGAE_DATA_LEN);

	struct calibrate_data *pcd = (struct calibrate_data *)sysData.data;

	strcpy(pcd->tag, "GSENCAL");
	pcd->v[0] = v[0];
	pcd->v[1] = v[1];
	pcd->v[2] = v[2];

	printf("STORE Calibrate: %+.4f %+.4f %+.4f\n",
		pcd->v[0], pcd->v[1], pcd->v[2]);
	/*rknand_print_hex_data("vendor_sector save:",
		(uint32*)sysData.data,32);*/
	ret = ioctl(sys_fd, RKNAND_STORE_VENDOR_SECTOR1, &sysData);
	close(sys_fd);
	if (ret) {
		fprintf(stderr, "save vendor_sector error\n");
		return -1;
	}
	return 0;
}

static int init_sensor_test(struct sensor_info *psi)
{
	int sensor_type = 0;
	int i = 0;
	int count = 0;
	int gsensor_calibrate = 0;
	char cmd[256];

	system("insmod /system/lib/modules/inv-mpu-iio.ko");
	system("insmod /system/lib/modules/drmboot.ko");
	system("akmd &");

	if (script_fetch("allsensor", "sensor_type",
		(int *)&sensor_type, sizeof(sensor_type)) == 0)
		printf("script_fetch sensor_type = %d.\n", sensor_type);


	if (sensor_type&SENSOR_TYPE_MASK(ID_GSENSOR_CALIBRATE))
		sensor_type |= SENSOR_TYPE_MASK(ID_GSENSOR);

	sprintf(cmd, "sensor_test -p -t %lld &",
		get_android_sensor_type_mask(sensor_type));
	printf("%s\n", cmd);
	system(cmd);

	float v[3];

	gsensor_calibrate_load(v);

	for (i = 0; i < MAX_SENSOR; i++) {
		psi[i].data = NULL;
		if (sensor_type&SENSOR_TYPE_MASK(i)) {
			struct display_info *pdi = &s_di[count];

			psi[i].type = id_to_type(i);
			psi[i].name = SensorName[i];
			pdi->col = 0;
			pdi->row = get_cur_print_y();
			pdi->r = 255;
			pdi->g = 255;
			pdi->b = 0;
			pdi->a = 255;
			sprintf(pdi->string, "%s:[%s..]\n",
				psi[i].name, PCBA_TESTING);
			psi[i].pdi = pdi;
			printf("Sensor test %d: %s: %d: %p\n",
				i, psi[i].name ? : "NULL",
				psi[i].type, psi[i].pdi);
			++count;
		} else {
			psi[i].type = -1;
			psi[i].name = NULL;
			psi[i].pdi = NULL;
		}
	}

	ui_print_xy_rgba_multi(s_di, count);

	return count;
}

void *all_sensor_test(void *argv)
{
	int pipe_fd = -1;
	struct timespec now, prev;
	int sensor_count;
	int i;
	int gsensor_calibrate = 0;
	float accel_total[3] = {0.0, 0.0, 0.0};
	int accel_count = 0;
	struct pollfd poll_fds[1];
	struct timespec startup_ts;

	property_set("ro.sensor.pcba", "1");

	sensor_count = init_sensor_test(s_sensor_info);

	if (sensor_count <= 0) {
		fprintf(stderr, "No sensors need test!\n");
		return -1;
	}

	if (s_sensor_info[ID_GSENSOR_CALIBRATE].name != NULL)
		gsensor_calibrate = 1;

	while (pipe_fd == -1) {
		pipe_fd = open(SENSOR_FIFO_NAME, O_RDONLY);
		printf("Process %d open read fifo result %d\n",
				getpid(), pipe_fd);
		if (pipe_fd == -1)
			sleep(1);
	}
	poll_fds[0].fd = pipe_fd;
	poll_fds[0].events = POLLIN;
	poll_fds[0].revents = 0;

	if (poll(poll_fds, 1, 5000) <= 0) {
		for (i = 0; i < MAX_SENSOR; i++) {
			if (s_sensor_info[i].pdi == NULL)
				continue;

			s_sensor_info[i].pdi->r = 255;
			s_sensor_info[i].pdi->g = 0;
			s_sensor_info[i].pdi->b = 0;
			s_sensor_info[i].pdi->a = 255;
			sprintf(s_sensor_info[i].pdi->string, "%s:[%s]\n",
					s_sensor_info[i].name, PCBA_FAILED);
		}
		ui_print_xy_rgba_multi(s_di, sensor_count);
		close(pipe_fd);
	}

	clock_gettime(CLOCK_MONOTONIC, &prev);
	memcpy(&startup_ts, &prev, sizeof(prev));
	do {
		sensors_event_t data;
		int res = read(pipe_fd, &data, sizeof(data));

		for (i = 0; i < MAX_SENSOR; i++) {
			if (s_sensor_info[i].type == data.type) {
				if (s_sensor_info[i].data == NULL)
					s_sensor_info[i].data =
					(sensors_event_t *)malloc(sizeof(data));
				memcpy(s_sensor_info[i].data,
					&data, sizeof(data));
			}

			/* gsensor calibrate */
			if (gsensor_calibrate && data.type ==
				SENSOR_TYPE_ACCELEROMETER) {
				if (data.acceleration.v[0] > 9 &&
					data.acceleration.v[0] < 11 &&
					data.acceleration.v[1] > -1 &&
					data.acceleration.v[1] < 1 &&
					data.acceleration.v[2] > -1 &&
					data.acceleration.v[2] < 1) {
					accel_total[0] +=
						data.acceleration.v[0];
					accel_total[1] +=
						data.acceleration.v[1];
					accel_total[2] +=
						data.acceleration.v[2];
					accel_count++;
				} else {
					accel_total[0] = 0.0f;
					accel_total[1] = 0.0f;
					accel_total[2] = 0.0f;
					accel_count = 0;
				}
				/* 1 seconds.*/
				if (accel_count >= 1000) {
					sensors_event_t *pdata =
						s_sensor_info[ID_GSENSOR_CALIBRATE].data;
					if (pdata == NULL) {
						pdata = (sensors_event_t *)malloc(sizeof(sensors_event_t));
						s_sensor_info[ID_GSENSOR_CALIBRATE].data
							= pdata;
					}
					pdata->uncalibrated_gyro.uncalib[0] =
						accel_total[0]/accel_count;
					pdata->uncalibrated_gyro.uncalib[1] =
						accel_total[1]/accel_count;
					pdata->uncalibrated_gyro.uncalib[2] =
						accel_total[2]/accel_count;
					pdata->uncalibrated_gyro.bias[0] =
						pdata->uncalibrated_gyro.uncalib[0] - 9.80665f;
					pdata->uncalibrated_gyro.bias[1] =
						pdata->uncalibrated_gyro.uncalib[1] - 0.0f;
					pdata->uncalibrated_gyro.bias[2] =
						pdata->uncalibrated_gyro.uncalib[2] - 0.0f;
					printf("Accel avg(%d): %+f %+f %+f\n", accel_count,
						pdata->uncalibrated_gyro.uncalib[0],
						pdata->uncalibrated_gyro.uncalib[1],
						pdata->uncalibrated_gyro.uncalib[2]);
					printf("Accel bias: %+f %+f %+f\n",
						pdata->uncalibrated_gyro.bias[0],
						pdata->uncalibrated_gyro.bias[1],
						pdata->uncalibrated_gyro.bias[2]);
					gsensor_calibrate = 0;
					gsensor_calibrate_store(pdata->uncalibrated_gyro.bias);
				}
			}
		}

		clock_gettime(CLOCK_MONOTONIC, &now);
		if (calcdiff_ns(now, prev) < 100000000)
			continue;

		for (i = 0; i < MAX_SENSOR; i++) {
			if (s_sensor_info[i].data) {
				s_sensor_info[i].pdi->r = 0;
				s_sensor_info[i].pdi->g = 255;
				s_sensor_info[i].pdi->b = 0;
				s_sensor_info[i].pdi->a = 255;

				if (s_sensor_info[i].type ==
					SENSOR_TYPE_ACCELEROMETER)
					sprintf(s_sensor_info[i].pdi->string,
						"%s:[%s] {%+.4f %+.4f %+.4f}\n",
						s_sensor_info[i].name, PCBA_SECCESS,
						s_sensor_info[i].data->acceleration.v[0],
						s_sensor_info[i].data->acceleration.v[1],
						s_sensor_info[i].data->acceleration.v[2]);
				else if (s_sensor_info[i].type ==
					SENSOR_TYPE_GYROSCOPE)
					sprintf(s_sensor_info[i].pdi->string,
						"%s:[%s] {%+.4f %+.4f %+.4f}\n",
						s_sensor_info[i].name, PCBA_SECCESS,
						s_sensor_info[i].data->gyro.v[0],
						s_sensor_info[i].data->gyro.v[1],
						s_sensor_info[i].data->gyro.v[2]);
				else if (s_sensor_info[i].type ==
					SENSOR_TYPE_MAGNETIC_FIELD)
					sprintf(s_sensor_info[i].pdi->string,
						"%s:[%s] {%+.4f %+.4f %+.4f}\n",
						s_sensor_info[i].name, PCBA_SECCESS,
						s_sensor_info[i].data->magnetic.v[0],
						s_sensor_info[i].data->magnetic.v[1],
						s_sensor_info[i].data->magnetic.v[2]);
				else if (s_sensor_info[i].type ==
					SENSOR_TYPE_LIGHT)
					sprintf(s_sensor_info[i].pdi->string,
						"%s:[%s] {%+.4f}\n",
						s_sensor_info[i].name, PCBA_SECCESS,
						s_sensor_info[i].data->light);
				else if (s_sensor_info[i].type ==
					SENSOR_TYPE_PRESSURE)
					sprintf(s_sensor_info[i].pdi->string,
						"%s:[%s] {%+.4f}\n",
						s_sensor_info[i].name, PCBA_SECCESS,
						s_sensor_info[i].data->pressure);
				else if (i == ID_GSENSOR_CALIBRATE) {
					sprintf(s_sensor_info[i].pdi->string,
						"%s:[%s] {%+.4f %+.4f %+.4f}\n",
					s_sensor_info[i].name, PCBA_SECCESS,
					s_sensor_info[i].data->uncalibrated_gyro.bias[0],
					s_sensor_info[i].data->uncalibrated_gyro.bias[1],
					s_sensor_info[i].data->uncalibrated_gyro.bias[2]);
				}
			} else if (s_sensor_info[i].pdi) {
				if (calcdiff_ns(now, startup_ts) > 5000000000) {
					s_sensor_info[i].pdi->r = 255;
					s_sensor_info[i].pdi->g = 0;
					s_sensor_info[i].pdi->b = 0;
					s_sensor_info[i].pdi->a = 255;
					sprintf(s_sensor_info[i].pdi->string,
						"%s:[%s]\n", s_sensor_info[i].name,
						PCBA_FAILED);

					s_sensor_info[i].type = -1;
					s_sensor_info[i].name = NULL;
					s_sensor_info[i].pdi = NULL;
					if (i == ID_GSENSOR_CALIBRATE)
						gsensor_calibrate = 0;
				}
			}
		}

		ui_print_xy_rgba_multi(s_di, sensor_count);
		clock_gettime(CLOCK_MONOTONIC, &prev);
	} while (1);

	close(pipe_fd);
	for (i = 0; i < MAX_SENSOR; i++) {
		if (s_sensor_info[i].data)
			free(s_sensor_info[i].data);
	}

	return NULL;
}

