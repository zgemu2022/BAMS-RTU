#include "protocol_bams.h"
#include "bams_main.h"
#include "sys.h"
#include <string.h>
#include <stdio.h>
post_list_t *post_list_bms = NULL;
// unsigned char val[10][255] = {0};
BmsData bmsdata[2][18];
int myprintbuf(int len, unsigned char *buf,int flag)
{
	int i = 0;
	if(flag==1)
		printf("\nbams buflen=%d\n", len);
	else
	    printf("\nbuflen=%d\n", len);
	for (i = 0; i < len; i++)
		printf("0x%x ", buf[i]);
	printf("\n");
	return 0;
}

int printbuf_msg(int len, unsigned char *buf, int pcsid,int bamsid)
{
	// void *pcs_buf1 = pcs_buf;
	// unsigned short buf[16];
	// memcmp(buf, pcs_buf,sizeof(buf));
	int pcsid_z;
	if (bamsid == 1){
		pcsid_z = pcsid + 16;
	}
	else {
		pcsid_z = pcsid;
	}

	printf("\n%d号Bams %d号pcs 总装置:%d号pcs buflen=%d\n", bamsid, pcsid+1, pcsid_z+1,len);
	printf("1.最大允许充电功率:%d \n",buf[0]*256 + buf[1]);
	printf("2.最大允许放电功率：%d \n", buf[2] * 256 + buf[3]);
	printf("3.通讯心跳:%d \n", buf[4]*256 + buf[5]);
	printf("4.总电压:%d \n", buf[6] * 256 + buf[7]);
	printf("5.最大允许充电电流:%d \n", buf[8] * 256 + buf[9]);
	printf("6.最大允许放电电流:%d \n", buf[10] * 256 + buf[11]);
	printf("7.电池总电流:%d \n", buf[12] * 256 + buf[13]);
	printf("8.电池 SOC:%d \n", buf[14] * 256 + buf[15]);
	printf("9.电池剩余可充电量:%d \n", buf[16] * 256 + buf[17]);
	printf("10.电池剩余可放电量:%d \n", buf[18] * 256 + buf[19]);
	printf("11.单体最高电压:%d \n", buf[20] * 256 + buf[21]);
	printf("12.单体最低电压:%d \n", buf[22] * 256 + buf[23]);
	printf("13.状态:%d \n", buf[24] * 256 + buf[25]);
	printf("14.需求:%d \n", buf[26] * 256 + buf[27]);
	printf("15.总故障状态:%d \n", buf[28] * 256 + buf[29]);
	printf("16.预留:%d \n", buf[30] * 256 + buf[31]);

	printf("\n\n");

	// printf("1.电池分系统 n 最大允许充电功率:%d \n", buf[0]);
	// printf("2.电池分系统 n 最大允许放电功率：%d \n", buf[1]);
	// printf("3.电池分系统 n 通讯心跳:%d \n", buf[2]);
	// printf("4.电池分系统 n 总电压:%d \n", buf[3]);
	// printf("5.电池分系统 n 最大允许充电电流:%d \n", buf[4]);
	// printf("6.电池分系统 n 最大允许放电电流:%d \n", buf[5]);
	// printf("7.电池分系统 n 电池总电流:%d \n", buf[6]);
	// printf("8.电池分系统 n 电池 SOC:%d \n", buf[7]);
	// printf("9.电池分系统 n 电池剩余可充电量:%d \n", buf[8]);
	// printf("10.电池分系统 n 电池剩余可放电量:%d \n", buf[9]);
	// printf("11.电池分系统 n 单体最高电压:%d \n", buf[10]);
	// printf("12.电池分系统 n 单体最低电压:%d \n", buf[11]);
	// printf("13.电池分系统 n 状态:%d \n", buf[12]);
	// printf("14.电池分系统 n 需求:%d \n", buf[13]);
	// printf("15.电池分系统 n 总故障状态:%d \n", buf[14]);
	// printf("16.预留:%d \n", buf[15]);
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
			printf("发送订阅全部数据到相应模块 bamsid=%d  pcsid=%d\n",bamsid,  pcsid);
			myprintbuf(bmsdata[bamsid][pcsid].lendata, (unsigned char *)&bmsdata[bamsid][pcsid].buf_data,0);
			printbuf_msg(bmsdata[bamsid][pcsid].lendata, (unsigned char *)&bmsdata[bamsid][pcsid].buf_data,pcsid, bamsid);
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
int AnalysFun10(int bamsid, unsigned short RegAddr, unsigned char *pbuf,int pcsNum)
{
	// POINT_ADDR poi;
	// poi.portID = 5;
	// printf("");
	int pcsid;
	int num_val;
	int i;
	num_val = 16;

	static int pcsid_last[] = {0, 0};
	static int flag_first[] = {1, 1};
	static int rev_num[] = {0, 0};

	pcsid = RegAddr / 16;
	// if (bamsid == 1){
	// 	pcsid+=16;
	// }

		// num_val = pbuf[0] * 256 + pbuf[1];

	rev_num[bamsid]++;

	printf("解析得到本次传输的数据个数=%d 数据包长度=%d %d  bamsid=%d\n", num_val, num_val, pbuf[2], bamsid);
	// if (flag_first[bamsid] == 1)
	// {
	// 	flag_first[bamsid] = 0;
	// 	pcsid_last[bamsid] = i;//pcsid;
	// }
	// else
	// {
	// 	// if (pcsid_last[bamsid] != pcsid)
	// 	// {
	// 	// 	printf("000BAMS模块bamsid=%d 总共收到pcsid=%d 的数据包%d个\n", bamsid, pcsid_last[bamsid], rev_num[bamsid]);
	// 	// 	rev_num[bamsid] = 0;
	// 	// 	pcsid_last[bamsid] = pcsid;
	// 	// }
	// 	// else
	// 	// {

	// 	// 	printf("BAMS模块重复发送的包bamsid=%d 总共收到pcsid=%d 的数据包%d个\n", bamsid, pcsid_last[bamsid], rev_num[bamsid]);
	// 	// 	return 1;
	// 	// }
	// }

	// if (memcmp((unsigned char *)&pbuf[3], (unsigned char *)&bmsdata[bamsid][pcsid].buf_data, num_val * 2) != 0)
	{
		for (i = 0; i < pcsNum; i++)
		{
			printf("比较后不同，保存并发送 pcsid=%d bamsid=%d\n", pcsid, bamsid);
			bmsdata[bamsid][pcsid].pcsid_bms = pcsid;
			bmsdata[bamsid][pcsid].bmsid = bamsid;
			bmsdata[bamsid][pcsid].lendata = num_val * 2;
			memcpy((char *)&bmsdata[bamsid][pcsid].buf_data, (char *)&pbuf[3 + i * 32], num_val * 2);
			myprintbuf(num_val * 2, (unsigned char *)&bmsdata[bamsid][pcsid].buf_data, 0);
			outputdata(bamsid, _ALL_, pcsid, num_val);
			pcsid++;
		}
		//	outputdata(bamsid, _SOC_, pcsid, num_val);
		}
	// else
	// 	printf("比较后相同，不发送 pcsid=%d\n",pcsid);
	return 0;
}
