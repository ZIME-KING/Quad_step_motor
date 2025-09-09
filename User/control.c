
#include "control.h"
#include "main.h"
#include <stdio.h>
#include "UART/uart.h"

char OutPutChennal2 = 0;
char OutPutChennal = 0;
uint8_t IRCUT_FLAG = 0;
uint8_t IRCUT_FLAG2 = 0;
uint16_t IRS_TGT = 0X000;  // 光圈 目标值
uint16_t PWMMODEAB = 0;	   // PWM频率 AB
uint16_t PWMRESAB = 0;	   // PWM细分 AB
uint16_t PHMODA = 0;	   // 相位矫正 A
uint16_t PHMODB = 0;	   // 相位矫正 B
uint16_t PPWAB = 0X80;	   // 峰值宽度(电流)AB
uint16_t PPWCD = 0X80;	   // 峰值宽度(电流)CD
uint16_t PPWEF = 0X80;	   // 峰值宽度(电流)EF
uint16_t PPWGH = 0X80;	   // 峰值宽度(电流)GH
uint16_t PPWIJ = 0X80;	   // 峰值宽度(电流)IJ

uint16_t CCWCWA = 0X1000;  // 方向A
uint16_t CCWCWB = 0X1000;  // 方向B
uint16_t BRAKEA = 0x2000;  // 刹车A
uint16_t BRAKEB = 0x2000;  // 刹车B
uint16_t ENDISA = 0X4000;  // 使能A
uint16_t ENDISB = 0X4000;  // 使能B
uint16_t MICROA = 0X0000;  // 细分数
uint16_t MICROB = 0X0000;  // 细分数




uint16_t PSUMA = 50;	   // 步进数A
uint16_t PSUMB = 50;	   // 步进数B
uint16_t PSUMC = 50;	   // 步进数C
uint16_t PSUMD = 50;	   // 步进数D

//当 INTCTAB[15:0]=900 时，64 细分下每步周期 12×900/27MHz=0.4ms

//当 INTCTAB[15:0]=900 时，256 细分下每步周期 3×900/27MHz=0.1ms


//设置为900，0.1ms一步 一个VD周期按20ms->50hz,最大设置为200步
uint16_t INTCTAB = 703; // 每步周期AB
uint16_t INTCTCD = 703; // 每步周期CD
uint16_t INTCTEF = 703; // 每步周期EF
uint16_t INTCTGH = 703; // 每步周期GH
uint16_t INTCTIJ = 703; // 每步周期GH



uint16_t LED1 = 0x0000;
uint16_t LED2 = 0x0000;
uint16_t LED3 = 0x0000;
uint16_t LED4 = 0x0000;


#define ZOOM1_ZERO_POS  0
#define ZOOM2_ZERO_POS  0
#define ZOOM3_ZERO_POS  0
#define FOUCE_ZERO_POS  0



// 步进电机控制相关变量
static bool stepper_running = false;
static uint16_t target_steps_A = 0;
static uint16_t target_steps_B = 0;
static uint16_t current_steps_A = 0;
static uint16_t current_steps_B = 0;

// VD_FZ次数控制相关变量
static uint16_t vd_fz_count = 0;        // 当前执行次数
static uint16_t vd_fz_target_count = 0; // 目标执行次数
static bool vd_fz_enabled = false;      // VD_FZ使能标志

// 步进电机12位置控制相关变量
static int32_t motor12_current_position = 0;  // 当前位置
static int32_t motor12_target_position = 0;   // 目标位置
static bool motor12_position_control_enabled = true; // 位置控制使能标志
static uint16_t motor12_max_steps_per_cycle = 32;    // 每个20ms周期最大步数

// 步进电机34位置控制相关变量
static int32_t motor34_current_position = 0;  // 当前位置
static int32_t motor34_target_position = 0;   // 目标位置
static bool motor34_position_control_enabled = true; // 位置控制使能标志
static uint16_t motor34_max_steps_per_cycle = 32;    // 每个20ms周期最大步数

// 步进电机56位置控制相关变量
static int32_t motor56_current_position = 0;  // 当前位置
static int32_t motor56_target_position = 0;   // 目标位置
static bool motor56_position_control_enabled = true; // 位置控制使能标志
static uint16_t motor56_max_steps_per_cycle = 32;    // 每个20ms周期最大步数

// 步进电机78位置控制相关变量
static int32_t motor78_current_position = 0;  // 当前位置
static int32_t motor78_target_position = 0;   // 目标位置
static bool motor78_position_control_enabled = true; // 位置控制使能标志
static uint16_t motor78_max_steps_per_cycle = 32;    // 每个20ms周期最大步数

// 步进电机9A位置控制相关变量
static int32_t motor9a_current_position = 0;  // 当前位置
static int32_t motor9a_target_position = 0;   // 目标位置
static bool motor9a_position_control_enabled = true; // 位置控制使能标志
static uint16_t motor9a_max_steps_per_cycle = 32;    // 每个20ms周期最大步数
//timer_oc_parameter_struct timer0_ocintpara;
//timer_parameter_struct timer0_initpara;

extern uint16_t u16ReadSpiData[20];

uint16_t Spi_ReadWrite(uint16_t u16WriteData)
{
    uint16_t ret_Data;
    uint8_t txData[2], rxData[2];

    // 将16位数据分解为两个8位数据
    txData[0] = (uint8_t)(u16WriteData & 0xFF);
    txData[1] = (uint8_t)((u16WriteData >> 8) & 0xFF);

    // 使用HAL库进行SPI传输
    HAL_SPI_TransmitReceive(&hspi2, txData, rxData, 2, HAL_MAX_DELAY);

    // 重新组合接收到的数据
    ret_Data = rxData[0] | (rxData[1] << 8);
    return (ret_Data);
}

uint8_t Spi_ReadWriteByte(uint8_t u8Byte)
{
    uint8_t ret_Data;
    // 使用HAL库进行SPI字节传输
    HAL_SPI_TransmitReceive(&hspi2, &u8Byte, &ret_Data, 1, HAL_MAX_DELAY);
    return (ret_Data);
}

void Spi_Write(uint8_t addr, uint16_t data)
{
    CS_SET();
    Spi_ReadWriteByte(addr);
    Spi_ReadWriteByte(data % 256);
    Spi_ReadWriteByte(data / 256);
    // delay_us(5);
    CS_CLR();
}

void Spi__Read(uint8_t addr, uint16_t *data)
{
    uint16_t tmp1, tmp2;
    CS_SET();
    Spi_ReadWriteByte(addr | 0x40);
    tmp2 = Spi_ReadWriteByte(0);
    tmp1 = Spi_ReadWriteByte(0);
    // delay_us(5);
    *data = tmp2 | (tmp1 << 8);
    CS_CLR();
}

void Reset_MS41968(void)
{
    RSTB_H;
    HAL_Delay(1);
    RSTB_L;
    HAL_Delay(1);
    RSTB_H;
    HAL_Delay(10);
}

void Init_MS41968(void)
{
    Spi_Write(0x20, 0x0001); // 不使能复用直流通道 不使能直流驱动外部管脚控制 设置DT1延时
   
    Spi_Write(0x22, 0x0001); // 选择256分频  相位矫正=0 设置DT2延时
    Spi_Write(0x23, 0xd8d8); // 设置最大占空比为 90%
    Spi_Write(0x24, (0xcfff&0xF000)|PSUMA); // 设置LED输出  电机输出使能/关断  启动/刹车 电流方向 步数
    Spi_Write(0x25, INTCTAB); // 设置步进周期
    Spi_Write(0x26, 0x9e5e); // 设置TESTEN2 FZTEST  过流保护检测时间OCP_dly  PWMRES PWMMODE

    Spi_Write(0x27, 0x0001); // 选择256分频  相位矫正=0 设置DT2延时
    Spi_Write(0x28, 0xd8d8); // 设置最大占空比为 90%
    Spi_Write(0x29, (0xcfff&0xF000)|PSUMB); // 设置LED输出  电机输出使能/关断  启动/刹车 电流方向 步数
    Spi_Write(0x2a, INTCTCD); // 设置步进周期
    Spi_Write(0x2b, 0x9e5e); // 设置TESTEN2 FZTEST  过流保护检测时间OCP_dly  PWMRES PWMMODE

    Spi_Write(0x2c, 0x0001); // 选择256分频  相位矫正=0 设置DT2延时
    Spi_Write(0x2d, 0xd8d8); // 设置最大占空比为 90%
    Spi_Write(0x2e, (0xcfff&0xF000)|PSUMC); // 设置LED输出  电机输出使能/关断  启动/刹车 电流方向 步数
    Spi_Write(0x2f, INTCTEF); // 设置步进周期
    Spi_Write(0x30, 0x9e5e); // 设置TESTEN2 FZTEST  过流保护检测时间OCP_dly  PWMRES PWMMODE

    Spi_Write(0x31, 0x0001); // 选择256分频  相位矫正=0 设置DT2延时
    Spi_Write(0x32, 0xd8d8); // 设置最大占空比为 90%
    Spi_Write(0x33, (0xcfff&0xF000)|PSUMD); // 设置LED输出  电机输出使能/关断  启动/刹车 电流方向 步数
    Spi_Write(0x34, INTCTGH); // 设置步进周期
    Spi_Write(0x35, 0x9e5e); // 设置TESTEN2 FZTEST  过流保护检测时间OCP_dly  PWMRES PWMMODE

    Spi_Write(0x36, 0x0001); // 选择256分频  相位矫正=0 设置DT2延时
    Spi_Write(0x37, 0xd8d8); // 设置最大占空比为 90%
    Spi_Write(0x38, 0xcfff); // 设置LED输出  电机输出使能/关断  启动/刹车 电流方向 步数
    Spi_Write(0x39, INTCTAB); // 设置步进周期
    Spi_Write(0x3a, 0x9e5e); // 设置TESTEN2 FZTEST  过流保护检测时间OCP_dly  PWMRES PWMMODE

    Spi_Write(0x3b, 0x0000); // 设置直流电机通道A驱动状态  PWM频率 PWM占空比
    Spi_Write(0x3c, 0x0000); // 设置直流电机通道B驱动状态  PWM频率 PWM占空比
    Spi_Write(0x3e, 0x0000); // 错误指示寄存器

    Spi_Write(0x00, 0x0200); // 设置光圈目标值  0x0200
    Spi_Write(0x01, 0x7c8a); // 设置ADC反馈滤波器截止频率 PID控制器数字增益   0x7c8a
    Spi_Write(0x02, 0x66f0); // 设置PID控制器零点 极点       0x66f0
    Spi_Write(0x03, 0x0e10); // 设置光圈输出PWM频率     0x0e10
    Spi_Write(0x04, 0x7070); // 设置霍尔元件偏置电流 偏置电压      0x80ff
    Spi_Write(0x05, 0x0004); // 设置光圈目标值低通滤波器器截止频率 0x0004
    Spi_Write(0x0e, 0x0300); // 设置光圈目标值移动平均速度        0x0300
    Spi_Write(0x0b, 0x0480); // 设置光圈模块使能 TESTEN1使能       0x0480
}

// void set_34(uint16_t pos){
//     Spi_Write(0x27, 0x0001); // 选择256分频  相位矫正=0 设置DT2延时
//     Spi_Write(0x28, 0xd8d8); // 设置最大占空比为 90%
//     // 保持0xcfff的高4位，D12位=0（正方向），设置步数（低12位）
//     Spi_Write(0x29, (0xcfff&0xF000)|(pos&0x0FFF)); // 设置LED输出 电机输出使能/关断 启动/刹车 电流方向(D12=0正方向) 步数(D11-D0)
//     Spi_Write(0x2a, INTCTCD); // 设置步进周期
//     Spi_Write(0x2b, 0x9e5e); // 设置TESTEN2 FZTEST  过流保护检测时间OCP_dly  PWMRES PWMMODE
//     VD_FZ_Motor12();
//  }

/**
 * @brief 打印步进电机12的当前位置和目标位置
 */
void Motor12_PrintPositions(void)
{
    LOG_Print(LOG_LEVEL_INFO,"=== 步进电机12位置信息 ===\r\n");
    LOG_Print(LOG_LEVEL_INFO,"当前位置: %d\r\n", motor12_current_position);
    LOG_Print(LOG_LEVEL_INFO,"目标位置: %d\r\n", motor12_target_position);
    LOG_Print(LOG_LEVEL_INFO,"位置误差: %d\r\n", motor12_target_position - motor12_current_position);
    LOG_Print(LOG_LEVEL_INFO,"控制状态: %s\r\n", motor12_position_control_enabled ? "启用" : "禁用");
    LOG_Print(LOG_LEVEL_INFO,"每周期最大步数: %d\r\n", motor12_max_steps_per_cycle);
    LOG_Print(LOG_LEVEL_INFO,"是否到达目标: %s\r\n", (motor12_current_position == motor12_target_position) ? "是" : "否");
    LOG_Print(LOG_LEVEL_INFO,"========================\r\n");
}

/**
 * @brief 打印步进电机34的当前位置和目标位置
 */
