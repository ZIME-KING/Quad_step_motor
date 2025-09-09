
#include "control.h"
#include "main.h"
#include <stdio.h>
#include "UART/uart.h"

char OutPutChennal2 = 0;
char OutPutChennal = 0;
uint8_t IRCUT_FLAG = 0;
uint8_t IRCUT_FLAG2 = 0;
uint16_t IRS_TGT = 0X000;  // ��Ȧ Ŀ��ֵ
uint16_t PWMMODEAB = 0;	   // PWMƵ�� AB
uint16_t PWMRESAB = 0;	   // PWMϸ�� AB
uint16_t PHMODA = 0;	   // ��λ���� A
uint16_t PHMODB = 0;	   // ��λ���� B
uint16_t PPWAB = 0X80;	   // ��ֵ���(����)AB
uint16_t PPWCD = 0X80;	   // ��ֵ���(����)CD
uint16_t PPWEF = 0X80;	   // ��ֵ���(����)EF
uint16_t PPWGH = 0X80;	   // ��ֵ���(����)GH
uint16_t PPWIJ = 0X80;	   // ��ֵ���(����)IJ

uint16_t CCWCWA = 0X1000;  // ����A
uint16_t CCWCWB = 0X1000;  // ����B
uint16_t BRAKEA = 0x2000;  // ɲ��A
uint16_t BRAKEB = 0x2000;  // ɲ��B
uint16_t ENDISA = 0X4000;  // ʹ��A
uint16_t ENDISB = 0X4000;  // ʹ��B
uint16_t MICROA = 0X0000;  // ϸ����
uint16_t MICROB = 0X0000;  // ϸ����




uint16_t PSUMA = 50;	   // ������A
uint16_t PSUMB = 50;	   // ������B
uint16_t PSUMC = 50;	   // ������C
uint16_t PSUMD = 50;	   // ������D

//�� INTCTAB[15:0]=900 ʱ��64 ϸ����ÿ������ 12��900/27MHz=0.4ms

//�� INTCTAB[15:0]=900 ʱ��256 ϸ����ÿ������ 3��900/27MHz=0.1ms


//����Ϊ900��0.1msһ�� һ��VD���ڰ�20ms->50hz,�������Ϊ200��
uint16_t INTCTAB = 703; // ÿ������AB
uint16_t INTCTCD = 703; // ÿ������CD
uint16_t INTCTEF = 703; // ÿ������EF
uint16_t INTCTGH = 703; // ÿ������GH
uint16_t INTCTIJ = 703; // ÿ������GH



uint16_t LED1 = 0x0000;
uint16_t LED2 = 0x0000;
uint16_t LED3 = 0x0000;
uint16_t LED4 = 0x0000;


#define ZOOM1_ZERO_POS  0
#define ZOOM2_ZERO_POS  0
#define ZOOM3_ZERO_POS  0
#define FOUCE_ZERO_POS  0



// �������������ر���
static bool stepper_running = false;
static uint16_t target_steps_A = 0;
static uint16_t target_steps_B = 0;
static uint16_t current_steps_A = 0;
static uint16_t current_steps_B = 0;

// VD_FZ����������ر���
static uint16_t vd_fz_count = 0;        // ��ǰִ�д���
static uint16_t vd_fz_target_count = 0; // Ŀ��ִ�д���
static bool vd_fz_enabled = false;      // VD_FZʹ�ܱ�־

// �������12λ�ÿ�����ر���
static int32_t motor12_current_position = 0;  // ��ǰλ��
static int32_t motor12_target_position = 0;   // Ŀ��λ��
static bool motor12_position_control_enabled = true; // λ�ÿ���ʹ�ܱ�־
static uint16_t motor12_max_steps_per_cycle = 32;    // ÿ��20ms���������

// �������34λ�ÿ�����ر���
static int32_t motor34_current_position = 0;  // ��ǰλ��
static int32_t motor34_target_position = 0;   // Ŀ��λ��
static bool motor34_position_control_enabled = true; // λ�ÿ���ʹ�ܱ�־
static uint16_t motor34_max_steps_per_cycle = 32;    // ÿ��20ms���������

// �������56λ�ÿ�����ر���
static int32_t motor56_current_position = 0;  // ��ǰλ��
static int32_t motor56_target_position = 0;   // Ŀ��λ��
static bool motor56_position_control_enabled = true; // λ�ÿ���ʹ�ܱ�־
static uint16_t motor56_max_steps_per_cycle = 32;    // ÿ��20ms���������

// �������78λ�ÿ�����ر���
static int32_t motor78_current_position = 0;  // ��ǰλ��
static int32_t motor78_target_position = 0;   // Ŀ��λ��
static bool motor78_position_control_enabled = true; // λ�ÿ���ʹ�ܱ�־
static uint16_t motor78_max_steps_per_cycle = 32;    // ÿ��20ms���������

// �������9Aλ�ÿ�����ر���
static int32_t motor9a_current_position = 0;  // ��ǰλ��
static int32_t motor9a_target_position = 0;   // Ŀ��λ��
static bool motor9a_position_control_enabled = true; // λ�ÿ���ʹ�ܱ�־
static uint16_t motor9a_max_steps_per_cycle = 32;    // ÿ��20ms���������
//timer_oc_parameter_struct timer0_ocintpara;
//timer_parameter_struct timer0_initpara;

extern uint16_t u16ReadSpiData[20];

uint16_t Spi_ReadWrite(uint16_t u16WriteData)
{
    uint16_t ret_Data;
    uint8_t txData[2], rxData[2];

    // ��16λ���ݷֽ�Ϊ����8λ����
    txData[0] = (uint8_t)(u16WriteData & 0xFF);
    txData[1] = (uint8_t)((u16WriteData >> 8) & 0xFF);

    // ʹ��HAL�����SPI����
    HAL_SPI_TransmitReceive(&hspi2, txData, rxData, 2, HAL_MAX_DELAY);

    // ������Ͻ��յ�������
    ret_Data = rxData[0] | (rxData[1] << 8);
    return (ret_Data);
}

uint8_t Spi_ReadWriteByte(uint8_t u8Byte)
{
    uint8_t ret_Data;
    // ʹ��HAL�����SPI�ֽڴ���
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
    Spi_Write(0x20, 0x0001); // ��ʹ�ܸ���ֱ��ͨ�� ��ʹ��ֱ�������ⲿ�ܽſ��� ����DT1��ʱ
   
    Spi_Write(0x22, 0x0001); // ѡ��256��Ƶ  ��λ����=0 ����DT2��ʱ
    Spi_Write(0x23, 0xd8d8); // �������ռ�ձ�Ϊ 90%
    Spi_Write(0x24, (0xcfff&0xF000)|PSUMA); // ����LED���  ������ʹ��/�ض�  ����/ɲ�� �������� ����
    Spi_Write(0x25, INTCTAB); // ���ò�������
    Spi_Write(0x26, 0x9e5e); // ����TESTEN2 FZTEST  �����������ʱ��OCP_dly  PWMRES PWMMODE

    Spi_Write(0x27, 0x0001); // ѡ��256��Ƶ  ��λ����=0 ����DT2��ʱ
    Spi_Write(0x28, 0xd8d8); // �������ռ�ձ�Ϊ 90%
    Spi_Write(0x29, (0xcfff&0xF000)|PSUMB); // ����LED���  ������ʹ��/�ض�  ����/ɲ�� �������� ����
    Spi_Write(0x2a, INTCTCD); // ���ò�������
    Spi_Write(0x2b, 0x9e5e); // ����TESTEN2 FZTEST  �����������ʱ��OCP_dly  PWMRES PWMMODE

    Spi_Write(0x2c, 0x0001); // ѡ��256��Ƶ  ��λ����=0 ����DT2��ʱ
    Spi_Write(0x2d, 0xd8d8); // �������ռ�ձ�Ϊ 90%
    Spi_Write(0x2e, (0xcfff&0xF000)|PSUMC); // ����LED���  ������ʹ��/�ض�  ����/ɲ�� �������� ����
    Spi_Write(0x2f, INTCTEF); // ���ò�������
    Spi_Write(0x30, 0x9e5e); // ����TESTEN2 FZTEST  �����������ʱ��OCP_dly  PWMRES PWMMODE

    Spi_Write(0x31, 0x0001); // ѡ��256��Ƶ  ��λ����=0 ����DT2��ʱ
    Spi_Write(0x32, 0xd8d8); // �������ռ�ձ�Ϊ 90%
    Spi_Write(0x33, (0xcfff&0xF000)|PSUMD); // ����LED���  ������ʹ��/�ض�  ����/ɲ�� �������� ����
    Spi_Write(0x34, INTCTGH); // ���ò�������
    Spi_Write(0x35, 0x9e5e); // ����TESTEN2 FZTEST  �����������ʱ��OCP_dly  PWMRES PWMMODE

    Spi_Write(0x36, 0x0001); // ѡ��256��Ƶ  ��λ����=0 ����DT2��ʱ
    Spi_Write(0x37, 0xd8d8); // �������ռ�ձ�Ϊ 90%
    Spi_Write(0x38, 0xcfff); // ����LED���  ������ʹ��/�ض�  ����/ɲ�� �������� ����
    Spi_Write(0x39, INTCTAB); // ���ò�������
    Spi_Write(0x3a, 0x9e5e); // ����TESTEN2 FZTEST  �����������ʱ��OCP_dly  PWMRES PWMMODE

    Spi_Write(0x3b, 0x0000); // ����ֱ�����ͨ��A����״̬  PWMƵ�� PWMռ�ձ�
    Spi_Write(0x3c, 0x0000); // ����ֱ�����ͨ��B����״̬  PWMƵ�� PWMռ�ձ�
    Spi_Write(0x3e, 0x0000); // ����ָʾ�Ĵ���

    Spi_Write(0x00, 0x0200); // ���ù�ȦĿ��ֵ  0x0200
    Spi_Write(0x01, 0x7c8a); // ����ADC�����˲�����ֹƵ�� PID��������������   0x7c8a
    Spi_Write(0x02, 0x66f0); // ����PID��������� ����       0x66f0
    Spi_Write(0x03, 0x0e10); // ���ù�Ȧ���PWMƵ��     0x0e10
    Spi_Write(0x04, 0x7070); // ���û���Ԫ��ƫ�õ��� ƫ�õ�ѹ      0x80ff
    Spi_Write(0x05, 0x0004); // ���ù�ȦĿ��ֵ��ͨ�˲�������ֹƵ�� 0x0004
    Spi_Write(0x0e, 0x0300); // ���ù�ȦĿ��ֵ�ƶ�ƽ���ٶ�        0x0300
    Spi_Write(0x0b, 0x0480); // ���ù�Ȧģ��ʹ�� TESTEN1ʹ��       0x0480
}

