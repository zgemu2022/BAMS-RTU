#include "protocol_bams.h"
#include "bams_main.h"
#include "sys.h"
#include <string.h>
#include <stdio.h>
post_list_t *post_list_bms = NULL;
// unsigned char val[10][255] = {0};
BmsData bmsdata[2][18];
int myprintbuf(int len, unsigned char *buf)
{
	int i = 0;
	printf("\nbuflen=%d\n", len);
	for (i = 0; i < len; i++)
		printf("0x%x ", buf[i]);
	printf("\n");
	return 0;
}

static void outputdata(unsigned char bamsid, unsigned char type, int pcsid, int num)
{
	post_list_t *pnote = post_list_bms;
	// short data_temp[32];

	if (pnote == NULL)
	{
		printf("xxxxxxxxxxbbbbbbbbbbbbbbbbbbb\n");
	}
	while (pnote != NULL)
	{
		if (pnote->type != type)
		{

			if (pnote->next != NULL)
			{
				pnote = pnote->next;
				continue;
			}
			else
				return;
		}
		switch (type)
		{
		case _ALL_:
		{
			printf("发送订阅全部数据到相应模块\n");
			myprintbuf(bmsdata[bamsid][pcsid].lendata, (unsigned char *)&bmsdata[bamsid][pcsid].buf_data);
			pnote->pfun(pcsid, type, (void *)&bmsdata[bamsid][pcsid]);
		}

		break;
		case _SOC_:
		{
			printf("发送订阅SOC数据到相应模块\n");
			short soc = bmsdata[bamsid][pcsid].buf_data[14] * 256 + bmsdata[bamsid][pcsid - 1].buf_data[15];
			pnote->pfun(pcsid, type, (void *)&soc);
		}

		break;

		default:
			break;
		}

		pnote = pnote->next;
	}
}
int AnalysFun10(int bamsid, unsigned short RegAddr, unsigned char *pbuf)
{
	// POINT_ADDR poi;
	// poi.portID = 5;
	int pcsid;
	int num_val;

	static int pcsid_last[] = {0, 0};
	static int flag_first[] = {1, 1};
	static int rev_num[] = {0, 0};

	pcsid = RegAddr / 16;
	num_val = pbuf[0] * 256 + pbuf[1];

	printf("解析得到本次传输的数据个数=%d pcs编号=%d 数据包长度=%d %d  bamsid=%d\n", num_val, pcsid, num_val, pbuf[2], bamsid);
	rev_num[bamsid]++;
	if (flag_first[bamsid] == 1)
	{
		flag_first[bamsid] = 0;
		pcsid_last[bamsid] = pcsid;
	}
	else
	{
		if (pcsid_last[bamsid] != pcsid)
		{
			printf("000BAMS模块bamsid=%d 总共收到pcsid=%d 的数据包%d个\n", bamsid, pcsid_last[bamsid], rev_num[bamsid]);
			rev_num[bamsid] = 0;
			pcsid_last[bamsid] = pcsid;
		}
		else
		{

			printf("BAMS模块重复发送的包bamsid=%d 总共收到pcsid=%d 的数据包%d个\n", bamsid, pcsid_last[bamsid], rev_num[bamsid]);
			return 1;
		}
	}

	// if (memcmp((unsigned char *)&pbuf[3], (unsigned char *)&bmsdata[bamsid][pcsid].buf_data, num_val * 2) != 0)
	{
		printf("比较后不同，保存并发送 pcsid=%d bamsid=%d\n", pcsid, bamsid);
		bmsdata[bamsid][pcsid].pcsid = pcsid;
		bmsdata[bamsid][pcsid].bmsid = bamsid;
		bmsdata[bamsid][pcsid].lendata = num_val * 2;
		memcpy((char *)&bmsdata[bamsid][pcsid].buf_data, (char *)&pbuf[3], num_val * 2);
		myprintbuf(num_val * 2, (unsigned char *)&bmsdata[bamsid][pcsid].buf_data);
		outputdata(bamsid, _ALL_, pcsid, num_val);
		//	outputdata(bamsid, _SOC_, pcsid, num_val);
	}
	// else
	// 	printf("比较后相同，不发送 pcsid=%d\n",pcsid);
	return 0;
}
