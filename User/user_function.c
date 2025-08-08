#include "main.h"
#include "user_function.h"
#include "control.h"


void PelcoD_ProcessReceivedFrame(uint8_t* frame_data, uint8_t frame_len);

void Uart_1_Data_Processing()
{			
		// 接收到数据后status=1;
    if (frame_1[uart_1_frame_id].status != 0)
    {
			// 处理完整帧,
			//PelcoD_ProcessReceivedFrame(frame_1[uart_1_frame_id].buffer, PELCO_D_FRAME_LENGTH);
			HAL_UART_Transmit(&huart1, (uint8_t *)frame_1[uart_1_frame_id].buffer, frame_1[uart_1_frame_id].length, 100);
			frame_1[uart_1_frame_id].status = 0; // 处理完数据后status 清0;
	}
}


void user_test_uart_Process(uint8_t* frame_data, uint8_t frame_len){
	

	
	if(frame_3[uart_3_frame_id].buffer[0]==0xAA && frame_3[uart_3_frame_id].buffer[9]==0xBB ){
		int16_t zoom1_step=(frame_3[uart_3_frame_id].buffer[1]<<8)+frame_3[uart_3_frame_id].buffer[2];
		int16_t zoom2_step=(frame_3[uart_3_frame_id].buffer[3]<<8)+frame_3[uart_3_frame_id].buffer[4];
		int16_t zoom3_step=(frame_3[uart_3_frame_id].buffer[5]<<8)+frame_3[uart_3_frame_id].buffer[6];
		int16_t fouce_step=(frame_3[uart_3_frame_id].buffer[7]<<8)+frame_3[uart_3_frame_id].buffer[8];
		
		Motor12_SetTargetPosition(zoom1_step*8);	//zoom1
		Motor78_SetTargetPosition(zoom2_step*8);	//zoom2
		Motor34_SetTargetPosition(zoom3_step*8); //zoom3
		Motor9A_SetTargetPosition(fouce_step*8);
		
//		Motor12_SetTargetPosition(zoom1_step);	//zoom1
//		Motor78_SetTargetPosition(zoom2_step);	//zoom2
//		Motor34_SetTargetPosition(zoom3_step); //zoom3
//		Motor9A_SetTargetPosition(fouce_step);
		
	}
	//复位命令
	if(frame_3[uart_3_frame_id].buffer[0]==0xAA &&frame_3[uart_3_frame_id].buffer[1]==0xBB &&frame_3[uart_3_frame_id].buffer[2]==0xCC){
		// 启动复位
    Motor_FOCUS_Reset_Start();
    Motor_ZOOM3_Reset_Start();
    Motor_ZOOM2_Reset_Start();
    Motor_ZOOM1_Reset_Start();
    Motor_IRIS_Reset_Start();
	}
	
}


//
void Uart_3_Data_Processing()
{
    if (frame_3[uart_3_frame_id].status != 0)
    {   // 接收到数据后status=1;
			//
			//user_test_uart_Process(frame_3[uart_3_frame_id].buffer, frame_3[uart_3_frame_id].length);
			PelcoD_ProcessReceivedFrame(frame_3[uart_3_frame_id].buffer, PELCO_D_FRAME_LENGTH);
      HAL_UART_Transmit(&huart1, (uint8_t *)frame_3[uart_3_frame_id].buffer, frame_3[uart_3_frame_id].length, 100);
			frame_3[uart_3_frame_id].status = 0; // 处理完数据后status 清0;
    }
}

//1ms运行一次
void ZOOM3_reset_handle() {
    MotorResetState_t state = Motor_ZOOM3_Reset_Process();
    if (state == MOTOR_RESET_COMPLETED)
    {
        Motor_ZOOM3_Reset_Clear();
    }
    else if (state == MOTOR_RESET_TIMEOUT)
    {
        Motor_ZOOM3_Reset_Clear();
    }
}

void FOCUS_reset_handle() {
    MotorResetState_t state = Motor_FOCUS_Reset_Process();
    if (state == MOTOR_RESET_COMPLETED)
    {
        Motor_FOCUS_Reset_Clear();
    }
    else if (state == MOTOR_RESET_TIMEOUT)
    {
        Motor_FOCUS_Reset_Clear();
    }
}

