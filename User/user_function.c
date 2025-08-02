#include "main.h"
#include "user_function.h"



void Uart_1_Data_Processing() {
	uint8_t uart1_send_buf[20];
	if(frame_1[uart_1_frame_id].status!=0) {   			//接收到数据后status=1;
		HAL_UART_Transmit(&huart1,(uint8_t*)frame_1[uart_1_frame_id].buffer,frame_1[uart_1_frame_id].length,100);
		frame_1[uart_1_frame_id].status=0;					//处理完数据后status 清0;
	}
}

//
void Uart_3_Data_Processing() {
	if(frame_3[uart_3_frame_id].status!=0) {   			//接收到数据后status=1;
		HAL_UART_Transmit(&huart1,(uint8_t*)frame_3[uart_3_frame_id].buffer,frame_3[uart_3_frame_id].length,100);
		frame_3[uart_3_frame_id].status=0;					//处理完数据后status 清0;
	}
}


void User_main(){

	Start_Peripheral();
	Reset_MS41968();
	
//测试SPI接口
	//SPI_Test_Dump();
	SPI_Test_ReadAllRegisters();
	
	
	for(;;){
	  Uart_3_Data_Processing();
	  Uart_1_Data_Processing();
	  if(CompareTime(&Task_1)){
		  GetTime(&Task_1);
	  }
	  if(CompareTime(&Task_25)){
		  GetTime(&Task_25);
		}
	  if(CompareTime(&Task_100)){
		  GetTime(&Task_100);
	  }
	  if(CompareTime(&Task_1000)){
		  GetTime(&Task_1000);

			LOG_Print(LOG_LEVEL_INFO,"test\r\n");
//		  HAL_GPIO_TogglePin(IRCUT_1_GPIO_Port, IRCUT_1_Pin);
	  }
	}
}

//		  sprintf((char*)temp_buf,"X1=%d\r\n",XenD101.distance);
//		  HAL_UART_Transmit(&huart1,(uint8_t*)temp_buf,strlen((char*)temp_buf),100);


