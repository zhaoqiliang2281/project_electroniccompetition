/**
 ******************************************************************************
 * @file    PID.c
 * @brief   PID 算法实现 + 各控制环的实例与调试输出
 * @details
 *   【两种模式】
 *     PID_POSITION：u = Kp*e + Ki*Σe + Kd*(e - e_last)，输出绝对值，直接给执行器；
 *     PID_DELTA   ：Δu = Kp*(e-e_last) + Ki*e + Kd*(e-2e_last+e_lastlast)，
 *                   每次只算“增量”再累加到 out，适合步进/积分型执行器。
 *   两者都做了输出限幅（max_out）与积分限幅（max_iout），防止积分饱和。
 *   【实例】
 *     PID_Speed ：编码器计数 -> 目标 45（单位是“每周期脉冲数”，不是 RPM），
 *                 由 All_PID_scan() 调用，本车未启用速度闭环；
 *     PID_Location / PID_Servo：预留，未初始化也未调用；
 *     PID_xunji ：循迹环，定义在 grayscale_sensor.c 里，是唯一真正在跑的 PID。
 * @note
 *   1. 增量式 PID 必须保证 out 不被外部清零，否则会丢掉累积量；
 *   2. 文件末尾“调参代码 勿删”区是当年用 IPS 屏/串口看波形用的，
 *      保留用于复现调参过程，不影响编译。
 ******************************************************************************
 */

/*
 * PID.c
 *
 *  Created on: 2025年6月15日
 *      Author: 吉祥止止
 */

#include "All_include.h"
#include "Serial.h"
#include "PID.h"
#include "OLED.h"

extern uint32_t encoderA_cnt;
/*********************************PID公式运算代码部分******************************************/

/* 对称限幅宏：把 input 夹在 [-max, max] 之间（注意传参必须是左值） */
#define LimitMax(input, max)   \
{                              \
        if (input > max)       \
        {                      \
            input = max;       \
        }                      \
        else if (input < -max) \
        {                      \
            input = -max;      \
        }                      \
}

/**
  * @brief          pid struct data init
  * @param[out]     pid: PID结构数据指针
  * @param[in]      mode: PID_POSITION:普通PID
  *                 PID_DELTA: 差分PID
  * @param[in]      PID: 0: kp, 1: ki, 2:kd
  * @param[in]      max_out: pid最大输出
  * @param[in]      max_iout: pid最大积分输出
  * @retval         none
  */
/************************************************
 *  函数名称: pid计算：PID_init(void)
 *  功能说明：
 *  参数说明：
 *  参   数：pid: PID结构数据指针; mode: PID_POSITION:普通PID,PID_DELTA: 差分PID;  PID: 0: kp, 1: ki, 2:kd;max_out: pid最大输出;max_iout: pid最大积分输出
 *  函数返回：无
 *  修改时间：2025年6月16日
 *  备   注：
 ************************************************/
void PID_init(pid_type_def *pid, uint8_t mode, const fp32 PID[3], fp32 max_out, fp32 max_iout)
{
    if (pid == NULL || PID == NULL)
    {
        return;
    }
    pid->mode = mode;
    pid->Kp = PID[0];
    pid->Ki = PID[1];
    pid->Kd = PID[2];
    pid->max_out = max_out;
    pid->max_iout = max_iout;
    pid->Dbuf[0] = pid->Dbuf[1] = pid->Dbuf[2] = 0.0f;
    pid->error[0] = pid->error[1] = pid->error[2] = pid->Pout = pid->Iout = pid->Dout = pid->out = 0.0f;
}


/************************************************
 *  函数名称: pid计算：fp32 PID_calc(pid_type_def *pid, fp32 ref, fp32 set)
 *  功能说明：
 *  参数说明：
 *  参   数：pid: PID结构数据指针; ref: 反馈数据; set: 设定值
 *  函数返回：pid输出
 *  修改时间：2025年6月16日
 *  备   注：
 ************************************************/
