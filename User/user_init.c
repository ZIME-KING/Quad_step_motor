#include "main.h"
#include "user_init.h"
#include "uart.h"
#include "control.h"

//启动外设
void Start_Peripheral(){
	HAL_TIM_Base_Start_IT(&htim2);  //Start timer interrupt启动定时器中断（同时就启动定时器了不用单独启动定时器）
	HAL_TIM_Base_Start_IT(&htim3);  //Start timer interrupt启动定时器中断（同时就启动定时器了不用单独启动定时器）
	HAL_TIM_Base_Start_IT(&htim4);  //Start timer interrupt启动定时器中断（同时就启动定时器了不用单独启动定时器）

    //HAL_UART_Receive_IT(&huart2,uart_2_buffer,1);		//uart2 模拟dmx512只发送不接收，不开启接受中断
	//HAL_UART_Receive_IT(&huart3,uart_3_buffer,1);
    HAL_UART_Receive_IT(&huart1,uart_1_buffer,1);


    UART_Transmit_Str(&huart1,(uint8_t*)"usart_1_ok\r\n");
    HAL_Delay(20);

}

int a;

//定时器回调函数
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if(htim->Instance==TIM2){
		// VD周期 - 步进电机控制
		VD_Stepper_Control();
	}
	if(htim->Instance==TIM3){
	}
	if(htim->Instance==TIM4){
		Uart_1_Time_Even();
		Uart_3_Time_Even();
		a++;
	}
}

/**
 * @brief VD周期步进电机控制函数
 * @retval None
 * @note 在定时器2中断中调用，控制步进电机按PSUMA/PSUMB步数运行
 */
void VD_Stepper_Control(void)
{
    // 调用步进电机执行函数
    Stepper_ExecuteStep();
}



//外部中断回调函数
//void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
//	if(GPIO_Pin == GPIO_PIN_9){
//		if(NRF24L01_RxPacket(rece_buf)==0)
////		{
//			HAL_GPIO_TogglePin(LED_0_GPIO_Port,  LED_0_Pin);
////			Rx_Buf();
////		}
//	}
//}