void Motor34_PrintPositions(void)
{
    LOG_Print(LOG_LEVEL_INFO,"=== 步进电机34位置信息 ===\r\n");
    LOG_Print(LOG_LEVEL_INFO,"当前位置: %d\r\n", motor34_current_position);
    LOG_Print(LOG_LEVEL_INFO,"目标位置: %d\r\n", motor34_target_position);
    LOG_Print(LOG_LEVEL_INFO,"位置误差: %d\r\n", motor34_target_position - motor34_current_position);
    LOG_Print(LOG_LEVEL_INFO,"控制状态: %s\r\n", motor34_position_control_enabled ? "启用" : "禁用");
    LOG_Print(LOG_LEVEL_INFO,"每周期最大步数: %d\r\n", motor34_max_steps_per_cycle);
    LOG_Print(LOG_LEVEL_INFO,"是否到达目标: %s\r\n", (motor34_current_position == motor34_target_position) ? "是" : "否");
    LOG_Print(LOG_LEVEL_INFO,"========================\r\n");
}

/**
 * @brief 打印步进电机56的当前位置和目标位置
 */
void Motor56_PrintPositions(void)
{
    LOG_Print(LOG_LEVEL_INFO,"=== 步进电机56位置信息 ===\r\n");
    LOG_Print(LOG_LEVEL_INFO,"当前位置: %d\r\n", motor56_current_position);
    LOG_Print(LOG_LEVEL_INFO,"目标位置: %d\r\n", motor56_target_position);
    LOG_Print(LOG_LEVEL_INFO,"位置误差: %d\r\n", motor56_target_position - motor56_current_position);
    LOG_Print(LOG_LEVEL_INFO,"控制状态: %s\r\n", motor56_position_control_enabled ? "启用" : "禁用");
    LOG_Print(LOG_LEVEL_INFO,"每周期最大步数: %d\r\n", motor56_max_steps_per_cycle);
    LOG_Print(LOG_LEVEL_INFO,"是否到达目标: %s\r\n", (motor56_current_position == motor56_target_position) ? "是" : "否");
    LOG_Print(LOG_LEVEL_INFO,"========================\r\n");
}

/**
 * @brief 打印步进电机78的当前位置和目标位置
 */
void Motor78_PrintPositions(void)
{
    LOG_Print(LOG_LEVEL_INFO,"=== 步进电机78位置信息 ===\r\n");
    LOG_Print(LOG_LEVEL_INFO,"当前位置: %d\r\n", motor78_current_position);
    LOG_Print(LOG_LEVEL_INFO,"目标位置: %d\r\n", motor78_target_position);
    LOG_Print(LOG_LEVEL_INFO,"位置误差: %d\r\n", motor78_target_position - motor78_current_position);
    LOG_Print(LOG_LEVEL_INFO,"控制状态: %s\r\n", motor78_position_control_enabled ? "启用" : "禁用");
    LOG_Print(LOG_LEVEL_INFO,"每周期最大步数: %d\r\n", motor78_max_steps_per_cycle);
    LOG_Print(LOG_LEVEL_INFO,"是否到达目标: %s\r\n", (motor78_current_position == motor78_target_position) ? "是" : "否");
    LOG_Print(LOG_LEVEL_INFO,"========================\r\n");
}

/**
 * @brief 打印步进电机9A的当前位置和目标位置
 */
void Motor9A_PrintPositions(void)
{
    LOG_Print(LOG_LEVEL_INFO,"=== 步进电机9A位置信息 ===\r\n");
    LOG_Print(LOG_LEVEL_INFO,"当前位置: %d\r\n", motor9a_current_position);
    LOG_Print(LOG_LEVEL_INFO,"目标位置: %d\r\n", motor9a_target_position);
    LOG_Print(LOG_LEVEL_INFO,"位置误差: %d\r\n", motor9a_target_position - motor9a_current_position);
    LOG_Print(LOG_LEVEL_INFO,"控制状态: %s\r\n", motor9a_position_control_enabled ? "启用" : "禁用");
    LOG_Print(LOG_LEVEL_INFO,"每周期最大步数: %d\r\n", motor9a_max_steps_per_cycle);
    LOG_Print(LOG_LEVEL_INFO,"是否到达目标: %s\r\n", (motor9a_current_position == motor9a_target_position) ? "是" : "否");
    LOG_Print(LOG_LEVEL_INFO,"========================\r\n");
}

/**
 * @brief 步进电机12位置控制函数 (每20ms调用一次)
 * 根据目标位置自动计算并执行步进
 */
void Motor12_PositionControl(void)
{
    if (!motor12_position_control_enabled) {
        return; // 位置控制未启用
    }
    // 计算位置差
    int32_t position_error = motor12_target_position - motor12_current_position;
    
    if (position_error == 0) {
			
				Spi_Write(0x24, 0x0000); //
				VD_FZ_Motor12();
				Motor12_DisablePositionControl(); 	//到达位置后关闭位置控制,设置目标值后要手动开启位置控制
        return; // 已到达目标位置
    }
    // 确定运动方向和步数
    uint16_t steps_to_move;
    uint16_t register_value;
    
    if (position_error > 0) {
        // 正方向运动 - D12位=0表示正方向
        steps_to_move = (position_error > motor12_max_steps_per_cycle) ? motor12_max_steps_per_cycle : (uint16_t)position_error;
        // 保持0xcfff的高4位，清除D12位（正方向），设置步数（低12位）
        register_value = (0xcfff & 0xF000) | (steps_to_move & 0x0FFF);
    } else {
        // 负方向运动 - D12位=1表示负方向
        int32_t abs_error = -position_error;
        steps_to_move = (abs_error > motor12_max_steps_per_cycle) ? motor12_max_steps_per_cycle : (uint16_t)abs_error;
        // 保持0xcfff的高4位，设置D12位（负方向），设置步数（低12位）
        register_value = (0xcfff & 0xF000) | 0x1000 | (steps_to_move & 0x0FFF);
    }
    
    // 限制步数在12位范围内
    if (steps_to_move > 0x0FFF) {
        steps_to_move = 0x0FFF;
        // 重新计算寄存器值
        if (position_error > 0) {
            register_value = (0xcfff & 0xF000) | 0x0FFF;
        } else {
            register_value = (0xcfff & 0xF000) | 0x1000 | 0x0FFF;
        }
    }
    if (steps_to_move > 0) {
        // 执行步进 - Motor12使用寄存器0x21-0x25
        Spi_Write(0x22, 0x0001); // 选择256分频  相位矫正=0 设置DT2延时
        Spi_Write(0x23, 0xd8d8); // 设置最大占空比为 90%
        Spi_Write(0x24, register_value); // 设置LED输出 电机输出使能/关断 启动/刹车 电流方向(D12) 步数(D11-D0)
        Spi_Write(0x25, INTCTAB); // 设置步进周期
        Spi_Write(0x26, 0x9e5e); // 设置TESTEN2 FZTEST  过流保护检测时间OCP_dly  PWMRES PWMMODE
        VD_FZ_Motor12();
        
        // 更新当前位置
        if (position_error > 0) {
            motor12_current_position += steps_to_move;
        } else {
            motor12_current_position -= steps_to_move;
        }
				
				Motor12_PrintPositions();
    }
}

/**
 * @brief 步进电机34位置控制函数 (每20ms调用一次)
 * 根据目标位置自动计算并执行步进
 */
void Motor34_PositionControl(void)
{
    if (!motor34_position_control_enabled) {
        return; // 位置控制未启用
    }
    // 计算位置差
    int32_t position_error = motor34_target_position - motor34_current_position;
    
    if (position_error == 0) {
			
				Spi_Write(0x29, 0x0000); //
				VD_FZ_Motor34();
				Motor34_DisablePositionControl(); 	//到达位置后关闭位置控制,设置目标值后要手动开启位置控制
			
        return; // 已到达目标位置
    }
    // 确定运动方向和步数
    uint16_t steps_to_move;
    uint16_t register_value;
    
    if (position_error > 0) {
        // 正方向运动 - D12位=0表示正方向
        steps_to_move = (position_error > motor34_max_steps_per_cycle) ? 
                       motor34_max_steps_per_cycle : (uint16_t)position_error;
        // 保持0xcfff的高4位，清除D12位（正方向），设置步数（低12位）
        register_value = (0xcfff & 0xF000) | (steps_to_move & 0x0FFF);
    } else {
        // 负方向运动 - D12位=1表示负方向
        int32_t abs_error = -position_error;
        steps_to_move = (abs_error > motor34_max_steps_per_cycle) ? 
                       motor34_max_steps_per_cycle : (uint16_t)abs_error;
        // 保持0xcfff的高4位，设置D12位（负方向），设置步数（低12位）
        register_value = (0xcfff & 0xF000) | 0x1000 | (steps_to_move & 0x0FFF);
    }
    
    // 限制步数在12位范围内
    if (steps_to_move > 0x0FFF) {
        steps_to_move = 0x0FFF;
        // 重新计算寄存器值
        if (position_error > 0) {
            register_value = (0xcfff & 0xF000) | 0x0FFF;
        } else {
            register_value = (0xcfff & 0xF000) | 0x1000 | 0x0FFF;
        }
    }
    if (steps_to_move > 0) {
        // 执行步进 - Motor34使用寄存器0x27-0x2b
        Spi_Write(0x27, 0x0001); // 选择256分频  相位矫正=0 设置DT2延时
        Spi_Write(0x28, 0xd8d8); // 设置最大占空比为 90%
        Spi_Write(0x29, register_value); // 设置LED输出 电机输出使能/关断 启动/刹车 电流方向(D12) 步数(D11-D0)
        Spi_Write(0x2a, INTCTCD); // 设置步进周期
        Spi_Write(0x2b, 0x9e5e); // 设置TESTEN2 FZTEST  过流保护检测时间OCP_dly  PWMRES PWMMODE
        VD_FZ_Motor34();
         
         // 更新当前位置
         if (position_error > 0) {
             motor34_current_position += steps_to_move;
        } else {
            motor34_current_position -= steps_to_move;
        }
				Motor34_PrintPositions();
    }
}

/**
 * @brief 步进电机56位置控制函数 (每20ms调用一次)
 * 根据目标位置自动计算并执行步进
 */
void Motor56_PositionControl(void)
{
    if (!motor56_position_control_enabled) {
        return; // 位置控制未启用
    }
    // 计算位置差
    int32_t position_error = motor56_target_position - motor56_current_position;
    
    if (position_error == 0) {
			
				Spi_Write(0x2e, 0x0000); //
				VD_FZ_Motor56();
				Motor56_DisablePositionControl(); 	//到达位置后关闭位置控制,设置目标值后要手动开启位置控制
			
        return; // 已到达目标位置
    }
    // 确定运动方向和步数
    uint16_t steps_to_move;
    uint16_t register_value;
    
    if (position_error > 0) {
        // 正方向运动 - D12位=0表示正方向
        steps_to_move = (position_error > motor56_max_steps_per_cycle) ? 
                       motor56_max_steps_per_cycle : (uint16_t)position_error;
        // 保持0xcfff的高4位，清除D12位（正方向），设置步数（低12位）
        register_value = (0xcfff & 0xF000) | (steps_to_move & 0x0FFF);
    } else {
        // 负方向运动 - D12位=1表示负方向
        int32_t abs_error = -position_error;
        steps_to_move = (abs_error > motor56_max_steps_per_cycle) ? 
                       motor56_max_steps_per_cycle : (uint16_t)abs_error;
        // 保持0xcfff的高4位，设置D12位（负方向），设置步数（低12位）
        register_value = (0xcfff & 0xF000) | 0x1000 | (steps_to_move & 0x0FFF);
    }
    
    // 限制步数在12位范围内
    if (steps_to_move > 0x0FFF) {
        steps_to_move = 0x0FFF;
        // 重新计算寄存器值
        if (position_error > 0) {
            register_value = (0xcfff & 0xF000) | 0x0FFF;
        } else {
            register_value = (0xcfff & 0xF000) | 0x1000 | 0x0FFF;
        }
    }
    if (steps_to_move > 0) {
        // 执行步进 - Motor56使用寄存器0x2d-0x31
        Spi_Write(0x2c, 0x0001); // 选择256分频  相位矫正=0 设置DT2延时
        Spi_Write(0x2d, 0xd8d8); // 设置最大占空比为 90%
        Spi_Write(0x2e, register_value); // 设置LED输出 电机输出使能/关断 启动/刹车 电流方向(D12) 步数(D11-D0)
        Spi_Write(0x2F, INTCTEF); // 设置步进周期
        Spi_Write(0x30, 0x9e5e); // 设置TESTEN2 FZTEST  过流保护检测时间OCP_dly  PWMRES PWMMODE
        VD_FZ_Motor56();
         
         // 更新当前位置
         if (position_error > 0) {
             motor56_current_position += steps_to_move;
        } else {
            motor56_current_position -= steps_to_move;
        }
    }
}

/**
 * @brief 步进电机78位置控制函数 (每20ms调用一次)
 * 根据目标位置自动计算并执行步进
 */
