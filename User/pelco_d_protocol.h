/**
 * @file pelco_d_protocol.h
 * @brief PelcoD三代协议解析头文件
 * @author Assistant
 * @date 2025-01-27
 * @version V1.0
 */

#ifndef __PELCO_D_PROTOCOL_H
#define __PELCO_D_PROTOCOL_H

#include "stdint.h"
#include "stdbool.h"

// 协议常量定义
#define PELCO_D_FRAME_HEADER    0xFF    // 帧头
#define PELCO_D_DEFAULT_ADDR    0x01    // 默认地址
#define PELCO_D_FRAME_LENGTH    7       // 帧长度
#define PELCO_D_BAUD_RATE       9600    // 波特率

// 命令码定义
#define PELCO_D_CMD_LENS_RESET      0x65    // 镜头复位
#define PELCO_D_CMD_ZOOM_STEP       0x67    // 变倍步进
#define PELCO_D_CMD_ZOOM_SET        0x69    // 设置变倍
#define PELCO_D_CMD_FOCUS_CTRL      0x6B    // 聚焦控制
#define PELCO_D_CMD_IRCUT_CTRL      0x6D    // IR-CUT控制
#define PELCO_D_CMD_ZOOM_QUERY      0x6F    // 变倍查询

// 变倍参数定义
#define PELCO_D_ZOOM_OUT            0x00    // 缩小
#define PELCO_D_ZOOM_IN             0x01    // 放大
#define PELCO_D_ZOOM_MIN            0x01    // 最小倍率
#define PELCO_D_ZOOM_MAX            0x10    // 最大倍率(16倍)

// 聚焦参数定义
#define PELCO_D_FOCUS_FAR           0x00    // 远焦
#define PELCO_D_FOCUS_NEAR          0x01    // 近焦
#define PELCO_D_FOCUS_SMALL_STEP    0x04    // 小步(4步)
#define PELCO_D_FOCUS_LARGE_STEP    0x20    // 大步(32步)

// IR-CUT参数定义
#define PELCO_D_IRCUT_OFF           0x00    // 关闭
#define PELCO_D_IRCUT_ON            0x01    // 打开
#define PELCO_D_IRCUT_CH0           0x00    // 通道0

// 帧结构体定义
typedef struct {
    uint8_t header;     // 帧头 0xFF
    uint8_t addr;       // 地址
    uint8_t reserved;   // 保留字节 0x00
    uint8_t opcode;     // 操作码
    uint8_t p1;         // 参数1
    uint8_t p2;         // 参数2
    uint8_t checksum;   // 校验和
} PelcoD_Frame_t;

// 解析结果枚举
typedef enum {
    PELCO_D_PARSE_OK = 0,           // 解析成功
    PELCO_D_PARSE_ERROR_HEADER,     // 帧头错误
    PELCO_D_PARSE_ERROR_ADDR,       // 地址错误
    PELCO_D_PARSE_ERROR_CHECKSUM,   // 校验和错误
    PELCO_D_PARSE_ERROR_LENGTH,     // 帧长度错误
    PELCO_D_PARSE_ERROR_UNKNOWN_CMD // 未知命令
} PelcoD_ParseResult_t;

// 命令类型枚举
typedef enum {
    PELCO_D_CMD_TYPE_LENS_RESET = 0,    // 镜头复位
    PELCO_D_CMD_TYPE_ZOOM_STEP,         // 变倍步进
    PELCO_D_CMD_TYPE_ZOOM_SET,          // 设置变倍
    PELCO_D_CMD_TYPE_FOCUS_CTRL,        // 聚焦控制
    PELCO_D_CMD_TYPE_IRCUT_CTRL,        // IR-CUT控制
    PELCO_D_CMD_TYPE_ZOOM_QUERY,        // 变倍查询
    PELCO_D_CMD_TYPE_UNKNOWN            // 未知命令
} PelcoD_CmdType_t;

// 解析后的命令结构体
typedef struct {
    PelcoD_CmdType_t cmd_type;      // 命令类型
    uint8_t addr;                   // 地址
    uint8_t p1;                     // 参数1
    uint8_t p2;                     // 参数2
} PelcoD_Command_t;

// 函数声明
PelcoD_ParseResult_t PelcoD_ParseFrame( uint8_t* frame_data, uint8_t frame_len, PelcoD_Command_t* cmd);
PelcoD_ParseResult_t PelcoD_BuildResponse(const PelcoD_Command_t* cmd, uint8_t* response_frame, uint8_t* frame_len);
uint8_t PelcoD_CalculateChecksum(uint8_t *p, uint8_t len);
bool PelcoD_ValidateFrame(uint8_t* frame_data, uint8_t frame_len);
const char* PelcoD_GetCmdName(PelcoD_CmdType_t cmd_type);
const char* PelcoD_GetParseResultName(PelcoD_ParseResult_t result);

#endif /* __PELCO_D_PROTOCOL_H */

