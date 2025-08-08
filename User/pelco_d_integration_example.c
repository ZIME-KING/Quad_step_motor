/**
 * @file pelco_d_integration_example.c
 * @brief PelcoD协议与电机控制系统集成示例
 * @author Assistant
 * @date 2025-01-27
 * @version V1.0
 */

#include "pelco_d_protocol.h"
#include "control.h"
#include "user_function.h"
#include "main.h"


//Z1-Step	Z2-Step	Z3-Step
int16_t x1_x16_cide[16][4]=
{
{	755	,	227	,	216	,	100	}	,
{	393	,	288	,	192	,	100	}	,
{	108	,	247	,	173	,	100	}	,
{	-108	,	160	,	50	,	100	}	,
{	-310	,	-13	,	-96	,	100	}	,
{	-485	,	-238	,	-265	,	100	}	,
{	-596	,	-381	,	-408	,	100	}	,
{	-664	,	-444	,	-516	,	100	}	,
{	-710	,	-470	,	-601	,	100	}	,
{	-740	,	-453	,	-662	,	100	}	,
{	-759	,	-398	,	-703	,	100	}	,
{	-771	,	-331	,	-730	,	100	}	,
{	-780	,	-258	,	-751	,	100	}	,
{	-787	,	-185	,	-768	,	100	}	,
{	-793 	,	-108	,	-782	,	100	}	,
{	-800 	,	0			,	-800	,	100	}	,


};






// 当前镜头状态
typedef struct {
    uint8_t current_zoom_level;     // 当前变倍级别 (1-16)
    uint8_t ircut_status;           // IR-CUT状态 (0-关闭, 1-开启)
    uint8_t lens_reset_status;      // 镜头复位状态
} LensStatus_t;

static LensStatus_t lens_status = {
    .current_zoom_level = 1,
    .ircut_status = 0,
    .lens_reset_status = 0
};

/**
 * @brief 执行镜头复位操作
 * @return 0-成功, 其他-失败
 */
static int PelcoD_ExecuteLensReset(void)
{
    // 启动所有电机的复位
    int result = 0;
    
    // 启动ZOOM1复位
    if (Motor_ZOOM1_Reset_Start() != 0) {
        result = -1;
    }
//    
    // 启动ZOOM2复位
    if (Motor_ZOOM2_Reset_Start() != 0) {
        result = -1;
    }
    
    // 启动ZOOM3复位
    if (Motor_ZOOM3_Reset_Start() != 0) {
        result = -1;
    }
    
    // 启动FOCUS复位
    if (Motor_FOCUS_Reset_Start() != 0) {
        result = -1;
    }
//    
    // 启动IRIS复位
    if (Motor_IRIS_Reset_Start() != 0) {
        result = -1;
    }
    
    if (result == 0) {
        lens_status.lens_reset_status = 1;
        lens_status.current_zoom_level = 1; // 复位后回到1倍
    }
    
    return result;
}

/**
 * @brief 执行变倍步进操作
 * @param direction 方向 (0-缩小, 1-放大)
 * @return 0-成功, 其他-失败
 */
static int PelcoD_ExecuteZoomStep(uint8_t direction)
{
    if (direction == PELCO_D_ZOOM_IN) {
        // 放大一级
        if (lens_status.current_zoom_level < PELCO_D_ZOOM_MAX) {
            lens_status.current_zoom_level++;
            // 这里应该调用实际的变倍控制函数
            // 例如: ZoomControl(lens_status.current_zoom_level);
					
						Motor12_SetTargetPosition(x1_x16_cide[lens_status.current_zoom_level-1][0]*8);	//zoom1
						Motor78_SetTargetPosition(x1_x16_cide[lens_status.current_zoom_level-1][1]*8);	//zoom2
						Motor34_SetTargetPosition(x1_x16_cide[lens_status.current_zoom_level-1][2]*8); //zoom3
				//		Motor9A_SetTargetPosition(x1_x16_cide[lens_status.current_zoom_level-1][3]*8);
            return 0;
        }
				
				else{
						Motor12_SetTargetPosition(x1_x16_cide[lens_status.current_zoom_level-1][0]*8);	//zoom1
						Motor78_SetTargetPosition(x1_x16_cide[lens_status.current_zoom_level-1][1]*8);	//zoom2
						Motor34_SetTargetPosition(x1_x16_cide[lens_status.current_zoom_level-1][2]*8); //zoom3
				//		Motor9A_SetTargetPosition(x1_x16_cide[lens_status.current_zoom_level-1][3]*8);
						return 0; // 
				}
    } else {
        // 缩小一级
        if (lens_status.current_zoom_level > PELCO_D_ZOOM_MIN) {
            lens_status.current_zoom_level--;
            // 这里应该调用实际的变倍控制函数
            // 例如: ZoomControl(lens_status.current_zoom_level);
					
						Motor12_SetTargetPosition(x1_x16_cide[lens_status.current_zoom_level-1][0]*8);	//zoom1
						Motor78_SetTargetPosition(x1_x16_cide[lens_status.current_zoom_level-1][1]*8);	//zoom2
						Motor34_SetTargetPosition(x1_x16_cide[lens_status.current_zoom_level-1][2]*8); //zoom3
				//		Motor9A_SetTargetPosition(x1_x16_cide[lens_status.current_zoom_level-1][3]*8);
            return 0;
        }
				else{
						Motor12_SetTargetPosition(x1_x16_cide[lens_status.current_zoom_level-1][0]*8);	//zoom1
						Motor78_SetTargetPosition(x1_x16_cide[lens_status.current_zoom_level-1][1]*8);	//zoom2
						Motor34_SetTargetPosition(x1_x16_cide[lens_status.current_zoom_level-1][2]*8); //zoom3
				//		Motor9A_SetTargetPosition(x1_x16_cide[lens_status.current_zoom_level-1][3]*8);
						return -1; // 已达到最小倍率
				}
    }
}

