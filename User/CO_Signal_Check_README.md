# CO信号检查功能说明

## 概述

本文档介绍了步进电机CO信号（到位信号）检查功能的实现和使用方法。CO信号用于检测电机是否已到达指定位置，低电平表示到位，高电平表示未到位。

## 功能特性

- ✅ 支持检查单个电机CO信号状态
- ✅ 支持检查所有电机CO信号状态
- ✅ 详细的状态日志输出
- ✅ 中文状态显示
- ✅ 返回布尔值便于程序判断
- ✅ 集成到主循环中自动监控

## 支持的电机

| 电机名称 | GPIO端口 | GPIO引脚 | 功能描述 |
|---------|----------|----------|----------|
| ZOOM1   | ZOOM1_PI_CO_GPIO_Port | ZOOM1_PI_CO_Pin | 变焦电机1 |
| ZOOM3   | ZOOM3_PI_CO_GPIO_Port | ZOOM3_PI_CO_Pin | 变焦电机3 |
| IRIS    | IRIS_PI_CO_GPIO_Port  | IRIS_PI_CO_Pin  | 光圈电机 |
| ZOOM2   | ZOOM2_PI_CO_GPIO_Port | ZOOM2_PI_CO_Pin | 变焦电机2 |
| FOCUS   | FOCUS_PI_CO_GPIO_Port | FOCUS_PI_CO_Pin | 对焦电机 |

## API函数

### 1. Motor_CheckCOStatus

```c
bool Motor_CheckCOStatus(const char* motor_name, GPIO_TypeDef* gpio_port, uint16_t gpio_pin);
```

**功能**: 检查单个电机的CO信号状态

**参数**:
- `motor_name`: 电机名称（用于日志显示）
- `gpio_port`: GPIO端口
- `gpio_pin`: GPIO引脚

**返回值**:
- `true`: CO信号为低电平（电机已到位）
- `false`: CO信号为高电平（电机未到位）

**示例**:
```c
bool zoom1_ok = Motor_CheckCOStatus("ZOOM1", ZOOM1_PI_CO_GPIO_Port, ZOOM1_PI_CO_Pin);
if (zoom1_ok) {
    LOG_Print(LOG_LEVEL_INFO, "ZOOM1电机已到位\r\n");
}
```

### 2. Motors_CheckAllCOStatus

```c
bool Motors_CheckAllCOStatus(void);
```

**功能**: 检查所有电机的CO信号状态

**参数**: 无

**返回值**:
- `true`: 所有电机的CO信号都为低电平（全部到位）
- `false`: 至少有一个电机的CO信号为高电平（部分未到位）

**示例**:
```c
bool all_motors_ok = Motors_CheckAllCOStatus();
if (all_motors_ok) {
    LOG_Print(LOG_LEVEL_INFO, "所有电机都已到位！\r\n");
} else {
    LOG_Print(LOG_LEVEL_WARNING, "部分电机未到位！\r\n");
}
```

## 使用示例

### 基本使用

```c
#include "main.h"
#include "control.h"

void check_motor_positions(void)
{
    // 检查所有电机状态
    bool all_ok = Motors_CheckAllCOStatus();
    
    if (all_ok) {
        // 所有电机都已到位，可以执行下一步操作
        LOG_Print(LOG_LEVEL_INFO, "所有电机就绪，可以开始操作\r\n");
    } else {
        // 部分电机未到位，需要等待或重新复位
        LOG_Print(LOG_LEVEL_WARNING, "等待电机到位...\r\n");
    }
}
```

### 在主循环中使用

```c
void User_main(void)
{
    // 初始化代码...
    
    for (;;)
    {
        // 其他任务...
        
        if (CompareTime(&Task_1000))
        {
            GetTime(&Task_1000);
            
            // 每1秒检查一次CO信号状态
            Motors_CheckAllCOStatus();
        }
    }
}
```

### 等待所有电机到位

```c
void wait_for_all_motors_ready(void)
{
    LOG_Print(LOG_LEVEL_INFO, "等待所有电机到位...\r\n");
    
    while (!Motors_CheckAllCOStatus()) {
        HAL_Delay(100); // 等待100ms后重新检查
    }
    
    LOG_Print(LOG_LEVEL_INFO, "所有电机已到位！\r\n");
}
```

## 日志输出示例

当调用`Motors_CheckAllCOStatus()`时，会输出类似以下的日志信息：

```
=== 检查所有电机CO信号状态 ===
ZOOM1 CO信号状态: 低电平（到位）
ZOOM3 CO信号状态: 高电平（未到位）
IRIS CO信号状态: 低电平（到位）
ZOOM2 CO信号状态: 低电平（到位）
FOCUS CO信号状态: 低电平（到位）

=== CO信号检查结果汇总 ===
ZOOM1: 到位
ZOOM3: 未到位
IRIS:  到位
ZOOM2: 到位
FOCUS: 到位

所有CO信号状态: 存在高电平信号（部分未到位）
========================
```

## 集成说明

### 1. 文件结构

- `control.c`: 包含CO信号检查函数的实现
- `control.h`: 包含函数声明
- `CO_Signal_Check_Example.c`: 使用示例代码
- `user_function.c`: 在主循环中集成CO信号检查

### 2. 依赖项

- HAL库（用于GPIO读取）
- user_log模块（用于日志输出）
- main.h（包含GPIO定义）

### 3. 编译要求

确保在项目中包含以下文件：
- `control.c`
- `CO_Signal_Check_Example.c`（可选，仅用于示例）

## 故障排除

### 1. 编译错误

**问题**: 找不到GPIO定义
**解决**: 确保包含了`main.h`文件，并且GPIO引脚在CubeMX中正确配置

**问题**: 找不到LOG_Print函数
**解决**: 确保包含了`user_log.h`文件

### 2. 运行时问题

**问题**: CO信号状态不正确
**解决**: 
1. 检查硬件连接
2. 确认GPIO配置正确
3. 使用万用表测量实际电压

**问题**: 日志输出乱码
**解决**: 确保串口波特率配置正确

## 性能特性

- **执行时间**: 每次检查约1-2ms
- **内存占用**: 最小化，无动态内存分配
- **CPU占用**: 极低，适合在主循环中频繁调用

## 扩展功能

### 1. 添加新电机

要添加新的电机CO信号检查，需要：
1. 在`Motors_CheckAllCOStatus()`函数中添加新电机的检查
2. 更新GPIO定义
3. 更新文档

### 2. 自定义检查间隔

可以通过修改主循环中的定时器来调整检查频率：

```c
// 每500ms检查一次（更频繁）
if (CompareTime(&Task_500))
{
    GetTime(&Task_500);
    Motors_CheckAllCOStatus();
}
```

### 3. 添加回调函数

可以扩展功能，在状态变化时触发回调：

```c
typedef void (*co_status_callback_t)(bool all_motors_ready);
void Motors_SetCOStatusCallback(co_status_callback_t callback);
```

## 版本历史

- **v1.0** (2024): 初始版本，支持5个电机的CO信号检查

## 许可证

本代码遵循项目的整体许可证协议。