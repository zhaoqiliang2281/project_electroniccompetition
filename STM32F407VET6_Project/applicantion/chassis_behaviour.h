/**
 ******************************************************************************
 * @file    chassis_behaviour.h
 * @brief   底盘行为层接口（模式选择、逆解、车头锁定）
 * @note    Car_H / Car_W 为车体长宽（米），当前只被注释掉的旧代码引用。
 ******************************************************************************
 */

/**
  ****************************(C) COPYRIGHT 2025 PAN****************************
  * @file       chassis_behaviour.c/h
  * @brief      Chassis motion control and behavior,
  *             底盘运动控制和行为
  * @note       
  * @history
  *  Version    Date            Author          Modification
  *  V1.0.0     Dec-26-2018     engineering              1. done
  *  V1.1.0     Nov-11-2019     engineering              1. add chassis power control
  *
  @verbatim
  ==============================================================================

  ==============================================================================
  @endverbatim
  ****************************(C) COPYRIGHT 2025 PAN****************************
  */
	
#ifndef CHASSIS_BEHAVIOUR_H
#define CHASSIS_BEHAVIOUR_H

#include "main.h"
#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "can.h"
#include "Emm_V5.h"
#include "motor.h"
#include "pid.h"
#include "HWT101.h"
#include "chassis_task.h"
#include "delay.h"

#define Car_H 0.3       //车体长度
#define Car_W 0.2       //车体宽度


void chassis_behaviour_mode_set(chassis_move_t *chassis_move_mode);
void Location_algorithm(float vx_set,float vy_set,float vz);
float chassisyaw_lock(float yawsetq);
void Chassis_position_move(void);


#endif
