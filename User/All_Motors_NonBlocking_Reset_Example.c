/**
 * @file All_Motors_NonBlocking_Reset_Example.c
 * @brief 所有电机非阻塞复位功能使用示例
 * @author Assistant
 * @date 2024
 */

#include "control.h"
#include "main.h"

// 电机复位管理结构体
typedef struct {
    uint8_t reset_requested;
    uint8_t reset_completed;
    const char* motor_name;
    int (*reset_start)(void);
    MotorResetState_t (*reset_process)(void);
    MotorResetState_t (*reset_get_state)(void);
    void (*reset_clear)(void);
} MotorResetManager_t;

// 所有电机的复位管理器 (按照新的映射关系: Motor12→ZOOM1, Motor34→ZOOM3, Motor56→IRIS, Motor78→ZOOM2, Motor9A→FOCUS)
static MotorResetManager_t motor_managers[5] = {
    {
        .reset_requested = 0,
        .reset_completed = 0,
        .motor_name = "ZOOM1",  // Motor12控制
        .reset_start = Motor_ZOOM1_Reset_Start,
        .reset_process = Motor_ZOOM1_Reset_Process,
        .reset_get_state = Motor_ZOOM1_Reset_GetState,
        .reset_clear = Motor_ZOOM1_Reset_Clear
    },
    {
        .reset_requested = 0,
        .reset_completed = 0,
        .motor_name = "ZOOM3",  // Motor34控制
        .reset_start = Motor_ZOOM3_Reset_Start,
        .reset_process = Motor_ZOOM3_Reset_Process,
        .reset_get_state = Motor_ZOOM3_Reset_GetState,
        .reset_clear = Motor_ZOOM3_Reset_Clear
    },
    {
        .reset_requested = 0,
        .reset_completed = 0,
        .motor_name = "IRIS",   // Motor56控制
        .reset_start = Motor_IRIS_Reset_Start,
        .reset_process = Motor_IRIS_Reset_Process,
        .reset_get_state = Motor_IRIS_Reset_GetState,
        .reset_clear = Motor_IRIS_Reset_Clear
    },
    {
        .reset_requested = 0,
        .reset_completed = 0,
        .motor_name = "ZOOM2",  // Motor78控制
        .reset_start = Motor_ZOOM2_Reset_Start,
        .reset_process = Motor_ZOOM2_Reset_Process,
        .reset_get_state = Motor_ZOOM2_Reset_GetState,
        .reset_clear = Motor_ZOOM2_Reset_Clear
    },
    {
        .reset_requested = 0,
        .reset_completed = 0,
        .motor_name = "FOCUS",  // Motor9A控制
        .reset_start = Motor_FOCUS_Reset_Start,
        .reset_process = Motor_FOCUS_Reset_Process,
        .reset_get_state = Motor_FOCUS_Reset_GetState,
        .reset_clear = Motor_FOCUS_Reset_Clear
    }
};

/**
 * @brief 启动所有电机的复位流程（非阻塞）
 * @return 成功启动的电机数量
 */
int Motors_StartAllReset_NonBlocking(void)
{
    int started_count = 0;
    
    LOG_Print("Starting all motors reset process (non-blocking)...\r\n");
    
    for (int i = 0; i < 5; i++)
    {
        if (!motor_managers[i].reset_requested)
        {
            if (motor_managers[i].reset_start() == 0)
            {
                motor_managers[i].reset_requested = 1;
                motor_managers[i].reset_completed = 0;
                started_count++;
                LOG_Print("%s motor reset started\r\n", motor_managers[i].motor_name);
            }
            else
            {
                LOG_Print("%s motor reset failed to start\r\n", motor_managers[i].motor_name);
            }
        }
    }
    
    LOG_Print("Total %d motors reset started\r\n", started_count);
    return started_count;
}

/**
 * @brief 处理所有电机的复位状态（非阻塞）
 * 需要在主循环中定期调用
 * @return 已完成复位的电机数量
 */
int Motors_ProcessAllReset_NonBlocking(void)
{
    int completed_count = 0;
    int active_count = 0;
    
    for (int i = 0; i < 5; i++)
    {
        if (motor_managers[i].reset_requested && !motor_managers[i].reset_completed)
        {
            MotorResetState_t state = motor_managers[i].reset_process();
            active_count++;
            
            switch (state)
            {
                case MOTOR_RESET_COMPLETED:
                    LOG_Print("%s motor reset completed successfully\r\n", motor_managers[i].motor_name);
                    motor_managers[i].reset_completed = 1;
                    motor_managers[i].reset_clear();
                    completed_count++;
                    break;
                    
                case MOTOR_RESET_TIMEOUT:
                    LOG_Print("%s motor reset timeout!\r\n", motor_managers[i].motor_name);
                    motor_managers[i].reset_completed = 1;
                    motor_managers[i].reset_clear();
                    completed_count++;
                    break;
                    
                case MOTOR_RESET_MOVING:
                    // 复位进行中，继续等待
                    break;
                    
                default:
                    break;
            }
        }
    }
    
    // 如果有活动的复位进程，定期打印状态
    static uint32_t last_status_print = 0;
    if (active_count > 0 && (HAL_GetTick() - last_status_print) > 2000)
    {
        LOG_Print("Reset status: %d motors still resetting...\r\n", active_count - completed_count);
        last_status_print = HAL_GetTick();
    }
    
    return completed_count;
}

/**
 * @brief 检查所有电机复位是否完成
 * @return 1: 所有复位完成, 0: 还有电机在复位中
 */
