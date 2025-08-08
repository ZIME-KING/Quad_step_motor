/**
 * @file CO_Signal_Check_Example.c
 * @brief CO信号检查示例代码
 * @author AI Assistant
 * @date 2024
 * 
 * 本文件提供了检查所有电机CO信号状态的示例代码
 * CO信号为低电平时表示电机已到位，高电平时表示电机未到位
 */

#include "main.h"
#include "control.h"
#include "user_log.h"

/**
 * @brief CO信号检查示例 - 检查所有电机
 * 该函数演示如何检查所有电机的CO信号状态
 */
void CO_Signal_Check_All_Example(void)
{
    LOG_Print(LOG_LEVEL_INFO, "\r\n开始检查所有电机CO信号状态...\r\n");
    
    // 检查所有电机的CO信号状态
    bool all_motors_in_position = Motors_CheckAllCOStatus();
    
    if (all_motors_in_position) {
        LOG_Print(LOG_LEVEL_INFO, "✓ 所有电机都已到位！\r\n");
    } else {
        LOG_Print(LOG_LEVEL_WARNING, "⚠ 部分电机未到位，请检查！\r\n");
    }
}

/**
 * @brief CO信号检查示例 - 检查单个电机
 * 该函数演示如何检查单个电机的CO信号状态
 */
void CO_Signal_Check_Individual_Example(void)
{
    LOG_Print(LOG_LEVEL_INFO, "\r\n开始逐个检查电机CO信号状态...\r\n");
    
    // 逐个检查每个电机的CO信号状态
    bool zoom1_ok = Motor_CheckCOStatus("ZOOM1", ZOOM1_PI_CO_GPIO_Port, ZOOM1_PI_CO_Pin);
    bool zoom3_ok = Motor_CheckCOStatus("ZOOM3", ZOOM3_PI_CO_GPIO_Port, ZOOM3_PI_CO_Pin);
    bool iris_ok = Motor_CheckCOStatus("IRIS", IRIS_PI_CO_GPIO_Port, IRIS_PI_CO_Pin);
    bool zoom2_ok = Motor_CheckCOStatus("ZOOM2", ZOOM2_PI_CO_GPIO_Port, ZOOM2_PI_CO_Pin);
    bool focus_ok = Motor_CheckCOStatus("FOCUS", FOCUS_PI_CO_GPIO_Port, FOCUS_PI_CO_Pin);
    
    // 统计结果
    int motors_in_position = zoom1_ok + zoom3_ok + iris_ok + zoom2_ok + focus_ok;
    LOG_Print(LOG_LEVEL_INFO, "\r\n检查结果: %d/5 个电机已到位\r\n", motors_in_position);
    
    if (motors_in_position == 5) {
        LOG_Print(LOG_LEVEL_INFO, "✓ 所有电机都已到位！\r\n");
    } else {
        LOG_Print(LOG_LEVEL_WARNING, "⚠ %d 个电机未到位\r\n", 5 - motors_in_position);
        
        // 列出未到位的电机
        LOG_Print(LOG_LEVEL_INFO, "未到位的电机: ");
        if (!zoom1_ok) LOG_Print(LOG_LEVEL_INFO, "ZOOM1 ");
        if (!zoom3_ok) LOG_Print(LOG_LEVEL_INFO, "ZOOM3 ");
        if (!iris_ok) LOG_Print(LOG_LEVEL_INFO, "IRIS ");
        if (!zoom2_ok) LOG_Print(LOG_LEVEL_INFO, "ZOOM2 ");
        if (!focus_ok) LOG_Print(LOG_LEVEL_INFO, "FOCUS ");
        LOG_Print(LOG_LEVEL_INFO, "\r\n");
    }
}

/**
 * @brief CO信号监控示例 - 持续监控
 * 该函数演示如何持续监控CO信号状态的变化
 */
void CO_Signal_Monitor_Example(void)
{
    static uint32_t last_check_time = 0;
    static bool last_all_status = false;
    
    uint32_t current_time = HAL_GetTick();
    
    // 每1秒检查一次
    if (current_time - last_check_time >= 1000) {
        last_check_time = current_time;
        
        bool current_all_status = Motors_CheckAllCOStatus();
        
        // 检测状态变化
        if (current_all_status != last_all_status) {
            if (current_all_status) {
                LOG_Print(LOG_LEVEL_INFO, "🎉 状态变化: 所有电机现在都已到位！\r\n");
            } else {
                LOG_Print(LOG_LEVEL_WARNING, "⚠ 状态变化: 检测到电机离开到位状态！\r\n");
            }
            last_all_status = current_all_status;
        }
    }
}

/**
 * @brief CO信号检查完整示例
 * 该函数提供了一个完整的CO信号检查流程示例
 */
void CO_Signal_Complete_Example(void)
{
    LOG_Print(LOG_LEVEL_INFO, "\r\n========== CO信号检查完整示例 ==========\r\n");
    
    // 1. 检查所有电机状态
    LOG_Print(LOG_LEVEL_INFO, "\r\n步骤1: 检查所有电机CO信号状态\r\n");
    CO_Signal_Check_All_Example();
    
    // 2. 逐个检查电机状态
    LOG_Print(LOG_LEVEL_INFO, "\r\n步骤2: 逐个检查电机CO信号状态\r\n");
    CO_Signal_Check_Individual_Example();
    
    LOG_Print(LOG_LEVEL_INFO, "\r\n========== CO信号检查示例完成 ==========\r\n");
}

/**
 * @brief 在主循环中调用CO信号检查的示例
 * 该函数演示如何在主循环中集成CO信号检查
 */
void CO_Signal_Main_Loop_Example(void)
{
    static uint32_t co_check_timer = 0;
    uint32_t current_time = HAL_GetTick();
    
    // 每5秒检查一次CO信号状态
    if (current_time - co_check_timer >= 5000) {
        co_check_timer = current_time;
        
        LOG_Print(LOG_LEVEL_INFO, "\r\n[定时检查] 检查所有电机CO信号状态...\r\n");
        bool all_ok = Motors_CheckAllCOStatus();
        
        if (all_ok) {
            LOG_Print(LOG_LEVEL_INFO, "[定时检查] ✓ 所有电机正常到位\r\n");
        } else {
            LOG_Print(LOG_LEVEL_WARNING, "[定时检查] ⚠ 发现电机未到位，请检查系统状态\r\n");
        }
    }
}