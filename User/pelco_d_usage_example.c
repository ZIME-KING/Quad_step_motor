/**
 * @file pelco_d_usage_example.c
 * @brief PelcoD三代协议使用示例
 * @author Assistant
 * @date 2025-01-27
 * @version V1.0
 */

#include "pelco_d_protocol.h"
#include "stdio.h"
#include "string.h"

// 示例：打印函数(需要根据实际系统实现)
#ifdef DEBUG
#define PELCO_D_PRINTF(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#define PELCO_D_PRINTF(fmt, ...) // 空实现
#endif

/**
 * @brief 打印帧数据
 * @param frame_data 帧数据
 * @param frame_len 帧长度
 * @param title 标题
 */
static void PelcoD_PrintFrame(const uint8_t* frame_data, uint8_t frame_len, const char* title)
{
    PELCO_D_PRINTF("%s: ", title);
    for (int i = 0; i < frame_len; i++) {
        PELCO_D_PRINTF("0x%02X ", frame_data[i]);
    }
    PELCO_D_PRINTF("\n");
}

/**
 * @brief 打印命令详情
 * @param cmd 命令结构体
 */
static void PelcoD_PrintCommand(const PelcoD_Command_t* cmd)
{
    PELCO_D_PRINTF("命令类型: %s\n", PelcoD_GetCmdName(cmd->cmd_type));
    PELCO_D_PRINTF("地址: 0x%02X\n", cmd->addr);
    PELCO_D_PRINTF("参数1: 0x%02X\n", cmd->p1);
    PELCO_D_PRINTF("参数2: 0x%02X\n", cmd->p2);
    
    // 根据命令类型解析参数含义
    switch (cmd->cmd_type) {
        case PELCO_D_CMD_TYPE_LENS_RESET:
            PELCO_D_PRINTF("操作: 镜头软复位\n");
            break;
            
        case PELCO_D_CMD_TYPE_ZOOM_STEP:
            PELCO_D_PRINTF("操作: %s\n", (cmd->p2 == PELCO_D_ZOOM_IN) ? "放大一级" : "缩小一级");
            break;
            
        case PELCO_D_CMD_TYPE_ZOOM_SET:
            PELCO_D_PRINTF("操作: 跳转到%d倍\n", cmd->p2);
            break;
            
        case PELCO_D_CMD_TYPE_FOCUS_CTRL:
            PELCO_D_PRINTF("操作: %s%s\n", 
                (cmd->p1 == PELCO_D_FOCUS_NEAR) ? "近焦" : "远焦",
                (cmd->p2 == PELCO_D_FOCUS_SMALL_STEP) ? "小步" : "大步");
            break;
            
        case PELCO_D_CMD_TYPE_IRCUT_CTRL:
            PELCO_D_PRINTF("操作: IRCUT Ch%d %s\n", cmd->p1, 
                (cmd->p2 == PELCO_D_IRCUT_ON) ? "ON" : "OFF");
            break;
            
        case PELCO_D_CMD_TYPE_ZOOM_QUERY:
            PELCO_D_PRINTF("操作: 查询当前倍率\n");
            break;
            
        default:
            PELCO_D_PRINTF("操作: 未知\n");
            break;
    }
}

/**
 * @brief 处理单个命令示例
 * @param frame_data 命令帧数据
 * @param frame_len 帧长度
 * @param description 描述
 */
