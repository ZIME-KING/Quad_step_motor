
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
uint16_t PSUMA = 0XFF;	   // 步进数A
uint16_t PSUMB = 0XFF;	   // 步进数B
uint16_t CCWCWA = 0X1000;  // 方向A
uint16_t CCWCWB = 0X1000;  // 方向B
uint16_t BRAKEA = 0x2000;  // 刹车A
uint16_t BRAKEB = 0x2000;  // 刹车B
uint16_t ENDISA = 0X4000;  // 使能A
uint16_t ENDISB = 0X4000;  // 使能B
uint16_t MICROA = 0X0000;  // 细分数
uint16_t MICROB = 0X0000;  // 细分数
uint16_t INTCTAB = 0x0000; // 每步周期AB
uint16_t INTCTCD = 0x0000; // 每步周期CD
uint16_t INTCTEF = 0x0000; // 每步周期EF
uint16_t INTCTGH = 0x0000; // 每步周期GH
uint16_t INTCTIJ = 0x0000; // 每步周期IJ

uint16_t LED1 = 0x0000;
uint16_t LED2 = 0x0000;
uint16_t LED3 = 0x0000;
uint16_t LED4 = 0x0000;

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
    Spi_Write(0x24, 0xcfff); // 设置LED输出  电机输出使能/关断  启动/刹车 电流方向 步数
    Spi_Write(0x25, 0x0800); // 设置步进周期
    Spi_Write(0x26, 0x9e5e); // 设置TESTEN2 FZTEST  过流保护检测时间OCP_dly  PWMRES PWMMODE

    Spi_Write(0x27, 0x0001); // 选择256分频  相位矫正=0 设置DT2延时
    Spi_Write(0x28, 0xd8d8); // 设置最大占空比为 90%
    Spi_Write(0x29, 0xcfff); // 设置LED输出  电机输出使能/关断  启动/刹车 电流方向 步数
    Spi_Write(0x2a, 0x0800); // 设置步进周期
    Spi_Write(0x2b, 0x9e5e); // 设置TESTEN2 FZTEST  过流保护检测时间OCP_dly  PWMRES PWMMODE

    Spi_Write(0x2c, 0x0001); // 选择256分频  相位矫正=0 设置DT2延时
    Spi_Write(0x2d, 0xd8d8); // 设置最大占空比为 90%
    Spi_Write(0x2e, 0xcfff); // 设置LED输出  电机输出使能/关断  启动/刹车 电流方向 步数
    Spi_Write(0x2f, 0x0800); // 设置步进周期
    Spi_Write(0x30, 0x9e5e); // 设置TESTEN2 FZTEST  过流保护检测时间OCP_dly  PWMRES PWMMODE

    Spi_Write(0x31, 0x0001); // 选择256分频  相位矫正=0 设置DT2延时
    Spi_Write(0x32, 0xd8d8); // 设置最大占空比为 90%
    Spi_Write(0x33, 0xcfff); // 设置LED输出  电机输出使能/关断  启动/刹车 电流方向 步数
    Spi_Write(0x34, 0x0800); // 设置步进周期
    Spi_Write(0x35, 0x9e5e); // 设置TESTEN2 FZTEST  过流保护检测时间OCP_dly  PWMRES PWMMODE

    Spi_Write(0x36, 0x0001); // 选择256分频  相位矫正=0 设置DT2延时
    Spi_Write(0x37, 0xd8d8); // 设置最大占空比为 90%
    Spi_Write(0x38, 0xcfff); // 设置LED输出  电机输出使能/关断  启动/刹车 电流方向 步数
    Spi_Write(0x39, 0x0800); // 设置步进周期
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

void VD_FZ(void)
{
    // 步进电机A的VD脉冲控制
    // 根据实际硬件连接，控制相应的GPIO引脚
    // 这里使用MS_PLS1和MS_PLS2引脚作为示例
    HAL_GPIO_WritePin(GPIOB, MS_PLS1_Pin, GPIO_PIN_SET);   // VD脉冲高电平
    HAL_GPIO_WritePin(GPIOB, MS_PLS2_Pin, GPIO_PIN_SET);   // VD脉冲高电平
    
    // 延时20微秒（在中断中使用简单的循环延时）
    for(volatile int i = 0; i < 100; i++);  // 简单延时，避免在中断中使用HAL_Delay
    
    HAL_GPIO_WritePin(GPIOB, MS_PLS1_Pin, GPIO_PIN_RESET); // VD脉冲低电平
    HAL_GPIO_WritePin(GPIOB, MS_PLS2_Pin, GPIO_PIN_RESET); // VD脉冲低电平
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
#define KEY_SW2_LEFT   			0x00000080
#define KEY_SW2_RIGHT  			0x00000100
#define KEY_SW2_ENTER				0x00000200

#define	KEY_SHORT_DOWN			0x01000000
#define KEY_SHORT_UP				0x02000000
#define	KEY_LONG_DOWN				0x04000000
#define	KEY_LONG_UP					0x08000000
#define	KEY_CONTI_DOWN			0x10000000
#define KEY_LONG_DOWN_INIT	0x20000000
#define KEY_LONG_UP_INIT		0x40000000
#define KEY_LONG_DOWN_2S		0x80000000
#define KEY_LONG_UP_2S			0x00800000


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
    
    UART_Transmit_Str(&huart1, (uint8_t*)"\r\n=== SPI Register Dump Start ===\r\n");
    
    // 读取常用的寄存器地址范围 (0x00-0x3F)
    for(addr = 0x00; addr <= 0x3F; addr++)
    {
        Spi__Read(addr, &regData);
        sprintf(buffer, "Reg[0x%02X] = 0x%04X\r\n", addr, regData);
        UART_Transmit_Str(&huart1, (uint8_t*)buffer);
        HAL_Delay(1); // 短暂延时确保SPI通信稳定
    }
    
    UART_Transmit_Str(&huart1, (uint8_t*)"=== SPI Register Dump End ===\r\n\r\n");
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

// 步进电机控制相关变量
static bool stepper_running = false;
static uint16_t target_steps_A = 0;
static uint16_t target_steps_B = 0;
static uint16_t current_steps_A = 0;
static uint16_t current_steps_B = 0;

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
