
#ifndef _PROTOCOL_BAMS_H_
#define _PROTOCOL_BAMS_H_
#include "bams_main.h"
#define _ALL_ 1
#define _SOC_ 2
typedef struct
{
	unsigned char funid;
	unsigned short RegStart; //寄存器开始地址

	short para; //设置参数

} BAMS_Fun_Struct;

typedef struct
{
	unsigned char bmsid;
	unsigned char pcsid_bms;
	unsigned char lendata;		 //收到的数据长度
	unsigned char buf_data[256]; //收到的数据部分
} BmsData;
extern post_list_t *post_list_bms;
extern BmsData bmsdata[2][18];
int myprintbuf(int len, unsigned char *buf,int flag);
int AnalysFun10(int bamsid, unsigned short RegAddr, unsigned char *pbuf,int pcsNum);
#endif
