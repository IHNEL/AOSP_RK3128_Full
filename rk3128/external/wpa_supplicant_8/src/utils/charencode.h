#ifndef CHARENCODE_H
#define CHARENCODE_H

int gbk2utf8(const char *inbuf, int inlen, char *outbuf);
int utf82gbk(const char *inbuf, int inlen, char *outbuf);
int IsTextUTF8(const char* str,long length);

#endif /* CHARENCODE_H */
