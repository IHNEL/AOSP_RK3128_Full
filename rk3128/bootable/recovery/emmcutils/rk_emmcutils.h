/*
 * rk_emmcutils.h
 *
 *  Created on: 2013-7-30
 *      Author: mmk@rock-chips.com
 */

#ifndef RK_EMMCUTILS_H_
#define RK_EMMCUTILS_H_


int getEmmcState();
int transformPath(const char *in, char *out);
char* getDevicePath(char *mtdDevice);

#endif /* EMMCUTILS_H_ */