void ZOOM2_reset_handle() {
    MotorResetState_t state = Motor_ZOOM2_Reset_Process();
    if (state == MOTOR_RESET_COMPLETED)
    {
        Motor_ZOOM2_Reset_Clear();
    }
    else if (state == MOTOR_RESET_TIMEOUT)
    {
        Motor_ZOOM2_Reset_Clear();
    }
}

void ZOOM1_reset_handle() {
    MotorResetState_t state = Motor_ZOOM1_Reset_Process();
    if (state == MOTOR_RESET_COMPLETED)
    {
        Motor_ZOOM1_Reset_Clear();
    }
    else if (state == MOTOR_RESET_TIMEOUT)
    {
        Motor_ZOOM1_Reset_Clear();
    }
}

void IRIS_reset_handle() {
    MotorResetState_t state = Motor_IRIS_Reset_Process();
    if (state == MOTOR_RESET_COMPLETED)
    {
        Motor_IRIS_Reset_Clear();
    }
    else if (state == MOTOR_RESET_TIMEOUT)
    {
        Motor_IRIS_Reset_Clear();
    }
}


void User_main()
{
    Start_Peripheral();
    Reset_MS41968();
    Init_MS41968();
    //测试SPI接口
    //SPI_Test_Dump();
    SPI_Test_ReadAllRegisters();
//    Motor12_SetTargetPosition(1000);
//    Motor34_SetTargetPosition(1000);
//    Motor56_SetTargetPosition(1000);
//    Motor78_SetTargetPosition(1000);
//    Motor9A_SetTargetPosition(1000);


    // 启动复位
    Motor_FOCUS_Reset_Start();
    Motor_ZOOM3_Reset_Start();
    Motor_ZOOM2_Reset_Start();
    Motor_ZOOM1_Reset_Start();
    Motor_IRIS_Reset_Start();

    for (;;)
    {
        Uart_3_Data_Processing();
        Uart_1_Data_Processing();
        if (CompareTime(&Task_1))
        {
            GetTime(&Task_1);
            ZOOM1_reset_handle();
            ZOOM2_reset_handle();
						ZOOM3_reset_handle();
            FOCUS_reset_handle();
            IRIS_reset_handle();
        }
        if (CompareTime(&Task_20))
        {
            GetTime(&Task_20);
						Motor12_PositionControl(); // zoom1
            Motor34_PositionControl(); // zoom3
            Motor56_PositionControl(); // iris
            Motor78_PositionControl(); // zoom2
            Motor9A_PositionControl(); // focus
        }
        if (CompareTime(&Task_50))
        {
            GetTime(&Task_50);
        }
        if (CompareTime(&Task_100))
        {
            GetTime(&Task_100);
					

					
					
        }
        if (CompareTime(&Task_1000))
        {
            GetTime(&Task_1000);
            //LOG_Print(LOG_LEVEL_INFO, "test=%d\r\n",HAL_GetTick());
            
            // 检查所有CO信号状态（每1秒执行一次）
            Motors_CheckAllCOStatus();
            
            // 打印所有电机的当前位置值（每1秒执行一次）
            LOG_Print(LOG_LEVEL_INFO, "========================\r\n");
            LOG_Print(LOG_LEVEL_INFO, "ZOOM1 (Motor12): %d\r\n", Motor12_GetCurrentPosition()/8);
            LOG_Print(LOG_LEVEL_INFO, "ZOOM3 (Motor34): %d\r\n", Motor34_GetCurrentPosition()/8);
            LOG_Print(LOG_LEVEL_INFO, "IRIS  (Motor56): %d\r\n", Motor56_GetCurrentPosition()/8);
            LOG_Print(LOG_LEVEL_INFO, "ZOOM2 (Motor78): %d\r\n", Motor78_GetCurrentPosition()/8);
            LOG_Print(LOG_LEVEL_INFO, "FOCUS (Motor9A): %d\r\n", Motor9A_GetCurrentPosition()/8);
            LOG_Print(LOG_LEVEL_INFO, "========================\r\n");
            
            //Motor_ToggleBrakeRun('A');
            //HAL_GPIO_TogglePin(IRCUT_1_GPIO_Port, IRCUT_1_Pin);
        }
    }
}

//		  sprintf((char*)temp_buf,"X1=%d\r\n",XenD101.distance);
//		  HAL_UART_Transmit(&huart1,(uint8_t*)temp_buf,strlen((char*)temp_buf),100);
