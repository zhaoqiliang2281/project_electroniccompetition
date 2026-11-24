/**
 ******************************************************************************
 * @file    motor.c
 * @brief   Emm_V5（张大头闭环步进）电机抽象层：读回速度/角度、位置/速度下发
 * @details
 *   本文件是“CAN 命令”与“上层业务”之间的中转站：
 *     - et_motor_speed / et_motor_angular  解析 0x35（读速度）、0x36（读位置）回帧；
 *     - MRcontrol                           按模式（位置/速度）下发带加速度的运动指令；
 *     - Motor_DE / chassis_stop             使能/失能、整车停车；
 *     - chassis_Motor_zeroing               单圈回零（软件置零 -> 回零 -> 触发）。
 *   Emm_V5 回帧格式：DLC=5 时 rxData[0]=0x35，data[1]=符号，data[2..3]=速度(uint16)；
 *   DLC=7 时 rxData[0]=0x36，data[2..5]=位置(uint32)，65536 计数对应 360 度。
 * @note
 *   1. 大量函数用 while(can.rxFrameFlag == false) 死等回帧，属于“同步式调用”，
 *      只能在主循环里用，不要放进中断；CAN 断线时会卡死在这里；
 *   2. et_motor_speed 里的等待被注释掉了，所以它读的可能是“上一帧残留数据”，
 *      做速度闭环时要把等待打开，否则数据会错位；
 *   3. 电机 ID 见 motor.h 的 can_msg_id_e：1~4 底盘、5 机械臂升降。
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
	
	
#include "motor.h"	

/*电机数据结构体定义*/
motor_measure_t motor_chassis[5];

/*电机基础初始数据定义*/
MRBASdata_type_def MRBASdata;

/*
motor data,  0:chassis motor1 ZDT;1:chassis motor2 ZDT;2:chassis motor3 ZDT;3:chassis motor4 ZDT;
4:X-axis motor for robot arm motor ZDT;5:Steering wheel motor ZDT;6:Y-axis motor of mechanical arm motor ZDT;
电机数据, 0:底盘电机1 ZDT电机,  1:底盘电机2 ZDT电机,2:底盘电机3 ZDT电机,3:底盘电机4 ZDT电机; 4:机械臂x轴电机 ZDT电机; 5:舵盘电机 ZDT电机; 6:机械臂y轴电机 ZDT电机*/

//motor data read
/**
  * @brief          Read all motor return speed data
  * @param[in]      none
  * @retval         none
  */
/**
  * @brief          读取全部电机回传速度数据
  * @param[in]      none
  * @retval         none
  */
	
 /**
  * @brief  读取指定电机的实时速度（RPM）
  * @param  numbe 电机 CAN ID
  * @retval 有符号转速；未收到合法回帧时返回 0
  * @note   回帧校验条件：rxData[0]==0x35 且 DLC==5；符号位在 rxData[1]。
  */
 float et_motor_speed(can_msg_id_e numbe)
{
	   float vel = 0.0f, Motor_Vel = 0.0f;/*获取电机速度和位置时，计算速度和角度的中间变量*/
	
     Emm_V5_Read_Sys_Params(numbe, S_VEL);	//读取电机实时速度
//     while(can.rxFrameFlag == false);       
//     can.rxFrameFlag = false;               //	等待返回命令
		 if(can.rxData[0] == 0x35 && can.CAN_RxMsg.DLC == 5)
		 {
			// 拼接成uint16_t类型数据
			vel = (uint16_t)(((uint16_t)can.rxData[2] << 8)	|((uint16_t)can.rxData[3] << 0));
			// 实时转速
			Motor_Vel = vel;
			// 符号
			if(can.rxData[1]) { Motor_Vel = -Motor_Vel; }
		 }
     return Motor_Vel;
 }

/**
  * @brief          Read all motor return Angle data
  * @param[in]      none
  * @retval         none
  */
/**
  * @brief         读取全部电机回传角度数据
  * @param[in]      none
  * @retval         none
  */
/**
 * @brief  读取指定电机的当前位置（角度制）
 * @param  numbe 电机 CAN ID
 * @retval 角度 = 原始计数 * 360 / 65536（负值表示反向）
 * @note   这里显式等待回帧（while 阻塞），是“读一次就必须拿到新值”的写法。
 */
