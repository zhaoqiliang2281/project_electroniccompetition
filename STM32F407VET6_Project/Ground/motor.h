/**
 ******************************************************************************
 * @file    motor.h
 * @brief   电机枚举、回传数据结构与电机抽象层接口声明
 * @note    can_msg_id_e 同时是 CAN 扩展帧里的“目标 ID”，与 Emm_V5 拨码一致。
 ******************************************************************************
 */

/**
  ****************************(C) COPYRIGHT 2019 PAN****************************
  * @file       motor.c/h
  * @brief      Here is the send seal function, receiving motor data, 
	*             and the send function sends motor data to control the motor.
  *             这里是发送封函数，接收电机数据,发送函数发送电机数据控制电机//中转站.
  * @note       
  * @history
  *  Version    Date            Author          Modification
  *  V1.0.0     Feb-17-2025     engineering              1. done
  *  V1.1.0     Nov-11-2025     engineering              1. support hal lib
  *
  @verbatim
  ==============================================================================

  ==============================================================================
  @endverbatim
  ****************************(C) COPYRIGHT 2025 PAN****************************
  */
#ifndef MOTOR_H
#define MOTOR_H

#include "main.h"
#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "can.h"
#include "Emm_V5.h"

/* CAN send and receive ID  电机id */
typedef enum
{
    /* 电机 ID 分配：1~4 底盘（麦轮）、5 机械臂升降；
     * 注意云台在 Dloud_deck.c 里直接用了 1/2 号，两套 ID 语义不要混用 */
    CAN_ZDTY42_M1_ID = 1,
    CAN_ZDTY42_M2_ID = 2,
    CAN_ZDTY42_M3_ID = 3,
    CAN_ZDTY42_M4_ID = 4,
    CAN_UP_MOTOR_ID  =5, //机械臂上升电机
	
/* 单个电机的回传数据：角度（脉冲计数换算）、RPM、方向 */
} can_msg_id_e;	


/* motor data 电机数据*/	
typedef struct
{
    uint16_t angular;//电机角度
    int16_t RPM;//电机转速(每分钟/转)
	  uint8_t direction; //电机转动方向
} motor_measure_t;	


/* Basic motor data 电机设置数据*/
typedef struct
{ 
    uint8_t acc;  //加速度      ，范围0 - 255，注意：0是直接启动
	  bool raF;     //相位/绝对标志，false为相对运动，true为绝对值运动
	  bool snF;     //多机同步标志 ，false为不启用，true为启用
}MRBASdata_type_def;	


/*Motor mode*/
enum MOTOR_MODE
{
    Positional = 0,
    velocity
};	
	
extern MRBASdata_type_def MRBASdata;

/*函数定义*/	
 /* 读实时速度（RPM）/ 读当前位置（度）：均为同步调用，会阻塞等回帧 */
 float et_motor_speed(can_msg_id_e numbe);
	
/* 读当前位置（单位：度，65536 计数 = 360） */
float et_motor_angular(can_msg_id_e numbe);
	
/* 底盘 / 机械臂运动参数初始化（加速度、相对/绝对、同步标志） */
void chassis_motor_init(MRBASdata_type_def*motordata,uint8_t acc);
	
/* 机械臂运动参数初始化（加速度固定 250） */
void rests_motor_init(MRBASdata_type_def*motordata,uint8_t acc);
	
/* 位置/速度模式运动指令下发 */
void MRcontrol(MRBASdata_type_def*MRSEND,can_msg_id_e numbe,uint8_t MRmode,int16_t motorseepd,uint32_t clk);

/* 电机使能(1)/失能(0) */
void Motor_DE(uint8_t numbe,uint8_t  Switch);

/* 底盘四轮失能急停 */
void chassis_stop(void);

/* 单电机软件回零 */
void chassis_Motor_zeroing(can_msg_id_e numbe);

#endif
