/**
 ******************************************************************************
 * @file    chassis_task.c
 * @brief   底盘对象与三套 PID 的初始化（RM 风格任务层，预留）
 * @details
 *   结构体 chassis_move_t 把“模式 + 四个电机数据 + 三个 PID”打包在一起：
 *     motor_angle_pid       车头朝向锁定（输入：HWT101 的 Yaw）
 *     motor_distance_pid    走固定距离
 *     chassis_calibration_pid  视觉校准
 * @note
 *   本工程最终方案是“云台追踪 + 机械臂”，底盘闭环没有接入 main.c；
 *   chassis_init() 目前没有任何调用者。保留此文件是为了把麦轮移动平台
 *   （配合 chassis_behaviour.c 的 Location_algorithm）作为可复用模板。
 ******************************************************************************
 */

/**
  ****************************(C) COPYRIGHT 2025 PAN****************************
  * @file       chassis.c/h
  * @brief      chassis control task,
  *             底盘控制任务初始化
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
	
	#include "chassis_task.h"
	
	/*底盘运动数据结构体嵌套初始化*/
	
	chassis_move_t chassis_move;


	can_msg_id_e can_msg_id_qe;
/*底盘初始化*/
/**
 * @brief  底盘初始化：三套 PID 参数下载 + 上电置“无力”模式 + 四轮失能
 * @param  chassis_move_init 全局底盘对象
 * @note   上电先失能是安全设计：避免上电瞬间轮子乱转。
 */
void chassis_init(chassis_move_t *chassis_move_init)
{
    uint8_t i;
	  //底盘方向角度pid初始化定义数据变量
	  const  float chassis_angle_pid[3] = {CHASSIS_ANGLE_PID_KP, CHASSIS_ANGLE_PID_KI, CHASSIS_ANGLE_PID_KD};
		//底盘距离环pid初始化定义数据变量
		const  float chassis_distance_pid[3]={CHASSIS_DISTANCE_PID_KP,CHASSIS_DISTANCE_PID_KI,CHASSIS_DISTANCE_PID_KD};
		//底盘校准距离环pid初始化定义数据变量
		const  float chassis_calibration_pid[3]={CHASSIS_CALIBRATION_PID_KP,CHASSIS_CALIBRATION_PID_KI,CHASSIS_CALIBRATION_PID_KD};
	
		
		//三项pid初始化
		PID_init(&chassis_move_init->motor_angle_pid,PID_POSITION, chassis_angle_pid, 2.5, 160);
		
		PID_init(&chassis_move_init->motor_distance_pid,PID_POSITION, chassis_distance_pid, 5000, 1600);
		
		PID_init(&chassis_move_init->chassis_calibration_pid,PID_POSITION, chassis_calibration_pid, 10, 160);
		
	  //底盘开机初始化状态为模式1
    chassis_move_init->chassis_mode=CHASSIS_POWER_FAILURE;//根据行为模式选择一个底盘控制模式//上电无力
    //检查电机是否在线
	  for(i=0;i<4;i++)//检查电机是否在线
		{
		    Motor_DE(i ,0);
		}
}







