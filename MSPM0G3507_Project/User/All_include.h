/**
 ******************************************************************************
 * @file    All_include.h
 * @brief   全局公共头文件入口 / 定长类型别名（MSPM0G3507 循迹小车工程）
 * @details
 *   所有模块头文件集中到这一处，各 .c 只需 #include "All_include.h" 就能拿到完整
 *   声明，避免头文件互相包含造成循环依赖。
 *   包含顺序：TI DriverLib + SysConfig 生成宏 -> C 标准库 -> 定长类型 -> 用户模块。
 * @note
 *   1. 新增模块时建议把它的头文件追加到“用户头文件”区，保持单一入口；
 *   2. 本文件额外定义了 uint8 / vint32 等 STM32 风格的定长类型别名，
 *      MSPM0 工程里大量沿用这套写法（与 stdint.h 的 uint8_t 并存，勿混用）。
 ******************************************************************************
 */

#ifndef __ALL_INCLUDE_H__
#define __ALL_INCLUDE_H__

/******************系统头文件*******************/
#include "ti_msp_dl_config.h"
#include "All_include.h"
#include <string.h>
#include <stdbool.h>
#include <math.h> 
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>


// 数据类型声明
// 尽量使用 stdint.h 定义的类型名称 避免冲突 这里可以裁剪
typedef unsigned char       uint8;                                              // 无符号  8 bits
typedef unsigned short int  uint16;                                             // 无符号 16 bits
typedef unsigned       int  uint32;                                             // 无符号 32 bits
typedef unsigned long long  uint64;                                             // 无符号 64 bits

typedef signed char         int8;                                               // 有符号  8 bits
typedef signed short int    int16;                                              // 有符号 16 bits
typedef signed       int    int32;                                              // 有符号 32 bits
typedef signed long long    int64;                                              // 有符号 64 bits

typedef volatile uint8      vuint8;                                             // 易变性修饰 无符号  8 bits
typedef volatile uint16     vuint16;                                            // 易变性修饰 无符号 16 bits
typedef volatile uint32     vuint32;                                            // 易变性修饰 无符号 32 bits
typedef volatile uint64     vuint64;                                            // 易变性修饰 无符号 64 bits

typedef volatile int8       vint8;                                              // 易变性修饰 有符号  8 bits
typedef volatile int16      vint16;                                             // 易变性修饰 有符号 16 bits
typedef volatile int32      vint32;                                             // 易变性修饰 有符号 32 bits
typedef volatile int64      vint64;                                             // 易变性修饰 有符号 64 bits
/******************用户头文件*******************/
/* 用户模块：delay 放在最前面，其余模块都会用到微秒/毫秒延时 */
#include "delay.h"
#include "PWM.h"
#include "Servo.h"
#include "OLED.h"
#include "key.h"

#endif