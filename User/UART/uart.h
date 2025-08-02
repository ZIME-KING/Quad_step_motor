#ifndef __UART_H
#define __UART_H

#include "main.h"
//#include "uart.h"

#define UART_BUFFER_LENTH	256	//帧数据缓存区长度
#define FRAME_QUANTITY  	2   //帧缓存数量
#define FRAME_TIMEOUT   	5   //超时
#define BUSY            	1
#define FREE				0

typedef struct {
unsigned char buffer[UART_BUFFER_LENTH];	//帧数据缓存区
unsigned char status;       				//帧数据状态
unsigned char length; 						//帧长度
}Frame_Typedef;

typedef struct {
unsigned char status;	    	//串口接收帧状态  0闲 1忙
unsigned char frame_id;	    	//当前处理的帧id
unsigned char time_out;	    	//超时计数
}Uart_Frame_Typedef;



extern Uart_Frame_Typedef uart_3;
extern Frame_Typedef frame_3[FRAME_QUANTITY];     		//开2个帧缓存
extern unsigned char uart_3_frame_id;
extern uint8_t uart_3_buffer[2];
void Uart_3_Time_Even(void);
void Receive_3_Interrupt(void);


extern Uart_Frame_Typedef uart_2;
extern Frame_Typedef frame_2[FRAME_QUANTITY];     		//开2个帧缓存
extern unsigned char uart_2_frame_id;
extern uint8_t uart_2_buffer[2];
void Uart_2_Time_Even(void);
void Uart_2_Receive_Interrupt(void);

extern Uart_Frame_Typedef uart_1;										//
extern Frame_Typedef frame_1[FRAME_QUANTITY];     		//开2个帧缓存
extern uint8_t uart_1_buffer[2];
extern unsigned char uart_1_frame_id;
void Uart_1_Time_Even(void);
void Uart_1_Receive_Interrupt(void);

void UART_Transmit_Str(UART_HandleTypeDef *huart,uint8_t *p);
#endif
