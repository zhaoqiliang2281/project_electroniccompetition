/*
 * Copyright (c) 2023, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ============ ti_msp_dl_config.c =============
 *  Configured MSPM0 DriverLib module definitions
 *
 *  DO NOT EDIT - This file is generated for the MSPM0G350X
 *  by the SysConfig tool.
 */

#include "ti_msp_dl_config.h"

DL_TimerG_backupConfig gPWMBackup;
DL_TimerA_backupConfig gMotor_PWMBackup;
DL_TimerG_backupConfig gServo_PWMBackup;
DL_UART_Main_backupConfig gUART_JY901SBackup;

/*
 *  ======== SYSCFG_DL_init ========
 *  Perform any initialization needed before using any board APIs
 */
SYSCONFIG_WEAK void SYSCFG_DL_init(void)
{
    SYSCFG_DL_initPower();
    SYSCFG_DL_GPIO_init();
    /* Module-Specific Initializations*/
    SYSCFG_DL_SYSCTL_init();
    SYSCFG_DL_PWM_init();
    SYSCFG_DL_Motor_PWM_init();
    SYSCFG_DL_Servo_PWM_init();
    SYSCFG_DL_TIMER_0_init();
    SYSCFG_DL_UART_Debug_init();
    SYSCFG_DL_UART1_init();
    SYSCFG_DL_UART2_init();
    SYSCFG_DL_UART_JY901S_init();
    SYSCFG_DL_DMA_init();
    SYSCFG_DL_SYSTICK_init();
    /* Ensure backup structures have no valid state */
	gPWMBackup.backupRdy 	= false;
	gMotor_PWMBackup.backupRdy 	= false;
	gServo_PWMBackup.backupRdy 	= false;

	gUART_JY901SBackup.backupRdy 	= false;

}
/*
 * User should take care to save and restore register configuration in application.
 * See Retention Configuration section for more details.
 */
SYSCONFIG_WEAK bool SYSCFG_DL_saveConfiguration(void)
{
    bool retStatus = true;

	retStatus &= DL_TimerG_saveConfiguration(PWM_INST, &gPWMBackup);
	retStatus &= DL_TimerA_saveConfiguration(Motor_PWM_INST, &gMotor_PWMBackup);
	retStatus &= DL_TimerG_saveConfiguration(Servo_PWM_INST, &gServo_PWMBackup);
	retStatus &= DL_UART_Main_saveConfiguration(UART_JY901S_INST, &gUART_JY901SBackup);

    return retStatus;
}


SYSCONFIG_WEAK bool SYSCFG_DL_restoreConfiguration(void)
{
    bool retStatus = true;

	retStatus &= DL_TimerG_restoreConfiguration(PWM_INST, &gPWMBackup, false);
	retStatus &= DL_TimerA_restoreConfiguration(Motor_PWM_INST, &gMotor_PWMBackup, false);
	retStatus &= DL_TimerG_restoreConfiguration(Servo_PWM_INST, &gServo_PWMBackup, false);
	retStatus &= DL_UART_Main_restoreConfiguration(UART_JY901S_INST, &gUART_JY901SBackup);

    return retStatus;
}

SYSCONFIG_WEAK void SYSCFG_DL_initPower(void)
{
    DL_GPIO_reset(GPIOA);
    DL_GPIO_reset(GPIOB);
    DL_TimerG_reset(PWM_INST);
    DL_TimerA_reset(Motor_PWM_INST);
    DL_TimerG_reset(Servo_PWM_INST);
    DL_TimerG_reset(TIMER_0_INST);
    DL_UART_Main_reset(UART_Debug_INST);
    DL_UART_Main_reset(UART1_INST);
    DL_UART_Main_reset(UART2_INST);
    DL_UART_Main_reset(UART_JY901S_INST);



    DL_GPIO_enablePower(GPIOA);
    DL_GPIO_enablePower(GPIOB);
    DL_TimerG_enablePower(PWM_INST);
    DL_TimerA_enablePower(Motor_PWM_INST);
    DL_TimerG_enablePower(Servo_PWM_INST);
    DL_TimerG_enablePower(TIMER_0_INST);
    DL_UART_Main_enablePower(UART_Debug_INST);
    DL_UART_Main_enablePower(UART1_INST);
    DL_UART_Main_enablePower(UART2_INST);
    DL_UART_Main_enablePower(UART_JY901S_INST);


    delay_cycles(POWER_STARTUP_DELAY);
}

