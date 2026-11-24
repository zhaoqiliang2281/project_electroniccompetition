/**
 ******************************************************************************
 * @file    grayscale_sensor.c
 * @brief   8 路灰度循迹：传感器采样 + 权值法偏差 + 位置式 PID 初始化
 * @details
 *   【电平约定】传感器输出低电平表示“看到黑线”（车灯亮），因此
 *   sensor_values[i] == 0 -> 压线，== 1 -> 未压线（白底）。
 *   【权重】从左到右依次 -3,-2,-1,0,0,1,2,3，与 sensor_values 相乘求和即偏差：
 *   车偏右 -> 右侧传感器压线多 -> 偏差为正 -> PID 输出使右轮减速/左轮加速 -> 回中。
 *   【location_flag】用“采样检测法”判特殊位置：
 *     1 = 8 路全部压线（整条横向黑线，通常用作转向/计圈的触发条件）
 *     2 = 中间四路中仅一路未压线（轻微偏线，仍按正常循迹处理）
 *     0 = 其它情况（正常循迹）
 * @note
 *   1. Read_Sensors() 在 TIMG0 中断里被调用，保持无阻塞、无打印；
 *   2. 采样检测法对线宽/车速敏感，若漏判可改为“连续 N 次满足条件再置位”的消抖写法；
 *   3. KP/KI/KD 三组宏是为不同赛道/车速试出来的经验值，改动前先备份。
 ******************************************************************************
 */

#include "grayscale_sensor.h"
#include "OLED.h"

pid_type_def PID_xunji;


uint8_t location_flag;//读取灰度位置状态值

// 权值数组
//1
/* 传感器权重：与物理排布一致（左->右），和为 0 保证居中时偏差为 0 */
float weights[NUM_SENSORS] = {-3, -2, -1, 0, 0, 1, 2, 3};

//float weights[NUM_SENSORS] = {-4.5, -3.5, -2.5, 0, 0, 2.5, 3.5, 4.5};

//灰度寻迹引脚读取值
uint8_t sensor_values[NUM_SENSORS];

/**
 * @brief  采样 8 路灰度并更新 location_flag（中断内调用）
 * @note   引脚顺序与权重数组一一对应，改接线必须同步改这里，否则偏差方向会反。
 */
