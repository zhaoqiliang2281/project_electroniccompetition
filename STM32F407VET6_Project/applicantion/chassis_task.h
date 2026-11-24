/**
 ******************************************************************************
 * @file    chassis_task.h
 * @brief   底盘模式枚举、底盘对象结构与 PID 参数宏（预留模块）
 * @note    PID 参数（角度环 -0.15 / 距离环 4.0,0.6）是麦轮平台的实测值，
 *          负号表示“Yaw 增大需要反向修正”，换陀螺仪安装方向要同步改符号。
 ******************************************************************************
 */

/**
  ****************************(C) COPYRIGHT 2019 DJI****************************
  * @file       chassis.c/h
  * @brief      chassis control task,
  *             底盘控制任务初始化
  * @note       
  * @history
  *  Version    Date            Author          Modification
  *  V1.0.0     Dec-26-2018     RM              1. done
  *  V1.1.0     Nov-11-2019     RM              1. add chassis power control
  *
  @verbatim
  ==============================================================================

  ==============================================================================
  @endverbatim
  ****************************(C) COPYRIGHT 2019 DJI****************************
  */
	
#ifndef CHASSIS_TASK_H
#define CHASSIS_TASK_H

#include "main.h"
#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "can.h"
#include "Emm_V5.h"
#include "motor.h"
#include "pid.h"

// 角度环pid宏定义//底盘方向角度pid
#define CHASSIS_ANGLE_PID_KP -0.15f
#define CHASSIS_ANGLE_PID_KI -0.0f
#define CHASSIS_ANGLE_PID_KD -0.0f

// 距离环pid宏定义
#define CHASSIS_DISTANCE_PID_KP 4.0f
#define CHASSIS_DISTANCE_PID_KI 0.0f
#define CHASSIS_DISTANCE_PID_KD 0.6f

//底盘校准距离环pid宏定义
#define CHASSIS_CALIBRATION_PID_KP 4.0f
#define CHASSIS_CALIBRATION_PID_KI 0.0f
#define CHASSIS_CALIBRATION_PID_KD 0.6f

/*上电模式*/
/* 三种底盘状态：无力(上电/失能)、车头锁定(定角)、比赛模式(直接给速度) */
typedef enum
{  
	
	CHASSIS_POWER_FAILURE,       //底盘电无力
    CHASSIS_VECTOR_Angle_YAW,    // 底盘有底盘角度，方向控制车头方向定死(停车模式)
	RACING_ONLY,                  //比赛专用
} chassis_mode_e;
//数据变量结构体初始化
/* 底盘对象：模式 + 四轮回传 + 三个闭环 PID + 电机 ID */
typedef struct
{
	chassis_mode_e chassis_mode;
	
    motor_measure_t chassis_motordata[4]; //chassis motor data.底盘电机数据/*电机回传数据结构体声明*/
	
	pid_type_def motor_angle_pid;         //Chassis speed pid.底盘方向角度pid/*底盘头朝方向pid结构体声明*/
	
	pid_type_def motor_distance_pid;      //Chassis speed pid.底盘行走距离pid/*电机行走*/
	
    pid_type_def chassis_calibration_pid; //Chassis speed pid.底盘视觉校准pid/*电机底盘视觉*/

	can_msg_id_e  chassis_id_e;           //电机id枚举声明
	
}chassis_move_t;


extern chassis_move_t chassis_move;//全局声明结构体给chassis_behaviour初始化用

void chassis_init(chassis_move_t *chassis_move_init);//定义声明函数给chassis_behaviour用


#endif
