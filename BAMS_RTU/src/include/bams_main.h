#ifndef _BAMS_MAIN_H_
#define _BAMS_MAIN_H_

#define PORTNUM_MAX    2
typedef struct
{
	unsigned char portnum;
    unsigned short baud[PORTNUM_MAX];
	unsigned char devid[PORTNUM_MAX];
	unsigned char pcs_num[PORTNUM_MAX];

} PARA_BAMS; //
typedef int			(*outData2Other)(unsigned char ,unsigned char ,void *pdata);	//输出数据

typedef struct _post_list_t
{
	outData2Other  pfun;
	unsigned char type;
    struct _post_list_t *next;
}post_list_t;

int bams_main(void* para);

int SubscribeBamsData(unsigned char type, outData2Other pfun);
#endif