fp32 PID_calc(pid_type_def *pid, fp32 ref, fp32 set)
{
    if (pid == NULL)
    {
        return 0.0f;
    }

    pid->error[2] = pid->error[1];
    pid->error[1] = pid->error[0];
    pid->set = set;
    pid->fdb = ref;
    pid->error[0] = set - ref;
    /* 位置式：P 用当前误差，I 累加误差（再限幅），D 用一阶差分。
     * 目标 vs 反馈的符号约定：error = set - ref，输出正表示“需要执行器加大”。 */
    if (pid->mode == PID_POSITION)
    {
        pid->Pout = pid->Kp * pid->error[0];
        pid->Iout += pid->Ki * pid->error[0];
        pid->Dbuf[2] = pid->Dbuf[1];
        pid->Dbuf[1] = pid->Dbuf[0];
        pid->Dbuf[0] = (pid->error[0] - pid->error[1]);
        pid->Dout = pid->Kd * pid->Dbuf[0];
        LimitMax(pid->Iout, pid->max_iout);
        pid->out = pid->Pout + pid->Iout + pid->Dout;
        LimitMax(pid->out, pid->max_out);
    }
    /* 增量式：P/I/D 全部按“变化量”计算，最后累加到 out。
     * 该模式对积分饱和更稳健，但要求调用频率固定，否则等效参数会漂移。 */
    else if (pid->mode == PID_DELTA)
    {
        pid->Pout = pid->Kp * (pid->error[0] - pid->error[1]);
        pid->Iout = pid->Ki * pid->error[0];
        pid->Dbuf[2] = pid->Dbuf[1];
        pid->Dbuf[1] = pid->Dbuf[0];
        pid->Dbuf[0] = (pid->error[0] - 2.0f * pid->error[1] + pid->error[2]);
        pid->Dout = pid->Kd * pid->Dbuf[0];
        pid->out += pid->Pout + pid->Iout + pid->Dout;
        LimitMax(pid->out, pid->max_out);
    }
    return pid->out;
}


/************************************************
 *  函数名称: pid 输出清除：PID_clear(void)
 *  功能说明：
 *  参数说明：
 *  参   数：PID结构数据指针
 *  函数返回：无
 *  修改时间：2025年6月16日
 *  备   注：
 ************************************************/
void PID_clear(pid_type_def *pid)
{
    if (pid == NULL)
    {
        return;
    }

    pid->error[0] = pid->error[1] = pid->error[2] = 0.0f;
    pid->Dbuf[0] = pid->Dbuf[1] = pid->Dbuf[2] = 0.0f;
    pid->out = pid->Pout = pid->Iout = pid->Dout = 0.0f;
    pid->fdb = pid->set = 0.0f;
}

/*********************************PID运算代码部分******************************************/

/*------------------------- PID 实例与调试量 -------------------------
 * PID_Servo     : 舵机环实例（未初始化，未使用）
 * PID_Speed     : 速度环实例，目标值 45 写在 All_PID_scan 里
 * PID_Location  : 位置环实例（未初始化，未使用）
 * encoderA_cnt  : 速度环的反馈量，来自 GROUP1 编码器中断
 * ----------------------------------------------------------------*/
pid_type_def PID_Servo;

float angle_Error;
int16 resultServo;  //舵机PID计算结果

//速度PID
pid_type_def PID_Speed;

int16 TargetSpeed=0;//目标速度
int16 practicalSpeed;//实际速度
int16 resultSpeed;//PID计算结果
int16 resultSpeed1;//PID计算结果

//位置PID
pid_type_def PID_Location;

int16 Target_Location=0;//目标距离
int16 practica_Location;//实际距离
int16 result_Location;//PID计算结果

/************************************************
 *  函数名称: 全部PID初始化：All_PID_Init(void)
 *  功能说明：速度与位置PID串级
 *  参数说明：
 *  参   数：无
 *  函数返回：无
 *  修改时间：2025年6月17日
 *  备   注：开始时使用，只调用一次
 ************************************************/
/**
 * @brief  初始化本文件内的 PID 实例（当前只初始化速度环）
 * @note   速度环限幅 1000、积分限幅 50；舵机/位置环的初始化语句被注释掉，
 *         需要时取消注释并补上对应反馈来源。
 */
void All_PID_Init(void)
{
//    //舵机PID
//    float ServoPID_Value[3]={Servo_pid_KP,Servo_pid_KI,Servo_pid_KD};
//    PID_init(&PID_Servo,PID_POSITION,ServoPID_Value,20,100); //舵机角度为75-95-115，差值为20，限幅20，
	
    //速度PID
    float SpeedPID_Value[3]={Speed_pid_KP,Speed_pid_KI,Speed_pid_KD};
    PID_init(&PID_Speed,PID_POSITION,SpeedPID_Value,1000,50); //速度限幅20

}

