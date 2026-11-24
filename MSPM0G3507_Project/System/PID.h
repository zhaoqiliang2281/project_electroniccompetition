/**
 ******************************************************************************
 * @file    PID.h
 * @brief   通用 PID 结构、两种算法模式与各控制环参数宏
 * @details
 *   算法部分移植自大疆 RoboMaster 开源 PID（位置式 / 增量式两套公式），
 *   本工程实际只用“位置式 + 循迹偏差”这一套，其余（舵机/速度/位置）为预留。
 *   PID 参数统一放在本文件的宏里，调参只改这里，不要在 .c 里写魔数。
 * @note
 *   1. Servo_pid_* / Speed_pid_* / Llocation_pid_* 三组宏属于历史预留，
 *      其中速度环与位置环的 .c 代码仍保留但未被主程序调用；
 *   2. 循迹环用的是 grayscale_sensor.c 里的 Xunji 参数，不在这三组宏里。
 ******************************************************************************
 */

/*
 * PID.h
 *
 *  Created on: 2025年6月15日
 *      Author: 吉祥止止
 */

#include "All_include.h"

#ifndef _PID_H_
#define _PID_H_

/*舵机PID*/
#define Servo_pid_KP    0.1   // 比例系数
#define Servo_pid_KI    0.0    // 积分系数
#define Servo_pid_KD    0.0    // 微分系数

/*速度PID*/
#define Speed_pid_KP    0.10    // 比例系数
#define Speed_pid_KI    0.00    // 积分系数
#define Speed_pid_KD    0.00       // 微分系数

/*位置PID*/
#define Llocation_pid_KP    0.0   // 比例系数
#define Llocation_pid_KI    0.0    // 积分系数
#define Llocation_pid_KD    0.0    // 微分系数


typedef float fp32;

enum PID_MODE
{
    PID_POSITION = 0,
    PID_DELTA
};

/* PID 结构体：三个误差 + 三个微分量构成“历史窗口”，用于增量式与微分项计算 */
typedef struct
{
    uint8_t mode;
    //PID 三参数
    fp32 Kp;
    fp32 Ki;
    fp32 Kd;

    fp32 max_out;  //最大输出
    fp32 max_iout; //最大积分输出

    fp32 set;
    fp32 fdb;

    fp32 out;
    fp32 Pout;
    fp32 Iout;
    fp32 Dout;
    fp32 Dbuf[3];  //微分项 0最新 1上一次 2上上次
    fp32 error[3]; //误差项 0最新 1上一次 2上上次

} pid_type_def;

/* 初始化：写参数、清历史、限幅；PID[3] 顺序为 Kp, Ki, Kd */
extern void PID_init(pid_type_def *pid, uint8_t mode, const fp32 PID[3], fp32 max_out, fp32 max_iout);
/* 单次计算：ref=反馈值，set=目标值，返回限幅后的输出 */
extern fp32 PID_calc(pid_type_def *pid, fp32 ref, fp32 set);
/* 清空历史与输出（切模式/重新起步前调用，避免积分残留） */
extern void PID_clear(pid_type_def *pid);

extern float angle_Error;
extern int16 resultServo;  //舵机PID计算结果

extern int16 resultSpeed;   //速度PID计算结果
extern int16 resultServo;  //舵机PID计算结果

void All_PID_Init(void);
void All_PID_scan(void);

void PID_DataShow(void);

void PID_adjust_parameter(void);
void All_PID_scan_adjust_parameter(void);

void Speed_PID_Init(int8 num);
void Speed_Location_Init(int16 num);

#endif /* CODE_PID_H_ */
