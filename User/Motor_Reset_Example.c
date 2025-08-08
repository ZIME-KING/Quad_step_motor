/**
 * @file Motor_Reset_Example.c
 * @brief 电机复位流程使用示例
 * @author Generated Code
 * @date 2024
 */

#include "control.h"
#include "main.h"
#include "UART/uart.h"

/**
 * @brief 单个电机复位示例
 */
void Motor_Reset_Single_Example(void)
{
    LOG_Print("=== Single Motor Reset Example ===\r\n");
    
    // 复位ZOOM3电机
    if (Motor_ZOOM3_Reset() == 0)
    {
        LOG_Print("ZOOM3 motor reset successful\r\n");
        LOG_Print("Current position: %ld\r\n", Motor12_GetCurrentPosition());
        LOG_Print("Target position: %ld\r\n", Motor12_GetTargetPosition());
    }
    else
    {
        LOG_Print("ZOOM3 motor reset failed\r\n");
    }
    
    // 复位FOCUS电机
    if (Motor_FOCUS_Reset() == 0)
    {
        LOG_Print("FOCUS motor reset successful\r\n");
        LOG_Print("Current position: %ld\r\n", Motor34_GetCurrentPosition());
        LOG_Print("Target position: %ld\r\n", Motor34_GetTargetPosition());
    }
    else
    {
        LOG_Print("FOCUS motor reset failed\r\n");
    }
}

/**
 * @brief 所有电机复位示例
 */
void Motor_Reset_All_Example(void)
{
    LOG_Print("=== All Motors Reset Example ===\r\n");
    
    // 复位所有电机
    if (Motors_ResetAll() == 0)
    {
        LOG_Print("All motors reset successful\r\n");
        
        // 显示所有电机的位置信息
        LOG_Print("Motor positions after reset:\r\n");
        LOG_Print("ZOOM3 (Motor12): Current=%ld, Target=%ld\r\n", 
                  Motor12_GetCurrentPosition(), Motor12_GetTargetPosition());
        LOG_Print("FOCUS (Motor34): Current=%ld, Target=%ld\r\n", 
                  Motor34_GetCurrentPosition(), Motor34_GetTargetPosition());
        LOG_Print("ZOOM2 (Motor56): Current=%ld, Target=%ld\r\n", 
                  Motor56_GetCurrentPosition(), Motor56_GetTargetPosition());
        LOG_Print("ZOOM1 (Motor78): Current=%ld, Target=%ld\r\n", 
                  Motor78_GetCurrentPosition(), Motor78_GetTargetPosition());
        LOG_Print("IRIS (Motor9A): Current=%ld, Target=%ld\r\n", 
                  Motor9A_GetCurrentPosition(), Motor9A_GetTargetPosition());
    }
    else
    {
        LOG_Print("Motors reset failed\r\n");
    }
}

/**
 * @brief 电机复位流程详细示例
 */
void Motor_Reset_Detailed_Example(void)
{
    LOG_Print("=== Detailed Motor Reset Process Example ===\r\n");
    
    // 步骤1: 检查复位前的CO状态
    LOG_Print("Step 1: Check CO status before reset\r\n");
    Motors_CheckAllCOStatus();
    
    // 步骤2: 显示复位前的位置信息
    LOG_Print("Step 2: Motor positions before reset\r\n");
    Motor12_PrintPositions();
    Motor34_PrintPositions();
    Motor56_PrintPositions();
    Motor78_PrintPositions();
    Motor9A_PrintPositions();
    
    // 步骤3: 执行复位流程
    LOG_Print("Step 3: Execute reset process\r\n");
    int reset_result = Motors_ResetAll();
    
    // 步骤4: 检查复位后的CO状态
    LOG_Print("Step 4: Check CO status after reset\r\n");
    Motors_CheckAllCOStatus();
    
    // 步骤5: 显示复位后的位置信息
    LOG_Print("Step 5: Motor positions after reset\r\n");
    Motor12_PrintPositions();
    Motor34_PrintPositions();
    Motor56_PrintPositions();
    Motor78_PrintPositions();
    Motor9A_PrintPositions();
    
    // 步骤6: 报告最终结果
    if (reset_result == 0)
    {
        LOG_Print("Motor reset process completed successfully\r\n");
    }
    else
    {
        LOG_Print("Motor reset process failed\r\n");
    }
}

/**
 * @brief 电机复位错误处理示例
 */