// void set_34(uint16_t pos){
//     Spi_Write(0x27, 0x0001); // ѡ��256��Ƶ  ��λ����=0 ����DT2��ʱ
//     Spi_Write(0x28, 0xd8d8); // �������ռ�ձ�Ϊ 90%
//     // ����0xcfff�ĸ�4λ��D12λ=0�������򣩣����ò�������12λ��
//     Spi_Write(0x29, (0xcfff&0xF000)|(pos&0x0FFF)); // ����LED��� ������ʹ��/�ض� ����/ɲ�� ��������(D12=0������) ����(D11-D0)
//     Spi_Write(0x2a, INTCTCD); // ���ò�������
//     Spi_Write(0x2b, 0x9e5e); // ����TESTEN2 FZTEST  �����������ʱ��OCP_dly  PWMRES PWMMODE
//     VD_FZ_Motor12();
//  }

/**
 * @brief ��ӡ�������12�ĵ�ǰλ�ú�Ŀ��λ��
 */
void Motor12_PrintPositions(void)
{
    LOG_Print(LOG_LEVEL_INFO,"=== �������12λ����Ϣ ===\r\n");
    LOG_Print(LOG_LEVEL_INFO,"��ǰλ��: %d\r\n", motor12_current_position);
    LOG_Print(LOG_LEVEL_INFO,"Ŀ��λ��: %d\r\n", motor12_target_position);
    LOG_Print(LOG_LEVEL_INFO,"λ�����: %d\r\n", motor12_target_position - motor12_current_position);
    LOG_Print(LOG_LEVEL_INFO,"����״̬: %s\r\n", motor12_position_control_enabled ? "����" : "����");
    LOG_Print(LOG_LEVEL_INFO,"ÿ���������: %d\r\n", motor12_max_steps_per_cycle);
    LOG_Print(LOG_LEVEL_INFO,"�Ƿ񵽴�Ŀ��: %s\r\n", (motor12_current_position == motor12_target_position) ? "��" : "��");
    LOG_Print(LOG_LEVEL_INFO,"========================\r\n");
}

/**
 * @brief ��ӡ�������34�ĵ�ǰλ�ú�Ŀ��λ��
 */
void Motor34_PrintPositions(void)
{
    LOG_Print(LOG_LEVEL_INFO,"=== �������34λ����Ϣ ===\r\n");
    LOG_Print(LOG_LEVEL_INFO,"��ǰλ��: %d\r\n", motor34_current_position);
    LOG_Print(LOG_LEVEL_INFO,"Ŀ��λ��: %d\r\n", motor34_target_position);
    LOG_Print(LOG_LEVEL_INFO,"λ�����: %d\r\n", motor34_target_position - motor34_current_position);
    LOG_Print(LOG_LEVEL_INFO,"����״̬: %s\r\n", motor34_position_control_enabled ? "����" : "����");
    LOG_Print(LOG_LEVEL_INFO,"ÿ���������: %d\r\n", motor34_max_steps_per_cycle);
    LOG_Print(LOG_LEVEL_INFO,"�Ƿ񵽴�Ŀ��: %s\r\n", (motor34_current_position == motor34_target_position) ? "��" : "��");
    LOG_Print(LOG_LEVEL_INFO,"========================\r\n");
}

/**
 * @brief ��ӡ�������56�ĵ�ǰλ�ú�Ŀ��λ��
 */
void Motor56_PrintPositions(void)
{
    LOG_Print(LOG_LEVEL_INFO,"=== �������56λ����Ϣ ===\r\n");
    LOG_Print(LOG_LEVEL_INFO,"��ǰλ��: %d\r\n", motor56_current_position);
    LOG_Print(LOG_LEVEL_INFO,"Ŀ��λ��: %d\r\n", motor56_target_position);
    LOG_Print(LOG_LEVEL_INFO,"λ�����: %d\r\n", motor56_target_position - motor56_current_position);
    LOG_Print(LOG_LEVEL_INFO,"����״̬: %s\r\n", motor56_position_control_enabled ? "����" : "����");
    LOG_Print(LOG_LEVEL_INFO,"ÿ���������: %d\r\n", motor56_max_steps_per_cycle);
    LOG_Print(LOG_LEVEL_INFO,"�Ƿ񵽴�Ŀ��: %s\r\n", (motor56_current_position == motor56_target_position) ? "��" : "��");
    LOG_Print(LOG_LEVEL_INFO,"========================\r\n");
}

/**
 * @brief ��ӡ�������78�ĵ�ǰλ�ú�Ŀ��λ��
 */
void Motor78_PrintPositions(void)
{
    LOG_Print(LOG_LEVEL_INFO,"=== �������78λ����Ϣ ===\r\n");
    LOG_Print(LOG_LEVEL_INFO,"��ǰλ��: %d\r\n", motor78_current_position);
    LOG_Print(LOG_LEVEL_INFO,"Ŀ��λ��: %d\r\n", motor78_target_position);
    LOG_Print(LOG_LEVEL_INFO,"λ�����: %d\r\n", motor78_target_position - motor78_current_position);
    LOG_Print(LOG_LEVEL_INFO,"����״̬: %s\r\n", motor78_position_control_enabled ? "����" : "����");
    LOG_Print(LOG_LEVEL_INFO,"ÿ���������: %d\r\n", motor78_max_steps_per_cycle);
    LOG_Print(LOG_LEVEL_INFO,"�Ƿ񵽴�Ŀ��: %s\r\n", (motor78_current_position == motor78_target_position) ? "��" : "��");
    LOG_Print(LOG_LEVEL_INFO,"========================\r\n");
}

/**
 * @brief ��ӡ�������9A�ĵ�ǰλ�ú�Ŀ��λ��
 */
void Motor9A_PrintPositions(void)
{
    LOG_Print(LOG_LEVEL_INFO,"=== �������9Aλ����Ϣ ===\r\n");
    LOG_Print(LOG_LEVEL_INFO,"��ǰλ��: %d\r\n", motor9a_current_position);
    LOG_Print(LOG_LEVEL_INFO,"Ŀ��λ��: %d\r\n", motor9a_target_position);
    LOG_Print(LOG_LEVEL_INFO,"λ�����: %d\r\n", motor9a_target_position - motor9a_current_position);
    LOG_Print(LOG_LEVEL_INFO,"����״̬: %s\r\n", motor9a_position_control_enabled ? "����" : "����");
    LOG_Print(LOG_LEVEL_INFO,"ÿ���������: %d\r\n", motor9a_max_steps_per_cycle);
    LOG_Print(LOG_LEVEL_INFO,"�Ƿ񵽴�Ŀ��: %s\r\n", (motor9a_current_position == motor9a_target_position) ? "��" : "��");
    LOG_Print(LOG_LEVEL_INFO,"========================\r\n");
}

/**
 * @brief �������12λ�ÿ��ƺ��� (ÿ20ms����һ��)
 * ����Ŀ��λ���Զ����㲢ִ�в���
 */
void Motor12_PositionControl(void)
{
    if (!motor12_position_control_enabled) {
        return; // λ�ÿ���δ����
    }
    // ����λ�ò�
    int32_t position_error = motor12_target_position - motor12_current_position;
    
    if (position_error == 0) {
			
				Spi_Write(0x24, 0x0000); //
				VD_FZ_Motor12();
				Motor12_DisablePositionControl(); 	//����λ�ú�ر�λ�ÿ���,����Ŀ��ֵ��Ҫ�ֶ�����λ�ÿ���
        return; // �ѵ���Ŀ��λ��
    }
    // ȷ���˶�����Ͳ���
    uint16_t steps_to_move;
    uint16_t register_value;
    
    if (position_error > 0) {
        // �������˶� - D12λ=0��ʾ������
        steps_to_move = (position_error > motor12_max_steps_per_cycle) ? motor12_max_steps_per_cycle : (uint16_t)position_error;
        // ����0xcfff�ĸ�4λ�����D12λ�������򣩣����ò�������12λ��
        register_value = (0xcfff & 0xF000) | (steps_to_move & 0x0FFF);
    } else {
        // �������˶� - D12λ=1��ʾ������
        int32_t abs_error = -position_error;
        steps_to_move = (abs_error > motor12_max_steps_per_cycle) ? motor12_max_steps_per_cycle : (uint16_t)abs_error;
        // ����0xcfff�ĸ�4λ������D12λ�������򣩣����ò�������12λ��
        register_value = (0xcfff & 0xF000) | 0x1000 | (steps_to_move & 0x0FFF);
    }
    
    // ���Ʋ�����12λ��Χ��
    if (steps_to_move > 0x0FFF) {
        steps_to_move = 0x0FFF;
        // ���¼���Ĵ���ֵ
        if (position_error > 0) {
            register_value = (0xcfff & 0xF000) | 0x0FFF;
        } else {
            register_value = (0xcfff & 0xF000) | 0x1000 | 0x0FFF;
        }
    }
    if (steps_to_move > 0) {
        // ִ�в��� - Motor12ʹ�üĴ���0x21-0x25
        Spi_Write(0x22, 0x0001); // ѡ��256��Ƶ  ��λ����=0 ����DT2��ʱ
        Spi_Write(0x23, 0xd8d8); // �������ռ�ձ�Ϊ 90%
        Spi_Write(0x24, register_value); // ����LED��� ������ʹ��/�ض� ����/ɲ�� ��������(D12) ����(D11-D0)
        Spi_Write(0x25, INTCTAB); // ���ò�������
        Spi_Write(0x26, 0x9e5e); // ����TESTEN2 FZTEST  �����������ʱ��OCP_dly  PWMRES PWMMODE
        VD_FZ_Motor12();
        
        // ���µ�ǰλ��
        if (position_error > 0) {
            motor12_current_position += steps_to_move;
        } else {
            motor12_current_position -= steps_to_move;
        }
				
				Motor12_PrintPositions();
    }
}

/**
 * @brief �������34λ�ÿ��ƺ��� (ÿ20ms����һ��)
 * ����Ŀ��λ���Զ����㲢ִ�в���
 */
