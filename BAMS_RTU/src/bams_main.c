
#include <stdio.h>
#include "threads.h"
#include "bams_main.h"
#include "protocol_bams.h"
#include <string.h>
#include <malloc.h>

int bams_main(void *para)
{
	printf("123456bams_main初始化\n");
	CreateThreads(para);
	// while (1)
	// {
	// }
}

int SubscribeBamsData(unsigned char type, outData2Other pfun) //订阅Bams数据
{
	printf("正在订阅BAMS数据 type=%d！！！！！\n", type);
	post_list_t *note = (post_list_t *)malloc(sizeof(post_list_t));
	note->type = type;

	note->pfun = pfun;
	note->next = post_list_bms;
	post_list_bms = note;

	return 0;
}