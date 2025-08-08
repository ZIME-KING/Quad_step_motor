#ifndef __USER_FUNCTION_H
#define __USER_FUNCTION_H
#include "main.h"
#include "control.h"

void User_main(void);

// 电机复位处理函数声明
void ZOOM3_reset_handle(void);
void FOCUS_reset_handle(void);
void ZOOM2_reset_handle(void);
void ZOOM1_reset_handle(void);
void IRIS_reset_handle(void);

#endif