/**
 * @brief 执行变倍设置操作
 * @param zoom_level 目标倍率 (1-16)
 * @return 0-成功, 其他-失败
 */
static int PelcoD_ExecuteZoomSet(uint8_t zoom_level)
{
    if (zoom_level >= PELCO_D_ZOOM_MIN && zoom_level <= PELCO_D_ZOOM_MAX) {
        lens_status.current_zoom_level = zoom_level;
  				Motor12_SetTargetPosition(x1_x16_cide[lens_status.current_zoom_level-1][0]*8);	//zoom1
   				Motor78_SetTargetPosition(x1_x16_cide[lens_status.current_zoom_level-1][1]*8);	//zoom2
					Motor34_SetTargetPosition(x1_x16_cide[lens_status.current_zoom_level-1][2]*8); //zoom3
				//	Motor9A_SetTargetPosition(x1_x16_cide[lens_status.current_zoom_level-1][3]*8);
				// 这里应该调用实际的变倍控制函数
        // 例如: ZoomControl(zoom_level);
        return 0;
    }
    return -1; // 无效的倍率值
}

/**
 * @brief 执行聚焦控制操作
 * @param direction 方向 (0-远焦, 1-近焦)
 * @param step_size 步长 (0x04-小步, 0x20-大步)
 * @return 0-成功, 其他-失败
 */
static int PelcoD_ExecuteFocusControl(uint8_t direction, uint8_t step_size)
{
		uint32_t now_pos;
    // 根据方向和步长控制聚焦电机
    if (direction == PELCO_D_FOCUS_FAR) {
				now_pos=Motor9A_GetCurrentPosition();
				Motor9A_SetTargetPosition(now_pos+=(step_size*8));
			
			
//				now_pos=Motor12_GetCurrentPosition();
//				Motor12_SetTargetPosition(now_pos+=(step_size*16));

//				now_pos=Motor34_GetCurrentPosition();
//				Motor34_SetTargetPosition(now_pos+=(step_size*16));			

//				now_pos=Motor78_GetCurrentPosition();
//				Motor78_SetTargetPosition(now_pos+=(step_size*10*8));		
			
    } else if (direction == PELCO_D_FOCUS_NEAR) {
			
			
			
				now_pos=Motor9A_GetCurrentPosition();
				Motor9A_SetTargetPosition(now_pos-=(step_size*8));
			
//				now_pos=Motor12_GetCurrentPosition();
//				Motor12_SetTargetPosition(now_pos-=(step_size*16));
//			
//				now_pos=Motor34_GetCurrentPosition();
//				Motor34_SetTargetPosition(now_pos-=(step_size*16));
		
//				now_pos=Motor78_GetCurrentPosition();
//				Motor78_SetTargetPosition(now_pos-=(step_size*10*8));		
		
		
		}
    
    return 0;
}

/**
 * @brief 执行IR-CUT控制操作
 * @param channel 通道号
 * @param state 状态 (0-关闭, 1-开启)
 * @return 0-成功, 其他-失败
 */