void Motor34_PositionControl(void)
{
    if (!motor34_position_control_enabled) {
        return; // λ�ÿ���δ����
    }
    // ����λ�ò�
    int32_t position_error = motor34_target_position - motor34_current_position;
    
    if (position_error == 0) {
			
				Spi_Write(0x29, 0x0000); //
				VD_FZ_Motor34();
				Motor34_DisablePositionControl(); 	//����λ�ú�ر�λ�ÿ���,����Ŀ��ֵ��Ҫ�ֶ�����λ�ÿ���
			
        return; // �ѵ���Ŀ��λ��
    }
    // ȷ���˶�����Ͳ���
    uint16_t steps_to_move;
    uint16_t register_value;
    
    if (position_error > 0) {
        // �������˶� - D12λ=0��ʾ������
        steps_to_move = (position_error > motor34_max_steps_per_cycle) ? 
                       motor34_max_steps_per_cycle : (uint16_t)position_error;
        // ����0xcfff�ĸ�4λ�����D12λ�������򣩣����ò�������12λ��
        register_value = (0xcfff & 0xF000) | (steps_to_move & 0x0FFF);
    } else {
        // �������˶� - D12λ=1��ʾ������
        int32_t abs_error = -position_error;
        steps_to_move = (abs_error > motor34_max_steps_per_cycle) ? 
                       motor34_max_steps_per_cycle : (uint16_t)abs_error;
        // ����0xcfff�ĸ�4λ������D12λ�������򣩣����ò�������12λ��
        register_value = (0xcfff & 0xF000) | 0x1000 | (steps_to_move & 0x0FFF);
    }
    
    // ���Ʋ�����12λ��Χ��
    if (steps_to_move > 0x0FFF) {
        steps_to_move = 0x0FFF;
        // ���¼���Ĵ���ֵ
        if (position_error > 0) {
            register_value = (0xcfff & 0xF000) | 0x0FFF;
        } else {
            register_value = (0xcfff & 0xF000) | 0x1000 | 0x0FFF;
        }
    }
    if (steps_to_move > 0) {
        // ִ�в��� - Motor34ʹ�üĴ���0x27-0x2b
        Spi_Write(0x27, 0x0001); // ѡ��256��Ƶ  ��λ����=0 ����DT2��ʱ
        Spi_Write(0x28, 0xd8d8); // �������ռ�ձ�Ϊ 90%
        Spi_Write(0x29, register_value); // ����LED��� ������ʹ��/�ض� ����/ɲ�� ��������(D12) ����(D11-D0)
        Spi_Write(0x2a, INTCTCD); // ���ò�������
        Spi_Write(0x2b, 0x9e5e); // ����TESTEN2 FZTEST  �����������ʱ��OCP_dly  PWMRES PWMMODE
        VD_FZ_Motor34();
         
         // ���µ�ǰλ��
         if (position_error > 0) {
             motor34_current_position += steps_to_move;
        } else {
            motor34_current_position -= steps_to_move;
        }
				Motor34_PrintPositions();
    }
}

/**
 * @brief �������56λ�ÿ��ƺ��� (ÿ20ms����һ��)
 * ����Ŀ��λ���Զ����㲢ִ�в���
 */
void Motor56_PositionControl(void)
{
    if (!motor56_position_control_enabled) {
        return; // λ�ÿ���δ����
    }
    // ����λ�ò�
    int32_t position_error = motor56_target_position - motor56_current_position;
    
    if (position_error == 0) {
			
				Spi_Write(0x2e, 0x0000); //
				VD_FZ_Motor56();
				Motor56_DisablePositionControl(); 	//����λ�ú�ر�λ�ÿ���,����Ŀ��ֵ��Ҫ�ֶ�����λ�ÿ���
			
        return; // �ѵ���Ŀ��λ��
    }
    // ȷ���˶�����Ͳ���
    uint16_t steps_to_move;
    uint16_t register_value;
    
    if (position_error > 0) {
        // �������˶� - D12λ=0��ʾ������
        steps_to_move = (position_error > motor56_max_steps_per_cycle) ? 
                       motor56_max_steps_per_cycle : (uint16_t)position_error;
        // ����0xcfff�ĸ�4λ�����D12λ�������򣩣����ò�������12λ��
        register_value = (0xcfff & 0xF000) | (steps_to_move & 0x0FFF);
    } else {
        // �������˶� - D12λ=1��ʾ������
        int32_t abs_error = -position_error;
        steps_to_move = (abs_error > motor56_max_steps_per_cycle) ? 
                       motor56_max_steps_per_cycle : (uint16_t)abs_error;
        // ����0xcfff�ĸ�4λ������D12λ�������򣩣����ò�������12λ��
        register_value = (0xcfff & 0xF000) | 0x1000 | (steps_to_move & 0x0FFF);
    }
    
    // ���Ʋ�����12λ��Χ��
    if (steps_to_move > 0x0FFF) {
        steps_to_move = 0x0FFF;
        // ���¼���Ĵ���ֵ
        if (position_error > 0) {
            register_value = (0xcfff & 0xF000) | 0x0FFF;
        } else {
            register_value = (0xcfff & 0xF000) | 0x1000 | 0x0FFF;
        }
    }
    if (steps_to_move > 0) {
        // ִ�в��� - Motor56ʹ�üĴ���0x2d-0x31
        Spi_Write(0x2c, 0x0001); // ѡ��256��Ƶ  ��λ����=0 ����DT2��ʱ
        Spi_Write(0x2d, 0xd8d8); // �������ռ�ձ�Ϊ 90%
        Spi_Write(0x2e, register_value); // ����LED��� ������ʹ��/�ض� ����/ɲ�� ��������(D12) ����(D11-D0)
        Spi_Write(0x2F, INTCTEF); // ���ò�������
        Spi_Write(0x30, 0x9e5e); // ����TESTEN2 FZTEST  �����������ʱ��OCP_dly  PWMRES PWMMODE
        VD_FZ_Motor56();
         
         // ���µ�ǰλ��
         if (position_error > 0) {
             motor56_current_position += steps_to_move;
        } else {
            motor56_current_position -= steps_to_move;
        }
    }
}

/**
 * @brief �������78λ�ÿ��ƺ��� (ÿ20ms����һ��)
 * ����Ŀ��λ���Զ����㲢ִ�в���
 */
void Motor78_PositionControl(void)
{
    if (!motor78_position_control_enabled) {
        return; // λ�ÿ���δ����
    }
    // ����λ�ò�
    int32_t position_error = motor78_target_position - motor78_current_position;
    
    if (position_error == 0) {
			
				Spi_Write(0x33, 0x0000); //
				VD_FZ_Motor78();
				Motor78_DisablePositionControl(); 	//����λ�ú�ر�λ�ÿ���,����Ŀ��ֵ��Ҫ�ֶ�����λ�ÿ���
        return; // �ѵ���Ŀ��λ��
    }
    // ȷ���˶�����Ͳ���
    uint16_t steps_to_move;
    uint16_t register_value;
   
		//ZOOM2��Ҫ����

    if (position_error > 0) {
        // �������˶� - D12λ=0��ʾ������
        steps_to_move = (position_error > motor78_max_steps_per_cycle) ? 
                       motor78_max_steps_per_cycle : (uint16_t)position_error;
        // ����0xcfff�ĸ�4λ�����D12λ�������򣩣����ò�������12λ��
        register_value = (0xcfff & 0xF000) | (steps_to_move & 0x0FFF);
    } else {
        // �������˶� - D12λ=1��ʾ������
        int32_t abs_error = -position_error;
        steps_to_move = (abs_error > motor78_max_steps_per_cycle) ? 
                       motor78_max_steps_per_cycle : (uint16_t)abs_error;
        // ����0xcfff�ĸ�4λ������D12λ�������򣩣����ò�������12λ��
        register_value = (0xcfff & 0xF000) | 0x1000 | (steps_to_move & 0x0FFF);
    }
    
    // ���Ʋ�����12λ��Χ��
    if (steps_to_move > 0x0FFF) {
        steps_to_move = 0x0FFF;
        // ���¼���Ĵ���ֵ
        if (position_error > 0) {
            register_value = (0xcfff & 0xF000) | 0x0FFF;
        } else {
            register_value = (0xcfff & 0xF000) | 0x1000 | 0x0FFF;
        }
    }
    if (steps_to_move > 0) {
        // ִ�в��� - Motor78ʹ�üĴ���0x33-0x37
        Spi_Write(0x31, 0x0001); // ѡ��256��Ƶ  ��λ����=0 ����DT2��ʱ
        Spi_Write(0x32, 0xd8d8); // �������ռ�ձ�Ϊ 90%
        Spi_Write(0x33, register_value); // ����LED��� ������ʹ��/�ض� ����/ɲ�� ��������(D12) ����(D11-D0)
        Spi_Write(0x34, INTCTGH); // ���ò�������
        Spi_Write(0x35, 0x9e5e); // ����TESTEN2 FZTEST  �����������ʱ��OCP_dly  PWMRES PWMMODE
        VD_FZ_Motor78();
         
         // ���µ�ǰλ��
         if (position_error > 0) {
             motor78_current_position += steps_to_move;
        } else {
            motor78_current_position -= steps_to_move;
        }
						Motor78_PrintPositions();

    }
}

/**
 * @brief �������9Aλ�ÿ��ƺ��� (ÿ20ms����һ��)
 * ����Ŀ��λ���Զ����㲢ִ�в���
 */
void Motor9A_PositionControl(void)
{
    if (!motor9a_position_control_enabled) {
        return; // λ�ÿ���δ����
    }
    // ����λ�ò�
    int32_t position_error = motor9a_target_position - motor9a_current_position;
    
    if (position_error == 0) {
			
				Spi_Write(0x38, 0x0000); //
				VD_FZ_Motor9A();
				Motor9A_DisablePositionControl(); 	//����λ�ú�ر�λ�ÿ���,����Ŀ��ֵ��Ҫ�ֶ�����λ�ÿ���
			
        return; // �ѵ���Ŀ��λ��
    }
    // ȷ���˶�����Ͳ���
    uint16_t steps_to_move;
    uint16_t register_value;
    
    if (position_error > 0) {
        // �������˶� - D12λ=0��ʾ������
        steps_to_move = (position_error > motor9a_max_steps_per_cycle) ? 
                       motor9a_max_steps_per_cycle : (uint16_t)position_error;
        // ����0xcfff�ĸ�4λ�����D12λ�������򣩣����ò�������12λ��
        register_value = (0xcfff & 0xF000) | (steps_to_move & 0x0FFF);
    } else {
        // �������˶� - D12λ=1��ʾ������
        int32_t abs_error = -position_error;
        steps_to_move = (abs_error > motor9a_max_steps_per_cycle) ? 
                       motor9a_max_steps_per_cycle : (uint16_t)abs_error;
        // ����0xcfff�ĸ�4λ������D12λ�������򣩣����ò�������12λ��
        register_value = (0xcfff & 0xF000) | 0x1000 | (steps_to_move & 0x0FFF);
    }
    
    // ���Ʋ�����12λ��Χ��
    if (steps_to_move > 0x0FFF) {
        steps_to_move = 0x0FFF;
        // ���¼���Ĵ���ֵ
        if (position_error > 0) {
            register_value = (0xcfff & 0xF000) | 0x0FFF;
        } else {
            register_value = (0xcfff & 0xF000) | 0x1000 | 0x0FFF;
        }
    }
    if (steps_to_move > 0) {
        // ִ�в��� - Motor9Aʹ�üĴ���0x39-0x3d
        Spi_Write(0x36, 0x0001); // ѡ��256��Ƶ  ��λ����=0 ����DT2��ʱ
        Spi_Write(0x37, 0xd8d8); // �������ռ�ձ�Ϊ 90%
        Spi_Write(0x38, register_value); // ����LED��� ������ʹ��/�ض� ����/ɲ�� ��������(D12) ����(D11-D0)
        Spi_Write(0x39, INTCTIJ); // ���ò�������
        Spi_Write(0x3a, 0x9e5e); // ����TESTEN2 FZTEST  �����������ʱ��OCP_dly  PWMRES PWMMODE
        VD_FZ_Motor9A();
         
         // ���µ�ǰλ��
         if (position_error > 0) {
             motor9a_current_position += steps_to_move;
        } else {
            motor9a_current_position -= steps_to_move;
        }
										Motor9A_PrintPositions();

    }
}

