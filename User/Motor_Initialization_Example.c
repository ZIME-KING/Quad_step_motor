/**
 * @file Motor_Initialization_Example.c
 * @brief 电机初始化功能使用示例
 * @author Generated Code
 * @date 2024
 */

#include "control.h"
#include "main.h"
#include "UART/uart.h"

/**
 * @brief 电机初始化示例 - 单个电机初始化
 */
void Motor_Init_Single_Example(void)
{
    LOG_Print("=== Single Motor Initialization Example ===\r\n");
    
    // 初始化ZOOM3电机
    if (Motor_ZOOM3_Initialize() == 0)
    {
        LOG_Print("ZOOM3 motor initialization successful\r\n");
    }
    else
    {
        LOG_Print("ZOOM3 motor initialization failed\r\n");
    }
    
    // 初始化FOCUS电机
    if (Motor_FOCUS_Initialize() == 0)
    {
        LOG_Print("FOCUS motor initialization successful\r\n");
    }
    else
    {
        LOG_Print("FOCUS motor initialization failed\r\n");
    }
}

/**
 * @brief 电机初始化示例 - 所有电机初始化
 */
void Motor_Init_All_Example(void)
{
    LOG_Print("=== All Motors Initialization Example ===\r\n");
    
    // 初始化所有电机
    int result = Motors_InitializeAll();
    
    if (result == 0)
    {
        LOG_Print("All motors initialization completed successfully\r\n");
    }
    else
    {
        LOG_Print("Some motors initialization failed\r\n");
    }
}

/**
 * @brief 电机CO状态检查示例
 */
void Motor_CO_Status_Check_Example(void)
{
    LOG_Print("=== Motor CO Status Check Example ===\r\n");
    
    // 检查单个电机CO状态
    bool zoom3_status = Motor_CheckCOStatus("ZOOM3", ZOOM3_PI_CO_GPIO_Port, ZOOM3_PI_CO_Pin);
    if (zoom3_status)
    {
        LOG_Print("ZOOM3 motor is properly reset\r\n");
    }
    else
    {
        LOG_Print("ZOOM3 motor is not reset\r\n");
    }
    
    // 检查所有电机CO状态
    Motors_CheckAllCOStatus();
}

/**
 * @brief 完整的电机初始化流程示例
 */
void Motor_Complete_Init_Example(void)
{
    LOG_Print("=== Complete Motor Initialization Process ===\r\n");
    
    // 步骤1: 检查初始CO状态
    LOG_Print("Step 1: Check initial CO status\r\n");
    Motors_CheckAllCOStatus();
    
    // 步骤2: 执行电机初始化
    LOG_Print("Step 2: Initialize all motors\r\n");
    int init_result = Motors_InitializeAll();
    
    // 步骤3: 检查初始化后的CO状态
    LOG_Print("Step 3: Check CO status after initialization\r\n");
    Motors_CheckAllCOStatus();
    
    // 步骤4: 报告最终结果
    if (init_result == 0)
    {
        LOG_Print("Motor initialization process completed successfully\r\n");
    }
    else
    {
        LOG_Print("Motor initialization process failed\r\n");
    }
}

/**
 * @brief 电机初始化错误处理示例
 */
void Motor_Error_Handling_Example(void)
{
    LOG_Print("=== Motor Error Handling Example ===\r\n");
    
    // 尝试初始化每个电机，并处理可能的错误
    const char* motor_names[] = {"ZOOM3", "FOCUS", "ZOOM2", "ZOOM1", "IRIS"};
    int (*init_functions[])(void) = {
        Motor_ZOOM3_Initialize,
        Motor_FOCUS_Initialize,
        Motor_ZOOM2_Initialize,
        Motor_ZOOM1_Initialize,
        Motor_IRIS_Initialize
    };
    
    int failed_count = 0;
    
    for (int i = 0; i < 5; i++)
    {
        LOG_Print("Initializing %s motor...\r\n", motor_names[i]);
        
        int result = init_functions[i]();
        
        if (result == 0)
        {
            LOG_Print("%s motor: OK\r\n", motor_names[i]);
        }
        else
        {
            LOG_Print("%s motor: FAILED\r\n", motor_names[i]);
            failed_count++;
            
            // 这里可以添加具体的错误处理逻辑
            // 例如：重试、报警、停止系统等
        }
    }
    
    LOG_Print("Initialization summary: %d failed, %d successful\r\n", 
              failed_count, 5 - failed_count);
}

/**
 * @brief 主函数中调用电机初始化的示例
 */
void Motor_Init_In_Main_Example(void)
{
    LOG_Print("=== Motor Initialization in Main Function Example ===\r\n");
    
    // 在main函数中，通常在系统初始化后调用
    // 这个示例展示了推荐的调用顺序
    
    LOG_Print("System startup...\r\n");
    
    // 1. 系统基本初始化（HAL_Init, 时钟配置等）已完成
    
    // 2. GPIO初始化已完成
    
    // 3. 执行电机初始化
    LOG_Print("Starting motor initialization...\r\n");
    
    if (Motors_InitializeAll() == 0)
    {
        LOG_Print("Motor system ready\r\n");
        
        // 4. 继续其他系统初始化
        LOG_Print("Continuing with other system initialization...\r\n");
    }
    else
    {
        LOG_Print("Motor initialization failed - system may not function properly\r\n");
        
        // 错误处理：可能需要进入安全模式或重试
    }
}