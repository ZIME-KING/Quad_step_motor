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
void VD_FZ_Motor12(void);
void VD_FZ_Motor34(void);
void VD_FZ_Motor56(void);
void VD_FZ_Motor78(void);
void VD_FZ_Motor9A(void);
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

// VD_FZ次数控制函数声明
extern void VD_FZ_SetTargetCount(uint16_t count);
extern void VD_FZ_Enable(void);
extern void VD_FZ_Disable(void);
extern uint16_t VD_FZ_GetCurrentCount(void);
extern uint16_t VD_FZ_GetTargetCount(void);
extern void VD_FZ_ResetCount(void);
extern bool VD_FZ_IsCompleted(void);

// PSUMA寄存器控制函数声明
extern void Set_PSUMA_Via_Register(uint16_t psuma_value);
extern uint16_t Get_PSUMA_From_Register(void);

// 步进电机12位置控制函数声明
extern void Motor12_PositionControl(void);
extern void Motor12_SetTargetPosition(int32_t target_pos);
extern int32_t Motor12_GetCurrentPosition(void);
extern int32_t Motor12_GetTargetPosition(void);
extern void Motor12_SetCurrentPosition(int32_t current_pos);
extern void Motor12_EnablePositionControl(void);
extern void Motor12_DisablePositionControl(void);
extern bool Motor12_IsAtTargetPosition(void);
extern int32_t Motor12_GetPositionError(void);
extern void Motor12_SetMaxStepsPerCycle(uint16_t max_steps);
extern uint16_t Motor12_GetMaxStepsPerCycle(void);
extern void Motor12_ResetPosition(void);

// 步进电机34位置控制函数声明
extern void Motor34_PositionControl(void);
extern void Motor34_SetTargetPosition(int32_t target_pos);
extern int32_t Motor34_GetCurrentPosition(void);
extern int32_t Motor34_GetTargetPosition(void);
extern void Motor34_SetCurrentPosition(int32_t current_pos);
extern void Motor34_EnablePositionControl(void);
extern void Motor34_DisablePositionControl(void);
extern bool Motor34_IsAtTargetPosition(void);
extern int32_t Motor34_GetPositionError(void);
extern void Motor34_SetMaxStepsPerCycle(uint16_t max_steps);
extern uint16_t Motor34_GetMaxStepsPerCycle(void);
extern void Motor34_ResetPosition(void);

// 步进电机56位置控制函数声明
extern void Motor56_PositionControl(void);
extern void Motor56_SetTargetPosition(int32_t target_pos);
extern int32_t Motor56_GetCurrentPosition(void);
extern int32_t Motor56_GetTargetPosition(void);
extern void Motor56_SetCurrentPosition(int32_t current_pos);
extern void Motor56_EnablePositionControl(void);
extern void Motor56_DisablePositionControl(void);
extern bool Motor56_IsAtTargetPosition(void);
extern int32_t Motor56_GetPositionError(void);
extern void Motor56_SetMaxStepsPerCycle(uint16_t max_steps);
extern uint16_t Motor56_GetMaxStepsPerCycle(void);
extern void Motor56_ResetPosition(void);

// 步进电机78位置控制函数声明
extern void Motor78_PositionControl(void);
extern void Motor78_SetTargetPosition(int32_t target_pos);
extern int32_t Motor78_GetCurrentPosition(void);
extern int32_t Motor78_GetTargetPosition(void);
extern void Motor78_SetCurrentPosition(int32_t current_pos);
extern void Motor78_EnablePositionControl(void);
extern void Motor78_DisablePositionControl(void);
extern bool Motor78_IsAtTargetPosition(void);
extern int32_t Motor78_GetPositionError(void);
extern void Motor78_SetMaxStepsPerCycle(uint16_t max_steps);
extern uint16_t Motor78_GetMaxStepsPerCycle(void);
extern void Motor78_ResetPosition(void);

// 步进电机9A位置控制函数声明
extern void Motor9A_PositionControl(void);
extern void Motor9A_SetTargetPosition(int32_t target_pos);
extern int32_t Motor9A_GetCurrentPosition(void);
extern int32_t Motor9A_GetTargetPosition(void);
extern void Motor9A_SetCurrentPosition(int32_t current_pos);
extern void Motor9A_EnablePositionControl(void);
extern void Motor9A_DisablePositionControl(void);
extern bool Motor9A_IsAtTargetPosition(void);
extern int32_t Motor9A_GetPositionError(void);
extern void Motor9A_SetMaxStepsPerCycle(uint16_t max_steps);
extern uint16_t Motor9A_GetMaxStepsPerCycle(void);
extern void Motor9A_ResetPosition(void);

// 步进电机34方向控制函数声明
extern void set_34_with_direction(uint16_t pos, uint8_t direction);

// 电机复位状态枚举
typedef enum {
    MOTOR_RESET_IDLE = 0,
    MOTOR_RESET_MOVING,
    MOTOR_RESET_COMPLETED,
    MOTOR_RESET_TIMEOUT
} MotorResetState_t;

// 电机复位函数声明
extern int Motor_ZOOM3_Reset(void);
extern int Motor_FOCUS_Reset(void);
extern int Motor_ZOOM2_Reset(void);
extern int Motor_ZOOM1_Reset(void);
extern int Motor_IRIS_Reset(void);
extern int Motors_ResetAll(void);

// 非阻塞复位函数声明
extern int Motor_ZOOM3_Reset_Start(void);
extern MotorResetState_t Motor_ZOOM3_Reset_Process(void);
extern MotorResetState_t Motor_ZOOM3_Reset_GetState(void);
extern void Motor_ZOOM3_Reset_Clear(void);

extern int Motor_FOCUS_Reset_Start(void);
extern MotorResetState_t Motor_FOCUS_Reset_Process(void);
extern MotorResetState_t Motor_FOCUS_Reset_GetState(void);
extern void Motor_FOCUS_Reset_Clear(void);

extern int Motor_ZOOM2_Reset_Start(void);
extern MotorResetState_t Motor_ZOOM2_Reset_Process(void);
extern MotorResetState_t Motor_ZOOM2_Reset_GetState(void);
extern void Motor_ZOOM2_Reset_Clear(void);

extern int Motor_ZOOM1_Reset_Start(void);
extern MotorResetState_t Motor_ZOOM1_Reset_Process(void);
extern MotorResetState_t Motor_ZOOM1_Reset_GetState(void);
extern void Motor_ZOOM1_Reset_Clear(void);

extern int Motor_IRIS_Reset_Start(void);
extern MotorResetState_t Motor_IRIS_Reset_Process(void);
extern MotorResetState_t Motor_IRIS_Reset_GetState(void);
extern void Motor_IRIS_Reset_Clear(void);

// 电机初始化函数声明
extern int Motor_ZOOM3_Initialize(void);
extern int Motor_FOCUS_Initialize(void);
extern int Motor_ZOOM2_Initialize(void);
extern int Motor_ZOOM1_Initialize(void);
extern int Motor_IRIS_Initialize(void);
extern int Motors_InitializeAll(void);
extern bool Motor_CheckCOStatus(const char* motor_name, GPIO_TypeDef* gpio_port, uint16_t gpio_pin);
extern void Motors_CheckAllCOStatus(void);

// 步进电机位置信息打印函数声明
extern void Motor12_PrintPositions(void);
extern void Motor34_PrintPositions(void);
extern void Motor56_PrintPositions(void);
extern void Motor78_PrintPositions(void);
extern void Motor9A_PrintPositions(void);

#endif
