#include "main.h"
#include "user_function.h"



void Uart_1_Data_Processing() {
	uint8_t uart1_send_buf[20];
	if(frame_1[uart_1_frame_id].status!=0) {   			//接收到数据后status=1;
		
		int16_t pos_val=(frame_1[uart_1_frame_id].buffer[0]<<8)+frame_1[uart_1_frame_id].buffer[1];
		Motor34_SetTargetPosition((int32_t)pos_val);

		
		HAL_UART_Transmit(&huart1,(uint8_t*)frame_1[uart_1_frame_id].buffer,frame_1[uart_1_frame_id].length,100);
		frame_1[uart_1_frame_id].status=0;					//处理完数据后status 清0;
	}
}

//
void Uart_3_Data_Processing() {
	if(frame_3[uart_3_frame_id].status!=0) {   			//接收到数据后status=1;
		
		
		Motor34_SetTargetPosition((frame_3[uart_3_frame_id].buffer[0]<<8)+frame_3[uart_3_frame_id].buffer[1]);

		HAL_UART_Transmit(&huart1,(uint8_t*)frame_3[uart_3_frame_id].buffer,frame_3[uart_3_frame_id].length,100);
		frame_3[uart_3_frame_id].status=0;					//处理完数据后status 清0;
	}
}

void set_34(uint16_t pos);

void User_main(){

	Start_Peripheral();
	Reset_MS41968();
	Init_MS41968();
//测试SPI接口
	//SPI_Test_Dump();
		SPI_Test_ReadAllRegisters();
		//Motor_ToggleBrakeRun('A');
		//Motor_ToggleBrakeRun('A');
	Motor34_SetTargetPosition(1000);
	
	for(;;){
	  Uart_3_Data_Processing();
	  Uart_1_Data_Processing();
	  if(CompareTime(&Task_1)){
		  GetTime(&Task_1);
	  }
		if(CompareTime(&Task_20)){
		  GetTime(&Task_20);
			Motor34_PositionControl();
		}
	  if(CompareTime(&Task_50)){
		  GetTime(&Task_50);
		}
	  if(CompareTime(&Task_100)){
		  GetTime(&Task_100);
	  }
	  if(CompareTime(&Task_1000)){
		  GetTime(&Task_1000);

			//set_34(10);

			LOG_Print(LOG_LEVEL_INFO,"test\r\n");
			//Motor_ToggleBrakeRun('A');
//		  HAL_GPIO_TogglePin(IRCUT_1_GPIO_Port, IRCUT_1_Pin);
	  }
	}
}

//		  sprintf((char*)temp_buf,"X1=%d\r\n",XenD101.distance);
//		  HAL_UART_Transmit(&huart1,(uint8_t*)temp_buf,strlen((char*)temp_buf),100);