void Motor78_PositionControl(void)
{
    if (!motor78_position_control_enabled) {
        return; // 位置控制未启用
    }
    // 计算位置差
    int32_t position_error = motor78_target_position - motor78_current_position;
    
    if (position_error == 0) {
			
				Spi_Write(0x33, 0x0000); //
				VD_FZ_Motor78();
				Motor78_DisablePositionControl(); 	//到达位置后关闭位置控制,设置目标值后要手动开启位置控制
        return; // 已到达目标位置
    }
    // 确定运动方向和步数
    uint16_t steps_to_move;
    uint16_t register_value;
   
		//ZOOM2需要反向

    if (position_error > 0) {
        // 正方向运动 - D12位=0表示正方向
        steps_to_move = (position_error > motor78_max_steps_per_cycle) ? 
                       motor78_max_steps_per_cycle : (uint16_t)position_error;
        // 保持0xcfff的高4位，清除D12位（正方向），设置步数（低12位）
        register_value = (0xcfff & 0xF000) | (steps_to_move & 0x0FFF);
    } else {
        // 负方向运动 - D12位=1表示负方向
        int32_t abs_error = -position_error;
        steps_to_move = (abs_error > motor78_max_steps_per_cycle) ? 
                       motor78_max_steps_per_cycle : (uint16_t)abs_error;
        // 保持0xcfff的高4位，设置D12位（负方向），设置步数（低12位）
        register_value = (0xcfff & 0xF000) | 0x1000 | (steps_to_move & 0x0FFF);
    }
    
    // 限制步数在12位范围内
    if (steps_to_move > 0x0FFF) {
        steps_to_move = 0x0FFF;
        // 重新计算寄存器值
        if (position_error > 0) {
            register_value = (0xcfff & 0xF000) | 0x0FFF;
        } else {
            register_value = (0xcfff & 0xF000) | 0x1000 | 0x0FFF;
        }
    }
    if (steps_to_move > 0) {
        // 执行步进 - Motor78使用寄存器0x33-0x37
        Spi_Write(0x31, 0x0001); // 选择256分频  相位矫正=0 设置DT2延时
        Spi_Write(0x32, 0xd8d8); // 设置最大占空比为 90%
        Spi_Write(0x33, register_value); // 设置LED输出 电机输出使能/关断 启动/刹车 电流方向(D12) 步数(D11-D0)
        Spi_Write(0x34, INTCTGH); // 设置步进周期
        Spi_Write(0x35, 0x9e5e); // 设置TESTEN2 FZTEST  过流保护检测时间OCP_dly  PWMRES PWMMODE
        VD_FZ_Motor78();
         
         // 更新当前位置
         if (position_error > 0) {
             motor78_current_position += steps_to_move;
        } else {
            motor78_current_position -= steps_to_move;
        }
						Motor78_PrintPositions();

    }
}

/**
 * @brief 步进电机9A位置控制函数 (每20ms调用一次)
 * 根据目标位置自动计算并执行步进
 */
void Motor9A_PositionControl(void)
{
    if (!motor9a_position_control_enabled) {
        return; // 位置控制未启用
    }
    // 计算位置差
    int32_t position_error = motor9a_target_position - motor9a_current_position;
    
    if (position_error == 0) {
			
				Spi_Write(0x38, 0x0000); //
				VD_FZ_Motor9A();
				Motor9A_DisablePositionControl(); 	//到达位置后关闭位置控制,设置目标值后要手动开启位置控制
			
        return; // 已到达目标位置
    }
    // 确定运动方向和步数
    uint16_t steps_to_move;
    uint16_t register_value;
    
    if (position_error > 0) {
        // 正方向运动 - D12位=0表示正方向
        steps_to_move = (position_error > motor9a_max_steps_per_cycle) ? 
                       motor9a_max_steps_per_cycle : (uint16_t)position_error;
        // 保持0xcfff的高4位，清除D12位（正方向），设置步数（低12位）
        register_value = (0xcfff & 0xF000) | (steps_to_move & 0x0FFF);
    } else {
        // 负方向运动 - D12位=1表示负方向
        int32_t abs_error = -position_error;
        steps_to_move = (abs_error > motor9a_max_steps_per_cycle) ? 
                       motor9a_max_steps_per_cycle : (uint16_t)abs_error;
        // 保持0xcfff的高4位，设置D12位（负方向），设置步数（低12位）
        register_value = (0xcfff & 0xF000) | 0x1000 | (steps_to_move & 0x0FFF);
    }
    
    // 限制步数在12位范围内
    if (steps_to_move > 0x0FFF) {
        steps_to_move = 0x0FFF;
        // 重新计算寄存器值
        if (position_error > 0) {
            register_value = (0xcfff & 0xF000) | 0x0FFF;
        } else {
            register_value = (0xcfff & 0xF000) | 0x1000 | 0x0FFF;
        }
    }
    if (steps_to_move > 0) {
        // 执行步进 - Motor9A使用寄存器0x39-0x3d
        Spi_Write(0x36, 0x0001); // 选择256分频  相位矫正=0 设置DT2延时
        Spi_Write(0x37, 0xd8d8); // 设置最大占空比为 90%
        Spi_Write(0x38, register_value); // 设置LED输出 电机输出使能/关断 启动/刹车 电流方向(D12) 步数(D11-D0)
        Spi_Write(0x39, INTCTIJ); // 设置步进周期
        Spi_Write(0x3a, 0x9e5e); // 设置TESTEN2 FZTEST  过流保护检测时间OCP_dly  PWMRES PWMMODE
        VD_FZ_Motor9A();
         
         // 更新当前位置
         if (position_error > 0) {
             motor9a_current_position += steps_to_move;
        } else {
            motor9a_current_position -= steps_to_move;
        }
										Motor9A_PrintPositions();

    }
}

/**
 * @brief 步进电机12的VD脉冲控制函数
 */
void VD_FZ_Motor12(void)
{
    // 检查是否启用次数控制
    if (vd_fz_enabled) {
        // 检查是否已达到目标次数
        if (vd_fz_count >= vd_fz_target_count) {
            return; // 已达到目标次数，不执行
        }
        vd_fz_count++; // 增加执行次数
    }
    
    // 步进电机12的VD脉冲控制 - 使用VD12引脚
    HAL_GPIO_WritePin(VD12_GPIO_Port, VD12_Pin, GPIO_PIN_SET);   // VD脉冲高电平
    
    // 延时20微秒（在中断中使用简单的循环延时）
    for(volatile int i = 0; i < 10; i++);  // 简单延时，避免在中断中使用HAL_Delay
    
    HAL_GPIO_WritePin(VD12_GPIO_Port, VD12_Pin, GPIO_PIN_RESET); // VD脉冲低电平
}

/**
 * @brief 步进电机34的VD脉冲控制函数
 */
void VD_FZ_Motor34(void)
{
    // 检查是否启用次数控制
    if (vd_fz_enabled) {
        // 检查是否已达到目标次数
        if (vd_fz_count >= vd_fz_target_count) {
            return; // 已达到目标次数，不执行
        }
        vd_fz_count++; // 增加执行次数
    }
    
    // 步进电机34的VD脉冲控制 - 使用VD34引脚
    HAL_GPIO_WritePin(VD34_GPIO_Port, VD34_Pin, GPIO_PIN_SET);   // VD脉冲高电平
    
    // 延时20微秒（在中断中使用简单的循环延时）
    for(volatile int i = 0; i < 10; i++);  // 简单延时，避免在中断中使用HAL_Delay
    
    HAL_GPIO_WritePin(VD34_GPIO_Port, VD34_Pin, GPIO_PIN_RESET); // VD脉冲低电平
}

/**
 * @brief 步进电机56的VD脉冲控制函数
 */
void VD_FZ_Motor56(void)
{
    // 检查是否启用次数控制
    if (vd_fz_enabled) {
        // 检查是否已达到目标次数
        if (vd_fz_count >= vd_fz_target_count) {
            return; // 已达到目标次数，不执行
        }
        vd_fz_count++; // 增加执行次数
    }
    
    // 步进电机56的VD脉冲控制 - 使用VD56引脚
    HAL_GPIO_WritePin(VD56_GPIO_Port, VD56_Pin, GPIO_PIN_SET);   // VD脉冲高电平
    
    // 延时20微秒（在中断中使用简单的循环延时）
    for(volatile int i = 0; i < 10; i++);  // 简单延时，避免在中断中使用HAL_Delay
    
    HAL_GPIO_WritePin(VD56_GPIO_Port, VD56_Pin, GPIO_PIN_RESET); // VD脉冲低电平
}

/**
 * @brief 步进电机78的VD脉冲控制函数
 */
void VD_FZ_Motor78(void)
{
    // 检查是否启用次数控制
    if (vd_fz_enabled) {
        // 检查是否已达到目标次数
        if (vd_fz_count >= vd_fz_target_count) {
            return; // 已达到目标次数，不执行
        }
        vd_fz_count++; // 增加执行次数
    }
    
    // 步进电机78的VD脉冲控制 - 使用VD78引脚
    HAL_GPIO_WritePin(VD78_GPIO_Port, VD78_Pin, GPIO_PIN_SET);   // VD脉冲高电平
    
    // 延时20微秒（在中断中使用简单的循环延时）
    for(volatile int i = 0; i < 10; i++);  // 简单延时，避免在中断中使用HAL_Delay
    
    HAL_GPIO_WritePin(VD78_GPIO_Port, VD78_Pin, GPIO_PIN_RESET); // VD脉冲低电平
}

/**
 * @brief 步进电机9A的VD脉冲控制函数
 */
void VD_FZ_Motor9A(void)
{
    // 检查是否启用次数控制
    if (vd_fz_enabled) {
        // 检查是否已达到目标次数
        if (vd_fz_count >= vd_fz_target_count) {
            return; // 已达到目标次数，不执行
        }
        vd_fz_count++; // 增加执行次数
    }
    
    // 步进电机9A的VD脉冲控制 - 使用VD9A引脚
    HAL_GPIO_WritePin(VD9A_GPIO_Port, VD9A_Pin, GPIO_PIN_SET);   // VD脉冲高电平
    
    // 延时20微秒（在中断中使用简单的循环延时）
    for(volatile int i = 0; i < 10; i++);  // 简单延时，避免在中断中使用HAL_Delay
    
    HAL_GPIO_WritePin(VD9A_GPIO_Port, VD9A_Pin, GPIO_PIN_RESET); // VD脉冲低电平
}

/**
 * @brief 通用VD脉冲控制函数（保持向后兼容）
 * 默认控制所有电机的VD引脚
 */
