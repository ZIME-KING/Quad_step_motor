/**
 * @file Motor_ZOOM3_NonBlocking_Reset_Example.c
 * @brief ZOOM3电机非阻塞复位功能使用示例
 * @author Assistant
 * @date 2024
 */

#include "control.h"
#include "main.h"

/**
 * @brief 在main函数中使用非阻塞ZOOM3复位的示例
 */
void main_loop_with_nonblocking_reset_example(void)
{
    static uint8_t reset_started = 0;
    MotorResetState_t reset_state;
    
    // 其他主循环代码...
    
    // 示例1: 启动复位（只需调用一次）
    if (!reset_started)
    {
        if (Motor_ZOOM3_Reset_Start() == 0)
        {
            reset_started = 1;
            LOG_Print("ZOOM3 reset started successfully\r\n");
        }
        else
        {
            LOG_Print("ZOOM3 reset already in progress\r\n");
        }
    }
    
    // 示例2: 在主循环中检查复位状态
    if (reset_started)
    {
        reset_state = Motor_ZOOM3_Reset_Process();
        
        switch (reset_state)
        {
            case MOTOR_RESET_MOVING:
                // 复位进行中，可以执行其他任务
                // 这里不会阻塞主循环
                break;
                
            case MOTOR_RESET_COMPLETED:
                LOG_Print("ZOOM3 reset completed!\r\n");
                reset_started = 0; // 重置标志
                Motor_ZOOM3_Reset_Clear(); // 清除状态
                break;
                
            case MOTOR_RESET_TIMEOUT:
                LOG_Print("ZOOM3 reset timeout!\r\n");
                reset_started = 0; // 重置标志
                Motor_ZOOM3_Reset_Clear(); // 清除状态
                break;
                
            default:
                break;
        }
    }
    
    // 其他主循环代码...
}

/**
 * @brief 带有状态检查的复位示例
 */
void reset_with_status_check_example(void)
{
    MotorResetState_t current_state;
    
    // 检查当前状态
    current_state = Motor_ZOOM3_Reset_GetState();
    
    if (current_state == MOTOR_RESET_IDLE || 
        current_state == MOTOR_RESET_COMPLETED || 
        current_state == MOTOR_RESET_TIMEOUT)
    {
        // 可以启动新的复位
        if (Motor_ZOOM3_Reset_Start() == 0)
        {
            LOG_Print("New ZOOM3 reset started\r\n");
        }
    }
    else
    {
        LOG_Print("ZOOM3 reset already in progress, current state: %d\r\n", current_state);
    }
}

/**
 * @brief 完整的主函数示例
 */
void main_function_example(void)
{
    // 系统初始化
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    // ... 其他初始化
    
    LOG_Print("System initialized\r\n");
    
    // 主循环
    while (1)
    {
        // 处理ZOOM3复位（非阻塞）
        static uint8_t zoom3_reset_requested = 0;
        
        // 检查是否需要启动复位（例如，通过按键或命令触发）
        if (/* 某个条件触发复位 */ 0) // 这里替换为实际的触发条件
        {
            if (!zoom3_reset_requested)
            {
                if (Motor_ZOOM3_Reset_Start() == 0)
                {
                    zoom3_reset_requested = 1;
                    LOG_Print("ZOOM3 reset initiated\r\n");
                }
            }
        }
        
        // 处理复位状态
        if (zoom3_reset_requested)
        {
            MotorResetState_t state = Motor_ZOOM3_Reset_Process();
            
            if (state == MOTOR_RESET_COMPLETED)
            {
                LOG_Print("ZOOM3 reset finished successfully\r\n");
                zoom3_reset_requested = 0;
                Motor_ZOOM3_Reset_Clear();
            }
            else if (state == MOTOR_RESET_TIMEOUT)
            {
                LOG_Print("ZOOM3 reset failed due to timeout\r\n");
                zoom3_reset_requested = 0;
                Motor_ZOOM3_Reset_Clear();
            }
        }
        
        // 其他主循环任务
        // 例如：处理其他电机、通信、传感器读取等
        
        // 短暂延时
        HAL_Delay(10);
    }
}

/**
 * @brief 错误处理示例
 */
void error_handling_example(void)
{
    MotorResetState_t state;
    uint32_t timeout_counter = 0;
    const uint32_t MAX_TIMEOUT_COUNT = 100; // 最大超时计数
    
    // 启动复位
    if (Motor_ZOOM3_Reset_Start() != 0)
    {
        LOG_Print("Failed to start ZOOM3 reset\r\n");
        return;
    }
    
    // 监控复位过程
    while (1)
    {
        state = Motor_ZOOM3_Reset_Process();
        
        switch (state)
        {
            case MOTOR_RESET_COMPLETED:
                LOG_Print("ZOOM3 reset completed successfully\r\n");
                Motor_ZOOM3_Reset_Clear();
                return;
                
            case MOTOR_RESET_TIMEOUT:
                LOG_Print("ZOOM3 reset timeout occurred\r\n");
                Motor_ZOOM3_Reset_Clear();
                return;
                
            case MOTOR_RESET_MOVING:
                timeout_counter++;
                if (timeout_counter > MAX_TIMEOUT_COUNT)
                {
                    LOG_Print("Manual timeout - stopping reset process\r\n");
                    Motor_ZOOM3_Reset_Clear();
                    return;
                }
                break;
                
            default:
                break;
        }
        
        // 执行其他任务
        HAL_Delay(50);
    }
}