float et_motor_angular(can_msg_id_e numbe)
 { 
	 
	      float pos = 0.0f, Motor_Cur_Pos = 0.0f;/*获取电机速度和位置时，计算速度和角度的中间变量*/
	 
				Emm_V5_Read_Sys_Params(numbe, S_CPOS); //读取电机实时位置
        while(can.rxFrameFlag == false);
        can.rxFrameFlag = false; 
				if(can.rxData[0] == 0x36 && can.CAN_RxMsg.DLC == 7)
			 {
				// 拼接成uint32_t类型
				pos = (uint32_t)(((uint32_t)can.rxData[2] << 24)|((uint32_t)can.rxData[3] << 16)|((uint32_t)can.rxData[4] << 8)|((uint32_t)can.rxData[5] << 0));
				// 转换成角度
				Motor_Cur_Pos = (float)pos * 360.0f / 65536.0f;
				// 符号
				if(can.rxData[1]) { Motor_Cur_Pos = -Motor_Cur_Pos; }
			  }
			 return Motor_Cur_Pos;
 } 
 
 /**
  * @brief          chassis_motorbasics struct data init
  * @param[out]     motorbasics: motorbasics struct data point
  * @param[in]      mod:Position and speed patterns
  * @param[in]      acc:accelerated speed
  * @param[in]      raF:Phase/absolute flag, false for relative motion, true for absolute motion
  * @param[in]      snF:Multiple host synchronization flag. If false is disabled, and true is enabled
  * @retval         none
  */
/**
  * @brief          底盘电机基础数据初始化
  * @param[out]     MRBASdata: 电机结构数据指针
  * @param[in]      mod:位置和速度模式
  * @param[in]      acc:加速度
  * @param[in]      raF:相位/绝对标志，false为相对运动，true为绝对值运动
  * @param[in]      snF:多机同步标志 ，false为不启用，true为启用
  * @retval         none:
  */
/**
 * @brief  底盘电机运动参数初始化：加速度 + 相对运动 + 不同步
 * @param  motordata 基础参数结构；acc 加速度（0~255，0 表示直接启动）
 * @note   acc 越大越“冲”，比赛实际用 220 左右；调小可减少打滑。
 */
void chassis_motor_init(MRBASdata_type_def*motordata,uint8_t acc)
{
	motordata->acc=acc;//225
	motordata->raF=0;
	motordata->snF=0;
} 


 /**
  * @brief          rests_motorbasics struct data init
  * @param[out]     motorbasics: motorbasics struct data point
  * @param[in]      mod:Position and speed patterns
  * @param[in]      acc:accelerated speed
  * @param[in]      raF:Phase/absolute flag, false for relative motion, true for absolute motion
  * @param[in]      snF:Multiple host synchronization flag. If false is disabled, and true is enabled
  * @retval         none
  */
/**
  * @brief          机械臂电机基础数据初始化
  * @param[out]     MRBASdata: 电机结构数据指针
  * @param[in]      mod:位置和速度模式
  * @param[in]      acc:加速度
  * @param[in]      raF:相位/绝对标志，false为相对运动，true为绝对值运动
  * @param[in]      snF:多机同步标志 ，false为不启用，true为启用
  * @retval         none:
  */
/**
 * @brief  机械臂电机参数初始化（加速度固定 250，响应更快）
 * @note   入口参数 acc 未被使用，写死 250 是现场试出来的值。
 */
void rests_motor_init(MRBASdata_type_def*motordata,uint8_t acc)
{
	motordata->acc=250;
	motordata->raF=0;
	motordata->snF=0;
} 
 

  /**
  * @brief          The motor sends data for rotation
  * @param[out]     MRSEND: motorbasics struct data point
  * @param[in]      can_msg_id_e:Motor canid data pointer structure
  * @param[in]      numbe:Motor canid
  * @param[in]      motorseepd:Motor speed /RPM
  * @param[in]      clk:The number of turns
  * @retval         none
  */
/**
  * @brief          电机发送数据转动
  * @param[out]     MRSEND:电机基础数据指针结构体 
  * @param[out]     can_msg_id_e:电机canid数据指针结构体 
  * @param[in]      numbe:电机canid
  * @param[in]      motorseepd:电机速/RPM
  * @param[in]      clk:转的圈数
  * @retval         none:
  */
/**
 * @brief  电机运动指令下发（位置模式 / 速度模式二选一）
 * @param  MRSEND    加速度、相对/绝对、同步标志等基础参数
 * @param  numbe     电机 CAN ID
 * @param  MRmode    Positional(位置) 或 velocity(速度)
 * @param  motorseepd 位置模式=目标脉冲数；速度模式=RPM（内部取绝对值，方向单独给）
 * @param  clk        位置模式的圈数（相对运动时表示“再走 N 圈”）
 * @note   方向由 Emm_V5 的 dir 参数表达（0=正、1=负），本函数把符号翻译成方向。
 */