void Motor_Reset_Error_Handling_Example(void)
{
    LOG_Print("=== Motor Reset Error Handling Example ===\r\n");
    
    // 尝试复位每个电机，并处理可能的错误
    const char* motor_names[] = {"ZOOM3", "FOCUS", "ZOOM2", "ZOOM1", "IRIS"};
    int (*reset_functions[])(void) = {
        Motor_ZOOM3_Reset,
        Motor_FOCUS_Reset,
        Motor_ZOOM2_Reset,
        Motor_ZOOM1_Reset,
        Motor_IRIS_Reset
    };
    
    int failed_count = 0;
    
    for (int i = 0; i < 5; i++)
    {
        LOG_Print("Resetting %s motor...\r\n", motor_names[i]);
        
        int result = reset_functions[i]();
        
        if (result == 0)
        {
            LOG_Print("%s motor reset: OK\r\n", motor_names[i]);
        }
        else
        {
            LOG_Print("%s motor reset: FAILED\r\n", motor_names[i]);
            failed_count++;
            
            // 这里可以添加具体的错误处理逻辑
            // 例如：重试、报警、停止系统等
            LOG_Print("Error handling for %s motor...\r\n", motor_names[i]);
        }
    }
    
    LOG_Print("Reset summary: %d failed, %d successful\r\n", 
              failed_count, 5 - failed_count);
}

/**
 * @brief 电机复位后的位置控制示例
 */
void Motor_Reset_And_Position_Control_Example(void)
{
    LOG_Print("=== Motor Reset and Position Control Example ===\r\n");
    
    // 步骤1: 执行复位
    LOG_Print("Step 1: Reset all motors\r\n");
    if (Motors_ResetAll() != 0)
    {
        LOG_Print("Motor reset failed, aborting position control test\r\n");
        return;
    }
    
    // 步骤2: 验证复位后位置为0
    LOG_Print("Step 2: Verify reset positions\r\n");
    if (Motor12_GetCurrentPosition() != 0 || Motor34_GetCurrentPosition() != 0 ||
        Motor56_GetCurrentPosition() != 0 || Motor78_GetCurrentPosition() != 0 ||
        Motor9A_GetCurrentPosition() != 0)
    {
        LOG_Print("Warning: Some motors not at zero position after reset\r\n");
    }
    else
    {
        LOG_Print("All motors successfully reset to zero position\r\n");
    }
    
    // 步骤3: 设置新的目标位置进行测试
    LOG_Print("Step 3: Set new target positions for testing\r\n");
    Motor12_SetTargetPosition(100);
    Motor34_SetTargetPosition(200);
    Motor56_SetTargetPosition(150);
    Motor78_SetTargetPosition(300);
    Motor9A_SetTargetPosition(250);
    
    LOG_Print("New target positions set:\r\n");
    LOG_Print("ZOOM3: %ld, FOCUS: %ld, ZOOM2: %ld, ZOOM1: %ld, IRIS: %ld\r\n",
              Motor12_GetTargetPosition(), Motor34_GetTargetPosition(),
              Motor56_GetTargetPosition(), Motor78_GetTargetPosition(),
              Motor9A_GetTargetPosition());
    
    LOG_Print("Motor reset and position control example completed\r\n");
}

/**
 * @brief 主函数中调用电机复位的示例
 */
void Motor_Reset_In_Main_Example(void)
{
    LOG_Print("=== Motor Reset in Main Function Example ===\r\n");
    
    // 在main函数中，通常在系统初始化后调用
    // 这个示例展示了推荐的调用顺序
    
    LOG_Print("System startup...\r\n");
    
    // 1. 系统基本初始化（HAL_Init, 时钟配置等）已完成
    
    // 2. GPIO初始化已完成
    
    // 3. 首先执行电机初始化（如果需要）
    LOG_Print("Step 1: Motor initialization (if needed)...\r\n");
    // Motors_InitializeAll(); // 可选
    
    // 4. 执行电机复位流程
    LOG_Print("Step 2: Motor reset process...\r\n");
    
    if (Motors_ResetAll() == 0)
    {
        LOG_Print("Motor reset completed - system ready for operation\r\n");
        
        // 5. 继续其他系统初始化
        LOG_Print("Step 3: Continuing with other system initialization...\r\n");
    }
    else
    {
        LOG_Print("Motor reset failed - system may not function properly\r\n");
        
        // 错误处理：可能需要进入安全模式或重试
        LOG_Print("Entering safe mode or retry logic...\r\n");
    }
}