static void PelcoD_ProcessCommandExample(const uint8_t* frame_data, uint8_t frame_len, const char* description)
{
    PELCO_D_PRINTF("\n=== %s ===\n", description);
    
    // 打印接收到的帧
    PelcoD_PrintFrame(frame_data, frame_len, "接收帧");
    
    // 解析命令
    PelcoD_Command_t cmd;
    PelcoD_ParseResult_t result = PelcoD_ParseFrame(frame_data, frame_len, &cmd);
    
    PELCO_D_PRINTF("解析结果: %s\n", PelcoD_GetParseResultName(result));
    
    if (result == PELCO_D_PARSE_OK) {
        // 打印命令详情
        PelcoD_PrintCommand(&cmd);
        
        // 构建响应帧
        uint8_t response_frame[PELCO_D_FRAME_LENGTH];
        uint8_t response_len;
        
        PelcoD_ParseResult_t resp_result = PelcoD_BuildResponse(&cmd, response_frame, &response_len);
        
        if (resp_result == PELCO_D_PARSE_OK) {
            PelcoD_PrintFrame(response_frame, response_len, "响应帧");
        } else {
            PELCO_D_PRINTF("构建响应失败: %s\n", PelcoD_GetParseResultName(resp_result));
        }
    }
}

/**
 * @brief PelcoD协议解析示例主函数
 */
void PelcoD_UsageExample(void)
{
    PELCO_D_PRINTF("\n========== PelcoD三代协议解析示例 ==========\n");
    
    // 1. 镜头复位命令示例
    uint8_t lens_reset_cmd[] = {0xFF, 0x01, 0x00, 0x65, 0x00, 0x00, 0x66};
    PelcoD_ProcessCommandExample(lens_reset_cmd, sizeof(lens_reset_cmd), "镜头复位命令");
    
    // 2. 变倍+1命令示例
    uint8_t zoom_in_cmd[] = {0xFF, 0x01, 0x00, 0x67, 0x00, 0x01, 0x69};
    PelcoD_ProcessCommandExample(zoom_in_cmd, sizeof(zoom_in_cmd), "变倍+1命令");
    
    // 3. 变倍-1命令示例
    uint8_t zoom_out_cmd[] = {0xFF, 0x01, 0x00, 0x67, 0x00, 0x00, 0x68};
    PelcoD_ProcessCommandExample(zoom_out_cmd, sizeof(zoom_out_cmd), "变倍-1命令");
    
    // 4. 跳转到5倍命令示例
    uint8_t zoom_5x_cmd[] = {0xFF, 0x01, 0x00, 0x69, 0x00, 0x05, 0x6E};
    PelcoD_ProcessCommandExample(zoom_5x_cmd, sizeof(zoom_5x_cmd), "跳转到5倍命令");
    
    // 5. 远焦小步命令示例
    uint8_t focus_far_small_cmd[] = {0xFF, 0x01, 0x00, 0x6B, 0x00, 0x04, 0x70};
    PelcoD_ProcessCommandExample(focus_far_small_cmd, sizeof(focus_far_small_cmd), "远焦小步命令");
    
    // 6. 近焦大步命令示例
    uint8_t focus_near_large_cmd[] = {0xFF, 0x01, 0x00, 0x6B, 0x01, 0x20, 0x8D};
    PelcoD_ProcessCommandExample(focus_near_large_cmd, sizeof(focus_near_large_cmd), "近焦大步命令");
    
    // 7. IRCUT开启命令示例
    uint8_t ircut_on_cmd[] = {0xFF, 0x01, 0x00, 0x6D, 0x00, 0x01, 0x6E};
    PelcoD_ProcessCommandExample(ircut_on_cmd, sizeof(ircut_on_cmd), "IRCUT开启命令");
    
    // 8. 变倍查询命令示例
    uint8_t zoom_query_cmd[] = {0xFF, 0x01, 0x00, 0x6F, 0x00, 0x00, 0x70};
    PelcoD_ProcessCommandExample(zoom_query_cmd, sizeof(zoom_query_cmd), "变倍查询命令");
    
    // 9. 错误帧示例(校验和错误)
    uint8_t error_cmd[] = {0xFF, 0x01, 0x00, 0x65, 0x00, 0x00, 0x99}; // 错误的校验和
    PelcoD_ProcessCommandExample(error_cmd, sizeof(error_cmd), "错误帧示例(校验和错误)");
    
    PELCO_D_PRINTF("\n========== 示例结束 ==========\n");
}

