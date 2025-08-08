/**
 * @file pelco_d_protocol.c
 * @brief PelcoD三代协议解析实现文件
 * @author Assistant
 * @date 2025-01-27
 * @version V1.0
 */

#include "pelco_d_protocol.h"
#include "string.h"

/**
 * @brief 计算PelcoD协议校验和
 * @param addr 地址
 * @param opcode 操作码
 * @param p1 参数1
 * @param p2 参数2
 * @return 校验和
 */
uint8_t PelcoD_CalculateChecksum(uint8_t *p, uint8_t len)
{
	
		uint8_t fsk=0;
		while(len--){
			fsk+=*p;
			p++;
		}
    return fsk & 0xFF;
}

/**
 * @brief 验证PelcoD帧的有效性
 * @param frame_data 帧数据
 * @param frame_len 帧长度
 * @return true-有效, false-无效
 */
bool PelcoD_ValidateFrame(uint8_t* frame_data, uint8_t frame_len)
{
    if (frame_data == NULL || frame_len != PELCO_D_FRAME_LENGTH) {
        return false;
    }
    
    // 检查帧头
    if (frame_data[0] != PELCO_D_FRAME_HEADER) {
        return false;
    }
    
    // 检查保留字节
    if (frame_data[2] != 0x00) {
        return false;
    }
    
    // 检查校验和   
		uint8_t calculated_checksum = PelcoD_CalculateChecksum(frame_data+1,4);
    
    return (calculated_checksum == frame_data[6]);
}

/**
 * @brief 获取命令类型
 * @param opcode 操作码
 * @return 命令类型
 */
static PelcoD_CmdType_t PelcoD_GetCmdType(uint8_t opcode)
{
    switch (opcode) {
        case PELCO_D_CMD_LENS_RESET:
            return PELCO_D_CMD_TYPE_LENS_RESET;
        case PELCO_D_CMD_ZOOM_STEP:
            return PELCO_D_CMD_TYPE_ZOOM_STEP;
        case PELCO_D_CMD_ZOOM_SET:
            return PELCO_D_CMD_TYPE_ZOOM_SET;
        case PELCO_D_CMD_FOCUS_CTRL:
            return PELCO_D_CMD_TYPE_FOCUS_CTRL;
        case PELCO_D_CMD_IRCUT_CTRL:
            return PELCO_D_CMD_TYPE_IRCUT_CTRL;
        case PELCO_D_CMD_ZOOM_QUERY:
            return PELCO_D_CMD_TYPE_ZOOM_QUERY;
        default:
            return PELCO_D_CMD_TYPE_UNKNOWN;
    }
}

/**
 * @brief 解析PelcoD协议帧
 * @param frame_data 帧数据
 * @param frame_len 帧长度
 * @param cmd 解析后的命令结构体
 * @return 解析结果
 */
PelcoD_ParseResult_t PelcoD_ParseFrame( uint8_t* frame_data, uint8_t frame_len, PelcoD_Command_t* cmd)
{
    if (frame_data == NULL || cmd == NULL) {
        return PELCO_D_PARSE_ERROR_LENGTH;
    }
    
    // 检查帧长度
    if (frame_len != PELCO_D_FRAME_LENGTH) {
        return PELCO_D_PARSE_ERROR_LENGTH;
    }
    
    // 检查帧头
    if (frame_data[0] != PELCO_D_FRAME_HEADER) {
        return PELCO_D_PARSE_ERROR_HEADER;
    }
    
    // 检查地址(可选，这里只检查是否为默认地址)
    if (frame_data[1] != PELCO_D_DEFAULT_ADDR) {
        return PELCO_D_PARSE_ERROR_ADDR;
    }
    
    // 检查校验和
    uint8_t calculated_checksum = PelcoD_CalculateChecksum(&frame_data[1], 5);
    
    if (calculated_checksum != frame_data[6]) {
        return PELCO_D_PARSE_ERROR_CHECKSUM;
    }
    
    // 解析命令
    cmd->addr = frame_data[1];
    cmd->cmd_type = PelcoD_GetCmdType(frame_data[3]);
    cmd->p1 = frame_data[4];
    cmd->p2 = frame_data[5];
    
    // 检查是否为未知命令
    if (cmd->cmd_type == PELCO_D_CMD_TYPE_UNKNOWN) {
        return PELCO_D_PARSE_ERROR_UNKNOWN_CMD;
    }
    
    return PELCO_D_PARSE_OK;
}

