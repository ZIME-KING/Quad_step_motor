/**
 * @file Reset_Handle_Usage_Example.c
 * @brief 电机复位处理函数使用示例
 * @author Assistant
 * @date 2024
 */

#include "user_function.h"
#include "control.h"
#include "main.h"

/**
 * @brief 在1ms定时器中断中调用所有电机复位处理函数的示例
 */
void Timer_1ms_Interrupt_Handler(void)
{
    // 在1ms定时器中断中处理所有电机的复位状态
    ZOOM3_reset_handle();
    FOCUS_reset_handle();
    ZOOM2_reset_handle();
    ZOOM1_reset_handle();
    IRIS_reset_handle();
}

/**
 * @brief 在主循环中调用复位处理函数的示例
 */
void Main_Loop_Reset_Handle_Example(void)
{
    static uint32_t last_reset_handle_time = 0;
    
    // 每1ms调用一次复位处理函数
    if ((HAL_GetTick() - last_reset_handle_time) >= 1)
    {
        last_reset_handle_time = HAL_GetTick();
        
        // 处理所有电机的复位状态
        ZOOM3_reset_handle();
        FOCUS_reset_handle();
        ZOOM2_reset_handle();
        ZOOM1_reset_handle();
        IRIS_reset_handle();
    }
}

/**
 * @brief 启动所有电机复位并使用reset_handle处理的完整示例
 */
void Complete_Reset_Example(void)
{
    // 启动所有电机的复位
    LOG_Print("Starting all motors reset...\r\n");
    
    Motor_ZOOM3_Reset_Start();
    Motor_FOCUS_Reset_Start();
    Motor_ZOOM2_Reset_Start();
    Motor_ZOOM1_Reset_Start();
    Motor_IRIS_Reset_Start();
    
    // 在主循环中持续调用reset_handle函数
    uint32_t start_time = HAL_GetTick();
    uint32_t timeout = 30000; // 30秒超时
    
    while ((HAL_GetTick() - start_time) < timeout)
    {
        // 每1ms调用一次复位处理函数
        Main_Loop_Reset_Handle_Example();
        
        // 检查是否所有复位都完成
        if (Motor_ZOOM3_Reset_GetState() == MOTOR_RESET_IDLE &&
            Motor_FOCUS_Reset_GetState() == MOTOR_RESET_IDLE &&
            Motor_ZOOM2_Reset_GetState() == MOTOR_RESET_IDLE &&
            Motor_ZOOM1_Reset_GetState() == MOTOR_RESET_IDLE &&
            Motor_IRIS_Reset_GetState() == MOTOR_RESET_IDLE)
        {
            LOG_Print("All motors reset completed!\r\n");
            break;
        }
        
        // 其他主循环任务
        HAL_Delay(1);
    }
    
    if ((HAL_GetTick() - start_time) >= timeout)
    {
        LOG_Print("Motors reset timeout!\r\n");
    }
}

/**
 * @brief 单独启动和处理特定电机复位的示例
 */
void Individual_Motor_Reset_Example(void)
{
    // 示例：只复位ZOOM3电机
    LOG_Print("Starting ZOOM3 motor reset...\r\n");
    
    if (Motor_ZOOM3_Reset_Start() == 0)
    {
        LOG_Print("ZOOM3 reset started successfully\r\n");
        
        // 持续调用reset_handle直到完成
        uint32_t start_time = HAL_GetTick();
        while ((HAL_GetTick() - start_time) < 15000) // 15秒超时
        {
            ZOOM3_reset_handle();
            
            // 检查复位状态
            MotorResetState_t state = Motor_ZOOM3_Reset_GetState();
            if (state == MOTOR_RESET_IDLE)
            {
                LOG_Print("ZOOM3 reset completed\r\n");
                break;
            }
            
            HAL_Delay(1);
        }
    }
    else
    {
        LOG_Print("Failed to start ZOOM3 reset\r\n");
    }
}

/**
 * @brief 在现有User_main函数中集成复位处理的示例
 */
void Enhanced_User_Main_Example(void)
{
    Start_Peripheral();
    Reset_MS41968();
    Init_MS41968();
    
    // 可选：启动所有电机复位
    // Motors_StartAllReset_NonBlocking();
    
    for (;;)
    {
        // 原有的数据处理
        Uart_1_Data_Processing();
        
        // 1ms任务
        if (CompareTime(&Task_1))
        {
            GetTime(&Task_1);
            
            // 添加复位处理函数调用
            ZOOM3_reset_handle();
            FOCUS_reset_handle();
            ZOOM2_reset_handle();
            ZOOM1_reset_handle();
            IRIS_reset_handle();
        }
        
        // 20ms任务 - 电机位置控制
        if (CompareTime(&Task_20))
        {
            GetTime(&Task_20);
            Motor12_PositionControl(); // zoom1
            Motor34_PositionControl(); // zoom3
            Motor56_PositionControl(); // iris
            Motor78_PositionControl(); // zoom2
            Motor9A_PositionControl(); // focus
        }
        
        // 其他任务...
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
            LOG_Print(LOG_LEVEL_INFO, "test\r\n");
        }
    }
}

/**
 * @brief 复位状态监控示例
 */
void Reset_Status_Monitor_Example(void)
{
    static uint32_t last_status_print = 0;
    
    // 每5秒打印一次复位状态
    if ((HAL_GetTick() - last_status_print) >= 5000)
    {
        last_status_print = HAL_GetTick();
        
        LOG_Print("=== Motor Reset Status ===\r\n");
        LOG_Print("ZOOM3: %d\r\n", Motor_ZOOM3_Reset_GetState());
        LOG_Print("FOCUS: %d\r\n", Motor_FOCUS_Reset_GetState());
        LOG_Print("ZOOM2: %d\r\n", Motor_ZOOM2_Reset_GetState());
        LOG_Print("ZOOM1: %d\r\n", Motor_ZOOM1_Reset_GetState());
        LOG_Print("IRIS: %d\r\n", Motor_IRIS_Reset_GetState());
        LOG_Print("========================\r\n");
    }
}