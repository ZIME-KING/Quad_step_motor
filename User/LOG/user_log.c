#include "user_log.h"

// 日志级别字符串数组
const char* log_level_strings[] = {
    "[DEBUG]",
    "[INFO] ",
    "[WARN] ",
    "[ERROR]",
    "[FATAL]"
};

// 全局日志配置
LogConfig_t g_log_config = {
    .level = LOG_LEVEL_INFO,
    .huart = NULL,
    .enable_timestamp = 0,
    .enable_level_tag = 1
};

// 日志缓冲区
static char log_buffer[LOG_BUFFER_SIZE];

/**
 * @brief 自动初始化函数，在main函数之前运行
 * 使用__attribute__((constructor))机制
 */
__attribute__((constructor))
void LOG_AutoInit(void)
{
    // 这里可以进行一些基本的初始化
    // 注意：此时UART可能还未初始化，所以不能发送数据
    g_log_config.level = LOG_LEVEL_INFO;
    g_log_config.huart = &huart1;
    g_log_config.enable_timestamp = 0;
    g_log_config.enable_level_tag = 1;
}

/**
 * @brief 初始化日志模块
 * @param huart UART句柄指针
 * @param level 日志级别
 */
void LOG_Init(UART_HandleTypeDef *huart, LogLevel_t level)
{
    g_log_config.huart = huart;
    g_log_config.level = level;
    g_log_config.enable_timestamp = 0;
    g_log_config.enable_level_tag = 1;
    
    // 发送初始化信息
    LOG_INFO("LOG System Initialized\r\n");
}

/**
 * @brief 设置日志级别
 * @param level 日志级别
 */
void LOG_SetLevel(LogLevel_t level)
{
    g_log_config.level = level;
}

/**
 * @brief 启用/禁用时间戳
 * @param enable 1-启用，0-禁用
 */
void LOG_EnableTimestamp(uint8_t enable)
{
    g_log_config.enable_timestamp = enable;
}

/**
 * @brief 启用/禁用级别标签
 * @param enable 1-启用，0-禁用
 */
void LOG_EnableLevelTag(uint8_t enable)
{
    g_log_config.enable_level_tag = enable;
}

/**
 * @brief 格式化日志输出
 * @param level 日志级别
 * @param format 格式化字符串
 * @param ... 可变参数
 */
void LOG_Print(LogLevel_t level, const char* format, ...)
{
    // 检查日志级别
    if (level < g_log_config.level || g_log_config.huart == NULL) {
        return;
    }
    
    va_list args;
    int offset = 0;
    
    // 清空缓冲区
    memset(log_buffer, 0, LOG_BUFFER_SIZE);
    
    // 添加级别标签
    if (g_log_config.enable_level_tag && level <= LOG_LEVEL_FATAL) {
        offset += snprintf(log_buffer + offset, LOG_BUFFER_SIZE - offset, 
                          "%s ", log_level_strings[level]);
    }
    
    // 添加时间戳（如果启用）
    if (g_log_config.enable_timestamp) {
        // 这里可以添加实际的时间戳获取代码
        // 目前使用简单的计数器或固定格式
        offset += snprintf(log_buffer + offset, LOG_BUFFER_SIZE - offset, 
                          "[%lu] ", HAL_GetTick());
    }
    
    // 添加用户格式化内容
    va_start(args, format);
    vsnprintf(log_buffer + offset, LOG_BUFFER_SIZE - offset, format, args);
    va_end(args);
    
    // 发送日志
    UART_Transmit_Str(g_log_config.huart, (uint8_t*)log_buffer);
}

/**
 * @brief 原始日志输出（无级别检查和格式化）
 * @param format 格式化字符串
 * @param ... 可变参数
 */
void LOG_PrintRaw(const char* format, ...)
{
    if (g_log_config.huart == NULL) {
        return;
    }
    
    va_list args;
    
    // 清空缓冲区
    memset(log_buffer, 0, LOG_BUFFER_SIZE);
    
    // 格式化内容
    va_start(args, format);
    vsnprintf(log_buffer, LOG_BUFFER_SIZE, format, args);
    va_end(args);
    
    // 发送日志
    UART_Transmit_Str(g_log_config.huart, (uint8_t*)log_buffer);
}