/************************************************
 *  函数名称: PID参数显示：PID_DataShow(void)
 *  功能说明：
 *  参数说明：
 *  参   数：无
 *  函数返回：无
 *  修改时间：2025年6月17日
 *  备   注：调参使用 跑车是看的数据
 ************************************************/
void PID_DataShow(void)
{
	OLED_ShowSignedNum(1,1,resultSpeed,3);
	OLED_ShowSignedNum(1,5,encoderA_cnt,3);

//    printf("%d,%d,%d,%d,%d\n\r",encoder[0],result_Location,resultSpeed,next,(int)Distance);
//    printf("%d,%d,%d\n\r",result_Location,resultSpeed,9);//上位机看值
	
		Serial_SendNumber(resultSpeed,3);
		Serial_SendString("\n\r");
		Serial_SendNumber(encoderA_cnt,3);
		Serial_SendString("\n\r");
		Serial_SendNumber(45,3);
		Serial_SendString("\n\r");
	
}

/************************************************
 *  函数名称: PID计算：All_PID_scan(void)
 *  功能说明：放在中断调用：时间间隔：10ms
 *  参数说明：
 *  参   数：无
 *  函数返回：无
 *  修改时间：2025年6月17日
 *  备   注：
 ************************************************/
/**
 * @brief  速度环单次计算（放在 10 ms 级定时中断里调用）
 * @note   反馈是编码器脉冲计数、目标是固定值 45，属于“开环标定式”用法；
 *         本工程 main.c 未调用它，实际跑的是寻迹环 PID_xunji。
 */
void All_PID_scan(void)
{
    //电机速度PID计算结果
    resultSpeed=PID_calc(&PID_Speed,encoderA_cnt,45);
}

/*=========================以下PID调参代码 == 勿删=================================*/

/*=========== 调参区（历史保留，勿删）===========
 * 这几个变量和下面两个函数是当初用串口/IPS 屏观察 PID 曲线时用的，
 * 现在没有调用者，保留是为了随时复现“给模拟反馈量 -> 看输出”的调参流程。
 * ===============================================*/
int CsSspeed;/*速度环测试 速度调参代码 勿删*/
float CsLocation;/*位置环测试 位置调参代码 勿删*/
void PID_adjust_parameter(void)/*速度调参代码，勿删*/
{
    /*速度调参代码,勿删*/
/*
    printf("%d,%d,%d\n\r",(int)encoder[0],resultSpeed,CsSspeed);  //给模拟速度测试
                              //实际编码速度，速度PID计算结果，目标编码速度
    ips200_show_int(  150 , 16*17, encoder[0] , 8); //实际编码速度
    ips200_show_int(  150 , 16*18, resultSpeed , 8);//速度PID计算结果
    ips200_show_float( 100 , 16*19, CsSspeed,4,6);     //目标编码速度
*/
    /*位置调参代码，勿删*/

//    printf("%d,%d,%d\n\r",(int)CsLocation,result_Location,0);
//                              //目标距离大小(设置为0)，位置PID计算结果，目标编码速度

//    ips200_show_float(  130 , 16*10, encoder[0] ,4,6); //实际编码速度
//    ips200_show_float(  130 , 16*11, resultSpeed ,4,6);//速度PID计算结果

//    ips200_show_float(  130 , 16*13, CsLocation , 4,6);  //实际位置距离
//    ips200_show_float(  130 , 16*14, result_Location ,4,6); //位置PID计算结果

//    ips200_show_float( 130 , 16*15, 0,4,6);   //目标0

//    Pwm_Motor(resultSpeed);
}

void All_PID_scan_adjust_parameter(void)/*速度调参代码，勿删*/
{
//    //GPS位置PID计算结果
//    result_Location=PID_calc(&PID_Location,(float)CsLocation,0);    //给模拟距离测试

//    //电机速度PID计算结果
//    resultSpeed=PID_calc(&PID_Speed,encoder[0],(float)result_Location);//CsSspeed
}

/*=========================上面PID调参代码===================================*/