SYSCONFIG_WEAK void SYSCFG_DL_GPIO_init(void)
{

    DL_GPIO_initPeripheralAnalogFunction(GPIO_HFXIN_IOMUX);
    DL_GPIO_initPeripheralAnalogFunction(GPIO_HFXOUT_IOMUX);

    DL_GPIO_initPeripheralOutputFunction(GPIO_PWM_C0_IOMUX,GPIO_PWM_C0_IOMUX_FUNC);
    DL_GPIO_enableOutput(GPIO_PWM_C0_PORT, GPIO_PWM_C0_PIN);
    DL_GPIO_initPeripheralOutputFunction(GPIO_PWM_C1_IOMUX,GPIO_PWM_C1_IOMUX_FUNC);
    DL_GPIO_enableOutput(GPIO_PWM_C1_PORT, GPIO_PWM_C1_PIN);
    DL_GPIO_initPeripheralOutputFunction(GPIO_Motor_PWM_C0_IOMUX,GPIO_Motor_PWM_C0_IOMUX_FUNC);
    DL_GPIO_enableOutput(GPIO_Motor_PWM_C0_PORT, GPIO_Motor_PWM_C0_PIN);
    DL_GPIO_initPeripheralOutputFunction(GPIO_Motor_PWM_C1_IOMUX,GPIO_Motor_PWM_C1_IOMUX_FUNC);
    DL_GPIO_enableOutput(GPIO_Motor_PWM_C1_PORT, GPIO_Motor_PWM_C1_PIN);
    DL_GPIO_initPeripheralOutputFunction(GPIO_Motor_PWM_C2_IOMUX,GPIO_Motor_PWM_C2_IOMUX_FUNC);
    DL_GPIO_enableOutput(GPIO_Motor_PWM_C2_PORT, GPIO_Motor_PWM_C2_PIN);
    DL_GPIO_initPeripheralOutputFunction(GPIO_Motor_PWM_C3_IOMUX,GPIO_Motor_PWM_C3_IOMUX_FUNC);
    DL_GPIO_enableOutput(GPIO_Motor_PWM_C3_PORT, GPIO_Motor_PWM_C3_PIN);
    DL_GPIO_initPeripheralOutputFunction(GPIO_Servo_PWM_C0_IOMUX,GPIO_Servo_PWM_C0_IOMUX_FUNC);
    DL_GPIO_enableOutput(GPIO_Servo_PWM_C0_PORT, GPIO_Servo_PWM_C0_PIN);
    DL_GPIO_initPeripheralOutputFunction(GPIO_Servo_PWM_C1_IOMUX,GPIO_Servo_PWM_C1_IOMUX_FUNC);
    DL_GPIO_enableOutput(GPIO_Servo_PWM_C1_PORT, GPIO_Servo_PWM_C1_PIN);

    DL_GPIO_initPeripheralOutputFunction(
        GPIO_UART_Debug_IOMUX_TX, GPIO_UART_Debug_IOMUX_TX_FUNC);
    DL_GPIO_initPeripheralInputFunction(
        GPIO_UART_Debug_IOMUX_RX, GPIO_UART_Debug_IOMUX_RX_FUNC);
    DL_GPIO_initPeripheralOutputFunction(
        GPIO_UART1_IOMUX_TX, GPIO_UART1_IOMUX_TX_FUNC);
    DL_GPIO_initPeripheralInputFunction(
        GPIO_UART1_IOMUX_RX, GPIO_UART1_IOMUX_RX_FUNC);
    DL_GPIO_initPeripheralOutputFunction(
        GPIO_UART2_IOMUX_TX, GPIO_UART2_IOMUX_TX_FUNC);
    DL_GPIO_initPeripheralInputFunction(
        GPIO_UART2_IOMUX_RX, GPIO_UART2_IOMUX_RX_FUNC);
    DL_GPIO_initPeripheralOutputFunction(
        GPIO_UART_JY901S_IOMUX_TX, GPIO_UART_JY901S_IOMUX_TX_FUNC);
    
	DL_GPIO_initPeripheralInputFunctionFeatures(
		 GPIO_UART_JY901S_IOMUX_RX, GPIO_UART_JY901S_IOMUX_RX_FUNC,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalOutput(GPIO_RGB_RED_IOMUX);

    DL_GPIO_initDigitalOutput(BUZZER_buzzer_IOMUX);

    DL_GPIO_initDigitalOutput(OLED_OLED_SCL_IOMUX);

    DL_GPIO_initDigitalOutput(OLED_OLED_SDA_IOMUX);

    DL_GPIO_initDigitalOutput(Motor_MotorA1_IOMUX);

    DL_GPIO_initDigitalOutput(Motor_MotorA2_IOMUX);

    DL_GPIO_initDigitalOutput(Motor_MotorC1_IOMUX);

    DL_GPIO_initDigitalOutput(Motor_MotorB2_IOMUX);

    DL_GPIO_initDigitalOutput(Motor_MotorB1_IOMUX);

    DL_GPIO_initDigitalOutput(Motor_MotorC2_IOMUX);

    DL_GPIO_initDigitalOutput(Motor_MotorD1_IOMUX);

    DL_GPIO_initDigitalOutput(Motor_MotorD2_IOMUX);

    DL_GPIO_initDigitalInputFeatures(ENCODER_E3A_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(ENCODER_E3B_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(ENCODER_E4A_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(ENCODER_E4B_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(sensor_D1_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_DOWN,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(sensor_D2_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_DOWN,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(sensor_D3_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_DOWN,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(sensor_D4_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_DOWN,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(sensor_D5_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_DOWN,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(sensor_D6_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_DOWN,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(sensor_D7_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_DOWN,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(sensor_D8_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_DOWN,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(Key_Key1_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(Key_Key2_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(Key_key3_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(Key_key4_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(Key_key5_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(Key_key6_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_clearPins(GPIOA, BUZZER_buzzer_PIN |
		OLED_OLED_SCL_PIN |
		OLED_OLED_SDA_PIN);
    DL_GPIO_setPins(GPIOA, GPIO_RGB_RED_PIN);
    DL_GPIO_enableOutput(GPIOA, GPIO_RGB_RED_PIN |
		BUZZER_buzzer_PIN |
		OLED_OLED_SCL_PIN |
		OLED_OLED_SDA_PIN);
    DL_GPIO_clearPins(GPIOB, Motor_MotorA1_PIN |
		Motor_MotorA2_PIN |
		Motor_MotorC1_PIN |
		Motor_MotorB2_PIN |
		Motor_MotorB1_PIN |
		Motor_MotorC2_PIN |
		Motor_MotorD1_PIN |
		Motor_MotorD2_PIN);
    DL_GPIO_enableOutput(GPIOB, Motor_MotorA1_PIN |
		Motor_MotorA2_PIN |
		Motor_MotorC1_PIN |
		Motor_MotorB2_PIN |
		Motor_MotorB1_PIN |
		Motor_MotorC2_PIN |
		Motor_MotorD1_PIN |
		Motor_MotorD2_PIN);
    DL_GPIO_setLowerPinsPolarity(GPIOB, DL_GPIO_PIN_4_EDGE_RISE |
		DL_GPIO_PIN_5_EDGE_RISE |
		DL_GPIO_PIN_6_EDGE_RISE |
		DL_GPIO_PIN_7_EDGE_RISE);
    DL_GPIO_clearInterruptStatus(GPIOB, ENCODER_E3A_PIN |
		ENCODER_E3B_PIN |
		ENCODER_E4A_PIN |
		ENCODER_E4B_PIN);
    DL_GPIO_enableInterrupt(GPIOB, ENCODER_E3A_PIN |
		ENCODER_E3B_PIN |
		ENCODER_E4A_PIN |
		ENCODER_E4B_PIN);

}


static const DL_SYSCTL_SYSPLLConfig gSYSPLLConfig = {
    .inputFreq              = DL_SYSCTL_SYSPLL_INPUT_FREQ_32_48_MHZ,
	.rDivClk2x              = 1,
	.rDivClk1               = 0,
	.rDivClk0               = 0,
	.enableCLK2x            = DL_SYSCTL_SYSPLL_CLK2X_ENABLE,
	.enableCLK1             = DL_SYSCTL_SYSPLL_CLK1_DISABLE,
	.enableCLK0             = DL_SYSCTL_SYSPLL_CLK0_DISABLE,
	.sysPLLMCLK             = DL_SYSCTL_SYSPLL_MCLK_CLK2X,
	.sysPLLRef              = DL_SYSCTL_SYSPLL_REF_HFCLK,
	.qDiv                   = 1,
	.pDiv                   = DL_SYSCTL_SYSPLL_PDIV_1
};
SYSCONFIG_WEAK void SYSCFG_DL_SYSCTL_init(void)
{

	//Low Power Mode is configured to be SLEEP0
    DL_SYSCTL_setBORThreshold(DL_SYSCTL_BOR_THRESHOLD_LEVEL_0);
    DL_SYSCTL_setFlashWaitState(DL_SYSCTL_FLASH_WAIT_STATE_2);

    
	DL_SYSCTL_setSYSOSCFreq(DL_SYSCTL_SYSOSC_FREQ_BASE);
	/* Set default configuration */
	DL_SYSCTL_disableHFXT();
	DL_SYSCTL_disableSYSPLL();
    DL_SYSCTL_setHFCLKSourceHFXTParams(DL_SYSCTL_HFXT_RANGE_32_48_MHZ,10, true);
    DL_SYSCTL_configSYSPLL((DL_SYSCTL_SYSPLLConfig *) &gSYSPLLConfig);
    DL_SYSCTL_setULPCLKDivider(DL_SYSCTL_ULPCLK_DIV_2);
    DL_SYSCTL_enableMFCLK();
    DL_SYSCTL_setMCLKSource(SYSOSC, HSCLK, DL_SYSCTL_HSCLK_SOURCE_SYSPLL);
    /* INT_GROUP1 Priority */
    NVIC_SetPriority(GPIOB_INT_IRQn, 0);

}


/*
 * Timer clock configuration to be sourced by  / 8 (10000000 Hz)
 * timerClkFreq = (timerClkSrc / (timerClkDivRatio * (timerClkPrescale + 1)))
 *   10000000 Hz = 10000000 Hz / (8 * (0 + 1))
 */
static const DL_TimerG_ClockConfig gPWMClockConfig = {
    .clockSel = DL_TIMER_CLOCK_BUSCLK,
    .divideRatio = DL_TIMER_CLOCK_DIVIDE_8,
    .prescale = 0U
};

static const DL_TimerG_PWMConfig gPWMConfig = {
    .pwmMode = DL_TIMER_PWM_MODE_EDGE_ALIGN,
    .period = 20000,
    .startTimer = DL_TIMER_START,
};

SYSCONFIG_WEAK void SYSCFG_DL_PWM_init(void) {

    DL_TimerG_setClockConfig(
        PWM_INST, (DL_TimerG_ClockConfig *) &gPWMClockConfig);

    DL_TimerG_initPWMMode(
        PWM_INST, (DL_TimerG_PWMConfig *) &gPWMConfig);

    DL_TimerG_setCaptureCompareOutCtl(PWM_INST, DL_TIMER_CC_OCTL_INIT_VAL_LOW,
		DL_TIMER_CC_OCTL_INV_OUT_ENABLED, DL_TIMER_CC_OCTL_SRC_FUNCVAL,
		DL_TIMERG_CAPTURE_COMPARE_0_INDEX);

    DL_TimerG_setCaptCompUpdateMethod(PWM_INST, DL_TIMER_CC_UPDATE_METHOD_IMMEDIATE, DL_TIMERG_CAPTURE_COMPARE_0_INDEX);
    DL_TimerG_setCaptureCompareValue(PWM_INST, 20000, DL_TIMER_CC_0_INDEX);

    DL_TimerG_setCaptureCompareOutCtl(PWM_INST, DL_TIMER_CC_OCTL_INIT_VAL_LOW,
		DL_TIMER_CC_OCTL_INV_OUT_ENABLED, DL_TIMER_CC_OCTL_SRC_FUNCVAL,
		DL_TIMERG_CAPTURE_COMPARE_1_INDEX);

    DL_TimerG_setCaptCompUpdateMethod(PWM_INST, DL_TIMER_CC_UPDATE_METHOD_IMMEDIATE, DL_TIMERG_CAPTURE_COMPARE_1_INDEX);
    DL_TimerG_setCaptureCompareValue(PWM_INST, 20000, DL_TIMER_CC_1_INDEX);

    DL_TimerG_enableClock(PWM_INST);


    
    DL_TimerG_setCCPDirection(PWM_INST , DL_TIMER_CC0_OUTPUT | DL_TIMER_CC1_OUTPUT );


}
/*
 * Timer clock configuration to be sourced by  / 8 (10000000 Hz)
 * timerClkFreq = (timerClkSrc / (timerClkDivRatio * (timerClkPrescale + 1)))
 *   5000000 Hz = 10000000 Hz / (8 * (1 + 1))
 */
static const DL_TimerA_ClockConfig gMotor_PWMClockConfig = {
    .clockSel = DL_TIMER_CLOCK_BUSCLK,
    .divideRatio = DL_TIMER_CLOCK_DIVIDE_8,
    .prescale = 1U
};

static const DL_TimerA_PWMConfig gMotor_PWMConfig = {
    .pwmMode = DL_TIMER_PWM_MODE_EDGE_ALIGN,
    .period = 1000,
    .isTimerWithFourCC = true,
    .startTimer = DL_TIMER_START,
};

SYSCONFIG_WEAK void SYSCFG_DL_Motor_PWM_init(void) {

    DL_TimerA_setClockConfig(
        Motor_PWM_INST, (DL_TimerA_ClockConfig *) &gMotor_PWMClockConfig);

    DL_TimerA_initPWMMode(
        Motor_PWM_INST, (DL_TimerA_PWMConfig *) &gMotor_PWMConfig);

    DL_TimerA_setCaptureCompareOutCtl(Motor_PWM_INST, DL_TIMER_CC_OCTL_INIT_VAL_LOW,
		DL_TIMER_CC_OCTL_INV_OUT_ENABLED, DL_TIMER_CC_OCTL_SRC_FUNCVAL,
		DL_TIMERA_CAPTURE_COMPARE_0_INDEX);

    DL_TimerA_setCaptCompUpdateMethod(Motor_PWM_INST, DL_TIMER_CC_UPDATE_METHOD_IMMEDIATE, DL_TIMERA_CAPTURE_COMPARE_0_INDEX);
    DL_TimerA_setCaptureCompareValue(Motor_PWM_INST, 1000, DL_TIMER_CC_0_INDEX);

    DL_TimerA_setCaptureCompareOutCtl(Motor_PWM_INST, DL_TIMER_CC_OCTL_INIT_VAL_LOW,
		DL_TIMER_CC_OCTL_INV_OUT_ENABLED, DL_TIMER_CC_OCTL_SRC_FUNCVAL,
		DL_TIMERA_CAPTURE_COMPARE_1_INDEX);

    DL_TimerA_setCaptCompUpdateMethod(Motor_PWM_INST, DL_TIMER_CC_UPDATE_METHOD_IMMEDIATE, DL_TIMERA_CAPTURE_COMPARE_1_INDEX);
    DL_TimerA_setCaptureCompareValue(Motor_PWM_INST, 1000, DL_TIMER_CC_1_INDEX);

    DL_TimerA_setCaptureCompareOutCtl(Motor_PWM_INST, DL_TIMER_CC_OCTL_INIT_VAL_LOW,
		DL_TIMER_CC_OCTL_INV_OUT_ENABLED, DL_TIMER_CC_OCTL_SRC_FUNCVAL,
		DL_TIMERA_CAPTURE_COMPARE_2_INDEX);

    DL_TimerA_setCaptCompUpdateMethod(Motor_PWM_INST, DL_TIMER_CC_UPDATE_METHOD_IMMEDIATE, DL_TIMERA_CAPTURE_COMPARE_2_INDEX);
    DL_TimerA_setCaptureCompareValue(Motor_PWM_INST, 1000, DL_TIMER_CC_2_INDEX);

    DL_TimerA_setCaptureCompareOutCtl(Motor_PWM_INST, DL_TIMER_CC_OCTL_INIT_VAL_LOW,
		DL_TIMER_CC_OCTL_INV_OUT_ENABLED, DL_TIMER_CC_OCTL_SRC_FUNCVAL,
		DL_TIMERA_CAPTURE_COMPARE_3_INDEX);

    DL_TimerA_setCaptCompUpdateMethod(Motor_PWM_INST, DL_TIMER_CC_UPDATE_METHOD_IMMEDIATE, DL_TIMERA_CAPTURE_COMPARE_3_INDEX);
    DL_TimerA_setCaptureCompareValue(Motor_PWM_INST, 1000, DL_TIMER_CC_3_INDEX);

    DL_TimerA_enableClock(Motor_PWM_INST);


    
    DL_TimerA_setCCPDirection(Motor_PWM_INST , DL_TIMER_CC0_OUTPUT | DL_TIMER_CC1_OUTPUT | DL_TIMER_CC2_OUTPUT | DL_TIMER_CC3_OUTPUT );


}
/*
 * Timer clock configuration to be sourced by  / 8 (10000000 Hz)
 * timerClkFreq = (timerClkSrc / (timerClkDivRatio * (timerClkPrescale + 1)))
 *   10000000 Hz = 10000000 Hz / (8 * (0 + 1))
 */
static const DL_TimerG_ClockConfig gServo_PWMClockConfig = {
    .clockSel = DL_TIMER_CLOCK_BUSCLK,
    .divideRatio = DL_TIMER_CLOCK_DIVIDE_8,
    .prescale = 0U
};

static const DL_TimerG_PWMConfig gServo_PWMConfig = {
    .pwmMode = DL_TIMER_PWM_MODE_EDGE_ALIGN,
    .period = 20000,
    .startTimer = DL_TIMER_START,
};

SYSCONFIG_WEAK void SYSCFG_DL_Servo_PWM_init(void) {

    DL_TimerG_setClockConfig(
        Servo_PWM_INST, (DL_TimerG_ClockConfig *) &gServo_PWMClockConfig);

    DL_TimerG_initPWMMode(
        Servo_PWM_INST, (DL_TimerG_PWMConfig *) &gServo_PWMConfig);

    DL_TimerG_setCaptureCompareOutCtl(Servo_PWM_INST, DL_TIMER_CC_OCTL_INIT_VAL_LOW,
		DL_TIMER_CC_OCTL_INV_OUT_ENABLED, DL_TIMER_CC_OCTL_SRC_FUNCVAL,
		DL_TIMERG_CAPTURE_COMPARE_0_INDEX);

    DL_TimerG_setCaptCompUpdateMethod(Servo_PWM_INST, DL_TIMER_CC_UPDATE_METHOD_IMMEDIATE, DL_TIMERG_CAPTURE_COMPARE_0_INDEX);
    DL_TimerG_setCaptureCompareValue(Servo_PWM_INST, 20000, DL_TIMER_CC_0_INDEX);

    DL_TimerG_setCaptureCompareOutCtl(Servo_PWM_INST, DL_TIMER_CC_OCTL_INIT_VAL_LOW,
		DL_TIMER_CC_OCTL_INV_OUT_ENABLED, DL_TIMER_CC_OCTL_SRC_FUNCVAL,
		DL_TIMERG_CAPTURE_COMPARE_1_INDEX);

    DL_TimerG_setCaptCompUpdateMethod(Servo_PWM_INST, DL_TIMER_CC_UPDATE_METHOD_IMMEDIATE, DL_TIMERG_CAPTURE_COMPARE_1_INDEX);
    DL_TimerG_setCaptureCompareValue(Servo_PWM_INST, 20000, DL_TIMER_CC_1_INDEX);

    DL_TimerG_enableClock(Servo_PWM_INST);


    
    DL_TimerG_setCCPDirection(Servo_PWM_INST , DL_TIMER_CC0_OUTPUT | DL_TIMER_CC1_OUTPUT );


}



/*
 * Timer clock configuration to be sourced by BUSCLK /  (5000000 Hz)
 * timerClkFreq = (timerClkSrc / (timerClkDivRatio * (timerClkPrescale + 1)))
 *   5000000 Hz = 5000000 Hz / (8 * (0 + 1))
 */
static const DL_TimerG_ClockConfig gTIMER_0ClockConfig = {
    .clockSel    = DL_TIMER_CLOCK_BUSCLK,
    .divideRatio = DL_TIMER_CLOCK_DIVIDE_8,
    .prescale    = 0U,
};

/*
 * Timer load value (where the counter starts from) is calculated as (timerPeriod * timerClockFreq) - 1
 * TIMER_0_INST_LOAD_VALUE = (10 ms * 5000000 Hz) - 1
 */
static const DL_TimerG_TimerConfig gTIMER_0TimerConfig = {
    .period     = TIMER_0_INST_LOAD_VALUE,
    .timerMode  = DL_TIMER_TIMER_MODE_PERIODIC_UP,
    .startTimer = DL_TIMER_STOP,
};

SYSCONFIG_WEAK void SYSCFG_DL_TIMER_0_init(void) {

    DL_TimerG_setClockConfig(TIMER_0_INST,
        (DL_TimerG_ClockConfig *) &gTIMER_0ClockConfig);

    DL_TimerG_initTimerMode(TIMER_0_INST,
        (DL_TimerG_TimerConfig *) &gTIMER_0TimerConfig);
    DL_TimerG_enableInterrupt(TIMER_0_INST , DL_TIMERG_INTERRUPT_ZERO_EVENT);
    DL_TimerG_enableClock(TIMER_0_INST);





}



static const DL_UART_Main_ClockConfig gUART_DebugClockConfig = {
    .clockSel    = DL_UART_MAIN_CLOCK_BUSCLK,
    .divideRatio = DL_UART_MAIN_CLOCK_DIVIDE_RATIO_1
};

static const DL_UART_Main_Config gUART_DebugConfig = {
    .mode        = DL_UART_MAIN_MODE_NORMAL,
    .direction   = DL_UART_MAIN_DIRECTION_TX_RX,
    .flowControl = DL_UART_MAIN_FLOW_CONTROL_NONE,
    .parity      = DL_UART_MAIN_PARITY_NONE,
    .wordLength  = DL_UART_MAIN_WORD_LENGTH_8_BITS,
    .stopBits    = DL_UART_MAIN_STOP_BITS_ONE
};

SYSCONFIG_WEAK void SYSCFG_DL_UART_Debug_init(void)
{
    DL_UART_Main_setClockConfig(UART_Debug_INST, (DL_UART_Main_ClockConfig *) &gUART_DebugClockConfig);

    DL_UART_Main_init(UART_Debug_INST, (DL_UART_Main_Config *) &gUART_DebugConfig);
    /*
     * Configure baud rate by setting oversampling and baud rate divisors.
     *  Target baud rate: 115200
     *  Actual baud rate: 115190.78
     */
    DL_UART_Main_setOversampling(UART_Debug_INST, DL_UART_OVERSAMPLING_RATE_16X);
    DL_UART_Main_setBaudRateDivisor(UART_Debug_INST, UART_Debug_IBRD_40_MHZ_115200_BAUD, UART_Debug_FBRD_40_MHZ_115200_BAUD);


    /* Configure Interrupts */
    DL_UART_Main_enableInterrupt(UART_Debug_INST,
                                 DL_UART_MAIN_INTERRUPT_RX);


    DL_UART_Main_enable(UART_Debug_INST);
}

static const DL_UART_Main_ClockConfig gUART1ClockConfig = {
    .clockSel    = DL_UART_MAIN_CLOCK_BUSCLK,
    .divideRatio = DL_UART_MAIN_CLOCK_DIVIDE_RATIO_1
};

static const DL_UART_Main_Config gUART1Config = {
    .mode        = DL_UART_MAIN_MODE_NORMAL,
    .direction   = DL_UART_MAIN_DIRECTION_TX_RX,
    .flowControl = DL_UART_MAIN_FLOW_CONTROL_NONE,
    .parity      = DL_UART_MAIN_PARITY_NONE,
    .wordLength  = DL_UART_MAIN_WORD_LENGTH_8_BITS,
    .stopBits    = DL_UART_MAIN_STOP_BITS_ONE
};

SYSCONFIG_WEAK void SYSCFG_DL_UART1_init(void)
{
    DL_UART_Main_setClockConfig(UART1_INST, (DL_UART_Main_ClockConfig *) &gUART1ClockConfig);

    DL_UART_Main_init(UART1_INST, (DL_UART_Main_Config *) &gUART1Config);
    /*
     * Configure baud rate by setting oversampling and baud rate divisors.
     *  Target baud rate: 115200
     *  Actual baud rate: 115190.78
     */
    DL_UART_Main_setOversampling(UART1_INST, DL_UART_OVERSAMPLING_RATE_16X);
    DL_UART_Main_setBaudRateDivisor(UART1_INST, UART1_IBRD_40_MHZ_115200_BAUD, UART1_FBRD_40_MHZ_115200_BAUD);


    /* Configure Interrupts */
    DL_UART_Main_enableInterrupt(UART1_INST,
                                 DL_UART_MAIN_INTERRUPT_RX);


    DL_UART_Main_enable(UART1_INST);
}

static const DL_UART_Main_ClockConfig gUART2ClockConfig = {
    .clockSel    = DL_UART_MAIN_CLOCK_BUSCLK,
    .divideRatio = DL_UART_MAIN_CLOCK_DIVIDE_RATIO_1
};

static const DL_UART_Main_Config gUART2Config = {
    .mode        = DL_UART_MAIN_MODE_NORMAL,
    .direction   = DL_UART_MAIN_DIRECTION_TX_RX,
    .flowControl = DL_UART_MAIN_FLOW_CONTROL_NONE,
    .parity      = DL_UART_MAIN_PARITY_NONE,
    .wordLength  = DL_UART_MAIN_WORD_LENGTH_8_BITS,
    .stopBits    = DL_UART_MAIN_STOP_BITS_ONE
};

SYSCONFIG_WEAK void SYSCFG_DL_UART2_init(void)
{
    DL_UART_Main_setClockConfig(UART2_INST, (DL_UART_Main_ClockConfig *) &gUART2ClockConfig);

    DL_UART_Main_init(UART2_INST, (DL_UART_Main_Config *) &gUART2Config);
    /*
     * Configure baud rate by setting oversampling and baud rate divisors.
     *  Target baud rate: 115200
     *  Actual baud rate: 115190.78
     */
    DL_UART_Main_setOversampling(UART2_INST, DL_UART_OVERSAMPLING_RATE_16X);
    DL_UART_Main_setBaudRateDivisor(UART2_INST, UART2_IBRD_40_MHZ_115200_BAUD, UART2_FBRD_40_MHZ_115200_BAUD);


    /* Configure Interrupts */
    DL_UART_Main_enableInterrupt(UART2_INST,
                                 DL_UART_MAIN_INTERRUPT_RX);


    DL_UART_Main_enable(UART2_INST);
}

static const DL_UART_Main_ClockConfig gUART_JY901SClockConfig = {
    .clockSel    = DL_UART_MAIN_CLOCK_BUSCLK,
    .divideRatio = DL_UART_MAIN_CLOCK_DIVIDE_RATIO_1
};

static const DL_UART_Main_Config gUART_JY901SConfig = {
    .mode        = DL_UART_MAIN_MODE_NORMAL,
    .direction   = DL_UART_MAIN_DIRECTION_TX_RX,
    .flowControl = DL_UART_MAIN_FLOW_CONTROL_NONE,
    .parity      = DL_UART_MAIN_PARITY_NONE,
    .wordLength  = DL_UART_MAIN_WORD_LENGTH_8_BITS,
    .stopBits    = DL_UART_MAIN_STOP_BITS_ONE
};

SYSCONFIG_WEAK void SYSCFG_DL_UART_JY901S_init(void)
{
    DL_UART_Main_setClockConfig(UART_JY901S_INST, (DL_UART_Main_ClockConfig *) &gUART_JY901SClockConfig);

    DL_UART_Main_init(UART_JY901S_INST, (DL_UART_Main_Config *) &gUART_JY901SConfig);
    /*
     * Configure baud rate by setting oversampling and baud rate divisors.
     *  Target baud rate: 115200
     *  Actual baud rate: 115190.78
     */
    DL_UART_Main_setOversampling(UART_JY901S_INST, DL_UART_OVERSAMPLING_RATE_16X);
    DL_UART_Main_setBaudRateDivisor(UART_JY901S_INST, UART_JY901S_IBRD_80_MHZ_115200_BAUD, UART_JY901S_FBRD_80_MHZ_115200_BAUD);


    /* Configure DMA Receive Event */
    DL_UART_Main_enableDMAReceiveEvent(UART_JY901S_INST, DL_UART_DMA_INTERRUPT_RX);

    DL_UART_Main_enable(UART_JY901S_INST);
}

static const DL_DMA_Config gDMA_UART_JY901SConfig = {
    .transferMode   = DL_DMA_FULL_CH_REPEAT_SINGLE_TRANSFER_MODE,
    .extendedMode   = DL_DMA_NORMAL_MODE,
    .destIncrement  = DL_DMA_ADDR_INCREMENT,
    .srcIncrement   = DL_DMA_ADDR_UNCHANGED,
    .destWidth      = DL_DMA_WIDTH_BYTE,
    .srcWidth       = DL_DMA_WIDTH_BYTE,
    .trigger        = UART_JY901S_INST_DMA_TRIGGER,
    .triggerType    = DL_DMA_TRIGGER_TYPE_EXTERNAL,
};

SYSCONFIG_WEAK void SYSCFG_DL_DMA_UART_JY901S_init(void)
{
    DL_DMA_setDestIncrement(DMA, DMA_UART_JY901S_CHAN_ID, DL_DMA_ADDR_INCREMENT);
    DL_DMA_setTransferSize(DMA, DMA_UART_JY901S_CHAN_ID, 66);
    DL_DMA_initChannel(DMA, DMA_UART_JY901S_CHAN_ID , (DL_DMA_Config *) &gDMA_UART_JY901SConfig);
}
SYSCONFIG_WEAK void SYSCFG_DL_DMA_init(void){
    SYSCFG_DL_DMA_UART_JY901S_init();
}


SYSCONFIG_WEAK void SYSCFG_DL_SYSTICK_init(void)
{
    /*
     * Initializes the SysTick period to 1.00 ms,
     * enables the interrupt, and starts the SysTick Timer
     */
    DL_SYSTICK_config(80000);
}