static int PelcoD_ExecuteIrcutControl(uint8_t channel, uint8_t state)
{
    if (channel == PELCO_D_IRCUT_CH0) {
        lens_status.ircut_status = state;
        
        // 这里应该调用实际的IR-CUT控制函数
        if (state == PELCO_D_IRCUT_ON) {
					HAL_GPIO_WritePin(IRCUT_1_GPIO_Port, IRCUT_1_Pin, GPIO_PIN_SET);
					HAL_GPIO_WritePin(IRCUT_2_GPIO_Port, IRCUT_1_Pin, GPIO_PIN_SET);

					// 开启IR-CUT
            // 例如: HAL_GPIO_WritePin(IRCUT_GPIO_Port, IRCUT_Pin, GPIO_PIN_SET);
        } else {
					HAL_GPIO_WritePin(IRCUT_1_GPIO_Port, IRCUT_1_Pin, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(IRCUT_2_GPIO_Port, IRCUT_1_Pin, GPIO_PIN_RESET);

					// 关闭IR-CUT
            // 例如: HAL_GPIO_WritePin(IRCUT_GPIO_Port, IRCUT_Pin, GPIO_PIN_RESET);
        }
        
        return 0;
    }
    
    return -1; // 无效的通道号
}

/**
 * @brief 获取当前变倍级别
 * @return 当前变倍级别
 */
static uint8_t PelcoD_GetCurrentZoomLevel(void)
{
    return lens_status.current_zoom_level;
}

/**
 * @brief 处理PelcoD命令并执行相应操作
 * @param cmd 解析后的命令
 * @return 0-成功, 其他-失败
 */
static int PelcoD_ExecuteCommand(const PelcoD_Command_t* cmd)
{
    int result = 0;
    
    switch (cmd->cmd_type) {
        case PELCO_D_CMD_TYPE_LENS_RESET:
            result = PelcoD_ExecuteLensReset();
            break;
            
        case PELCO_D_CMD_TYPE_ZOOM_STEP:
            result = PelcoD_ExecuteZoomStep(cmd->p2);
            break;
            
        case PELCO_D_CMD_TYPE_ZOOM_SET:
            result = PelcoD_ExecuteZoomSet(cmd->p2);
            break;
            
        case PELCO_D_CMD_TYPE_FOCUS_CTRL:
            result = PelcoD_ExecuteFocusControl(cmd->p1, cmd->p2);
            break;
            
        case PELCO_D_CMD_TYPE_IRCUT_CTRL:
            result = PelcoD_ExecuteIrcutControl(cmd->p1, cmd->p2);
            break;
            
        case PELCO_D_CMD_TYPE_ZOOM_QUERY:
            // 查询命令不需要执行操作，只需要在响应中返回当前状态
            result = 0;
            break;
            
        default:
            result = -1; // 未知命令
            break;
    }
    
    return result;
}

/**
 * @brief 构建带有实际状态的响应帧
 * @param cmd 原始命令
 * @param response_frame 响应帧缓冲区
 * @param frame_len 帧长度(输出)
 * @return 构建结果
 */
static PelcoD_ParseResult_t PelcoD_BuildResponseWithStatus(const PelcoD_Command_t* cmd, uint8_t* response_frame, uint8_t* frame_len)
{
    if (cmd == NULL || response_frame == NULL || frame_len == NULL) {
        return PELCO_D_PARSE_ERROR_LENGTH;
    }
    
    uint8_t opcode;
    uint8_t resp_p1 = 0x00;
    uint8_t resp_p2 = 0x00;
    uint8_t resp_p3 = 0x00;

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
            // IR-CUT响应中返回当前状态
            resp_p2 = lens_status.ircut_status;
            break;
            
        case PELCO_D_CMD_TYPE_ZOOM_QUERY:
            opcode = PELCO_D_CMD_ZOOM_QUERY;
            // 变倍查询响应中返回当前倍率
            resp_p3 = PelcoD_GetCurrentZoomLevel();
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
    response_frame[5] = resp_p3;                   // 保留字节
    response_frame[6] = PelcoD_CalculateChecksum(&response_frame[1], 5); // 校验和
    
    *frame_len = PELCO_D_FRAME_LENGTH;
    
    return PELCO_D_PARSE_OK;
}

/**
 * @brief UART接收中断处理函数
 * @param received_byte 接收到的字节
 */
void PelcoD_UartRxInterruptHandler(uint8_t received_byte)
{
//    // 检查是否为帧头
//    if (received_byte == PELCO_D_FRAME_HEADER) {
//        pelco_d_rx_index = 0;
//    }
//    
//    // 存储接收到的字节
//    if (pelco_d_rx_index < PELCO_D_RX_BUFFER_SIZE) {
//        pelco_d_rx_buffer[pelco_d_rx_index++] = received_byte;
//        
//        // 检查是否接收到完整帧
//        if (pelco_d_rx_index == PELCO_D_FRAME_LENGTH) {
//            // 处理完整帧
//            PelcoD_ProcessReceivedFrame(pelco_d_rx_buffer, PELCO_D_FRAME_LENGTH);
//            pelco_d_rx_index = 0; // 重置索引
//        }
//    } else {
//        // 缓冲区溢出，重置
//        pelco_d_rx_index = 0;
//    }
}


/**
 * @brief 发送PelcoD响应帧
 * @param response_frame 响应帧数据
 * @param frame_len 帧长度
 */
void PelcoD_SendResponse(uint8_t* response_frame, uint8_t frame_len)
{
    // 这里应该调用实际的UART发送函数
    // 例如: HAL_UART_Transmit(&huart1, response_frame, frame_len, 1000);
    HAL_UART_Transmit(&huart3, response_frame, frame_len, 1000);
//		HAL_UART_Transmit(&huart3, response_frame, frame_len, 1000);

    // 或者使用DMA发送
    // HAL_UART_Transmit_DMA(&huart1, response_frame, frame_len);
}

/**
 * @brief 处理接收到的PelcoD帧
 * @param frame_data 帧数据
 * @param frame_len 帧长度
 */
void PelcoD_ProcessReceivedFrame(uint8_t* frame_data, uint8_t frame_len)
{
    PelcoD_Command_t cmd;
    PelcoD_ParseResult_t parse_result = PelcoD_ParseFrame(frame_data, frame_len, &cmd);
    
    if (parse_result == PELCO_D_PARSE_OK) {
        // 执行命令
        int exec_result = PelcoD_ExecuteCommand(&cmd);
        
        // 构建响应帧
        uint8_t response_frame[PELCO_D_FRAME_LENGTH];
        uint8_t response_len;
        
        PelcoD_ParseResult_t resp_result = PelcoD_BuildResponseWithStatus(&cmd, response_frame, &response_len);
        
        if (resp_result == PELCO_D_PARSE_OK) {
            // 发送响应帧
            PelcoD_SendResponse(response_frame, response_len);
        }
    }
}


/**
 * @brief PelcoD协议初始化
 */


static __attribute__((constructor)) void PelcoD_Init(void);

void PelcoD_Init(void)
{
    // 初始化镜头状态
    lens_status.current_zoom_level = 1;
    lens_status.ircut_status = 0;
    lens_status.lens_reset_status = 0;
}

/**
 * @brief 获取镜头状态
 * @return 镜头状态结构体指针
 */
const LensStatus_t* PelcoD_GetLensStatus(void)
{
    return &lens_status;
}

/**
 * @brief 1ms定时器中断处理函数(用于处理复位状态)
 */
void PelcoD_1msTimerHandler(void)
{
    // 处理各个电机的复位状态
    if (lens_status.lens_reset_status) {
        // 检查所有电机复位是否完成
        MotorResetState_t zoom1_state = Motor_ZOOM1_Reset_GetState();
        MotorResetState_t zoom2_state = Motor_ZOOM2_Reset_GetState();
        MotorResetState_t zoom3_state = Motor_ZOOM3_Reset_GetState();
        MotorResetState_t focus_state = Motor_FOCUS_Reset_GetState();
        MotorResetState_t iris_state = Motor_IRIS_Reset_GetState();
        
        // 如果所有电机复位都完成或超时，则清除复位状态
        if ((zoom1_state == MOTOR_RESET_COMPLETED || zoom1_state == MOTOR_RESET_TIMEOUT) &&
            (zoom2_state == MOTOR_RESET_COMPLETED || zoom2_state == MOTOR_RESET_TIMEOUT) &&
            (zoom3_state == MOTOR_RESET_COMPLETED || zoom3_state == MOTOR_RESET_TIMEOUT) &&
            (focus_state == MOTOR_RESET_COMPLETED || focus_state == MOTOR_RESET_TIMEOUT) &&
            (iris_state == MOTOR_RESET_COMPLETED || iris_state == MOTOR_RESET_TIMEOUT)) {
            
            lens_status.lens_reset_status = 0;
            
            // 清除所有电机复位状态
            Motor_ZOOM1_Reset_Clear();
            Motor_ZOOM2_Reset_Clear();
            Motor_ZOOM3_Reset_Clear();
            Motor_FOCUS_Reset_Clear();
            Motor_IRIS_Reset_Clear();
        }
    }
}
