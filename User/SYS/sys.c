/*
 * user_code.c
 *
 *  Created on: Aug 12, 2020
 *      Author: king_pc1
 */
#include "main.h"
#include "sys.h"

/*' '转0
 *
 */
void String_Handle(char* p){
	while(*p!='\0'){
		if(*p==' '){
			*p='0';
		}
		p++;
	}
}


void Rank(uint16_t a[],char len)
{
	int n;  //存放数组a中元素的个数
    int i;  //比较的轮数
    int j;  //每轮比较的次数
    int buf;  //交换数据时用于存放中间数据
	n=len;
	for (i=0; i<n-1; ++i)  //比较n-1轮
    {
        for (j=0; j<n-1-i; ++j)  //每轮比较n-1-i次,
        {
            if (a[j] < a[j+1])
            {
                buf = a[j];
                a[j] = a[j+1];
                a[j+1] = buf;
            }
        }
    }
}
