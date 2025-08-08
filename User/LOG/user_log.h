#ifndef __USER_LOG_H
#define __USER_LOG_H

#include "main.h"
#include "uart.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

// 日志级别定义
typedef enum {
    LOG_LEVEL_DEBUG = 0,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_FATAL
} LogLevel_t;

// 日志配置结构体
typedef struct {
    LogLevel_t level;           // 当前日志级别
    UART_HandleTypeDef *huart;  // 使用的UART句柄
    uint8_t enable_timestamp;   // 是否启用时间戳
    uint8_t enable_level_tag;   // 是否启用级别标签
} LogConfig_t;

// 日志缓冲区大小
#define LOG_BUFFER_SIZE 256

// 日志级别字符串
extern const char* log_level_strings[];

// 全局日志配置
extern LogConfig_t g_log_config;

// 函数声明
void LOG_AutoInit(void);  // 自动初始化函数（在main之前运行）
void LOG_Init(UART_HandleTypeDef *huart, LogLevel_t level);
void LOG_SetLevel(LogLevel_t level);
void LOG_EnableTimestamp(uint8_t enable);
void LOG_EnableLevelTag(uint8_t enable);
void LOG_Print(LogLevel_t level, const char* format, ...);
void LOG_PrintRaw(const char* format, ...);

// 便捷宏定义
#define LOG_DEBUG(format, ...) LOG_Print(LOG_LEVEL_DEBUG, format, ##__VA_ARGS__)
#define LOG_INFO(format, ...)  LOG_Print(LOG_LEVEL_INFO, format, ##__VA_ARGS__)
#define LOG_WARN(format, ...)  LOG_Print(LOG_LEVEL_WARN, format, ##__VA_ARGS__)
#define LOG_ERROR(format, ...) LOG_Print(LOG_LEVEL_ERROR, format, ##__VA_ARGS__)
#define LOG_FATAL(format, ...) LOG_Print(LOG_LEVEL_FATAL, format, ##__VA_ARGS__)

// 原始输出宏（无格式化）
#define LOG_RAW(format, ...) LOG_PrintRaw(format, ##__VA_ARGS__)

#endif
