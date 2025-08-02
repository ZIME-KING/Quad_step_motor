#ifndef _CONTROL_H_
#define _CONTROL_H_

#include "main.h"
#include <stdbool.h>


#define CS_CLR()						HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_RESET)
#define CS_SET()						HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_SET)

#define RSTB_H					HAL_GPIO_WritePin(MS_RSTB_GPIO_Port, MS_RSTB_Pin, GPIO_PIN_SET)
#define RSTB_L					HAL_GPIO_WritePin(MS_RSTB_GPIO_Port, MS_RSTB_Pin, GPIO_PIN_RESET)

//#define VDIS_H					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET)
//#define VDIS_L					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET)
//#define VD12_H					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET)
//#define VD12_L					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET)
//#define VD34_H					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET)
//#define VD34_L					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET)
//#define VD56_H					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET)
//#define VD56_L					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET)
//#define VD78_H					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET)
//#define VD78_L					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET)
//#define VD9A_H					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_SET)
//#define VD9A_L					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_RESET)

#define TEST_H			    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET)
#define TEST_L			    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET)

extern void Init_MS41968(void);
extern void Key_Function(uint32_t u32KeyValue);
extern void Reset_MS41968(void);
extern void VD_FZ(void);
extern void VD_IS(void);

// 电机控制函数声明
extern void Motor_ToggleBrakeRun(char channel);
extern void Motor_ToggleDirection(char channel);
extern void Motor_DecreaseSpeed(char channel);
extern void Motor_IncreaseSpeed(char channel);

// SPI测试函数声明
extern void SPI_Test_Dump(void);
extern void SPI_Test_ReadAllRegisters(void);

// 步进电机VD周期控制函数
extern void Stepper_SetTargetSteps(uint16_t stepsA, uint16_t stepsB);
extern void Stepper_Start(void);
extern void Stepper_Stop(void);
extern uint16_t Stepper_GetStepsA(void);
extern uint16_t Stepper_GetStepsB(void);
extern bool Stepper_IsRunning(void);
extern void Stepper_ExecuteStep(void);
extern void Stepper_Test_Demo(void);


#endif