/**
 * @brief �������12��VD������ƺ���
 */
void VD_FZ_Motor12(void)
{
    // ����Ƿ����ô�������
    if (vd_fz_enabled) {
        // ����Ƿ��ѴﵽĿ�����
        if (vd_fz_count >= vd_fz_target_count) {
            return; // �ѴﵽĿ���������ִ��
        }
        vd_fz_count++; // ����ִ�д���
    }
    
    // �������12��VD������� - ʹ��VD12����
    HAL_GPIO_WritePin(VD12_GPIO_Port, VD12_Pin, GPIO_PIN_SET);   // VD����ߵ�ƽ
    
    // ��ʱ20΢�루���ж���ʹ�ü򵥵�ѭ����ʱ��
    for(volatile int i = 0; i < 10; i++);  // ����ʱ���������ж���ʹ��HAL_Delay
    
    HAL_GPIO_WritePin(VD12_GPIO_Port, VD12_Pin, GPIO_PIN_RESET); // VD����͵�ƽ
}

/**
 * @brief �������34��VD������ƺ���
 */
void VD_FZ_Motor34(void)
{
    // ����Ƿ����ô�������
    if (vd_fz_enabled) {
        // ����Ƿ��ѴﵽĿ�����
        if (vd_fz_count >= vd_fz_target_count) {
            return; // �ѴﵽĿ���������ִ��
        }
        vd_fz_count++; // ����ִ�д���
    }
    
    // �������34��VD������� - ʹ��VD34����
    HAL_GPIO_WritePin(VD34_GPIO_Port, VD34_Pin, GPIO_PIN_SET);   // VD����ߵ�ƽ
    
    // ��ʱ20΢�루���ж���ʹ�ü򵥵�ѭ����ʱ��
    for(volatile int i = 0; i < 10; i++);  // ����ʱ���������ж���ʹ��HAL_Delay
    
    HAL_GPIO_WritePin(VD34_GPIO_Port, VD34_Pin, GPIO_PIN_RESET); // VD����͵�ƽ
}

/**
 * @brief �������56��VD������ƺ���
 */
void VD_FZ_Motor56(void)
{
    // ����Ƿ����ô�������
    if (vd_fz_enabled) {
        // ����Ƿ��ѴﵽĿ�����
        if (vd_fz_count >= vd_fz_target_count) {
            return; // �ѴﵽĿ���������ִ��
        }
        vd_fz_count++; // ����ִ�д���
    }
    
    // �������56��VD������� - ʹ��VD56����
    HAL_GPIO_WritePin(VD56_GPIO_Port, VD56_Pin, GPIO_PIN_SET);   // VD����ߵ�ƽ
    
    // ��ʱ20΢�루���ж���ʹ�ü򵥵�ѭ����ʱ��
    for(volatile int i = 0; i < 10; i++);  // ����ʱ���������ж���ʹ��HAL_Delay
    
    HAL_GPIO_WritePin(VD56_GPIO_Port, VD56_Pin, GPIO_PIN_RESET); // VD����͵�ƽ
}

/**
 * @brief �������78��VD������ƺ���
 */
void VD_FZ_Motor78(void)
{
    // ����Ƿ����ô�������
    if (vd_fz_enabled) {
        // ����Ƿ��ѴﵽĿ�����
        if (vd_fz_count >= vd_fz_target_count) {
            return; // �ѴﵽĿ���������ִ��
        }
        vd_fz_count++; // ����ִ�д���
    }
    
    // �������78��VD������� - ʹ��VD78����
    HAL_GPIO_WritePin(VD78_GPIO_Port, VD78_Pin, GPIO_PIN_SET);   // VD����ߵ�ƽ
    
    // ��ʱ20΢�루���ж���ʹ�ü򵥵�ѭ����ʱ��
    for(volatile int i = 0; i < 10; i++);  // ����ʱ���������ж���ʹ��HAL_Delay
    
    HAL_GPIO_WritePin(VD78_GPIO_Port, VD78_Pin, GPIO_PIN_RESET); // VD����͵�ƽ
}

/**
 * @brief �������9A��VD������ƺ���
 */
void VD_FZ_Motor9A(void)
{
    // ����Ƿ����ô�������
    if (vd_fz_enabled) {
        // ����Ƿ��ѴﵽĿ�����
        if (vd_fz_count >= vd_fz_target_count) {
            return; // �ѴﵽĿ���������ִ��
        }
        vd_fz_count++; // ����ִ�д���
    }
    
    // �������9A��VD������� - ʹ��VD9A����
    HAL_GPIO_WritePin(VD9A_GPIO_Port, VD9A_Pin, GPIO_PIN_SET);   // VD����ߵ�ƽ
    
    // ��ʱ20΢�루���ж���ʹ�ü򵥵�ѭ����ʱ��
    for(volatile int i = 0; i < 10; i++);  // ����ʱ���������ж���ʹ��HAL_Delay
    
    HAL_GPIO_WritePin(VD9A_GPIO_Port, VD9A_Pin, GPIO_PIN_RESET); // VD����͵�ƽ
}

/**
 * @brief ͨ��VD������ƺ��������������ݣ�
 * Ĭ�Ͽ������е����VD����
 */
