/**
 ******************************************************************************
 * @file    chassis_behaviour.c
 * @brief   麦轮运动学逆解 + 车头锁定（预留模块，未接入 main.c）
 * @details
 *   Location_algorithm(vx, vy, wz) 把“车体坐标系速度”解算成四个轮的线速度：
 *     每个轮速 = ±(vx - vy - wz*L) 的四种符号组合，L = 0.15 + 0.10 = 0.25 m（旋转力臂）；
 *   再按 60*v/(π*D) 换算成 RPM（D 按 0.16 m 取，换轮子必须改这个系数）。
 *   最后逐轮下发速度指令，每发一轮都等一次回帧（阻塞）。
 * @note
 *   1. 代码里 vx = -vy_set、vy = vx_set 是一次坐标轴交换，等价于把
 *      “前后/左右”对调到本车实际的电机排布，改接线后要重新确认符号；
 *   2. 0.08*3.14159*2 看起来是“半径 0.08 m 的轮子”，与 (0.15+0.10) 的单位
 *      保持一致（米），改车体尺寸记得同时改这两个数。
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
#include "chassis_behaviour.h"
	
/*运动学解算变量*/
float TargetA;
float TargetB;
float TargetC;
float TargetD;

int16_t Target1;
int16_t Target2;
int16_t Target3;
int16_t Target4;

float vx;
float vy;
float vz;
/***********/	
float speedZ;//陀螺仪车辆方向pid计算值	
	
/*底盘电机初始化基础数据结构体定义*/

/*(循环执行)4个模式选择执行*///(模式初始化)
/**
  * @brief          通过逻辑判断，赋值"chassis_behaviour_mode"成哪种模式//设置
  * @param[in]      chassis_move_mode: 底盘数据
  * @retval         none
  */
/**
 * @brief  设置底盘行为模式：强制进入比赛模式，四轮使能，加速度 220
 * @note   正常 RM 工程会在这里读遥控器/上位机来切换模式，本版简化成写死。
 */
void chassis_behaviour_mode_set(chassis_move_t *chassis_move_mode)
{
	      uint8_t i;
        chassis_move_mode->chassis_mode =RACING_ONLY; //比赛模式
				for(i=0;i<4;i++)
				{
					 Motor_DE(i ,1);                          //电机使能
				}
			  chassis_motor_init(&MRBASdata,220);//设置加速度为220
			  //模式
}


/**
  * @brief          底盘运动学解算
  * @param[out]     vx_set, 通常控制纵向移动.
  * @param[out]     vy_set, 通常控制横向移动.
  * @param[out]     wz_set, 通常控制旋转运动.
  * @param[in]      chassis_move_rc_to_vector,  包括底盘所有信息.
  * @retval         none
  */
/**
 * @brief  麦轮逆运动学：车体速度 -> 四轮 RPM 并下发
 * @param  vx_set 纵向速度（m/s，正=前进）；vy_set 横向速度；vz 旋转角速度（rad/s）
 * @note   Target1~4 为整数 RPM，函数内部直接发 CAN，调用一次就动一次。
 */
void Location_algorithm(float vx_set,float vy_set,float vz)//输入车辆速度坐标速度
{
	vx=-vy_set;
	vy=vx_set;
	TargetA= -(vx-vy-vz*(0.15+0.10));
	TargetB=  (vx+vy+vz*(0.15+0.10));
	TargetC= -(vx+vy-vz*(0.15+0.10));
	TargetD=  (vx-vy+vz*(0.15+0.10));
	Target1=(int)(60*TargetA/(0.08*3.14159*2));
	Target2=(int)(60*TargetB/(0.08*3.14159*2));	
	Target3=(int)(60*TargetC/(0.08*3.14159*2));	
	Target4=(int)(60*TargetD/(0.08*3.14159*2));	
	
	MRcontrol(&MRBASdata,CAN_ZDTY42_M1_ID ,velocity,Target1, 0);
    while(can.rxFrameFlag == false){}; can.rxFrameFlag = false;
		
	MRcontrol(&MRBASdata,CAN_ZDTY42_M2_ID ,velocity,Target2, 0);	
    while(can.rxFrameFlag == false){}; can.rxFrameFlag = false;
		
	MRcontrol(&MRBASdata,CAN_ZDTY42_M3_ID ,velocity,Target3, 0);	
    while(can.rxFrameFlag == false){}; can.rxFrameFlag = false;
		
	MRcontrol(&MRBASdata,CAN_ZDTY42_M4_ID ,velocity,Target4, 0);
    while(can.rxFrameFlag == false){}; can.rxFrameFlag = false;
}

/**
  * @brief          底盘方向pid
  * @param[out]     yawsetq, 底盘方向设定值
  * @param[in]      chassis_move_rc_to_vector,  包括底盘所有信息.
  * @retval         none
  */
//车头方锁定
/**
 * @brief  车头朝向锁定：用 HWT101 的 Yaw 做位置式 PID，输出转向速度
 * @param  yawsetq 目标航向角（度）
 * @retval 用于叠加到 wz 的修正速度
 * @note   反馈 Yaw 恒为 0（陀螺仪接收未开启），本函数当前无实际效果。
 */
float chassisyaw_lock(float yawsetq)//loop数据更新接收
{
	speedZ=PID_calc(&chassis_move.motor_angle_pid,Yaw, yawsetq);
	return speedZ;
}

/**
  * @brief          底盘位置运动
  * @param[out]     yawsetq, 底盘方向设定值
  * @retval         none
  */
/**
 * @brief  底盘位置微调（当前只驱动 1 号轮，作为“抵近”动作）
 * @note   调试遗留函数，量程与方向需按场地重新标定。
 */
void Chassis_position_move(void)
{
     MRcontrol(&MRBASdata,CAN_ZDTY42_M1_ID ,velocity,Target4, 0);	
}
