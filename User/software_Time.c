/*
 * software_Time.c
 *
 *  Created on: 2020年9月10日
 *      Author: champion
 */
#include "main.h"
#include "software_Time.h"

 TIME_TYPE_ST Task_1;
 TIME_TYPE_ST Task_5;
 TIME_TYPE_ST Task_20;
 TIME_TYPE_ST Task_25;
 TIME_TYPE_ST Task_50;
 TIME_TYPE_ST Task_100;
 TIME_TYPE_ST Task_200;
 TIME_TYPE_ST Task_500;
 TIME_TYPE_ST Task_1000;

void delay_us(uint32_t i){
	while(i--){
		for(uint16_t cnt=0;cnt<10;cnt++);
	}
}


 //__attribute__机制在main之前运行一次 set_time
static __attribute__((constructor)) void SetTime(void);

//设定间隔时间
void SetTime(void){
	Task_1.TimeInter     = 1;//设定间隔时间
	Task_5.TimeInter     = 5;
	Task_20.TimeInter    = 20;
	Task_25.TimeInter    = 25;
	Task_50.TimeInter    = 50;
	Task_100.TimeInter   = 100;
	Task_200.TimeInter   = 200;
	Task_500.TimeInter   = 500;
	Task_1000.TimeInter  = 1000;
}
void GetTime(TIME_TYPE_ST *TimeType)
{
    TimeType->TimeStart = HAL_GetTick();//获取起始时间
}
 
uint8_t CompareTime(TIME_TYPE_ST *TimeType)//每隔1毫秒，计数器就会增加1
{
    return ((HAL_GetTick()-TimeType->TimeStart) >= TimeType->TimeInter);
}