int Motors_IsAllResetCompleted(void)
{
    for (int i = 0; i < 5; i++)
    {
        if (motor_managers[i].reset_requested && !motor_managers[i].reset_completed)
        {
            return 0; // 还有电机在复位中
        }
    }
    return 1; // 所有复位完成
}

/**
 * @brief 重置所有电机的复位状态
 */
void Motors_ClearAllResetStatus(void)
{
    for (int i = 0; i < 5; i++)
    {
        motor_managers[i].reset_requested = 0;
        motor_managers[i].reset_completed = 0;
        motor_managers[i].reset_clear();
    }
    LOG_Print("All motor reset status cleared\r\n");
}

/**
 * @brief 获取所有电机的复位状态报告
 */
void Motors_PrintResetStatus(void)
{
    LOG_Print("=== Motor Reset Status Report ===\r\n");
    
    for (int i = 0; i < 5; i++)
    {
        MotorResetState_t state = motor_managers[i].reset_get_state();
        const char* state_str;
        
        switch (state)
        {
            case MOTOR_RESET_IDLE:
                state_str = "IDLE";
                break;
            case MOTOR_RESET_MOVING:
                state_str = "MOVING";
                break;
            case MOTOR_RESET_COMPLETED:
                state_str = "COMPLETED";
                break;
            case MOTOR_RESET_TIMEOUT:
                state_str = "TIMEOUT";
                break;
            default:
                state_str = "UNKNOWN";
                break;
        }
        
        LOG_Print("%s: %s (Req:%d, Done:%d)\r\n", 
                  motor_managers[i].motor_name, 
                  state_str,
                  motor_managers[i].reset_requested,
                  motor_managers[i].reset_completed);
    }
    
    LOG_Print("================================\r\n");
}

/**
 * @brief 主循环中使用所有电机非阻塞复位的示例
 */
void main_loop_all_motors_reset_example(void)
{
    static uint8_t all_reset_started = 0;
    static uint32_t reset_start_time = 0;
    
    // 示例：启动所有电机复位（只需调用一次）
    if (!all_reset_started)
    {
        // 这里可以根据实际需求触发复位，例如按键、命令等
        if (/* 触发条件 */ 0) // 替换为实际的触发条件
        {
            Motors_StartAllReset_NonBlocking();
            all_reset_started = 1;
            reset_start_time = HAL_GetTick();
        }
    }
    
    // 处理复位状态
    if (all_reset_started)
    {
        Motors_ProcessAllReset_NonBlocking();
        
        // 检查是否所有复位完成
        if (Motors_IsAllResetCompleted())
        {
            uint32_t total_time = HAL_GetTick() - reset_start_time;
            LOG_Print("All motors reset completed in %lu ms\r\n", total_time);
            Motors_PrintResetStatus();
            Motors_ClearAllResetStatus();
            all_reset_started = 0;
        }
        
        // 超时保护（例如30秒）
        if ((HAL_GetTick() - reset_start_time) > 30000)
        {
            LOG_Print("All motors reset process timeout!\r\n");
            Motors_PrintResetStatus();
            Motors_ClearAllResetStatus();
            all_reset_started = 0;
        }
    }
    
    // 其他主循环任务...
}

/**
 * @brief 单独控制特定电机复位的示例
 */
void individual_motor_reset_example(void)
{
    // 示例：只复位ZOOM3和FOCUS电机
    static uint8_t zoom3_started = 0, focus_started = 0;
    
    // 启动ZOOM3复位
    if (!zoom3_started && /* 某个条件 */ 0)
    {
        if (Motor_ZOOM3_Reset_Start() == 0)
        {
            zoom3_started = 1;
            LOG_Print("ZOOM3 reset started\r\n");
        }
    }
    
    // 启动FOCUS复位
    if (!focus_started && /* 某个条件 */ 0)
    {
        if (Motor_FOCUS_Reset_Start() == 0)
        {
            focus_started = 1;
            LOG_Print("FOCUS reset started\r\n");
        }
    }
    
    // 处理ZOOM3复位状态
    if (zoom3_started)
    {
        MotorResetState_t state = Motor_ZOOM3_Reset_Process();
        if (state == MOTOR_RESET_COMPLETED || state == MOTOR_RESET_TIMEOUT)
        {
            LOG_Print("ZOOM3 reset finished with state: %d\r\n", state);
            Motor_ZOOM3_Reset_Clear();
            zoom3_started = 0;
        }
    }
    
    // 处理FOCUS复位状态
    if (focus_started)
    {
        MotorResetState_t state = Motor_FOCUS_Reset_Process();
        if (state == MOTOR_RESET_COMPLETED || state == MOTOR_RESET_TIMEOUT)
        {
            LOG_Print("FOCUS reset finished with state: %d\r\n", state);
            Motor_FOCUS_Reset_Clear();
            focus_started = 0;
        }
    }
}

/**
 * @brief 完整的主函数示例
 */
void main_function_all_motors_example(void)
{
    // 系统初始化
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    // ... 其他初始化
    
    LOG_Print("System initialized - All Motors Non-blocking Reset Example\r\n");
    
    // 主循环
    while (1)
    {
        // 处理所有电机的非阻塞复位
        main_loop_all_motors_reset_example();
        
        // 或者使用单独控制
        // individual_motor_reset_example();
        
        // 其他主循环任务
        // 例如：通信处理、传感器读取、其他控制逻辑等
        
        // 短暂延时
        HAL_Delay(10);
    }
}