void VD_FZ(void)
{
    // ����Ƿ����ô�������
    if (vd_fz_enabled) {
        // ����Ƿ��ѴﵽĿ�����
        if (vd_fz_count >= vd_fz_target_count) {
            return; // �ѴﵽĿ���������ִ��
        }
        vd_fz_count++; // ����ִ�д���
    }
    
    // ͬʱ�������е����VD���ţ������ݣ�
    HAL_GPIO_WritePin(VD12_GPIO_Port, VD12_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(VD34_GPIO_Port, VD34_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(VD56_GPIO_Port, VD56_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(VD78_GPIO_Port, VD78_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(VD9A_GPIO_Port, VD9A_Pin, GPIO_PIN_SET);
    
    // ��ʱ20΢�루���ж���ʹ�ü򵥵�ѭ����ʱ��
    for(volatile int i = 0; i < 10; i++);  // ����ʱ���������ж���ʹ��HAL_Delay
    
    HAL_GPIO_WritePin(VD12_GPIO_Port, VD12_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(VD34_GPIO_Port, VD34_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(VD56_GPIO_Port, VD56_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(VD78_GPIO_Port, VD78_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(VD9A_GPIO_Port, VD9A_Pin, GPIO_PIN_RESET);
}

void VD_IS(void)
{
    // �������B��VD�������
    // ����ʵ��Ӳ�����ӣ�������Ӧ��GPIO����
    // ����ʹ��IRCUT���������Ϊʾ��
    HAL_GPIO_WritePin(GPIOB, IRCUT_1_Pin, GPIO_PIN_SET);   // VDIS����ߵ�ƽ
    
    // ��ʱ20΢��
    for(volatile int i = 0; i < 100; i++);  // ����ʱ
    
    HAL_GPIO_WritePin(GPIOB, IRCUT_1_Pin, GPIO_PIN_RESET); // VDIS����͵�ƽ
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
	������Ӹ��������ܣ��������̰�����������������ϰ�����
*/
uint16_t u16KeyTest = 0;
/**
 * @brief �������ܴ�����
 * @param u32KeyValue ����ֵ�������������ͺͰ���״̬�����
 *                     ֧�ֶ̰�����������������ϰ����Ȳ���
 *                     ͨ����ͬ�������Ʋ��������ɲ��/���кͻ�����
 * @retval None
 * @note ����OutPutChennal����ѡ����Ƶĵ��ͨ��(1-5��A��ʾȫ��)
 *       SW1_UP: ���Ʋ������ɲ��/����״̬�л�
 *       SW1_DOWN: ���Ʋ����������
 */
void Key_Function(uint32_t u32KeyValue)
{
    if (u32KeyValue == (KEY_SHORT_DOWN | KEY_SW1_UP) || u32KeyValue == (KEY_CONTI_DOWN | KEY_SW1_UP))
    {   // SW1_UP   �̰������߳�������
        // ����ɲ��������
        Motor_ToggleBrakeRun(OutPutChennal);
    }
    else if (u32KeyValue == (KEY_SHORT_DOWN | KEY_SW1_DOWN) || u32KeyValue == (KEY_CONTI_DOWN | KEY_SW1_DOWN))
    {   // SW1_DOWN   �̰������߳�������
        // ��������
        Motor_ToggleDirection(OutPutChennal);
    }
    else if (u32KeyValue == (KEY_SHORT_DOWN | KEY_SW1_LEFT) || u32KeyValue == (KEY_CONTI_DOWN | KEY_SW1_LEFT))
    {   // SW1_LEFT   �̰������߳�������
        u16KeyTest = 3;
        // ��������
        Motor_DecreaseSpeed(OutPutChennal);
    }
    else if (u32KeyValue == (KEY_SHORT_DOWN | KEY_SW1_RIGHT) || u32KeyValue == (KEY_CONTI_DOWN | KEY_SW1_RIGHT))
    {   // SW1_RIGHT   �̰������߳�������
        // ��������
        u16KeyTest = 4;
        Motor_IncreaseSpeed(OutPutChennal);
    }
    else if (u32KeyValue == (KEY_SHORT_DOWN | KEY_SW1_ENTER) || u32KeyValue == (KEY_CONTI_DOWN | KEY_SW1_ENTER))
    {   // SW1_ENTER   �̰������߳�������
        // ģʽѡ��  LEDͨ������
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
    {   // SW2_LEFT	�̰������߳�������
        // ��С����
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
    {   // SW2_RIGHT	�̰������߳�������
        // �������
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
    {   // SW2_RIGHT	�̰������߳�������
        // ģʽѡ��
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
 * @brief SPI���Ժ��� - ��ȡ���мĴ�����ͨ���������
 * @param None
 * @retval None
 */
void SPI_Test_ReadAllRegisters(void)
{
    uint16_t regData;
    uint8_t addr;
    char buffer[64];
    
    UART_Transmit_Str(&huart3, (uint8_t*)"\r\n=== SPI Register Dump Start ===\r\n");
    
    // ��ȡ���õļĴ�����ַ��Χ (0x00-0x3F)
    for(addr = 0x00; addr <= 0x3F; addr++)
    {
        Spi__Read(addr, &regData);
        sprintf(buffer, "Reg[0x%02X] = 0x%04X\r\n", addr, regData);
        UART_Transmit_Str(&huart3, (uint8_t*)buffer);
        HAL_Delay(1); // ������ʱȷ��SPIͨ���ȶ�
    }
    
    UART_Transmit_Str(&huart3, (uint8_t*)"=== SPI Register Dump End ===\r\n\r\n");
}

/**
 * @brief SPI���Ժ��� - ������д����
 * @param None
 * @retval None
 */
void SPI_Test_Dump(void)
{
    uint16_t testData, readData;
    uint8_t testAddr = 0x25; // ʹ��һ�����ԼĴ�����ַ
    char buffer[64];
    
    UART_Transmit_Str(&huart1, (uint8_t*)"\r\n=== SPI Basic Test Start ===\r\n");
    
    // ��ȡԭʼ����
    Spi__Read(testAddr, &readData);
    sprintf(buffer, "Original Reg[0x%02X] = 0x%04X\r\n", testAddr, readData);
    UART_Transmit_Str(&huart1, (uint8_t*)buffer);
    
    // д���������
    testData = 0x1234;
    sprintf(buffer, "Writing 0x%04X to Reg[0x%02X]\r\n", testData, testAddr);
    UART_Transmit_Str(&huart1, (uint8_t*)buffer);
    Spi_Write(testAddr, testData);
    HAL_Delay(1);
    
    // ������֤
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
    
    // ����SPI�ֽڴ���
    UART_Transmit_Str(&huart1, (uint8_t*)"\r\nTesting SPI Byte Transfer:\r\n");
    uint8_t testByte = 0xAA;
    uint8_t receivedByte = Spi_ReadWriteByte(testByte);
    sprintf(buffer, "Sent: 0x%02X, Received: 0x%02X\r\n", testByte, receivedByte);
    UART_Transmit_Str(&huart1, (uint8_t*)buffer);
    
    UART_Transmit_Str(&huart1, (uint8_t*)"=== SPI Basic Test End ===\r\n\r\n");
}

/**
 * @brief �л����ɲ��/����״̬
 * @param channel ���ͨ�� ('1'-'5', 'A'��ʾ����ͨ��)
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
        // �������е��
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
 * @brief ���ò������Ŀ�경��
 * @param stepsA �������A��Ŀ�경��
 * @param stepsB �������B��Ŀ�경��
 * @retval None
 */
void Stepper_SetTargetSteps(uint16_t stepsA, uint16_t stepsB)
{
    target_steps_A = stepsA;
    target_steps_B = stepsB;
    current_steps_A = 0;
    current_steps_B = 0;
    
    // ����PSUMA��PSUMB����
    PSUMA = stepsA;
    PSUMB = stepsB;
}

/**
 * @brief ���������������
 * @retval None
 */
void Stepper_Start(void)
{
    stepper_running = true;
    current_steps_A = 0;
    current_steps_B = 0;
}

/**
 * @brief ֹͣ�����������
 * @retval None
 */
void Stepper_Stop(void)
{
    stepper_running = false;
    current_steps_A = 0;
    current_steps_B = 0;
}

/**
 * @brief ��ȡ�������A��ǰ����
 * @retval ��ǰ����
 */
uint16_t Stepper_GetStepsA(void)
{
    return current_steps_A;
}

/**
 * @brief ��ȡ�������B��ǰ����
 * @retval ��ǰ����
 */
uint16_t Stepper_GetStepsB(void)
{
    return current_steps_B;
}

/**
 * @brief ��鲽������Ƿ���������
 * @retval true: ��������, false: ��ֹͣ
 */
bool Stepper_IsRunning(void)
{
    return stepper_running && (current_steps_A < target_steps_A || current_steps_B < target_steps_B);
}

/**
 * @brief �����������ִ�к������ڶ�ʱ���ж��е��ã�
 * @retval None
 * @note �˺���Ӧ��VD_Stepper_Control�е���
 */
void Stepper_ExecuteStep(void)
{
    if (!stepper_running) {
        return;
    }
    
    // ִ�в������A��һ��
    if (current_steps_A < target_steps_A) {
        VD_FZ();
        current_steps_A++;
    }
    
    // ִ�в������B��һ��
    if (current_steps_B < target_steps_B) {
        VD_IS();
        current_steps_B++;
    }
    
    // ����Ƿ�������в���
    if (current_steps_A >= target_steps_A && current_steps_B >= target_steps_B) {
        stepper_running = false;
    }
}

/**
 * @brief �������
 * @param channel ���ͨ�� ('1'-'5', 'A'��ʾ����ͨ��)
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
        // �������е��
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
 * @brief �������
 * @param channel ���ͨ�� ('1'-'5', 'A'��ʾ����ͨ��)
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
        // �������е��
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
 * @brief �л��������
 * @param channel ���ͨ�� ('1'-'5', 'A'��ʾ����ͨ��)
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
        // �������е��
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
 * @brief ����������Ʋ��Ժ���
 * @retval None
 * @note ��ʾ���ʹ�ò����������ϵͳ
 */
void Stepper_Test_Demo(void)
{
    char buffer[100];
    
    // ���ò������A����100�����������B����50��
    Stepper_SetTargetSteps(100, 50);
    
    sprintf(buffer, "����Ŀ�경��: A=%d, B=%d\r\n", 100, 50);
    UART_Transmit_Str(&huart1, (uint8_t*)buffer);
    
    // �����������
    Stepper_Start();
    UART_Transmit_Str(&huart1, (uint8_t*)"�������������\r\n");
    
    // �ȴ���������������
    while(Stepper_IsRunning()) {
        HAL_Delay(100);  // �ȴ�100ms
        
        // �����ǰ����
        sprintf(buffer, "��ǰ����: A=%d/%d, B=%d/%d\r\n", 
                Stepper_GetStepsA(), 100,
                Stepper_GetStepsB(), 50);
        UART_Transmit_Str(&huart1, (uint8_t*)buffer);
    }
    
    UART_Transmit_Str(&huart1, (uint8_t*)"��������������\r\n");
}

/**
 * @brief ����VD_FZ������Ŀ��ִ�д���
 * @param count Ŀ��ִ�д���
 */
void VD_FZ_SetTargetCount(uint16_t count)
{
    vd_fz_target_count = count;
    vd_fz_count = 0; // ���õ�ǰ����
}

/**
 * @brief ����VD_FZ��������
 */
void VD_FZ_Enable(void)
{
    vd_fz_enabled = true;
}

/**
 * @brief ����VD_FZ��������
 */
void VD_FZ_Disable(void)
{
    vd_fz_enabled = false;
}

/**
 * @brief ��ȡVD_FZ��ǰִ�д���
 * @return ��ǰִ�д���
 */
uint16_t VD_FZ_GetCurrentCount(void)
{
    return vd_fz_count;
}

/**
 * @brief ��ȡVD_FZĿ��ִ�д���
 * @return Ŀ��ִ�д���
 */
uint16_t VD_FZ_GetTargetCount(void)
{
    return vd_fz_target_count;
}

/**
 * @brief ����VD_FZִ�д���
 */
void VD_FZ_ResetCount(void)
{
    vd_fz_count = 0;
}

/**
 * @brief ���VD_FZ�Ƿ������Ŀ�����
 * @return true: �����, false: δ���
 */
bool VD_FZ_IsCompleted(void)
{
    if (!vd_fz_enabled) {
        return false; // δ���ô�������ʱ����false
    }
    return (vd_fz_count >= vd_fz_target_count);
}

/**
 * @brief ͨ��0x24�Ĵ�������PSUMA��ֵ
 * @param psuma_value PSUMA��ֵ (0-4095, 12λ)
 * @note 0x24�Ĵ�����0-11λ��ӦPSUMA��ֵ
 */
void Set_PSUMA_Via_Register(uint16_t psuma_value)
{
    uint16_t reg_value;
    
    // ����PSUMAֵ��12λ��Χ�� (0-4095)
    psuma_value &= 0x0FFF;
    
    // ��ȡ0x24�Ĵ�����ǰֵ
    Spi__Read(0x24, &reg_value);
    
    // ���0-11λ (PSUMAλ)
    reg_value &= 0xF000;
    
    // �����µ�PSUMAֵ��0-11λ
    reg_value |= psuma_value;
    
    // д��0x24�Ĵ���
    Spi_Write(0x24, reg_value);
    
    // ͬʱ����ȫ�ֱ���PSUMA
    PSUMA = psuma_value;
}

/**
 * @brief ��0x24�Ĵ�����ȡPSUMA��ֵ
 * @return PSUMA�ĵ�ǰֵ (0-4095)
 */
uint16_t Get_PSUMA_From_Register(void)
{
    uint16_t reg_value;
    
    // ��ȡ0x24�Ĵ���ֵ
    Spi__Read(0x24, &reg_value);
    
    // ��ȡ0-11λ��ΪPSUMAֵ
    uint16_t psuma_value = reg_value & 0x0FFF;
    
    // ͬʱ����ȫ�ֱ���PSUMA
    PSUMA = psuma_value;
    
    return psuma_value;
}

// ==================== �������12 API���� ====================
/**
 * @brief ���ò������12��Ŀ��λ��
 * @param target_pos Ŀ��λ��ֵ
 */
void Motor12_SetTargetPosition(int32_t target_pos)
{
		Motor12_EnablePositionControl();
    motor12_target_position = target_pos;
}

/**
 * @brief ��ȡ�������12�ĵ�ǰλ��
 * @return ��ǰλ��ֵ
 */
int32_t Motor12_GetCurrentPosition(void)
{
    return motor12_current_position;
}

/**
 * @brief ��ȡ�������12��Ŀ��λ��
 * @return Ŀ��λ��ֵ
 */
int32_t Motor12_GetTargetPosition(void)
{
    return motor12_target_position;
}

/**
 * @brief ���ò������12�ĵ�ǰλ�� (����λ��У׼)
 * @param current_pos ��ǰλ��ֵ
 */
void Motor12_SetCurrentPosition(int32_t current_pos)
{
    motor12_current_position = current_pos;
}

/**
 * @brief ���ò������12λ�ÿ���
 */
void Motor12_EnablePositionControl(void)
{
    motor12_position_control_enabled = true;
}

/**
 * @brief ���ò������12λ�ÿ���
 */
void Motor12_DisablePositionControl(void)
{
    motor12_position_control_enabled = false;
}

/**
 * @brief ��鲽�����12�Ƿ��ѵ���Ŀ��λ��
 * @return true: �ѵ���, false: δ����
 */
bool Motor12_IsAtTargetPosition(void)
{
    return (motor12_current_position == motor12_target_position);
}

/**
 * @brief ��ȡ�������12��λ�����
 * @return λ����� (Ŀ��λ�� - ��ǰλ��)
 */
int32_t Motor12_GetPositionError(void)
{
    return (motor12_target_position - motor12_current_position);
}

/**
 * @brief ���ò������12ÿ�����ڵ������
 * @param max_steps ����� (1-4095)
 */
void Motor12_SetMaxStepsPerCycle(uint16_t max_steps)
{
    if (max_steps > 0 && max_steps <= 0x0FFF) {
        motor12_max_steps_per_cycle = max_steps;
    }
}

/**
 * @brief ��ȡ�������12ÿ�����ڵ������
 * @return �����
 */
uint16_t Motor12_GetMaxStepsPerCycle(void)
{
    return motor12_max_steps_per_cycle;
}

/**
 * @brief �������12λ�ø�λ (���õ�ǰλ�ú�Ŀ��λ��Ϊ0)
 */
void Motor12_ResetPosition(void)
{
    motor12_current_position = 0;
    motor12_target_position = 0;
}

// ==================== �������34 API���� ====================
/**
 * @brief ���ò������34��Ŀ��λ��
 * @param target_pos Ŀ��λ��ֵ
 */
void Motor34_SetTargetPosition(int32_t target_pos)
{
		Motor34_EnablePositionControl();
    motor34_target_position = target_pos;
}

/**
 * @brief ��ȡ�������34�ĵ�ǰλ��
 * @return ��ǰλ��ֵ
 */
int32_t Motor34_GetCurrentPosition(void)
{
    return motor34_current_position;
}

/**
 * @brief ��ȡ�������34��Ŀ��λ��
 * @return Ŀ��λ��ֵ
 */
int32_t Motor34_GetTargetPosition(void)
{
    return motor34_target_position;
}

/**
 * @brief ���ò������34�ĵ�ǰλ�� (����λ��У׼)
 * @param current_pos ��ǰλ��ֵ
 */
void Motor34_SetCurrentPosition(int32_t current_pos)
{
    motor34_current_position = current_pos;
}

/**
 * @brief ���ò������34λ�ÿ���
 */
void Motor34_EnablePositionControl(void)
{
    motor34_position_control_enabled = true;
}

/**
 * @brief ���ò������34λ�ÿ���
 */
void Motor34_DisablePositionControl(void)
{
    motor34_position_control_enabled = false;
}

/**
 * @brief ��鲽�����34�Ƿ��ѵ���Ŀ��λ��
 * @return true: �ѵ���, false: δ����
 */
bool Motor34_IsAtTargetPosition(void)
{
    return (motor34_current_position == motor34_target_position);
}

/**
 * @brief ��ȡ�������34��λ�����
 * @return λ����� (Ŀ��λ�� - ��ǰλ��)
 */
int32_t Motor34_GetPositionError(void)
{
    return (motor34_target_position - motor34_current_position);
}

/**
 * @brief ���ò������34ÿ�����ڵ������
 * @param max_steps ����� (1-4095)
 */
void Motor34_SetMaxStepsPerCycle(uint16_t max_steps)
{
    if (max_steps > 0 && max_steps <= 0x0FFF) {
        motor34_max_steps_per_cycle = max_steps;
    }
}

/**
 * @brief ��ȡ�������34ÿ�����ڵ������
 * @return �����
 */
uint16_t Motor34_GetMaxStepsPerCycle(void)
{
    return motor34_max_steps_per_cycle;
}

/**
 * @brief �������34λ�ø�λ (���õ�ǰλ�ú�Ŀ��λ��Ϊ0)
 */
void Motor34_ResetPosition(void)
{
    motor34_current_position = 0;
    motor34_target_position = 0;
}

// ==================== �������56 API���� ====================
/**
 * @brief ���ò������56��Ŀ��λ��
 * @param target_pos Ŀ��λ��ֵ
 */
void Motor56_SetTargetPosition(int32_t target_pos)
{
		Motor56_EnablePositionControl();
    motor56_target_position = target_pos;
}

/**
 * @brief ��ȡ�������56�ĵ�ǰλ��
 * @return ��ǰλ��ֵ
 */
int32_t Motor56_GetCurrentPosition(void)
{
    return motor56_current_position;
}

/**
 * @brief ��ȡ�������56��Ŀ��λ��
 * @return Ŀ��λ��ֵ
 */
int32_t Motor56_GetTargetPosition(void)
{
    return motor56_target_position;
}

/**
 * @brief ���ò������56�ĵ�ǰλ�� (����λ��У׼)
 * @param current_pos ��ǰλ��ֵ
 */
void Motor56_SetCurrentPosition(int32_t current_pos)
{
    motor56_current_position = current_pos;
}

/**
 * @brief ���ò������56λ�ÿ���
 */
void Motor56_EnablePositionControl(void)
{
    motor56_position_control_enabled = true;
}

/**
 * @brief ���ò������56λ�ÿ���
 */
void Motor56_DisablePositionControl(void)
{
    motor56_position_control_enabled = false;
}

/**
 * @brief ��鲽�����56�Ƿ��ѵ���Ŀ��λ��
 * @return true: �ѵ���, false: δ����
 */
bool Motor56_IsAtTargetPosition(void)
{
    return (motor56_current_position == motor56_target_position);
}

/**
 * @brief ��ȡ�������56��λ�����
 * @return λ����� (Ŀ��λ�� - ��ǰλ��)
 */
int32_t Motor56_GetPositionError(void)
{
    return (motor56_target_position - motor56_current_position);
}

/**
 * @brief ���ò������56ÿ�����ڵ������
 * @param max_steps ����� (1-4095)
 */
void Motor56_SetMaxStepsPerCycle(uint16_t max_steps)
{
    if (max_steps > 0 && max_steps <= 0x0FFF) {
        motor56_max_steps_per_cycle = max_steps;
    }
}

/**
 * @brief ��ȡ�������56ÿ�����ڵ������
 * @return �����
 */
uint16_t Motor56_GetMaxStepsPerCycle(void)
{
    return motor56_max_steps_per_cycle;
}

/**
 * @brief �������56λ�ø�λ (���õ�ǰλ�ú�Ŀ��λ��Ϊ0)
 */
void Motor56_ResetPosition(void)
{
    motor56_current_position = 0;
    motor56_target_position = 0;
}

// ==================== �������78 API���� ====================
/**
 * @brief ���ò������78��Ŀ��λ��
 * @param target_pos Ŀ��λ��ֵ
 */
void Motor78_SetTargetPosition(int32_t target_pos)
{
		Motor78_EnablePositionControl();
    motor78_target_position = target_pos;
}

/**
 * @brief ��ȡ�������78�ĵ�ǰλ��
 * @return ��ǰλ��ֵ
 */
int32_t Motor78_GetCurrentPosition(void)
{
    return motor78_current_position;
}

/**
 * @brief ��ȡ�������78��Ŀ��λ��
 * @return Ŀ��λ��ֵ
 */
int32_t Motor78_GetTargetPosition(void)
{
    return motor78_target_position;
}

/**
 * @brief ���ò������78�ĵ�ǰλ�� (����λ��У׼)
 * @param current_pos ��ǰλ��ֵ
 */
void Motor78_SetCurrentPosition(int32_t current_pos)
{
    motor78_current_position = current_pos;
}

/**
 * @brief ���ò������78λ�ÿ���
 */
void Motor78_EnablePositionControl(void)
{
    motor78_position_control_enabled = true;
}

/**
 * @brief ���ò������78λ�ÿ���
 */
void Motor78_DisablePositionControl(void)
{
    motor78_position_control_enabled = false;
}

/**
 * @brief ��鲽�����78�Ƿ��ѵ���Ŀ��λ��
 * @return true: �ѵ���, false: δ����
 */
bool Motor78_IsAtTargetPosition(void)
{
    return (motor78_current_position == motor78_target_position);
}

/**
 * @brief ��ȡ�������78��λ�����
 * @return λ����� (Ŀ��λ�� - ��ǰλ��)
 */
int32_t Motor78_GetPositionError(void)
{
    return (motor78_target_position - motor78_current_position);
}

/**
 * @brief ���ò������78ÿ�����ڵ������
 * @param max_steps ����� (1-4095)
 */
void Motor78_SetMaxStepsPerCycle(uint16_t max_steps)
{
    if (max_steps > 0 && max_steps <= 0x0FFF) {
        motor78_max_steps_per_cycle = max_steps;
    }
}

/**
 * @brief ��ȡ�������78ÿ�����ڵ������
 * @return �����
 */
uint16_t Motor78_GetMaxStepsPerCycle(void)
{
    return motor78_max_steps_per_cycle;
}

/**
 * @brief �������78λ�ø�λ (���õ�ǰλ�ú�Ŀ��λ��Ϊ0)
 */
void Motor78_ResetPosition(void)
{
    motor78_current_position = 0;
    motor78_target_position = 0;
}
// ==================== Motor9A API Functions ====================

/**
 * @brief ���ò������9A��Ŀ��λ��
 * @param target_pos Ŀ��λ��
 */
void Motor9A_SetTargetPosition(int32_t target_pos)
{
    Motor9A_EnablePositionControl();
    motor9a_target_position = target_pos;
}

/**
 * @brief ��ȡ�������9A�ĵ�ǰλ��
 * @return ��ǰλ��
 */
int32_t Motor9A_GetCurrentPosition(void)
{
    return motor9a_current_position;
}

/**
 * @brief ��ȡ�������9A��Ŀ��λ��
 * @return Ŀ��λ��
 */
int32_t Motor9A_GetTargetPosition(void)
{
    return motor9a_target_position;
}

/**
 * @brief ���ò������9A�ĵ�ǰλ��
 * @param current_pos ��ǰλ��
 */
void Motor9A_SetCurrentPosition(int32_t current_pos)
{
    motor9a_current_position = current_pos;
}

/**
 * @brief ���ò������9A��λ�ÿ���
 */
void Motor9A_EnablePositionControl(void)
{
    motor9a_position_control_enabled = true;
}

/**
 * @brief ���ò������9A��λ�ÿ���
 */
void Motor9A_DisablePositionControl(void)
{
    motor9a_position_control_enabled = false;
}

/**
 * @brief ��鲽�����9A�Ƿ��ѵ���Ŀ��λ��
 * @return true ����ѵ���Ŀ��λ�ã�false ����
 */
bool Motor9A_IsAtTargetPosition(void)
{
    return motor9a_current_position == motor9a_target_position;
}

/**
 * @brief ��ȡ�������9A��λ�����
 * @return λ����Ŀ��λ�� - ��ǰλ�ã�
 */
int32_t Motor9A_GetPositionError(void)
{
    return motor9a_target_position - motor9a_current_position;
}

/**
 * @brief ���ò������9Aÿ�����ڵ������
 * @param max_steps �����
 */
void Motor9A_SetMaxStepsPerCycle(uint16_t max_steps)
{
    if (max_steps > 0 && max_steps <= 0x0FFF) {
        motor9a_max_steps_per_cycle = max_steps;
    }
}

/**
 * @brief ��ȡ�������9Aÿ�����ڵ������
 * @return �����
 */
uint16_t Motor9A_GetMaxStepsPerCycle(void)
{
    return motor9a_max_steps_per_cycle;
}

/**
 * @brief ���ò������9A��λ��
 */
void Motor9A_ResetPosition(void)
{
    motor9a_current_position = 0;
    motor9a_target_position = 0;
}

// ==================== Motor Reset Functions ====================

/**
 * @brief �����λ���λ��ֵ
 */
#define MOTOR_RESET_MAX_POSITION 0x0FFFFFFF

/**
 * @brief �����λ��ʱʱ�䣨���룩
 */
#define MOTOR_RESET_TIMEOUT_MS 10000

// �����λ״̬����
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
 * @brief ����ZOOM3�����λ���̣���������
 * @return 0: �ɹ�����, -1: ���ڸ�λ��
 */
int Motor_ZOOM3_Reset_Start(void)
{
    if (zoom3_reset_state != MOTOR_RESET_IDLE && zoom3_reset_state != MOTOR_RESET_COMPLETED && zoom3_reset_state != MOTOR_RESET_TIMEOUT)
    {
        return -1; // ���ڸ�λ��
    }
    
    LOG_Print(LOG_LEVEL_INFO,"Starting ZOOM3 motor reset process...\r\n");
    
    // ��ʼ��CO�ź�״̬
    zoom3_last_co_state = HAL_GPIO_ReadPin(ZOOM3_PI_CO_GPIO_Port, ZOOM3_PI_CO_Pin);
    
    // ����1: ����Ŀ��λ��Ϊ���ֵ
    if(zoom3_last_co_state==GPIO_PIN_RESET){
        Motor34_SetTargetPosition(MOTOR_RESET_MAX_POSITION);
    }
    else{
        Motor34_SetTargetPosition(-MOTOR_RESET_MAX_POSITION);
    }
    Motor34_EnablePositionControl();
    
    LOG_Print(LOG_LEVEL_INFO,"ZOOM3 target position set to maximum: %d\r\n", MOTOR_RESET_MAX_POSITION);
    
    
    // ����״̬�Ϳ�ʼʱ��
    zoom3_reset_state = MOTOR_RESET_MOVING;
    zoom3_reset_start_time = HAL_GetTick();
    
    return 0;
}

/**
 * @brief ���ZOOM3�����λ״̬����������
 * ��Ҫ����ѭ���ж��ڵ���
 * @return MOTOR_RESET_IDLE: ����, MOTOR_RESET_MOVING: �˶���, 
 *         MOTOR_RESET_COMPLETED: ���, MOTOR_RESET_TIMEOUT: ��ʱ
 */
MotorResetState_t Motor_ZOOM3_Reset_Process(void)
{
    if (zoom3_reset_state == MOTOR_RESET_MOVING)
    {
        // ��鳬ʱ
        if ((HAL_GetTick() - zoom3_reset_start_time) > MOTOR_RESET_TIMEOUT_MS)
        {
            LOG_Print(LOG_LEVEL_INFO,"ZOOM3 motor reset timeout!\r\n");
            zoom3_reset_state = MOTOR_RESET_TIMEOUT;
            return zoom3_reset_state;
        }
        
        // ���CO�ź��Ƿ����仯����ƽ�仯��λ�źţ�
        GPIO_PinState current_co_state = HAL_GPIO_ReadPin(ZOOM3_PI_CO_GPIO_Port, ZOOM3_PI_CO_Pin);
        if (current_co_state != zoom3_last_co_state)
        {
            // ����3: ��⵽��ƽ�仯������ǰλ�ø���Ϊ0
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
 * @brief ��ȡZOOM3�����λ״̬
 * @return ��ǰ��λ״̬
 */
MotorResetState_t Motor_ZOOM3_Reset_GetState(void)
{
    return zoom3_reset_state;
}

/**
 * @brief ����ZOOM3�����λ״̬Ϊ����
 */
void Motor_ZOOM3_Reset_Clear(void)
{
		Motor34_SetCurrentPosition(ZOOM3_ZERO_POS*4);
		Motor34_SetTargetPosition(ZOOM3_ZERO_POS*4);
    zoom3_reset_state = MOTOR_RESET_IDLE;
		Motor34_PrintPositions();

}

/**
 * @brief ����FOCUS�����λ���̣���������
 * @return 0: �ɹ�����, -1: ���ڸ�λ��
 */
int Motor_FOCUS_Reset_Start(void)
{
    if (focus_reset_state != MOTOR_RESET_IDLE && focus_reset_state != MOTOR_RESET_COMPLETED && focus_reset_state != MOTOR_RESET_TIMEOUT)
    {
        return -1; // ���ڸ�λ��
    }
    
    LOG_Print(LOG_LEVEL_INFO,"Starting FOCUS motor reset process...\r\n");
    
    // ��ʼ��CO�ź�״̬
    focus_last_co_state = HAL_GPIO_ReadPin(FOCUS_PI_CO_GPIO_Port, FOCUS_PI_CO_Pin);
    
    // ����1: ���ݳ�ʼCO�ź�״̬����Ŀ��λ��
    if(focus_last_co_state==GPIO_PIN_RESET){
        Motor9A_SetTargetPosition(-MOTOR_RESET_MAX_POSITION);
    }
    else{
        Motor9A_SetTargetPosition(MOTOR_RESET_MAX_POSITION);
    }
    Motor9A_EnablePositionControl();
    
    LOG_Print(LOG_LEVEL_INFO,"FOCUS target position set to maximum: %d\r\n", MOTOR_RESET_MAX_POSITION);
    
    // ����״̬�Ϳ�ʼʱ��
    focus_reset_state = MOTOR_RESET_MOVING;
    focus_reset_start_time = HAL_GetTick();
    
    return 0;
}

/**
 * @brief ���FOCUS�����λ״̬����������
 * ��Ҫ����ѭ���ж��ڵ���
 * @return MOTOR_RESET_IDLE: ����, MOTOR_RESET_MOVING: �˶���, 
 *         MOTOR_RESET_COMPLETED: ���, MOTOR_RESET_TIMEOUT: ��ʱ
 */
MotorResetState_t Motor_FOCUS_Reset_Process(void)
{
    if (focus_reset_state == MOTOR_RESET_MOVING)
    {
        // ��鳬ʱ
        if ((HAL_GetTick() - focus_reset_start_time) > MOTOR_RESET_TIMEOUT_MS)
        {
            LOG_Print(LOG_LEVEL_INFO,"FOCUS motor reset timeout!\r\n");
            focus_reset_state = MOTOR_RESET_TIMEOUT;
            return focus_reset_state;
        }
        
        // ���CO�ź��Ƿ����仯����ƽ�仯��λ�źţ�
        GPIO_PinState current_co_state = HAL_GPIO_ReadPin(FOCUS_PI_CO_GPIO_Port, FOCUS_PI_CO_Pin);
        if (current_co_state != focus_last_co_state)
        {
            // ����3: ��⵽��ƽ�仯������ǰλ�ø���Ϊ0
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
 * @brief ��ȡFOCUS�����λ״̬
 * @return ��ǰ��λ״̬
 */
MotorResetState_t Motor_FOCUS_Reset_GetState(void)
{
    return focus_reset_state;
}

/**
 * @brief ����FOCUS�����λ״̬Ϊ����
 */
void Motor_FOCUS_Reset_Clear(void)
{
		Motor9A_SetCurrentPosition(FOUCE_ZERO_POS*4);
		Motor9A_SetTargetPosition(FOUCE_ZERO_POS*4);
    focus_reset_state = MOTOR_RESET_IDLE;
}

/**
 * @brief ����ZOOM2�����λ���̣���������
 * @return 0: �ɹ�����, -1: ���ڸ�λ��
 */
int Motor_ZOOM2_Reset_Start(void)
{
    if (zoom2_reset_state != MOTOR_RESET_IDLE && zoom2_reset_state != MOTOR_RESET_COMPLETED && zoom2_reset_state != MOTOR_RESET_TIMEOUT)
    {
        return -1; // ���ڸ�λ��
    }
    
    LOG_Print(LOG_LEVEL_INFO,"Starting ZOOM2 motor reset process...\r\n");
    
    // ��ʼ��CO�ź�״̬
    zoom2_last_co_state = HAL_GPIO_ReadPin(ZOOM2_PI_CO_GPIO_Port, ZOOM2_PI_CO_Pin);
    
    // ����1: ���ݳ�ʼCO�ź�״̬����Ŀ��λ��
    if(zoom2_last_co_state==GPIO_PIN_RESET){
        Motor78_SetTargetPosition(MOTOR_RESET_MAX_POSITION);
    }
    else{
        Motor78_SetTargetPosition(-MOTOR_RESET_MAX_POSITION);
    }
    Motor78_EnablePositionControl();
    
    LOG_Print(LOG_LEVEL_INFO,"ZOOM2 target position set to maximum: %d\r\n", MOTOR_RESET_MAX_POSITION);
    
    // ����״̬�Ϳ�ʼʱ��
    zoom2_reset_state = MOTOR_RESET_MOVING;
    zoom2_reset_start_time = HAL_GetTick();
    
    return 0;
}

/**
 * @brief ���ZOOM2�����λ״̬����������
 * ��Ҫ����ѭ���ж��ڵ���
 * @return MOTOR_RESET_IDLE: ����, MOTOR_RESET_MOVING: �˶���, 
 *         MOTOR_RESET_COMPLETED: ���, MOTOR_RESET_TIMEOUT: ��ʱ
 */
MotorResetState_t Motor_ZOOM2_Reset_Process(void)
{
    if (zoom2_reset_state == MOTOR_RESET_MOVING)
    {
        // ��鳬ʱ
        if ((HAL_GetTick() - zoom2_reset_start_time) > MOTOR_RESET_TIMEOUT_MS)
        {
            LOG_Print(LOG_LEVEL_INFO,"ZOOM2 motor reset timeout!\r\n");
            zoom2_reset_state = MOTOR_RESET_TIMEOUT;
            return zoom2_reset_state;
        }
        
        // ���CO�ź��Ƿ����仯����λ�źţ�
        GPIO_PinState current_co_state = HAL_GPIO_ReadPin(ZOOM2_PI_CO_GPIO_Port, ZOOM2_PI_CO_Pin);
        if (current_co_state != zoom2_last_co_state)
        {
            // ����3: ��⵽CO�źű仯������ǰλ�ø���Ϊ0
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
 * @brief ��ȡZOOM2�����λ״̬
 * @return ��ǰ��λ״̬
 */
MotorResetState_t Motor_ZOOM2_Reset_GetState(void)
{
    return zoom2_reset_state;
}

/**
 * @brief ����ZOOM2�����λ״̬Ϊ����
 */
void Motor_ZOOM2_Reset_Clear(void)
{
		Motor78_SetCurrentPosition(ZOOM2_ZERO_POS*4);
		Motor78_SetTargetPosition(ZOOM2_ZERO_POS*4);
    zoom2_reset_state = MOTOR_RESET_IDLE;
}

/**
 * @brief ����ZOOM1�����λ���̣���������
 * @return 0: �ɹ�����, -1: ���ڸ�λ��
 */
int Motor_ZOOM1_Reset_Start(void)
{
    if (zoom1_reset_state != MOTOR_RESET_IDLE && zoom1_reset_state != MOTOR_RESET_COMPLETED && zoom1_reset_state != MOTOR_RESET_TIMEOUT)
    {
        return -1; // ���ڸ�λ��
    }
    
    LOG_Print(LOG_LEVEL_INFO,"Starting ZOOM1 motor reset process...\r\n");
    
    // ��ʼ��CO�ź�״̬
    zoom1_last_co_state = HAL_GPIO_ReadPin(ZOOM1_PI_CO_GPIO_Port, ZOOM1_PI_CO_Pin);
    
    // ����1: ���ݳ�ʼCO�ź�״̬����Ŀ��λ��
    if(zoom1_last_co_state==GPIO_PIN_RESET){
        Motor12_SetTargetPosition(MOTOR_RESET_MAX_POSITION);
    }
    else{
        Motor12_SetTargetPosition(-MOTOR_RESET_MAX_POSITION);
    }
    Motor12_EnablePositionControl();
    
    LOG_Print(LOG_LEVEL_INFO,"ZOOM1 target position set to maximum: %d\r\n", MOTOR_RESET_MAX_POSITION);
    
    // ����״̬�Ϳ�ʼʱ��
    zoom1_reset_state = MOTOR_RESET_MOVING;
    zoom1_reset_start_time = HAL_GetTick();
    
    return 0;
}

/**
 * @brief ���ZOOM1�����λ״̬����������
 * ��Ҫ����ѭ���ж��ڵ���
 * @return MOTOR_RESET_IDLE: ����, MOTOR_RESET_MOVING: �˶���, 
 *         MOTOR_RESET_COMPLETED: ���, MOTOR_RESET_TIMEOUT: ��ʱ
 */
MotorResetState_t Motor_ZOOM1_Reset_Process(void)
{
    if (zoom1_reset_state == MOTOR_RESET_MOVING)
    {
        // ��鳬ʱ
        if ((HAL_GetTick() - zoom1_reset_start_time) > MOTOR_RESET_TIMEOUT_MS)
        {
            LOG_Print(LOG_LEVEL_INFO,"ZOOM1 motor reset timeout!\r\n");
            zoom1_reset_state = MOTOR_RESET_TIMEOUT;
            return zoom1_reset_state;
        }
        
        // ���CO�ź��Ƿ����仯����λ�źţ�
        GPIO_PinState current_co_state = HAL_GPIO_ReadPin(ZOOM1_PI_CO_GPIO_Port, ZOOM1_PI_CO_Pin);
        if (current_co_state != zoom1_last_co_state)
        {
            // ����3: ��⵽CO�źű仯������ǰλ�ø���Ϊ0
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
 * @brief ��ȡZOOM1�����λ״̬
 * @return ��ǰ��λ״̬
 */
MotorResetState_t Motor_ZOOM1_Reset_GetState(void)
{
    return zoom1_reset_state;
}

/**
 * @brief ����ZOOM1�����λ״̬Ϊ����
 */
void Motor_ZOOM1_Reset_Clear(void)
{
		Motor12_SetCurrentPosition(ZOOM1_ZERO_POS*8);
		Motor12_SetTargetPosition(ZOOM1_ZERO_POS*8);
    zoom1_reset_state = MOTOR_RESET_IDLE;
}

/**
 * @brief ����IRIS�����λ���̣���������
 * @return 0: �ɹ�����, -1: ���ڸ�λ��
 */
int Motor_IRIS_Reset_Start(void)
{
    if (iris_reset_state != MOTOR_RESET_IDLE && iris_reset_state != MOTOR_RESET_COMPLETED && iris_reset_state != MOTOR_RESET_TIMEOUT)
    {
        return -1; // ���ڸ�λ��
    }
    
    LOG_Print(LOG_LEVEL_INFO,"Starting IRIS motor reset process...\r\n");
    
    // ��ʼ��CO�ź�״̬
    iris_last_co_state = HAL_GPIO_ReadPin(IRIS_PI_CO_GPIO_Port, IRIS_PI_CO_Pin);
    
    // ����1: ���ݳ�ʼCO�ź�״̬����Ŀ��λ��
    if(iris_last_co_state==GPIO_PIN_RESET){
        Motor56_SetTargetPosition(-MOTOR_RESET_MAX_POSITION);
    }
    else{
        Motor56_SetTargetPosition(MOTOR_RESET_MAX_POSITION);
    }
    Motor56_EnablePositionControl();
    
    LOG_Print(LOG_LEVEL_INFO,"IRIS target position set to maximum: %d\r\n", MOTOR_RESET_MAX_POSITION);
    
    // ����״̬�Ϳ�ʼʱ��
    iris_reset_state = MOTOR_RESET_MOVING;
    iris_reset_start_time = HAL_GetTick();
    
    return 0;
}

/**
 * @brief ���IRIS�����λ״̬����������
 * ��Ҫ����ѭ���ж��ڵ���
 * @return MOTOR_RESET_IDLE: ����, MOTOR_RESET_MOVING: �˶���, 
 *         MOTOR_RESET_COMPLETED: ���, MOTOR_RESET_TIMEOUT: ��ʱ
 */
MotorResetState_t Motor_IRIS_Reset_Process(void)
{
    if (iris_reset_state == MOTOR_RESET_MOVING)
    {
        // ��鳬ʱ
        if ((HAL_GetTick() - iris_reset_start_time) > MOTOR_RESET_TIMEOUT_MS)
        {
            LOG_Print(LOG_LEVEL_INFO,"IRIS motor reset timeout!\r\n");
            iris_reset_state = MOTOR_RESET_TIMEOUT;
            return iris_reset_state;
        }
        
        // ���CO�ź��Ƿ����仯����λ�źţ�
        GPIO_PinState current_co_state = HAL_GPIO_ReadPin(IRIS_PI_CO_GPIO_Port, IRIS_PI_CO_Pin);
        if (current_co_state != iris_last_co_state)
        {
            // ����3: ��⵽CO�źű仯������ǰλ�ø���Ϊ0
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
 * @brief ��ȡIRIS�����λ״̬
 * @return ��ǰ��λ״̬
 */
MotorResetState_t Motor_IRIS_Reset_GetState(void)
{
    return iris_reset_state;
}

/**
 * @brief ����IRIS�����λ״̬Ϊ����
 */
void Motor_IRIS_Reset_Clear(void)
{
		Motor56_SetCurrentPosition(0);
		Motor56_SetTargetPosition(0);
    iris_reset_state = MOTOR_RESET_IDLE;
}

/**
 * @brief ��鵥�������CO�ź�״̬
 * @param motor_name �������
 * @param gpio_port GPIO�˿�
 * @param gpio_pin GPIO����
 * @return true��ʾCO�ź�Ϊ�ߵ�ƽ����λ����false��ʾ�͵�ƽ��δ��λ��
 */
bool Motor_CheckCOStatus(const char* motor_name, GPIO_TypeDef* gpio_port, uint16_t gpio_pin)
{
    GPIO_PinState pin_state = HAL_GPIO_ReadPin(gpio_port, gpio_pin);
    bool is_high = (pin_state == GPIO_PIN_SET);
    
    LOG_Print(LOG_LEVEL_INFO, "%s CO�ź�״̬: %s\r\n", motor_name, is_high ? "�ߵ�ƽ����λ��" : "�͵�ƽ��δ��λ��");
    return is_high;
}

/**
 * @brief ������е����CO�ź�״̬
 * @return true��ʾ����CO�źŶ�Ϊ�ߵ�ƽ��ȫ����λ����false��ʾ������һ��δ��λ
 */
void Motors_CheckAllCOStatus(void)
{
    //LOG_Print(LOG_LEVEL_INFO, "=== ������е��CO�ź�״̬ ===\r\n");
    
    bool zoom1_status = Motor_CheckCOStatus("ZOOM1", ZOOM1_PI_CO_GPIO_Port, ZOOM1_PI_CO_Pin);
    bool zoom3_status = Motor_CheckCOStatus("ZOOM3", ZOOM3_PI_CO_GPIO_Port, ZOOM3_PI_CO_Pin);
    bool iris_status = Motor_CheckCOStatus("IRIS", IRIS_PI_CO_GPIO_Port, IRIS_PI_CO_Pin);
    bool zoom2_status = Motor_CheckCOStatus("ZOOM2", ZOOM2_PI_CO_GPIO_Port, ZOOM2_PI_CO_Pin);
    bool focus_status = Motor_CheckCOStatus("FOCUS", FOCUS_PI_CO_GPIO_Port, FOCUS_PI_CO_Pin);
    
//    bool all_high = zoom1_status && zoom3_status && iris_status && zoom2_status && focus_status;
    
 //   LOG_Print(LOG_LEVEL_INFO, "\r\n=== CO�źż�������� ===\r\n");
 //   LOG_Print(LOG_LEVEL_INFO, "ZOOM1: %s\r\n", zoom1_status ? "��λ" : "δ��λ");
//    LOG_Print(LOG_LEVEL_INFO, "ZOOM3: %s\r\n", zoom3_status ? "��λ" : "δ��λ");
//    LOG_Print(LOG_LEVEL_INFO, "IRIS:  %s\r\n", iris_status ? "��λ" : "δ��λ");
//    LOG_Print(LOG_LEVEL_INFO, "ZOOM2: %s\r\n", zoom2_status ? "��λ" : "δ��λ");
//    LOG_Print(LOG_LEVEL_INFO, "FOCUS: %s\r\n", focus_status ? "��λ" : "δ��λ");
//    LOG_Print(LOG_LEVEL_INFO, "\r\n����CO�ź�״̬: %s\r\n", 
//              all_high ? "ȫ��Ϊ�ߵ�ƽ��ȫ����λ��" : "���ڵ͵�ƽ�źţ�����δ��λ��");
//    LOG_Print(LOG_LEVEL_INFO, "========================\r\n");
    
 //   return all_high;
}