/**
 * @brief 构建自定义命令帧示例
 */
void PelcoD_BuildCustomCommand(void)
{
    PELCO_D_PRINTF("\n=== 构建自定义命令帧示例 ===\n");
    
    // 构建跳转到10倍的命令
    uint8_t addr = PELCO_D_DEFAULT_ADDR;
    uint8_t opcode = PELCO_D_CMD_ZOOM_SET;
    uint8_t p1 = 0x00;
    uint8_t p2 = 0x0A; // 10倍
    
    uint8_t custom_frame[PELCO_D_FRAME_LENGTH];
    custom_frame[0] = PELCO_D_FRAME_HEADER;
    custom_frame[1] = addr;
    custom_frame[2] = 0x00; // 保留字节
    custom_frame[3] = opcode;
    custom_frame[4] = p1;
    custom_frame[5] = p2;
    custom_frame[6] = PelcoD_CalculateChecksum(addr, opcode, p1, p2);
    
    PELCO_D_PRINTF("构建的命令帧(跳转到10倍): ");
    for (int i = 0; i < PELCO_D_FRAME_LENGTH; i++) {
        PELCO_D_PRINTF("0x%02X ", custom_frame[i]);
    }
    PELCO_D_PRINTF("\n");
    
    // 验证构建的帧
    if (PelcoD_ValidateFrame(custom_frame, PELCO_D_FRAME_LENGTH)) {
        PELCO_D_PRINTF("帧验证: 通过\n");
    } else {
        PELCO_D_PRINTF("帧验证: 失败\n");
    }
}

/**
 * @brief UART接收处理示例(伪代码)
 * @param received_data 接收到的数据
 * @param data_len 数据长度
 */
void PelcoD_UartReceiveHandler(uint8_t* received_data, uint8_t data_len)
{
    // 检查是否为完整的PelcoD帧
    if (data_len == PELCO_D_FRAME_LENGTH && received_data[0] == PELCO_D_FRAME_HEADER) {
        PelcoD_Command_t cmd;
        PelcoD_ParseResult_t result = PelcoD_ParseFrame(received_data, data_len, &cmd);
        
        if (result == PELCO_D_PARSE_OK) {
            // 根据命令类型执行相应操作
            switch (cmd.cmd_type) {
                case PELCO_D_CMD_TYPE_LENS_RESET:
                    // 执行镜头复位操作
                    // LensReset();
                    break;
                    
                case PELCO_D_CMD_TYPE_ZOOM_STEP:
                    // 执行变倍步进操作
                    if (cmd.p2 == PELCO_D_ZOOM_IN) {
                        // ZoomIn();
                    } else {
                        // ZoomOut();
                    }
                    break;
                    
                case PELCO_D_CMD_TYPE_ZOOM_SET:
                    // 执行变倍设置操作
                    // SetZoomLevel(cmd.p2);
                    break;
                    
                case PELCO_D_CMD_TYPE_FOCUS_CTRL:
                    // 执行聚焦控制操作
                    // FocusControl(cmd.p1, cmd.p2);
                    break;
                    
                case PELCO_D_CMD_TYPE_IRCUT_CTRL:
                    // 执行IR-CUT控制操作
                    // IrcutControl(cmd.p1, cmd.p2);
                    break;
                    
                case PELCO_D_CMD_TYPE_ZOOM_QUERY:
                    // 处理变倍查询
                    // uint8_t current_zoom = GetCurrentZoomLevel();
                    break;
                    
                default:
                    break;
            }
            
            // 构建并发送响应帧
            uint8_t response_frame[PELCO_D_FRAME_LENGTH];
            uint8_t response_len;
            
            if (PelcoD_BuildResponse(&cmd, response_frame, &response_len) == PELCO_D_PARSE_OK) {
                // 发送响应帧
                // UartSendData(response_frame, response_len);
            }
        }
    }
}