void MRcontrol(MRBASdata_type_def*MRSEND,can_msg_id_e numbe,uint8_t MRmode,int16_t motorseepd,uint32_t clk)
{
  uint16_t absmotorseepd;
  if(MRmode==Positional)/*位置模式*/
	{
		if(motorseepd>0)/*正方向，0*/
		{
			motor_chassis[numbe].direction=1;
			Emm_V5_Pos_Control(numbe, 0, motorseepd, MRSEND->acc, clk, MRSEND->raF, MRSEND->snF);
			
		}
		else/* 反转 */
		{
			motor_chassis[numbe].direction=0;
			Emm_V5_Pos_Control(numbe, 1, motorseepd, MRSEND->acc, clk, MRSEND->raF, MRSEND->snF);
		    
		}
	}             
	 if(MRmode==velocity)
	 {
		 absmotorseepd=abs(motorseepd);
		if(motorseepd>0)/*正方向，0*/
		{
			motor_chassis[numbe].direction=1;
			Emm_V5_Vel_Control(numbe, 0, absmotorseepd, MRSEND->acc, MRSEND->snF);
		}
		else/* 反转 */
		{ 
			motor_chassis[numbe].direction=0;
			Emm_V5_Vel_Control(numbe, 1, absmotorseepd, MRSEND->acc, MRSEND->snF);
		}
	 }
}
 
/**
  * @brief          Motor enable and disable
  * @param[out]     can_msg_id_e：motorbasics struct data point
  * @retval         noe

  */
/**
  * @brief          电机使能失能
  * @param[out]     can_msg_id_e：电机canid数据结构体     
  * @retval         noe
  */
/**
 * @brief  电机使能 / 失能（相当于“通电锁轴”与“松开”）
 * @param  numbe  电机 CAN ID；Switch 1=使能、0=失能
 * @note   失能后电机不再保持力矩，云台会因重力下垂，比赛结束前才失能。
 */
void Motor_DE(uint8_t numbe,uint8_t  Switch) 
{
	if(Switch==1)//使能电机
	{
		Emm_V5_En_Control(numbe, 1, 0);
		while(can.rxFrameFlag == false);
		can.rxFrameFlag = false;
	}
	if(Switch==0)//失使能电机
	{
		Emm_V5_En_Control(numbe, 0, 0);
		 while(can.rxFrameFlag == false);
		 can.rxFrameFlag = false;
	}
} 
 
/**
  * @brief          emergency braking
  * @retval         none
  * @return         none
  */
/**
  * @brief          底盘停车
  * @retval         none
  * @return         none
  */
/**
 * @brief  底盘四电机全部失能（急停）
 * @note   是“断电松开”而不是“速度 0”，响应更快但会失去保持力。
 */
void chassis_stop(void)
{  
	Motor_DE(CAN_ZDTY42_M1_ID,0);
	Motor_DE(CAN_ZDTY42_M2_ID,0);
	Motor_DE(CAN_ZDTY42_M3_ID,0);
	Motor_DE(CAN_ZDTY42_M4_ID,0); 
}
 
/**
  * @brief          return the ALL motor data point
  * @param[out]     can_msg_id_e：motorbasics struct data point
  * @param[in]      i: motor number,range [0,7]
  * @retval         motor data point
  */
/**
  * @brief          单个电机数据清零设置当前位置为单圈回零的零点位置，不存储
  * @param[out]     can_msg_id_e：电机canid数据指针结构体 
  * @param[in]      
  * @retval         电机数据指针
  */
/**
 * @brief  单电机软件回零：设当前位置为单圈零点 -> 走 1 圈 -> 触发回零
 * @param  numbe 电机 CAN ID
 * @note   三步都等回帧，整个过程约几十毫秒；回零后位置计数清零。
 */
void chassis_Motor_zeroing(can_msg_id_e numbe)
{
	Emm_V5_Origin_Set_O(numbe, 0);
	while(can.rxFrameFlag == false);
	can.rxFrameFlag = false; 
	Emm_V5_Pos_Control(numbe, 0, 1000, 0, 1, 0, 0);	
	while(can.rxFrameFlag == false);
	can.rxFrameFlag = false; 
	Emm_V5_Origin_Trigger_Return(numbe, 0, 0);
	while(can.rxFrameFlag == false);
	can.rxFrameFlag = false; 
} 




