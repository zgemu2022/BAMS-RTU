
#include "threads.h"
#include "sys.h"
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include "serial.h"
#include "bams_main.h"
#include "protocol_bams.h"
#include "crc.h"
#include "IEC61850_type.h"

PARA_BAMS ParaBams;
PARA_BAMS *pParaBams = &ParaBams;

void Uart_Init(unsigned char portid, unsigned int baud)
{
	int ret;

	printf("正在试图打开串口%d   波特率=%d！！！！！\n", portid, baud);
	ret = OpenComPort(portid, baud, 8, "1", 'N');
	if (ret == -1)
		printf("串口%d打开失败！！！！！\n", portid);
	else
	{
		printf("串口%d打开成功  波特率=%d！！！！！\n", portid, baud);
	}
}

static int doRecvFunTasks(int portid)
{
	unsigned char commbuf[256];
	int lencomm = 255, lentemp1, lentemp2, lenframe;
	unsigned short crcval;
	unsigned char b1, b2;
	int bmsid = portid;
	unsigned short regAddr;
	lentemp1 = ReadComPort(portid, commbuf, lencomm);

	// printf("1收到的字符串长度 = %d \n", lentemp1);

	myprintbuf(lentemp1, commbuf);
	if (lentemp1 < 2)
		return 255;

	if (commbuf[1] != 0x10 && commbuf[1] != 0x04 && commbuf[1] != 0x03 && commbuf[1] != 0x06)
	{
		return 254;
	}
	printf("2收到的字符串长度 = %d \n", lentemp1);

	if (0x10 == commbuf[1])
	{
		lentemp2 = (commbuf[4] * 256 + commbuf[5]) * 2;
		printf("3.1计算字符串长度 = %d %d\n", lentemp2, commbuf[6]);

		if (lentemp2 != commbuf[6])
			return 252;
		lenframe = lentemp2 + 9;
		printf("4收到的字符串长度 = %d %d\n", lentemp1, lenframe);

		// lenframe = lentemp2 + 9;
		// lencomm = lenframe - lentemp1;

		// lentemp2 = ReadComPort(portid, commbuf, lencomm);
		// if ((lentemp1 + lentemp2) < lenframe)
		// 	return 251;
	}
	printf("5收到的字符串长度 = %d \n", lentemp1);

	crcval = crc16_check(commbuf, lenframe - 2);

	b1 = crcval / 256;
	b2 = crcval % 256;

	printf("crc校验  = %d \n", lentemp1);

	if (b1 != commbuf[lenframe - 2] || b2 != commbuf[lenframe - 1])
		return 250;

	printf("6收到正确包，包长度长度 = %d bmsid=%d \n", lentemp1,portid);

	if (commbuf[1] != 0x10) //本程序暂时不处理其他功能码，以非法功能直接返回
	{
		commbuf[1] += 0x80;
		commbuf[2] = 1;

		crcval = crc16_check(commbuf, 3);

		commbuf[3] = crcval / 255;
		commbuf[4] = crcval % 255;
		WriteComPort(portid, commbuf, 5);
		return 1;
	}

	regAddr = commbuf[2] * 256 + commbuf[3];
	AnalysFun10(portid, regAddr, (unsigned short *)&commbuf[4]);

	crcval = crc16_check(commbuf, 6);

	commbuf[6] = crcval / 256;
	commbuf[7] = crcval % 256;

	WriteComPort(portid, commbuf, 8);

	return 0;
}

void *serial_thread(void *arg)
{

	int portid = (int)arg;
	int taskid;
	int ret;

	printf("serial_thread 111端口号 =%d \"n", portid);
	Uart_Init(portid, pParaBams->baud[portid]);
	while (1)
	{
		printf("BAMS准备接收数据！！！！\n");
		ret = doRecvFunTasks(portid);
		if (ret != 0)
			usleep(100000); //延时100ms
	}
}

void CreateThreads(void *para)
{
	pthread_t ThreadID;
	pthread_attr_t Thread_attr;
	int i;
	memcpy((void *)pParaBams, (PARA_BAMS *)para, sizeof(PARA_BAMS));
	for (i = 0; i < pParaBams->portnum; i++)
	{
		if (FAIL == CreateSettingThread(&ThreadID, &Thread_attr, (void *)serial_thread, (int *)i, 1, 1))
		{
			printf("serial_thread CREATE ERR!\n");

			exit(1);
		}
	}
	printf("serial_thread CREATE success!\n");
}