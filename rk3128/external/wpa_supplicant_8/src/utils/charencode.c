/*
 * charencode.c
 * 2015-2, wilen <wilen.gao@rock-chips.com>
 *
 * This file is created to offer some apis for character transcoding.
 * wpa_supplicant is not support gbk ssid(e.g. chinese), so i try
 * to make wpa_supplicant to support this function.
 * Any question please contact me.<wilen.gao@rock-chips.com>
 */

#include <iconv.h>
#include "utils/includes.h"
#include "utils/common.h"
#include "common/ieee802_11_common.h"
#include "wpa_supplicant_i.h"
#include "driver.h"
#include "bss.h"

#define true	1
#define false	0

int gbk2utf8(const char *inbuf, int inlen, char *outbuf)
{
    char **pin = &inbuf;
    char **pout = &outbuf;
    size_t dlen= 32 * 4;

    iconv_t cd = iconv_open("UTF-8","GB2312");
    if (cd==NULL)
    {
        wpa_printf(MSG_ERROR, "gbk2utf8: open error");
        return -1;
    }
    iconv(cd, pin, &inlen, pout, &dlen);
    iconv_close(cd);

    return 0;
}


int utf82gbk(const char *inbuf, int inlen, char *outbuf)
{
    char **pin = &inbuf;
    char **pout = &outbuf;
    size_t dlen= 32 * 4;

    iconv_t cd = iconv_open("GB2312", "UTF-8");
    if (cd==NULL)
    {
        wpa_printf(MSG_ERROR, "gbk2utf8: open error");
        return -1;
    }
    iconv(cd, pin, &inlen, pout, &dlen);
    iconv_close(cd);
    return 0;
}

unsigned char Ascii2Hex(char *ascii, int len) {
	int i = 0;

	if (ascii == NULL) return -1;
	for (i = 0; i < len; i++) {
        if ((ascii[i] >= 0x30) && (ascii[i] <= 0x39))
            ascii[i] -= 0x30;
        else if ((ascii[i] >= 0x41) && (ascii[i] <= 0x46))//大写字母
            ascii[i] -= 0x37;
        else if ((ascii[i] >= 0x61) && (ascii[i] <= 0x66))//小写字母
            ascii[i] -= 0x57;
        else return -1;
	}
    return 0; 
} 


unsigned char Ascii2Ul(char *ascii, int len)
{
	int i = 0;
	unsigned char tmp[32 * 4] = {0};

	if (ascii == NULL || (len%2 > 0)) return -1;
	Ascii2Hex(ascii, len);
	for (i = 0; i < len/2; i++) {
		tmp[i] = ascii[i*2] * 16 + ascii[i*2 +1];
	}
	memset(ascii, 0, len);
	strcpy(ascii, tmp);
	ascii[len/2] = '\0';

	return 0;
}

int isAsciiRange(char *buf, int len)
{
	int i = 0;
	if (buf == NULL) return -1;
	for (i = 0; i < len; i++) {
		if (buf[i] >= 32 && buf[i] < 127)
			continue;
		else
			return false;
	}
	return true;
}

int IsTextUTF8(const char* str,long length)
{
    int i;
    int nBytes=0;
    unsigned char chr;
    int bAllAscii=true;

	if (str == NULL)
		return false;

    for (i=0; i<length; i++) {
        chr= *(str+i);
        if ((chr&0x80) != 0) bAllAscii= false;
        if (nBytes==0) {
            if (chr>=0x80) {
                if (chr>=0xFC&&chr<=0xFD) nBytes=6;
                else if (chr>=0xF8) nBytes=5;
                else if (chr>=0xF0) nBytes=4;
                else if (chr>=0xE0) nBytes=3;
                else if (chr>=0xC0) nBytes=2;
                else { return false;}
                nBytes--;
            }
        } else {
            if( (chr&0xC0) != 0x80 ) { return false; }
            nBytes--;
        }
    }

    if( nBytes > 0 ) { return false; }
    if( bAllAscii ) { return false; }
    return true;
}

int CheckIsSsidTextUtf8(struct wpa_supplicant *wpa_s, char *ssid)
{
    struct wpa_bss *bss;
	char ssid_tmp[32 * 4] = {0};
    int ret;

	if (wpa_s == NULL || ssid == NULL)
		return false;

	if (*ssid == '"' && *(ssid + strlen(ssid) -1 ) == '"')
		memcpy(ssid_tmp, ssid+1, strlen(ssid) - 2);

	dl_list_for_each(bss, &wpa_s->bss_id, struct wpa_bss, list_id) {
		//if (bss->ssid != NULL && !os_strcmp(bss->ssid, ssid_tmp)) {
		if (!os_strcmp(bss->ssid, ssid_tmp)) {
			wpa_printf(MSG_ERROR, "CheckIsSsidTextUtf8: %s:%s", bss->ssid, ssid);
			return true;
		}
    }
	
	return false;
}