void VD_FZ(void)
{
    // 检查是否启用次数控制
    if (vd_fz_enabled) {
        // 检查是否已达到目标次数
        if (vd_fz_count >= vd_fz_target_count) {
            return; // 已达到目标次数，不执行
        }
        vd_fz_count++; // 增加执行次数
    }
    
    // 同时控制所有电机的VD引脚（向后兼容）
    HAL_GPIO_WritePin(VD12_GPIO_Port, VD12_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(VD34_GPIO_Port, VD34_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(VD56_GPIO_Port, VD56_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(VD78_GPIO_Port, VD78_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(VD9A_GPIO_Port, VD9A_Pin, GPIO_PIN_SET);
    
    // 延时20微秒（在中断中使用简单的循环延时）
    for(volatile int i = 0; i < 10; i++);  // 简单延时，避免在中断中使用HAL_Delay
    
    HAL_GPIO_WritePin(VD12_GPIO_Port, VD12_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(VD34_GPIO_Port, VD34_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(VD56_GPIO_Port, VD56_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(VD78_GPIO_Port, VD78_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(VD9A_GPIO_Port, VD9A_Pin, GPIO_PIN_RESET);
}

void VD_IS(void)
{
    // 步进电机B的VD脉冲控制
    // 根据实际硬件连接，控制相应的GPIO引脚
    // 这里使用IRCUT相关引脚作为示例
    HAL_GPIO_WritePin(GPIOB, IRCUT_1_Pin, GPIO_PIN_SET);   // VDIS脉冲高电平
    
    // 延时20微秒
    for(volatile int i = 0; i < 100; i++);  // 简单延时
    
    HAL_GPIO_WritePin(GPIOB, IRCUT_1_Pin, GPIO_PIN_RESET); // VDIS脉冲低电平
}


#define KEY_NO							0x00000000
#define KEY_SW1_UP					0x00000001
#define	KEY_SW1_DOWN				0x00000002
#define	KEY_SW1_LEFT 				0x00000004
#define KEY_SW1_RIGHT				0x00000008
#define KEY_SW1_ENTER				0x00000010
#define KEY_SW2_UP 					0x00000020
#define KEY_SW2_DOWN				0x00000040
#define KEY_SW2_LEFT				0x00000080
#define KEY_SW2_RIGHT				0x00000100
#define KEY_SW2_ENTER				0x00000200

#define	KEY_SHORT_DOWN					0x01000000
#define KEY_SHORT_UP						0x02000000
#define	KEY_LONG_DOWN						0x04000000
#define	KEY_LONG_UP							0x08000000
#define	KEY_CONTI_DOWN					0x10000000
#define KEY_LONG_DOWN_INIT			0x20000000
#define KEY_LONG_UP_INIT		    0x40000000
#define KEY_LONG_DOWN_2S		    0x80000000
#define KEY_LONG_UP_2S			    0x00800000


/*
	自行添加各按键功能：包括：短按、长按、连按、组合按键等
*/
uint16_t u16KeyTest = 0;
/**
 * @brief 按键功能处理函数
 * @param u32KeyValue 按键值，包含按键类型和按键状态的组合
 *                     支持短按、长按、连按、组合按键等操作
 *                     通过不同按键控制步进电机的刹车/运行和换向功能
 * @retval None
 * @note 根据OutPutChennal变量选择控制的电机通道(1-5或A表示全部)
 *       SW1_UP: 控制步进电机刹车/运行状态切换
 *       SW1_DOWN: 控制步进电机换向
 */
void Key_Function(uint32_t u32KeyValue)
{
    if (u32KeyValue == (KEY_SHORT_DOWN | KEY_SW1_UP) || u32KeyValue == (KEY_CONTI_DOWN | KEY_SW1_UP))
    {   // SW1_UP   短按键或者持续按键
        // 步进刹车、运行
        Motor_ToggleBrakeRun(OutPutChennal);
    }
    else if (u32KeyValue == (KEY_SHORT_DOWN | KEY_SW1_DOWN) || u32KeyValue == (KEY_CONTI_DOWN | KEY_SW1_DOWN))
    {   // SW1_DOWN   短按键或者持续按键
        // 步进换向
        Motor_ToggleDirection(OutPutChennal);
    }
    else if (u32KeyValue == (KEY_SHORT_DOWN | KEY_SW1_LEFT) || u32KeyValue == (KEY_CONTI_DOWN | KEY_SW1_LEFT))
    {   // SW1_LEFT   短按键或者持续按键
        u16KeyTest = 3;
        // 步进减速
        Motor_DecreaseSpeed(OutPutChennal);
    }
    else if (u32KeyValue == (KEY_SHORT_DOWN | KEY_SW1_RIGHT) || u32KeyValue == (KEY_CONTI_DOWN | KEY_SW1_RIGHT))
    {   // SW1_RIGHT   短按键或者持续按键
        // 步进加速
        u16KeyTest = 4;
        Motor_IncreaseSpeed(OutPutChennal);
    }
    else if (u32KeyValue == (KEY_SHORT_DOWN | KEY_SW1_ENTER) || u32KeyValue == (KEY_CONTI_DOWN | KEY_SW1_ENTER))
    {   // SW1_ENTER   短按键或者持续按键
        // 模式选择  LED通道控制
        switch (OutPutChennal)
        {
        case 0:
            OutPutChennal = '1';
            Spi__Read(0x24, &LED1);
            LED1 < 0xc000 ? (LED1 = LED1 | 0x8000) : (LED1 = LED1 & 0x7fff);
            Spi_Write(0x24, LED1);
            break;
        case '1':
            OutPutChennal = '2';
            Spi__Read(0x24, &LED1);
            LED1 < 0xc000 ? (LED1 = LED1 | 0x8000) : (LED1 = LED1 & 0x7fff);
            Spi_Write(0x24, LED1);
            Spi__Read(0x29, &LED2);
            LED2 < 0xc000 ? (LED2 = LED2 | 0x8000) : (LED2 = LED2 & 0x7fff);
            Spi_Write(0x29, LED2);
            break;

        case '2':
            OutPutChennal = '3';
            Spi__Read(0x29, &LED2);
            LED2 < 0xc000 ? (LED2 = LED2 | 0x8000) : (LED2 = LED2 & 0x7fff);
            Spi_Write(0x29, LED2);
            Spi__Read(0x2e, &LED3);
            LED3 < 0xc000 ? (LED3 = LED3 | 0x8000) : (LED3 = LED3 & 0x7fff);
            Spi_Write(0x2e, LED3);
            break;
        case '3':
            OutPutChennal = '4';
            Spi__Read(0x2e, &LED3);
            LED3 < 0xc000 ? (LED3 = LED3 | 0x8000) : (LED3 = LED3 & 0x7fff);
            Spi_Write(0x2e, LED3);
            Spi__Read(0x33, &LED4);
            LED4 < 0xc000 ? (LED4 = LED4 | 0x8000) : (LED4 = LED4 & 0x7fff);
            Spi_Write(0x33, LED4);
            break;
        case '4':
            OutPutChennal = '5';
            Spi__Read(0x2e, &LED3);
            LED3 < 0xc000 ? (LED3 = LED3 | 0x8000) : (LED3 = LED3 & 0x7fff);
            Spi_Write(0x2e, LED3);
            break;
        case '5':
            OutPutChennal = 'A';
            Spi__Read(0x24, &LED1);
            LED1 < 0xc000 ? (LED1 = LED1 | 0x8000) : (LED1 = LED1 & 0x7fff);
            Spi_Write(0x24, LED1);
            Spi__Read(0x29, &LED2);
            LED2 < 0xc000 ? (LED2 = LED2 | 0x8000) : (LED2 = LED2 & 0x7fff);
            Spi_Write(0x29, LED2);
            Spi__Read(0x2e, &LED3);
            LED3 < 0xc000 ? (LED3 = LED3 | 0x8000) : (LED3 = LED3 & 0x7fff);
            Spi_Write(0x2e, LED3);
            Spi__Read(0x33, &LED4);
            LED4 < 0xc000 ? (LED4 = LED4 | 0x8000) : (LED4 = LED4 & 0x7fff);
            Spi_Write(0x33, LED4);
            break;
        case 'A':
            OutPutChennal = 0;
            Spi__Read(0x24, &LED1);
            LED1 < 0xc000 ? (LED1 = LED1 | 0x8000) : (LED1 = LED1 & 0x7fff);
            Spi_Write(0x24, LED1);
            Spi__Read(0x29, &LED2);
            LED2 < 0xc000 ? (LED2 = LED2 | 0x8000) : (LED2 = LED2 & 0x7fff);
            Spi_Write(0x29, LED2);
            break;
        default:
            break;
        }
        //	OUT_CH_LED(OutPutChennal);
    }
    else if (u32KeyValue == (KEY_SHORT_DOWN | KEY_SW2_LEFT) || u32KeyValue == (KEY_CONTI_DOWN | KEY_SW2_LEFT))
    {   // SW2_LEFT	短按键或者持续按键
        // 减小电流
        switch (OutPutChennal)
        {
        case '1':
            Spi__Read(0x23, &PPWAB);
            PPWAB = PPWAB & 0X00FF;
            PPWAB = (PPWAB <= 0X30) ? 0x30 : (PPWAB - 0x04);
            Spi_Write(0x23, (PPWAB << 8) | PPWAB);
            break;
        case '2':
            Spi__Read(0x28, &PPWCD);
            PPWCD = PPWCD & 0X00FF;
            PPWCD = (PPWCD <= 0X30) ? 0x30 : (PPWCD - 0x04);
            Spi_Write(0x28, (PPWCD << 8) | PPWCD);
            break;
        case '3':
            Spi__Read(0x2d, &PPWEF);
            PPWEF = PPWEF & 0X00FF;
            PPWEF = (PPWEF <= 0X30) ? 0x30 : (PPWEF - 0x04);
            Spi_Write(0x2d, (PPWEF << 8) | PPWEF);
            break;
        case '4':
            Spi__Read(0x32, &PPWGH);
            PPWGH = PPWGH & 0X00FF;
            PPWGH = (PPWGH <= 0X30) ? 0x30 : (PPWGH - 0x04);
            Spi_Write(0x32, (PPWGH << 8) | PPWGH);
            break;
        case '5':
            Spi__Read(0x37, &PPWIJ);
            PPWIJ = PPWIJ & 0X00FF;
            PPWIJ = (PPWIJ <= 0X30) ? 0x30 : (PPWIJ - 0x04);
            Spi_Write(0x37, (PPWIJ << 8) | PPWIJ);
            break;
        case 'A':
            Spi__Read(0x23, &PPWAB);
            PPWAB = PPWAB & 0X00FF;
            PPWAB = (PPWAB <= 0X30) ? 0x30 : (PPWAB - 0x04);
            Spi_Write(0x23, (PPWAB << 8) | PPWAB);
            Spi__Read(0x28, &PPWCD);
            PPWCD = PPWCD & 0X00FF;
            PPWCD = (PPWCD <= 0X30) ? 0x30 : (PPWCD - 0x04);
            Spi_Write(0x28, (PPWCD << 8) | PPWCD);
            Spi__Read(0x2d, &PPWEF);
            PPWEF = PPWEF & 0X00FF;
            PPWEF = (PPWEF <= 0X30) ? 0x30 : (PPWEF - 0x04);
            Spi_Write(0x2d, (PPWEF << 8) | PPWEF);
            Spi__Read(0x32, &PPWGH);
            PPWGH = PPWGH & 0X00FF;
            PPWGH = (PPWGH <= 0X30) ? 0x30 : (PPWGH - 0x04);
            Spi_Write(0x32, (PPWGH << 8) | PPWGH);
            Spi__Read(0x37, &PPWIJ);
            PPWIJ = PPWIJ & 0X00FF;
            PPWIJ = (PPWIJ <= 0X30) ? 0x30 : (PPWIJ - 0x04);
            Spi_Write(0x37, (PPWIJ << 8) | PPWIJ);
            break;
        default:
            break;
        }
    }
    else if (u32KeyValue == (KEY_SHORT_DOWN | KEY_SW2_RIGHT) || u32KeyValue == (KEY_CONTI_DOWN | KEY_SW2_RIGHT))
    {   // SW2_RIGHT	短按键或者持续按键
        // 增大电流
        switch (OutPutChennal)
        {
        case '1':
            Spi__Read(0x23, &PPWAB);
            PPWAB = PPWAB & 0X00FF;
            PPWAB = (PPWAB >= 0XF0) ? 0xFF : (PPWAB + 0x04);
            Spi_Write(0x23, (PPWAB << 8) | PPWAB);
            break;
        case '2':
            Spi__Read(0x28, &PPWCD);
            PPWCD = PPWCD & 0X00FF;
            PPWCD = (PPWCD >= 0XF0) ? 0xFF : (PPWCD + 0x04);
            Spi_Write(0x28, (PPWCD << 8) | PPWCD);
            break;
        case '3':
            Spi__Read(0x2d, &PPWEF);
            PPWEF = PPWEF & 0X00FF;
            PPWEF = (PPWEF >= 0XF0) ? 0xFF : (PPWEF + 0x04);
            Spi_Write(0x2d, (PPWEF << 8) | PPWEF);
            break;
        case '4':
            Spi__Read(0x32, &PPWGH);
            PPWGH = PPWGH & 0X00FF;
            PPWGH = (PPWGH >= 0XF0) ? 0xFF : (PPWGH + 0x04);
            Spi_Write(0x32, (PPWGH << 8) | PPWGH);
            break;
        case '5':
            Spi__Read(0x37, &PPWIJ);
            PPWIJ = PPWIJ & 0X00FF;
            PPWIJ = (PPWIJ >= 0XF0) ? 0xFF : (PPWIJ + 0x04);
            Spi_Write(0x37, (PPWIJ << 8) | PPWIJ);
            break;
        case 'A':
            Spi__Read(0x23, &PPWAB);
            PPWAB = PPWAB & 0X00FF;
            PPWAB = (PPWAB >= 0XF0) ? 0xFF : (PPWAB + 0x04);
            Spi_Write(0x23, (PPWAB << 8) | PPWAB);
            Spi__Read(0x28, &PPWCD);
            PPWCD = PPWCD & 0X00FF;
            PPWCD = (PPWCD >= 0XF0) ? 0xFF : (PPWCD + 0x04);
            Spi_Write(0x28, (PPWCD << 8) | PPWCD);
            Spi__Read(0x2d, &PPWEF);
            PPWEF = PPWEF & 0X00FF;
            PPWEF = (PPWEF >= 0XF0) ? 0xFF : (PPWEF + 0x04);
            Spi_Write(0x2d, (PPWEF << 8) | PPWEF);
            Spi__Read(0x32, &PPWGH);
            PPWGH = PPWGH & 0X00FF;
            PPWGH = (PPWGH >= 0XF0) ? 0xFF : (PPWGH + 0x04);
            Spi_Write(0x32, (PPWGH << 8) | PPWGH);
            Spi__Read(0x37, &PPWIJ);
            PPWIJ = PPWIJ & 0X00FF;
            PPWIJ = (PPWIJ >= 0XF0) ? 0xFF : (PPWIJ + 0x04);
            Spi_Write(0x37, (PPWIJ << 8) | PPWIJ);
            break;
        default:
            break;
        }
    }
    else if (u32KeyValue == (KEY_SHORT_DOWN | KEY_SW2_ENTER) || u32KeyValue == (KEY_CONTI_DOWN | KEY_SW2_ENTER))
    {   // SW2_RIGHT	短按键或者持续按键
        // 模式选择
        switch (OutPutChennal2)
        {
        case 0:
            OutPutChennal2 = '1';
            break;
        case '1':
            OutPutChennal2 = '2';
            break;
        case '2':
            OutPutChennal2 = 0;
            Spi_Write(0x20, 0x0001);
            Spi_Write(0x3b, 0x0000);
            Spi_Write(0x3c, 0x0000);
            break;

        default:
            break;
        }
    }
}

/**
 * @brief SPI测试函数 - 读取所有寄存器并通过串口输出
 * @param None
 * @retval None
 */
void SPI_Test_ReadAllRegisters(void)
{
    uint16_t regData;
    uint8_t addr;
    char buffer[64];
    
    UART_Transmit_Str(&huart3, (uint8_t*)"\r\n=== SPI Register Dump Start ===\r\n");
    
    // 读取常用的寄存器地址范围 (0x00-0x3F)
    for(addr = 0x00; addr <= 0x3F; addr++)
    {
        Spi__Read(addr, &regData);
        sprintf(buffer, "Reg[0x%02X] = 0x%04X\r\n", addr, regData);
        UART_Transmit_Str(&huart3, (uint8_t*)buffer);
        HAL_Delay(1); // 短暂延时确保SPI通信稳定
    }
    
    UART_Transmit_Str(&huart3, (uint8_t*)"=== SPI Register Dump End ===\r\n\r\n");
}

/**
 * @brief SPI测试函数 - 基本读写测试
 * @param None
 * @retval None
 */
void SPI_Test_Dump(void)
{
    uint16_t testData, readData;
    uint8_t testAddr = 0x25; // 使用一个测试寄存器地址
    char buffer[64];
    
    UART_Transmit_Str(&huart1, (uint8_t*)"\r\n=== SPI Basic Test Start ===\r\n");
    
    // 读取原始数据
    Spi__Read(testAddr, &readData);
    sprintf(buffer, "Original Reg[0x%02X] = 0x%04X\r\n", testAddr, readData);
    UART_Transmit_Str(&huart1, (uint8_t*)buffer);
    
    // 写入测试数据
    testData = 0x1234;
    sprintf(buffer, "Writing 0x%04X to Reg[0x%02X]\r\n", testData, testAddr);
    UART_Transmit_Str(&huart1, (uint8_t*)buffer);
    Spi_Write(testAddr, testData);
    HAL_Delay(1);
    
    // 读回验证
    Spi__Read(testAddr, &readData);
    sprintf(buffer, "Read back Reg[0x%02X] = 0x%04X\r\n", testAddr, readData);
    UART_Transmit_Str(&huart1, (uint8_t*)buffer);
    
    if(readData == testData)
    {
        UART_Transmit_Str(&huart1, (uint8_t*)"SPI Write/Read Test: PASS\r\n");
    }
    else
    {
        UART_Transmit_Str(&huart1, (uint8_t*)"SPI Write/Read Test: FAIL\r\n");
    }
    
    // 测试SPI字节传输
    UART_Transmit_Str(&huart1, (uint8_t*)"\r\nTesting SPI Byte Transfer:\r\n");
    uint8_t testByte = 0xAA;
    uint8_t receivedByte = Spi_ReadWriteByte(testByte);
    sprintf(buffer, "Sent: 0x%02X, Received: 0x%02X\r\n", testByte, receivedByte);
    UART_Transmit_Str(&huart1, (uint8_t*)buffer);
    
    UART_Transmit_Str(&huart1, (uint8_t*)"=== SPI Basic Test End ===\r\n\r\n");
}

/**
 * @brief 切换电机刹车/运行状态
 * @param channel 电机通道 ('1'-'5', 'A'表示所有通道)
 * @retval None
 */
void Motor_ToggleBrakeRun(char channel)
{
    uint16_t value;
    switch (channel)
    {
    case '1':
        Spi__Read(0x24, &value);
        value = ((value & 0x2000) == 0) ? (value | 0x2000) : (value & 0xdfff);
        Spi_Write(0x24, value);
        break;
    case '2':
        Spi__Read(0x29, &value);
        value = ((value & 0x2000) == 0) ? (value | 0x2000) : (value & 0xdfff);
        Spi_Write(0x29, value);
        break;
    case '3':
        Spi__Read(0x2e, &value);
        value = ((value & 0x2000) == 0) ? (value | 0x2000) : (value & 0xdfff);
        Spi_Write(0x2e, value);
        break;
    case '4':
        Spi__Read(0x33, &value);
        value = ((value & 0x2000) == 0) ? (value | 0x2000) : (value & 0xdfff);
        Spi_Write(0x33, value);
        break;
    case '5':
        Spi__Read(0x38, &value);
        value = ((value & 0x2000) == 0) ? (value | 0x2000) : (value & 0xdfff);
        Spi_Write(0x38, value);
        break;
    case 'A':
        // 控制所有电机
        Spi__Read(0x24, &value);
        value = ((value & 0x2000) == 0) ? (value | 0x2000) : (value & 0xdfff);
        Spi_Write(0x24, value);
        Spi__Read(0x29, &value);
        value = ((value & 0x2000) == 0) ? (value | 0x2000) : (value & 0xdfff);
        Spi_Write(0x29, value);
        Spi__Read(0x2e, &value);
        value = ((value & 0x2000) == 0) ? (value | 0x2000) : (value & 0xdfff);
        Spi_Write(0x2e, value);
        Spi__Read(0x33, &value);
        value = ((value & 0x2000) == 0) ? (value | 0x2000) : (value & 0xdfff);
        Spi_Write(0x33, value);
        Spi__Read(0x38, &value);
        value = ((value & 0x2000) == 0) ? (value | 0x2000) : (value & 0xdfff);
        Spi_Write(0x38, value);
        break;
    default:
        break;
    }
}



/**
 * @brief 设置步进电机目标步数
 * @param stepsA 步进电机A的目标步数
 * @param stepsB 步进电机B的目标步数
 * @retval None
 */
void Stepper_SetTargetSteps(uint16_t stepsA, uint16_t stepsB)
{
    target_steps_A = stepsA;
    target_steps_B = stepsB;
    current_steps_A = 0;
    current_steps_B = 0;
    
    // 更新PSUMA和PSUMB变量
    PSUMA = stepsA;
    PSUMB = stepsB;
}

/**
 * @brief 启动步进电机运行
 * @retval None
 */
void Stepper_Start(void)
{
    stepper_running = true;
    current_steps_A = 0;
    current_steps_B = 0;
}

/**
 * @brief 停止步进电机运行
 * @retval None
 */
void Stepper_Stop(void)
{
    stepper_running = false;
    current_steps_A = 0;
    current_steps_B = 0;
}

/**
 * @brief 获取步进电机A当前步数
 * @retval 当前步数
 */
uint16_t Stepper_GetStepsA(void)
{
    return current_steps_A;
}

/**
 * @brief 获取步进电机B当前步数
 * @retval 当前步数
 */
uint16_t Stepper_GetStepsB(void)
{
    return current_steps_B;
}

/**
 * @brief 检查步进电机是否正在运行
 * @retval true: 正在运行, false: 已停止
 */
bool Stepper_IsRunning(void)
{
    return stepper_running && (current_steps_A < target_steps_A || current_steps_B < target_steps_B);
}

/**
 * @brief 步进电机单步执行函数（在定时器中断中调用）
 * @retval None
 * @note 此函数应在VD_Stepper_Control中调用
 */
void Stepper_ExecuteStep(void)
{
    if (!stepper_running) {
        return;
    }
    
    // 执行步进电机A的一步
    if (current_steps_A < target_steps_A) {
        VD_FZ();
        current_steps_A++;
    }
    
    // 执行步进电机B的一步
    if (current_steps_B < target_steps_B) {
        VD_IS();
        current_steps_B++;
    }
    
    // 检查是否完成所有步数
    if (current_steps_A >= target_steps_A && current_steps_B >= target_steps_B) {
        stepper_running = false;
    }
}

/**
 * @brief 电机减速
 * @param channel 电机通道 ('1'-'5', 'A'表示所有通道)
 * @retval None
 */
void Motor_DecreaseSpeed(char channel)
{
    switch (channel)
    {
    case '1':
        Spi__Read(0x25, &INTCTAB);
        INTCTAB = (INTCTAB >= 0X4000) ? 0x4000 : (INTCTAB + 0x0080);
        Spi_Write(0x25, INTCTAB);
        break;
    case '2':
        Spi__Read(0x2A, &INTCTCD);
        INTCTCD = (INTCTCD >= 0X4000) ? 0x4000 : (INTCTCD + 0x0080);
        Spi_Write(0x2A, INTCTCD);
        break;
    case '3':
        Spi__Read(0x2F, &INTCTEF);
        INTCTEF = (INTCTEF >= 0X4000) ? 0x4000 : (INTCTEF + 0x0080);
        Spi_Write(0x2F, INTCTEF);
        break;
    case '4':
        Spi__Read(0x34, &INTCTGH);
        INTCTGH = (INTCTGH >= 0X4000) ? 0x4000 : (INTCTGH + 0x0080);
        Spi_Write(0x34, INTCTGH);
        break;
    case '5':
        Spi__Read(0x39, &INTCTIJ);
        INTCTIJ = (INTCTIJ >= 0X4000) ? 0x4000 : (INTCTIJ + 0x0080);
        Spi_Write(0x39, INTCTIJ);
        break;
    case 'A':
        // 控制所有电机
        Spi__Read(0x25, &INTCTAB);
        INTCTAB = (INTCTAB >= 0X4000) ? 0x4000 : (INTCTAB + 0x0080);
        Spi_Write(0x25, INTCTAB);
        Spi__Read(0x2A, &INTCTCD);
        INTCTCD = (INTCTCD >= 0X4000) ? 0x4000 : (INTCTCD + 0x0080);
        Spi_Write(0x2A, INTCTCD);
        Spi__Read(0x2F, &INTCTEF);
        INTCTEF = (INTCTEF >= 0X4000) ? 0x4000 : (INTCTEF + 0x0080);
        Spi_Write(0x2F, INTCTEF);
        Spi__Read(0x34, &INTCTGH);
        INTCTGH = (INTCTGH >= 0X4000) ? 0x4000 : (INTCTGH + 0x0080);
        Spi_Write(0x34, INTCTGH);
        Spi__Read(0x39, &INTCTIJ);
        INTCTIJ = (INTCTIJ >= 0X4000) ? 0x4000 : (INTCTIJ + 0x0080);
        Spi_Write(0x39, INTCTIJ);
        break;
    default:
        break;
    }
}

/**
 * @brief 电机加速
 * @param channel 电机通道 ('1'-'5', 'A'表示所有通道)
 * @retval None
 */
void Motor_IncreaseSpeed(char channel)
{
    switch (channel)
    {
    case '1':
        Spi__Read(0x25, &INTCTAB);
        INTCTAB = (INTCTAB <= 0X0080) ? 0x0080 : (INTCTAB - 0x0080);
        Spi_Write(0x25, INTCTAB);
        break;
    case '2':
        Spi__Read(0x2A, &INTCTCD);
        INTCTCD = (INTCTCD <= 0X0080) ? 0x0080 : (INTCTCD - 0x0080);
        Spi_Write(0x2A, INTCTCD);
        break;
    case '3':
        Spi__Read(0x2F, &INTCTEF);
        INTCTEF = (INTCTEF <= 0X0080) ? 0x0080 : (INTCTEF - 0x0080);
        Spi_Write(0x2F, INTCTEF);
        break;
    case '4':
        Spi__Read(0x34, &INTCTGH);
        INTCTGH = (INTCTGH <= 0X0080) ? 0x0080 : (INTCTGH - 0x0080);
        Spi_Write(0x34, INTCTGH);
        break;
    case '5':
        Spi__Read(0x39, &INTCTIJ);
        INTCTIJ = (INTCTIJ <= 0X0080) ? 0x0080 : (INTCTIJ - 0x0080);
        Spi_Write(0x39, INTCTIJ);
        break;
    case 'A':
        // 控制所有电机
        Spi__Read(0x25, &INTCTAB);
        INTCTAB = (INTCTAB <= 0X0080) ? 0x0080 : (INTCTAB - 0x0080);
        Spi_Write(0x25, INTCTAB);
        Spi__Read(0x2A, &INTCTCD);
        INTCTCD = (INTCTCD <= 0X0080) ? 0x0080 : (INTCTCD - 0x0080);
        Spi_Write(0x2A, INTCTCD);
        Spi__Read(0x2F, &INTCTEF);
        INTCTEF = (INTCTEF <= 0X0080) ? 0x0080 : (INTCTEF - 0x0080);
        Spi_Write(0x2F, INTCTEF);
        Spi__Read(0x34, &INTCTGH);
        INTCTGH = (INTCTGH <= 0X0080) ? 0x0080 : (INTCTGH - 0x0080);
        Spi_Write(0x34, INTCTGH);
        Spi__Read(0x39, &INTCTIJ);
        INTCTIJ = (INTCTIJ <= 0X0080) ? 0x0080 : (INTCTIJ - 0x0080);
        Spi_Write(0x39, INTCTIJ);
        break;
    default:
        break;
    }
}

/**
 * @brief 切换电机方向
 * @param channel 电机通道 ('1'-'5', 'A'表示所有通道)
 * @retval None
 */
void Motor_ToggleDirection(char channel)
{
    uint16_t value;
    switch (channel)
    {
    case '1':
        Spi__Read(0x24, &value);
        value = ((value & 0x1000) == 0) ? (value | 0x1000) : (value & 0xefff);
        Spi_Write(0x24, value);
        break;
    case '2':
        Spi__Read(0x29, &value);
        value = ((value & 0x1000) == 0) ? (value | 0x1000) : (value & 0xefff);
        Spi_Write(0x29, value);
        break;
    case '3':
        Spi__Read(0x2e, &value);
        value = ((value & 0x1000) == 0) ? (value | 0x1000) : (value & 0xefff);
        Spi_Write(0x2e, value);
        break;
    case '4':
        Spi__Read(0x33, &value);
        value = ((value & 0x1000) == 0) ? (value | 0x1000) : (value & 0xefff);
        Spi_Write(0x33, value);
        break;
    case '5':
        Spi__Read(0x38, &value);
        value = ((value & 0x1000) == 0) ? (value | 0x1000) : (value & 0xefff);
        Spi_Write(0x38, value);
        break;
    case 'A':
        // 控制所有电机
        Spi__Read(0x24, &value);
        value = ((value & 0x1000) == 0) ? (value | 0x1000) : (value & 0xefff);
        Spi_Write(0x24, value);
        Spi__Read(0x29, &value);
        value = ((value & 0x1000) == 0) ? (value | 0x1000) : (value & 0xefff);
        Spi_Write(0x29, value);
        Spi__Read(0x2e, &value);
        value = ((value & 0x1000) == 0) ? (value | 0x1000) : (value & 0xefff);
        Spi_Write(0x2e, value);
        Spi__Read(0x33, &value);
        value = ((value & 0x1000) == 0) ? (value | 0x1000) : (value & 0xefff);
        Spi_Write(0x33, value);
        Spi__Read(0x38, &value);
        value = ((value & 0x1000) == 0) ? (value | 0x1000) : (value & 0xefff);
        Spi_Write(0x38, value);
        break;
    default:
        break;
    }
}

/**
 * @brief 步进电机控制测试函数
 * @retval None
 * @note 演示如何使用步进电机控制系统
 */
void Stepper_Test_Demo(void)
{
    char buffer[100];
    
    // 设置步进电机A运行100步，步进电机B运行50步
    Stepper_SetTargetSteps(100, 50);
    
    sprintf(buffer, "设置目标步数: A=%d, B=%d\r\n", 100, 50);
    UART_Transmit_Str(&huart1, (uint8_t*)buffer);
    
    // 启动步进电机
    Stepper_Start();
    UART_Transmit_Str(&huart1, (uint8_t*)"步进电机已启动\r\n");
    
    // 等待步进电机完成运行
    while(Stepper_IsRunning()) {
        HAL_Delay(100);  // 等待100ms
        
        // 输出当前进度
        sprintf(buffer, "当前步数: A=%d/%d, B=%d/%d\r\n", 
                Stepper_GetStepsA(), 100,
                Stepper_GetStepsB(), 50);
        UART_Transmit_Str(&huart1, (uint8_t*)buffer);
    }
    
    UART_Transmit_Str(&huart1, (uint8_t*)"步进电机运行完成\r\n");
}

/**
 * @brief 设置VD_FZ函数的目标执行次数
 * @param count 目标执行次数
 */
void VD_FZ_SetTargetCount(uint16_t count)
{
    vd_fz_target_count = count;
    vd_fz_count = 0; // 重置当前次数
}

/**
 * @brief 启用VD_FZ次数控制
 */
void VD_FZ_Enable(void)
{
    vd_fz_enabled = true;
}

/**
 * @brief 禁用VD_FZ次数控制
 */
void VD_FZ_Disable(void)
{
    vd_fz_enabled = false;
}

/**
 * @brief 获取VD_FZ当前执行次数
 * @return 当前执行次数
 */
uint16_t VD_FZ_GetCurrentCount(void)
{
    return vd_fz_count;
}

/**
 * @brief 获取VD_FZ目标执行次数
 * @return 目标执行次数
 */
uint16_t VD_FZ_GetTargetCount(void)
{
    return vd_fz_target_count;
}

/**
 * @brief 重置VD_FZ执行次数
 */
void VD_FZ_ResetCount(void)
{
    vd_fz_count = 0;
}

/**
 * @brief 检查VD_FZ是否已完成目标次数
 * @return true: 已完成, false: 未完成
 */
bool VD_FZ_IsCompleted(void)
{
    if (!vd_fz_enabled) {
        return false; // 未启用次数控制时返回false
    }
    return (vd_fz_count >= vd_fz_target_count);
}

/**
 * @brief 通过0x24寄存器设置PSUMA的值
 * @param psuma_value PSUMA的值 (0-4095, 12位)
 * @note 0x24寄存器的0-11位对应PSUMA的值
 */
void Set_PSUMA_Via_Register(uint16_t psuma_value)
{
    uint16_t reg_value;
    
    // 限制PSUMA值在12位范围内 (0-4095)
    psuma_value &= 0x0FFF;
    
    // 读取0x24寄存器当前值
    Spi__Read(0x24, &reg_value);
    
    // 清除0-11位 (PSUMA位)
    reg_value &= 0xF000;
    
    // 设置新的PSUMA值到0-11位
    reg_value |= psuma_value;
    
    // 写回0x24寄存器
    Spi_Write(0x24, reg_value);
    
    // 同时更新全局变量PSUMA
    PSUMA = psuma_value;
}

/**
 * @brief 从0x24寄存器读取PSUMA的值
 * @return PSUMA的当前值 (0-4095)
 */
uint16_t Get_PSUMA_From_Register(void)
{
    uint16_t reg_value;
    
    // 读取0x24寄存器值
    Spi__Read(0x24, &reg_value);
    
    // 提取0-11位作为PSUMA值
    uint16_t psuma_value = reg_value & 0x0FFF;
    
    // 同时更新全局变量PSUMA
    PSUMA = psuma_value;
    
    return psuma_value;
}

// ==================== 步进电机12 API函数 ====================
/**
 * @brief 设置步进电机12的目标位置
 * @param target_pos 目标位置值
 */
void Motor12_SetTargetPosition(int32_t target_pos)
{
		Motor12_EnablePositionControl();
    motor12_target_position = target_pos;
}

/**
 * @brief 获取步进电机12的当前位置
 * @return 当前位置值
 */
int32_t Motor12_GetCurrentPosition(void)
{
    return motor12_current_position;
}

/**
 * @brief 获取步进电机12的目标位置
 * @return 目标位置值
 */
int32_t Motor12_GetTargetPosition(void)
{
    return motor12_target_position;
}

/**
 * @brief 设置步进电机12的当前位置 (用于位置校准)
 * @param current_pos 当前位置值
 */
void Motor12_SetCurrentPosition(int32_t current_pos)
{
    motor12_current_position = current_pos;
}

/**
 * @brief 启用步进电机12位置控制
 */
void Motor12_EnablePositionControl(void)
{
    motor12_position_control_enabled = true;
}

/**
 * @brief 禁用步进电机12位置控制
 */
void Motor12_DisablePositionControl(void)
{
    motor12_position_control_enabled = false;
}

/**
 * @brief 检查步进电机12是否已到达目标位置
 * @return true: 已到达, false: 未到达
 */
bool Motor12_IsAtTargetPosition(void)
{
    return (motor12_current_position == motor12_target_position);
}

/**
 * @brief 获取步进电机12的位置误差
 * @return 位置误差 (目标位置 - 当前位置)
 */
int32_t Motor12_GetPositionError(void)
{
    return (motor12_target_position - motor12_current_position);
}

/**
 * @brief 设置步进电机12每个周期的最大步数
 * @param max_steps 最大步数 (1-4095)
 */
void Motor12_SetMaxStepsPerCycle(uint16_t max_steps)
{
    if (max_steps > 0 && max_steps <= 0x0FFF) {
        motor12_max_steps_per_cycle = max_steps;
    }
}

/**
 * @brief 获取步进电机12每个周期的最大步数
 * @return 最大步数
 */
uint16_t Motor12_GetMaxStepsPerCycle(void)
{
    return motor12_max_steps_per_cycle;
}

/**
 * @brief 步进电机12位置复位 (设置当前位置和目标位置为0)
 */
void Motor12_ResetPosition(void)
{
    motor12_current_position = 0;
    motor12_target_position = 0;
}

// ==================== 步进电机34 API函数 ====================
/**
 * @brief 设置步进电机34的目标位置
 * @param target_pos 目标位置值
 */
void Motor34_SetTargetPosition(int32_t target_pos)
{
		Motor34_EnablePositionControl();
    motor34_target_position = target_pos;
}

/**
 * @brief 获取步进电机34的当前位置
 * @return 当前位置值
 */
int32_t Motor34_GetCurrentPosition(void)
{
    return motor34_current_position;
}

/**
 * @brief 获取步进电机34的目标位置
 * @return 目标位置值
 */
int32_t Motor34_GetTargetPosition(void)
{
    return motor34_target_position;
}

/**
 * @brief 设置步进电机34的当前位置 (用于位置校准)
 * @param current_pos 当前位置值
 */
void Motor34_SetCurrentPosition(int32_t current_pos)
{
    motor34_current_position = current_pos;
}

/**
 * @brief 启用步进电机34位置控制
 */
void Motor34_EnablePositionControl(void)
{
    motor34_position_control_enabled = true;
}

/**
 * @brief 禁用步进电机34位置控制
 */
void Motor34_DisablePositionControl(void)
{
    motor34_position_control_enabled = false;
}

/**
 * @brief 检查步进电机34是否已到达目标位置
 * @return true: 已到达, false: 未到达
 */
bool Motor34_IsAtTargetPosition(void)
{
    return (motor34_current_position == motor34_target_position);
}

/**
 * @brief 获取步进电机34的位置误差
 * @return 位置误差 (目标位置 - 当前位置)
 */
int32_t Motor34_GetPositionError(void)
{
    return (motor34_target_position - motor34_current_position);
}

/**
 * @brief 设置步进电机34每个周期的最大步数
 * @param max_steps 最大步数 (1-4095)
 */
void Motor34_SetMaxStepsPerCycle(uint16_t max_steps)
{
    if (max_steps > 0 && max_steps <= 0x0FFF) {
        motor34_max_steps_per_cycle = max_steps;
    }
}

/**
 * @brief 获取步进电机34每个周期的最大步数
 * @return 最大步数
 */
uint16_t Motor34_GetMaxStepsPerCycle(void)
{
    return motor34_max_steps_per_cycle;
}

/**
 * @brief 步进电机34位置复位 (设置当前位置和目标位置为0)
 */
void Motor34_ResetPosition(void)
{
    motor34_current_position = 0;
    motor34_target_position = 0;
}

// ==================== 步进电机56 API函数 ====================
/**
 * @brief 设置步进电机56的目标位置
 * @param target_pos 目标位置值
 */
void Motor56_SetTargetPosition(int32_t target_pos)
{
		Motor56_EnablePositionControl();
    motor56_target_position = target_pos;
}

/**
 * @brief 获取步进电机56的当前位置
 * @return 当前位置值
 */
int32_t Motor56_GetCurrentPosition(void)
{
    return motor56_current_position;
}

/**
 * @brief 获取步进电机56的目标位置
 * @return 目标位置值
 */
int32_t Motor56_GetTargetPosition(void)
{
    return motor56_target_position;
}

/**
 * @brief 设置步进电机56的当前位置 (用于位置校准)
 * @param current_pos 当前位置值
 */
void Motor56_SetCurrentPosition(int32_t current_pos)
{
    motor56_current_position = current_pos;
}

/**
 * @brief 启用步进电机56位置控制
 */
void Motor56_EnablePositionControl(void)
{
    motor56_position_control_enabled = true;
}

/**
 * @brief 禁用步进电机56位置控制
 */
void Motor56_DisablePositionControl(void)
{
    motor56_position_control_enabled = false;
}

/**
 * @brief 检查步进电机56是否已到达目标位置
 * @return true: 已到达, false: 未到达
 */
bool Motor56_IsAtTargetPosition(void)
{
    return (motor56_current_position == motor56_target_position);
}

/**
 * @brief 获取步进电机56的位置误差
 * @return 位置误差 (目标位置 - 当前位置)
 */
int32_t Motor56_GetPositionError(void)
{
    return (motor56_target_position - motor56_current_position);
}

/**
 * @brief 设置步进电机56每个周期的最大步数
 * @param max_steps 最大步数 (1-4095)
 */
void Motor56_SetMaxStepsPerCycle(uint16_t max_steps)
{
    if (max_steps > 0 && max_steps <= 0x0FFF) {
        motor56_max_steps_per_cycle = max_steps;
    }
}

/**
 * @brief 获取步进电机56每个周期的最大步数
 * @return 最大步数
 */
uint16_t Motor56_GetMaxStepsPerCycle(void)
{
    return motor56_max_steps_per_cycle;
}

/**
 * @brief 步进电机56位置复位 (设置当前位置和目标位置为0)
 */
void Motor56_ResetPosition(void)
{
    motor56_current_position = 0;
    motor56_target_position = 0;
}

// ==================== 步进电机78 API函数 ====================
/**
 * @brief 设置步进电机78的目标位置
 * @param target_pos 目标位置值
 */
void Motor78_SetTargetPosition(int32_t target_pos)
{
		Motor78_EnablePositionControl();
    motor78_target_position = target_pos;
}

/**
 * @brief 获取步进电机78的当前位置
 * @return 当前位置值
 */
int32_t Motor78_GetCurrentPosition(void)
{
    return motor78_current_position;
}

/**
 * @brief 获取步进电机78的目标位置
 * @return 目标位置值
 */
int32_t Motor78_GetTargetPosition(void)
{
    return motor78_target_position;
}

/**
 * @brief 设置步进电机78的当前位置 (用于位置校准)
 * @param current_pos 当前位置值
 */
void Motor78_SetCurrentPosition(int32_t current_pos)
{
    motor78_current_position = current_pos;
}

/**
 * @brief 启用步进电机78位置控制
 */
void Motor78_EnablePositionControl(void)
{
    motor78_position_control_enabled = true;
}

/**
 * @brief 禁用步进电机78位置控制
 */
void Motor78_DisablePositionControl(void)
{
    motor78_position_control_enabled = false;
}

/**
 * @brief 检查步进电机78是否已到达目标位置
 * @return true: 已到达, false: 未到达
 */
bool Motor78_IsAtTargetPosition(void)
{
    return (motor78_current_position == motor78_target_position);
}

/**
 * @brief 获取步进电机78的位置误差
 * @return 位置误差 (目标位置 - 当前位置)
 */
int32_t Motor78_GetPositionError(void)
{
    return (motor78_target_position - motor78_current_position);
}

/**
 * @brief 设置步进电机78每个周期的最大步数
 * @param max_steps 最大步数 (1-4095)
 */
void Motor78_SetMaxStepsPerCycle(uint16_t max_steps)
{
    if (max_steps > 0 && max_steps <= 0x0FFF) {
        motor78_max_steps_per_cycle = max_steps;
    }
}

/**
 * @brief 获取步进电机78每个周期的最大步数
 * @return 最大步数
 */
uint16_t Motor78_GetMaxStepsPerCycle(void)
{
    return motor78_max_steps_per_cycle;
}

/**
 * @brief 步进电机78位置复位 (设置当前位置和目标位置为0)
 */
void Motor78_ResetPosition(void)
{
    motor78_current_position = 0;
    motor78_target_position = 0;
}
// ==================== Motor9A API Functions ====================

/**
 * @brief 设置步进电机9A的目标位置
 * @param target_pos 目标位置
 */
void Motor9A_SetTargetPosition(int32_t target_pos)
{
    Motor9A_EnablePositionControl();
    motor9a_target_position = target_pos;
}

/**
 * @brief 获取步进电机9A的当前位置
 * @return 当前位置
 */
int32_t Motor9A_GetCurrentPosition(void)
{
    return motor9a_current_position;
}

/**
 * @brief 获取步进电机9A的目标位置
 * @return 目标位置
 */
int32_t Motor9A_GetTargetPosition(void)
{
    return motor9a_target_position;
}

/**
 * @brief 设置步进电机9A的当前位置
 * @param current_pos 当前位置
 */
void Motor9A_SetCurrentPosition(int32_t current_pos)
{
    motor9a_current_position = current_pos;
}

/**
 * @brief 启用步进电机9A的位置控制
 */
void Motor9A_EnablePositionControl(void)
{
    motor9a_position_control_enabled = true;
}

/**
 * @brief 禁用步进电机9A的位置控制
 */
void Motor9A_DisablePositionControl(void)
{
    motor9a_position_control_enabled = false;
}

/**
 * @brief 检查步进电机9A是否已到达目标位置
 * @return true 如果已到达目标位置，false 否则
 */
bool Motor9A_IsAtTargetPosition(void)
{
    return motor9a_current_position == motor9a_target_position;
}

/**
 * @brief 获取步进电机9A的位置误差
 * @return 位置误差（目标位置 - 当前位置）
 */
int32_t Motor9A_GetPositionError(void)
{
    return motor9a_target_position - motor9a_current_position;
}

/**
 * @brief 设置步进电机9A每个周期的最大步数
 * @param max_steps 最大步数
 */
void Motor9A_SetMaxStepsPerCycle(uint16_t max_steps)
{
    if (max_steps > 0 && max_steps <= 0x0FFF) {
        motor9a_max_steps_per_cycle = max_steps;
    }
}

/**
 * @brief 获取步进电机9A每个周期的最大步数
 * @return 最大步数
 */
uint16_t Motor9A_GetMaxStepsPerCycle(void)
{
    return motor9a_max_steps_per_cycle;
}

/**
 * @brief 重置步进电机9A的位置
 */
void Motor9A_ResetPosition(void)
{
    motor9a_current_position = 0;
    motor9a_target_position = 0;
}

// ==================== Motor Reset Functions ====================

/**
 * @brief 电机复位最大位置值
 */
#define MOTOR_RESET_MAX_POSITION 0x0FFFFFFF

/**
 * @brief 电机复位超时时间（毫秒）
 */
#define MOTOR_RESET_TIMEOUT_MS 10000

// 电机复位状态变量
static MotorResetState_t zoom3_reset_state = MOTOR_RESET_IDLE;
static uint32_t zoom3_reset_start_time = 0;
static GPIO_PinState zoom3_last_co_state = GPIO_PIN_RESET;
static MotorResetState_t focus_reset_state = MOTOR_RESET_IDLE;
static uint32_t focus_reset_start_time = 0;
static GPIO_PinState focus_last_co_state = GPIO_PIN_RESET;
static MotorResetState_t zoom2_reset_state = MOTOR_RESET_IDLE;
static uint32_t zoom2_reset_start_time = 0;
static GPIO_PinState zoom2_last_co_state = GPIO_PIN_RESET;
static MotorResetState_t zoom1_reset_state = MOTOR_RESET_IDLE;
static uint32_t zoom1_reset_start_time = 0;
static GPIO_PinState zoom1_last_co_state = GPIO_PIN_RESET;
static MotorResetState_t iris_reset_state = MOTOR_RESET_IDLE;
static uint32_t iris_reset_start_time = 0;
static GPIO_PinState iris_last_co_state = GPIO_PIN_RESET;

/**
 * @brief 启动ZOOM3电机复位流程（非阻塞）
 * @return 0: 成功启动, -1: 已在复位中
 */
int Motor_ZOOM3_Reset_Start(void)
{
    if (zoom3_reset_state != MOTOR_RESET_IDLE && zoom3_reset_state != MOTOR_RESET_COMPLETED && zoom3_reset_state != MOTOR_RESET_TIMEOUT)
    {
        return -1; // 已在复位中
    }
    
    LOG_Print(LOG_LEVEL_INFO,"Starting ZOOM3 motor reset process...\r\n");
    
    // 初始化CO信号状态
    zoom3_last_co_state = HAL_GPIO_ReadPin(ZOOM3_PI_CO_GPIO_Port, ZOOM3_PI_CO_Pin);
    
    // 步骤1: 设置目标位置为最大值
    if(zoom3_last_co_state==GPIO_PIN_RESET){
        Motor34_SetTargetPosition(MOTOR_RESET_MAX_POSITION);
    }
    else{
        Motor34_SetTargetPosition(-MOTOR_RESET_MAX_POSITION);
    }
    Motor34_EnablePositionControl();
    
    LOG_Print(LOG_LEVEL_INFO,"ZOOM3 target position set to maximum: %d\r\n", MOTOR_RESET_MAX_POSITION);
    
    
    // 设置状态和开始时间
    zoom3_reset_state = MOTOR_RESET_MOVING;
    zoom3_reset_start_time = HAL_GetTick();
    
    return 0;
}

/**
 * @brief 检查ZOOM3电机复位状态（非阻塞）
 * 需要在主循环中定期调用
 * @return MOTOR_RESET_IDLE: 空闲, MOTOR_RESET_MOVING: 运动中, 
 *         MOTOR_RESET_COMPLETED: 完成, MOTOR_RESET_TIMEOUT: 超时
 */
MotorResetState_t Motor_ZOOM3_Reset_Process(void)
{
    if (zoom3_reset_state == MOTOR_RESET_MOVING)
    {
        // 检查超时
        if ((HAL_GetTick() - zoom3_reset_start_time) > MOTOR_RESET_TIMEOUT_MS)
        {
            LOG_Print(LOG_LEVEL_INFO,"ZOOM3 motor reset timeout!\r\n");
            zoom3_reset_state = MOTOR_RESET_TIMEOUT;
            return zoom3_reset_state;
        }
        
        // 检查CO信号是否发生变化（电平变化到位信号）
        GPIO_PinState current_co_state = HAL_GPIO_ReadPin(ZOOM3_PI_CO_GPIO_Port, ZOOM3_PI_CO_Pin);
        if (current_co_state != zoom3_last_co_state)
        {
            // 步骤3: 检测到电平变化，将当前位置更新为0
            Motor34_SetCurrentPosition(ZOOM3_ZERO_POS*4);
            Motor34_SetTargetPosition(ZOOM3_ZERO_POS*4);
            
            LOG_Print(LOG_LEVEL_INFO,"ZOOM3 motor reset completed successfully (CO signal changed from %s to %s)\r\n",
                     zoom3_last_co_state == GPIO_PIN_SET ? "HIGH" : "LOW",
                     current_co_state == GPIO_PIN_SET ? "HIGH" : "LOW");
            zoom3_reset_state = MOTOR_RESET_COMPLETED;
        }
        zoom3_last_co_state = current_co_state;
    }
    
    return zoom3_reset_state;
}

/**
 * @brief 获取ZOOM3电机复位状态
 * @return 当前复位状态
 */
MotorResetState_t Motor_ZOOM3_Reset_GetState(void)
{
    return zoom3_reset_state;
}

/**
 * @brief 重置ZOOM3电机复位状态为空闲
 */
void Motor_ZOOM3_Reset_Clear(void)
{
		Motor34_SetCurrentPosition(ZOOM3_ZERO_POS*4);
		Motor34_SetTargetPosition(ZOOM3_ZERO_POS*4);
    zoom3_reset_state = MOTOR_RESET_IDLE;
		Motor34_PrintPositions();

}

/**
 * @brief 启动FOCUS电机复位流程（非阻塞）
 * @return 0: 成功启动, -1: 已在复位中
 */
int Motor_FOCUS_Reset_Start(void)
{
    if (focus_reset_state != MOTOR_RESET_IDLE && focus_reset_state != MOTOR_RESET_COMPLETED && focus_reset_state != MOTOR_RESET_TIMEOUT)
    {
        return -1; // 已在复位中
    }
    
    LOG_Print(LOG_LEVEL_INFO,"Starting FOCUS motor reset process...\r\n");
    
    // 初始化CO信号状态
    focus_last_co_state = HAL_GPIO_ReadPin(FOCUS_PI_CO_GPIO_Port, FOCUS_PI_CO_Pin);
    
    // 步骤1: 根据初始CO信号状态设置目标位置
    if(focus_last_co_state==GPIO_PIN_RESET){
        Motor9A_SetTargetPosition(-MOTOR_RESET_MAX_POSITION);
    }
    else{
        Motor9A_SetTargetPosition(MOTOR_RESET_MAX_POSITION);
    }
    Motor9A_EnablePositionControl();
    
    LOG_Print(LOG_LEVEL_INFO,"FOCUS target position set to maximum: %d\r\n", MOTOR_RESET_MAX_POSITION);
    
    // 设置状态和开始时间
    focus_reset_state = MOTOR_RESET_MOVING;
    focus_reset_start_time = HAL_GetTick();
    
    return 0;
}

/**
 * @brief 检查FOCUS电机复位状态（非阻塞）
 * 需要在主循环中定期调用
 * @return MOTOR_RESET_IDLE: 空闲, MOTOR_RESET_MOVING: 运动中, 
 *         MOTOR_RESET_COMPLETED: 完成, MOTOR_RESET_TIMEOUT: 超时
 */
MotorResetState_t Motor_FOCUS_Reset_Process(void)
{
    if (focus_reset_state == MOTOR_RESET_MOVING)
    {
        // 检查超时
        if ((HAL_GetTick() - focus_reset_start_time) > MOTOR_RESET_TIMEOUT_MS)
        {
            LOG_Print(LOG_LEVEL_INFO,"FOCUS motor reset timeout!\r\n");
            focus_reset_state = MOTOR_RESET_TIMEOUT;
            return focus_reset_state;
        }
        
        // 检查CO信号是否发生变化（电平变化到位信号）
        GPIO_PinState current_co_state = HAL_GPIO_ReadPin(FOCUS_PI_CO_GPIO_Port, FOCUS_PI_CO_Pin);
        if (current_co_state != focus_last_co_state)
        {
            // 步骤3: 检测到电平变化，将当前位置更新为0
            Motor9A_SetCurrentPosition(FOUCE_ZERO_POS*4);
            Motor9A_SetTargetPosition(FOUCE_ZERO_POS*4);
            
            LOG_Print(LOG_LEVEL_INFO,"FOCUS motor reset completed successfully (CO signal changed from %s to %s)\r\n",
                     focus_last_co_state == GPIO_PIN_SET ? "HIGH" : "LOW",
                     current_co_state == GPIO_PIN_SET ? "HIGH" : "LOW");
            focus_reset_state = MOTOR_RESET_COMPLETED;
        }
        focus_last_co_state = current_co_state;
    }
    
    return focus_reset_state;
}

/**
 * @brief 获取FOCUS电机复位状态
 * @return 当前复位状态
 */
MotorResetState_t Motor_FOCUS_Reset_GetState(void)
{
    return focus_reset_state;
}

/**
 * @brief 重置FOCUS电机复位状态为空闲
 */
void Motor_FOCUS_Reset_Clear(void)
{
		Motor9A_SetCurrentPosition(FOUCE_ZERO_POS*4);
		Motor9A_SetTargetPosition(FOUCE_ZERO_POS*4);
    focus_reset_state = MOTOR_RESET_IDLE;
}

/**
 * @brief 启动ZOOM2电机复位流程（非阻塞）
 * @return 0: 成功启动, -1: 已在复位中
 */
int Motor_ZOOM2_Reset_Start(void)
{
    if (zoom2_reset_state != MOTOR_RESET_IDLE && zoom2_reset_state != MOTOR_RESET_COMPLETED && zoom2_reset_state != MOTOR_RESET_TIMEOUT)
    {
        return -1; // 已在复位中
    }
    
    LOG_Print(LOG_LEVEL_INFO,"Starting ZOOM2 motor reset process...\r\n");
    
    // 初始化CO信号状态
    zoom2_last_co_state = HAL_GPIO_ReadPin(ZOOM2_PI_CO_GPIO_Port, ZOOM2_PI_CO_Pin);
    
    // 步骤1: 根据初始CO信号状态设置目标位置
    if(zoom2_last_co_state==GPIO_PIN_RESET){
        Motor78_SetTargetPosition(MOTOR_RESET_MAX_POSITION);
    }
    else{
        Motor78_SetTargetPosition(-MOTOR_RESET_MAX_POSITION);
    }
    Motor78_EnablePositionControl();
    
    LOG_Print(LOG_LEVEL_INFO,"ZOOM2 target position set to maximum: %d\r\n", MOTOR_RESET_MAX_POSITION);
    
    // 设置状态和开始时间
    zoom2_reset_state = MOTOR_RESET_MOVING;
    zoom2_reset_start_time = HAL_GetTick();
    
    return 0;
}

/**
 * @brief 检查ZOOM2电机复位状态（非阻塞）
 * 需要在主循环中定期调用
 * @return MOTOR_RESET_IDLE: 空闲, MOTOR_RESET_MOVING: 运动中, 
 *         MOTOR_RESET_COMPLETED: 完成, MOTOR_RESET_TIMEOUT: 超时
 */
MotorResetState_t Motor_ZOOM2_Reset_Process(void)
{
    if (zoom2_reset_state == MOTOR_RESET_MOVING)
    {
        // 检查超时
        if ((HAL_GetTick() - zoom2_reset_start_time) > MOTOR_RESET_TIMEOUT_MS)
        {
            LOG_Print(LOG_LEVEL_INFO,"ZOOM2 motor reset timeout!\r\n");
            zoom2_reset_state = MOTOR_RESET_TIMEOUT;
            return zoom2_reset_state;
        }
        
        // 检查CO信号是否发生变化（到位信号）
        GPIO_PinState current_co_state = HAL_GPIO_ReadPin(ZOOM2_PI_CO_GPIO_Port, ZOOM2_PI_CO_Pin);
        if (current_co_state != zoom2_last_co_state)
        {
            // 步骤3: 检测到CO信号变化，将当前位置更新为0
            Motor78_SetCurrentPosition(ZOOM2_ZERO_POS*4);
            Motor78_SetTargetPosition(ZOOM2_ZERO_POS*4);
            
            LOG_Print(LOG_LEVEL_INFO,"ZOOM2 motor reset completed successfully - CO signal changed\r\n");
            zoom2_reset_state = MOTOR_RESET_COMPLETED;
        }
        zoom2_last_co_state = current_co_state;
    }
    
    return zoom2_reset_state;
}

/**
 * @brief 获取ZOOM2电机复位状态
 * @return 当前复位状态
 */
MotorResetState_t Motor_ZOOM2_Reset_GetState(void)
{
    return zoom2_reset_state;
}

/**
 * @brief 重置ZOOM2电机复位状态为空闲
 */
void Motor_ZOOM2_Reset_Clear(void)
{
		Motor78_SetCurrentPosition(ZOOM2_ZERO_POS*4);
		Motor78_SetTargetPosition(ZOOM2_ZERO_POS*4);
    zoom2_reset_state = MOTOR_RESET_IDLE;
}

/**
 * @brief 启动ZOOM1电机复位流程（非阻塞）
 * @return 0: 成功启动, -1: 已在复位中
 */
int Motor_ZOOM1_Reset_Start(void)
{
    if (zoom1_reset_state != MOTOR_RESET_IDLE && zoom1_reset_state != MOTOR_RESET_COMPLETED && zoom1_reset_state != MOTOR_RESET_TIMEOUT)
    {
        return -1; // 已在复位中
    }
    
    LOG_Print(LOG_LEVEL_INFO,"Starting ZOOM1 motor reset process...\r\n");
    
    // 初始化CO信号状态
    zoom1_last_co_state = HAL_GPIO_ReadPin(ZOOM1_PI_CO_GPIO_Port, ZOOM1_PI_CO_Pin);
    
    // 步骤1: 根据初始CO信号状态设置目标位置
    if(zoom1_last_co_state==GPIO_PIN_RESET){
        Motor12_SetTargetPosition(MOTOR_RESET_MAX_POSITION);
    }
    else{
        Motor12_SetTargetPosition(-MOTOR_RESET_MAX_POSITION);
    }
    Motor12_EnablePositionControl();
    
    LOG_Print(LOG_LEVEL_INFO,"ZOOM1 target position set to maximum: %d\r\n", MOTOR_RESET_MAX_POSITION);
    
    // 设置状态和开始时间
    zoom1_reset_state = MOTOR_RESET_MOVING;
    zoom1_reset_start_time = HAL_GetTick();
    
    return 0;
}

/**
 * @brief 检查ZOOM1电机复位状态（非阻塞）
 * 需要在主循环中定期调用
 * @return MOTOR_RESET_IDLE: 空闲, MOTOR_RESET_MOVING: 运动中, 
 *         MOTOR_RESET_COMPLETED: 完成, MOTOR_RESET_TIMEOUT: 超时
 */
MotorResetState_t Motor_ZOOM1_Reset_Process(void)
{
    if (zoom1_reset_state == MOTOR_RESET_MOVING)
    {
        // 检查超时
        if ((HAL_GetTick() - zoom1_reset_start_time) > MOTOR_RESET_TIMEOUT_MS)
        {
            LOG_Print(LOG_LEVEL_INFO,"ZOOM1 motor reset timeout!\r\n");
            zoom1_reset_state = MOTOR_RESET_TIMEOUT;
            return zoom1_reset_state;
        }
        
        // 检查CO信号是否发生变化（到位信号）
        GPIO_PinState current_co_state = HAL_GPIO_ReadPin(ZOOM1_PI_CO_GPIO_Port, ZOOM1_PI_CO_Pin);
        if (current_co_state != zoom1_last_co_state)
        {
            // 步骤3: 检测到CO信号变化，将当前位置更新为0
            Motor12_SetCurrentPosition(ZOOM1_ZERO_POS*8);
            Motor12_SetTargetPosition(ZOOM1_ZERO_POS*8);
            
            LOG_Print(LOG_LEVEL_INFO,"ZOOM1 motor reset completed successfully - CO signal changed\r\n");
            zoom1_reset_state = MOTOR_RESET_COMPLETED;
        }
        zoom1_last_co_state = current_co_state;
    }
    
    return zoom1_reset_state;
}

/**
 * @brief 获取ZOOM1电机复位状态
 * @return 当前复位状态
 */
MotorResetState_t Motor_ZOOM1_Reset_GetState(void)
{
    return zoom1_reset_state;
}

/**
 * @brief 重置ZOOM1电机复位状态为空闲
 */
void Motor_ZOOM1_Reset_Clear(void)
{
		Motor12_SetCurrentPosition(ZOOM1_ZERO_POS*8);
		Motor12_SetTargetPosition(ZOOM1_ZERO_POS*8);
    zoom1_reset_state = MOTOR_RESET_IDLE;
}

/**
 * @brief 启动IRIS电机复位流程（非阻塞）
 * @return 0: 成功启动, -1: 已在复位中
 */
int Motor_IRIS_Reset_Start(void)
{
    if (iris_reset_state != MOTOR_RESET_IDLE && iris_reset_state != MOTOR_RESET_COMPLETED && iris_reset_state != MOTOR_RESET_TIMEOUT)
    {
        return -1; // 已在复位中
    }
    
    LOG_Print(LOG_LEVEL_INFO,"Starting IRIS motor reset process...\r\n");
    
    // 初始化CO信号状态
    iris_last_co_state = HAL_GPIO_ReadPin(IRIS_PI_CO_GPIO_Port, IRIS_PI_CO_Pin);
    
    // 步骤1: 根据初始CO信号状态设置目标位置
    if(iris_last_co_state==GPIO_PIN_RESET){
        Motor56_SetTargetPosition(-MOTOR_RESET_MAX_POSITION);
    }
    else{
        Motor56_SetTargetPosition(MOTOR_RESET_MAX_POSITION);
    }
    Motor56_EnablePositionControl();
    
    LOG_Print(LOG_LEVEL_INFO,"IRIS target position set to maximum: %d\r\n", MOTOR_RESET_MAX_POSITION);
    
    // 设置状态和开始时间
    iris_reset_state = MOTOR_RESET_MOVING;
    iris_reset_start_time = HAL_GetTick();
    
    return 0;
}

/**
 * @brief 检查IRIS电机复位状态（非阻塞）
 * 需要在主循环中定期调用
 * @return MOTOR_RESET_IDLE: 空闲, MOTOR_RESET_MOVING: 运动中, 
 *         MOTOR_RESET_COMPLETED: 完成, MOTOR_RESET_TIMEOUT: 超时
 */
MotorResetState_t Motor_IRIS_Reset_Process(void)
{
    if (iris_reset_state == MOTOR_RESET_MOVING)
    {
        // 检查超时
        if ((HAL_GetTick() - iris_reset_start_time) > MOTOR_RESET_TIMEOUT_MS)
        {
            LOG_Print(LOG_LEVEL_INFO,"IRIS motor reset timeout!\r\n");
            iris_reset_state = MOTOR_RESET_TIMEOUT;
            return iris_reset_state;
        }
        
        // 检查CO信号是否发生变化（到位信号）
        GPIO_PinState current_co_state = HAL_GPIO_ReadPin(IRIS_PI_CO_GPIO_Port, IRIS_PI_CO_Pin);
        if (current_co_state != iris_last_co_state)
        {
            // 步骤3: 检测到CO信号变化，将当前位置更新为0
            Motor56_SetCurrentPosition(0);
            Motor56_SetTargetPosition(0);
            
            LOG_Print(LOG_LEVEL_INFO,"IRIS motor reset completed successfully - CO signal changed\r\n");
            iris_reset_state = MOTOR_RESET_COMPLETED;
        }
        iris_last_co_state = current_co_state;
    }
    
    return iris_reset_state;
}

/**
 * @brief 获取IRIS电机复位状态
 * @return 当前复位状态
 */
MotorResetState_t Motor_IRIS_Reset_GetState(void)
{
    return iris_reset_state;
}

/**
 * @brief 重置IRIS电机复位状态为空闲
 */
void Motor_IRIS_Reset_Clear(void)
{
		Motor56_SetCurrentPosition(0);
		Motor56_SetTargetPosition(0);
    iris_reset_state = MOTOR_RESET_IDLE;
}

/**
 * @brief 检查单个电机的CO信号状态
 * @param motor_name 电机名称
 * @param gpio_port GPIO端口
 * @param gpio_pin GPIO引脚
 * @return true表示CO信号为高电平（到位），false表示低电平（未到位）
 */
bool Motor_CheckCOStatus(const char* motor_name, GPIO_TypeDef* gpio_port, uint16_t gpio_pin)
{
    GPIO_PinState pin_state = HAL_GPIO_ReadPin(gpio_port, gpio_pin);
    bool is_high = (pin_state == GPIO_PIN_SET);
    
    LOG_Print(LOG_LEVEL_INFO, "%s CO信号状态: %s\r\n", motor_name, is_high ? "高电平（到位）" : "低电平（未到位）");
    return is_high;
}

/**
 * @brief 检查所有电机的CO信号状态
 * @return true表示所有CO信号都为高电平（全部到位），false表示至少有一个未到位
 */
void Motors_CheckAllCOStatus(void)
{
    //LOG_Print(LOG_LEVEL_INFO, "=== 检查所有电机CO信号状态 ===\r\n");
    
    bool zoom1_status = Motor_CheckCOStatus("ZOOM1", ZOOM1_PI_CO_GPIO_Port, ZOOM1_PI_CO_Pin);
    bool zoom3_status = Motor_CheckCOStatus("ZOOM3", ZOOM3_PI_CO_GPIO_Port, ZOOM3_PI_CO_Pin);
    bool iris_status = Motor_CheckCOStatus("IRIS", IRIS_PI_CO_GPIO_Port, IRIS_PI_CO_Pin);
    bool zoom2_status = Motor_CheckCOStatus("ZOOM2", ZOOM2_PI_CO_GPIO_Port, ZOOM2_PI_CO_Pin);
    bool focus_status = Motor_CheckCOStatus("FOCUS", FOCUS_PI_CO_GPIO_Port, FOCUS_PI_CO_Pin);
    
//    bool all_high = zoom1_status && zoom3_status && iris_status && zoom2_status && focus_status;
    
 //   LOG_Print(LOG_LEVEL_INFO, "\r\n=== CO信号检查结果汇总 ===\r\n");
 //   LOG_Print(LOG_LEVEL_INFO, "ZOOM1: %s\r\n", zoom1_status ? "到位" : "未到位");
//    LOG_Print(LOG_LEVEL_INFO, "ZOOM3: %s\r\n", zoom3_status ? "到位" : "未到位");
//    LOG_Print(LOG_LEVEL_INFO, "IRIS:  %s\r\n", iris_status ? "到位" : "未到位");
//    LOG_Print(LOG_LEVEL_INFO, "ZOOM2: %s\r\n", zoom2_status ? "到位" : "未到位");
//    LOG_Print(LOG_LEVEL_INFO, "FOCUS: %s\r\n", focus_status ? "到位" : "未到位");
//    LOG_Print(LOG_LEVEL_INFO, "\r\n所有CO信号状态: %s\r\n", 
//              all_high ? "全部为高电平（全部到位）" : "存在低电平信号（部分未到位）");
//    LOG_Print(LOG_LEVEL_INFO, "========================\r\n");
    
 //   return all_high;
}