/**
 * @brief 构建PelcoD协议响应帧
 * @param cmd 命令结构体
 * @param response_frame 响应帧缓冲区
 * @param frame_len 帧长度(输出)
 * @return 构建结果
 */
PelcoD_ParseResult_t PelcoD_BuildResponse(const PelcoD_Command_t* cmd, uint8_t* response_frame, uint8_t* frame_len)
{
    if (cmd == NULL || response_frame == NULL || frame_len == NULL) {
        return PELCO_D_PARSE_ERROR_LENGTH;
    }
    
    uint8_t opcode;
    uint8_t resp_p1 = 0x00;
    uint8_t resp_p2 = 0x00;
    
    // 根据命令类型设置响应参数
    switch (cmd->cmd_type) {
        case PELCO_D_CMD_TYPE_LENS_RESET:
            opcode = PELCO_D_CMD_LENS_RESET;
            break;
            
        case PELCO_D_CMD_TYPE_ZOOM_STEP:
            opcode = PELCO_D_CMD_ZOOM_STEP;
            break;
            
        case PELCO_D_CMD_TYPE_ZOOM_SET:
            opcode = PELCO_D_CMD_ZOOM_SET;
            break;
            
        case PELCO_D_CMD_TYPE_FOCUS_CTRL:
            opcode = PELCO_D_CMD_FOCUS_CTRL;
            break;
            
        case PELCO_D_CMD_TYPE_IRCUT_CTRL:
            opcode = PELCO_D_CMD_IRCUT_CTRL;
            // IR-CUT响应中P2返回当前状态
            resp_p2 = cmd->p2;
            break;
            
        case PELCO_D_CMD_TYPE_ZOOM_QUERY:
            opcode = PELCO_D_CMD_ZOOM_QUERY;
            // 变倍查询响应中P2返回当前倍率(这里需要实际获取当前倍率)
            //resp_p2 = 0x01; // 示例：返回1倍，实际应用中需要获取真实倍率
            break;
            
        default:
            return PELCO_D_PARSE_ERROR_UNKNOWN_CMD;
    }
    
    // 构建响应帧
    response_frame[0] = PELCO_D_FRAME_HEADER;   // 帧头
    response_frame[1] = 0x00;                   // 响应地址固定为0x00
    response_frame[2] = opcode;                 // 操作码
    response_frame[3] = resp_p1;                // 参数1
    response_frame[4] = resp_p2;                // 参数2
    response_frame[5] = 0x00;                   // 保留字节
    response_frame[6] = PelcoD_CalculateChecksum(&response_frame[1], 5); // 校验和
    
    *frame_len = PELCO_D_FRAME_LENGTH;
    
    return PELCO_D_PARSE_OK;
}

/**
 * @brief 获取命令名称字符串
 * @param cmd_type 命令类型
 * @return 命令名称
 */
const char* PelcoD_GetCmdName(PelcoD_CmdType_t cmd_type)
{
//    switch (cmd_type) {
//        case PELCO_D_CMD_TYPE_LENS_RESET:
//            return "镜头复位";
//        case PELCO_D_CMD_TYPE_ZOOM_STEP:
//            return "变倍步进";
//        case PELCO_D_CMD_TYPE_ZOOM_SET:
//            return "设置变倍";
//        case PELCO_D_CMD_TYPE_FOCUS_CTRL:
//            return "聚焦控制";
//        case PELCO_D_CMD_TYPE_IRCUT_CTRL:
//            return "IR-CUT控制";
//        case PELCO_D_CMD_TYPE_ZOOM_QUERY:
//            return "变倍查询";
//        default:
//            return "未知命令";
//    }
}

/**
 * @brief 获取解析结果名称字符串
 * @param result 解析结果
 * @return 结果名称
 */
const char* PelcoD_GetParseResultName(PelcoD_ParseResult_t result)
{
//    switch (result) {
//        case PELCO_D_PARSE_OK:
//            return "解析成功";
//        case PELCO_D_PARSE_ERROR_HEADER:
//            return "帧头错误";
//        case PELCO_D_PARSE_ERROR_ADDR:
//            return "地址错误";
//        case PELCO_D_PARSE_ERROR_CHECKSUM:
//            return "校验和错误";
//        case PELCO_D_PARSE_ERROR_LENGTH:
//            return "帧长度错误";
//        case PELCO_D_PARSE_ERROR_UNKNOWN_CMD:
//            return "未知命令";
//        default:
//            return "未知错误";
//    }
}