void Read_Sensors(void) // 读取12路红外传感器的值//识别到黑线返回0//灯亮
{
	if(DL_GPIO_readPins(GPIOB,DL_GPIO_PIN_0)) {sensor_values[0]=1;}else{sensor_values[0]=0;}//左1
	if(DL_GPIO_readPins(GPIOB,DL_GPIO_PIN_27)){sensor_values[1]=1;}else{sensor_values[1]=0;}
	if(DL_GPIO_readPins(GPIOB,DL_GPIO_PIN_26)){sensor_values[2]=1;}else{sensor_values[2]=0;}
	if(DL_GPIO_readPins(GPIOA,DL_GPIO_PIN_26)){sensor_values[3]=1;}else{sensor_values[3]=0;}
	if(DL_GPIO_readPins(GPIOA,DL_GPIO_PIN_25)){sensor_values[4]=1;}else{sensor_values[4]=0;}
	if(DL_GPIO_readPins(GPIOA,DL_GPIO_PIN_24)){sensor_values[5]=1;}else{sensor_values[5]=0;}
	if(DL_GPIO_readPins(GPIOA,DL_GPIO_PIN_12)){sensor_values[6]=1;}else{sensor_values[6]=0;}
	if(DL_GPIO_readPins(GPIOA,DL_GPIO_PIN_7)) {sensor_values[7]=1;}else{sensor_values[7]=0;}//右1
	
	//采样检测法
	if(sensor_values[0]==0&&sensor_values[1]==0&&sensor_values[2]==0&&sensor_values[3]==0&&
	sensor_values[4]==0&&sensor_values[5]==0&&sensor_values[6]==0&&sensor_values[7]==0)
	{
		location_flag = 1;	//全部为1  全部识别到线
	}
	else if(sensor_values[0]==0&&sensor_values[1]==0&&sensor_values[2]==1&&sensor_values[3]==0&&
	sensor_values[4]==0&&sensor_values[5]==0&&sensor_values[6]==0&&sensor_values[7]==0 ||
	
	sensor_values[0]==0&&sensor_values[1]==0&&sensor_values[2]==0&&sensor_values[3]==1&&
	sensor_values[4]==0&&sensor_values[5]==0&&sensor_values[6]==0&&sensor_values[7]==0 || 
	
	sensor_values[0]==0&&sensor_values[1]==0&&sensor_values[2]==0&&sensor_values[3]==0&&
	sensor_values[4]==1&&sensor_values[5]==0&&sensor_values[6]==0&&sensor_values[7]==0 || 
	
	sensor_values[0]==0&&sensor_values[1]==0&&sensor_values[2]==0&&sensor_values[3]==0&&
	sensor_values[4]==0&&sensor_values[5]==1&&sensor_values[6]==0&&sensor_values[7]==0)
	{
		location_flag = 2;	//中间4个或为一
	}
	else
	{
		location_flag = 0;
	}	
	
	
//	else if(sensor_values[0]==1 && sensor_values[1]==1 && sensor_values[2]==1 && sensor_values[5]==0 && sensor_values[6]==0 && sensor_values[7]==0)
//	{
//		location_flag = 2;	//左边全部为1//左边识别到黑线
//	}
//	else if(sensor_values[0]==0 && sensor_values[1]==0 && sensor_values[6]==1 && sensor_values[7]==1)
//	{
//		location_flag = 3;	//右边全部为1//右边识别到黑线
//	}
//	else if(sensor_values[0]==0&& sensor_values[1]==0 && sensor_values[3]==1 && sensor_values[4]==1 && sensor_values[6]==0 && sensor_values[7]==1)
//	{
//		location_flag = 4;//中间全部为1//正常寻迹
//	}
//	else if(sensor_values[0]==0&&sensor_values[1]==0&&sensor_values[2]==0&&sensor_values[3]==0&&sensor_values[4]==0&&sensor_values[5]==0
//		&&sensor_values[6]==0&&sensor_values[7]==0)
//	{
//		location_flag = 5;//全部为0//等全亮，没识别到线
//	}
//	else if(sensor_values[5]==1 && sensor_values[6]==1)
//	{
//		location_flag = 6;
//	}

}


/**
 * @brief  调试显示：把 8 路电平画在 OLED 第 2 行（1=白 0=黑）
 */
void OLED_SENSOR_Show(void)//寻迹模块调试OLED显示
{
	OLED_ShowNum(2,1,sensor_values[0],1);
	OLED_ShowNum(2,2,sensor_values[1],1);
	OLED_ShowNum(2,3,sensor_values[2],1);
	OLED_ShowNum(2,4,sensor_values[3],1);
	OLED_ShowNum(2,5,sensor_values[4],1);
	OLED_ShowNum(2,6,sensor_values[5],1);
	OLED_ShowNum(2,7,sensor_values[6],1);
	OLED_ShowNum(2,8,sensor_values[7],1);
//	OLED_ShowNum(3,1,1,1);//
//	OLED_ShowNum(3,2,2,1);//
//	OLED_ShowNum(3,3,3,1);//
//	OLED_ShowNum(3,4,4,1);//
//	OLED_ShowNum(3,5,5,1);//
//	OLED_ShowNum(3,6,6,1);//
//	OLED_ShowNum(3,7,7,1);//
//	OLED_ShowNum(3,8,8,1);//
}

/**
 * @brief  循迹 PID 初始化（位置式，输出限幅 500，积分限幅 100）
 * @note   位置式 PID 输出直接是“差速修正量”，不需要再做积分累加，改动方便。
 */
void automatic_Init(void)// 初始化 PID 控制器
{
	const  float automaticPID_Value[3]={automatic_KP,automatic_KI,automatic_KD};
	
	PID_init(&PID_xunji,PID_POSITION,automaticPID_Value,500,100);
}

/**
 * @brief  权值法计算循迹偏差
 * @retval 偏差值，负=偏左、正=偏右；全黑/全白等极端情况也按权重求和，由限幅处理
 */
float Calculate_Error(void) //灰度寻迹权值法赋值
{
    float error = 0;
    for (int i = 0; i < NUM_SENSORS; i++) 
	{
        error += weights[i] * sensor_values[i];
    }
    return error;
}

//speed=PID_calc(&PID_xunji,Calculate_Error(),0); //每10ms计算一次寻迹PID

