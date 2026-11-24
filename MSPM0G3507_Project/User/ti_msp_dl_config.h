/*
 * Copyright (c) 2023, Texas Instruments Incorporated - http://www.ti.com
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
 *  ============ ti_msp_dl_config.h =============
 *  Configured MSPM0 DriverLib module declarations
 *
 *  DO NOT EDIT - This file is generated for the MSPM0G350X
 *  by the SysConfig tool.
 */
#ifndef ti_msp_dl_config_h
#define ti_msp_dl_config_h

#define CONFIG_MSPM0G350X

#if defined(__ti_version__) || defined(__TI_COMPILER_VERSION__)
#define SYSCONFIG_WEAK __attribute__((weak))
#elif defined(__IAR_SYSTEMS_ICC__)
#define SYSCONFIG_WEAK __weak
#elif defined(__GNUC__)
#define SYSCONFIG_WEAK __attribute__((weak))
#endif

#include <ti/devices/msp/msp.h>
#include <ti/driverlib/driverlib.h>
#include <ti/driverlib/m0p/dl_core.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  ======== SYSCFG_DL_init ========
 *  Perform all required MSP DL initialization
 *
 *  This function should be called once at a point before any use of
 *  MSP DL.
 */


/* clang-format off */

#define POWER_STARTUP_DELAY                                                (16)


#define GPIO_HFXT_PORT                                                     GPIOA
#define GPIO_HFXIN_PIN                                             DL_GPIO_PIN_5
#define GPIO_HFXIN_IOMUX                                         (IOMUX_PINCM10)
#define GPIO_HFXOUT_PIN                                            DL_GPIO_PIN_6
#define GPIO_HFXOUT_IOMUX                                        (IOMUX_PINCM11)
#define CPUCLK_FREQ                                                     80000000



/* Defines for PWM */
#define PWM_INST                                                           TIMG7
#define PWM_INST_IRQHandler                                     TIMG7_IRQHandler
#define PWM_INST_INT_IRQN                                       (TIMG7_INT_IRQn)
#define PWM_INST_CLK_FREQ                                               10000000
/* GPIO defines for channel 0 */
#define GPIO_PWM_C0_PORT                                                   GPIOA
#define GPIO_PWM_C0_PIN                                           DL_GPIO_PIN_28
#define GPIO_PWM_C0_IOMUX                                         (IOMUX_PINCM3)
#define GPIO_PWM_C0_IOMUX_FUNC                        IOMUX_PINCM3_PF_TIMG7_CCP0
#define GPIO_PWM_C0_IDX                                      DL_TIMER_CC_0_INDEX
/* GPIO defines for channel 1 */
#define GPIO_PWM_C1_PORT                                                   GPIOA
#define GPIO_PWM_C1_PIN                                           DL_GPIO_PIN_27
#define GPIO_PWM_C1_IOMUX                                        (IOMUX_PINCM60)
#define GPIO_PWM_C1_IOMUX_FUNC                       IOMUX_PINCM60_PF_TIMG7_CCP1
#define GPIO_PWM_C1_IDX                                      DL_TIMER_CC_1_INDEX

/* Defines for Motor_PWM */
#define Motor_PWM_INST                                                     TIMA0
#define Motor_PWM_INST_IRQHandler                               TIMA0_IRQHandler
#define Motor_PWM_INST_INT_IRQN                                 (TIMA0_INT_IRQn)
#define Motor_PWM_INST_CLK_FREQ                                          5000000
/* GPIO defines for channel 0 */
#define GPIO_Motor_PWM_C0_PORT                                             GPIOB
#define GPIO_Motor_PWM_C0_PIN                                      DL_GPIO_PIN_8
#define GPIO_Motor_PWM_C0_IOMUX                                  (IOMUX_PINCM25)
#define GPIO_Motor_PWM_C0_IOMUX_FUNC                 IOMUX_PINCM25_PF_TIMA0_CCP0
#define GPIO_Motor_PWM_C0_IDX                                DL_TIMER_CC_0_INDEX
/* GPIO defines for channel 1 */
#define GPIO_Motor_PWM_C1_PORT                                             GPIOB
#define GPIO_Motor_PWM_C1_PIN                                      DL_GPIO_PIN_9
#define GPIO_Motor_PWM_C1_IOMUX                                  (IOMUX_PINCM26)
#define GPIO_Motor_PWM_C1_IOMUX_FUNC                 IOMUX_PINCM26_PF_TIMA0_CCP1
#define GPIO_Motor_PWM_C1_IDX                                DL_TIMER_CC_1_INDEX
/* GPIO defines for channel 2 */
#define GPIO_Motor_PWM_C2_PORT                                             GPIOB
#define GPIO_Motor_PWM_C2_PIN                                     DL_GPIO_PIN_12
#define GPIO_Motor_PWM_C2_IOMUX                                  (IOMUX_PINCM29)
#define GPIO_Motor_PWM_C2_IOMUX_FUNC                 IOMUX_PINCM29_PF_TIMA0_CCP2
#define GPIO_Motor_PWM_C2_IDX                                DL_TIMER_CC_2_INDEX
/* GPIO defines for channel 3 */
#define GPIO_Motor_PWM_C3_PORT                                             GPIOB
#define GPIO_Motor_PWM_C3_PIN                                     DL_GPIO_PIN_13
#define GPIO_Motor_PWM_C3_IOMUX                                  (IOMUX_PINCM30)
#define GPIO_Motor_PWM_C3_IOMUX_FUNC                 IOMUX_PINCM30_PF_TIMA0_CCP3
#define GPIO_Motor_PWM_C3_IDX                                DL_TIMER_CC_3_INDEX

/* Defines for Servo_PWM */
#define Servo_PWM_INST                                                     TIMG6
#define Servo_PWM_INST_IRQHandler                               TIMG6_IRQHandler
#define Servo_PWM_INST_INT_IRQN                                 (TIMG6_INT_IRQn)
#define Servo_PWM_INST_CLK_FREQ                                         10000000
/* GPIO defines for channel 0 */
#define GPIO_Servo_PWM_C0_PORT                                             GPIOA
#define GPIO_Servo_PWM_C0_PIN                                     DL_GPIO_PIN_21
#define GPIO_Servo_PWM_C0_IOMUX                                  (IOMUX_PINCM46)
#define GPIO_Servo_PWM_C0_IOMUX_FUNC                 IOMUX_PINCM46_PF_TIMG6_CCP0
#define GPIO_Servo_PWM_C0_IDX                                DL_TIMER_CC_0_INDEX
/* GPIO defines for channel 1 */
#define GPIO_Servo_PWM_C1_PORT                                             GPIOA
#define GPIO_Servo_PWM_C1_PIN                                     DL_GPIO_PIN_22
#define GPIO_Servo_PWM_C1_IOMUX                                  (IOMUX_PINCM47)
#define GPIO_Servo_PWM_C1_IOMUX_FUNC                 IOMUX_PINCM47_PF_TIMG6_CCP1
#define GPIO_Servo_PWM_C1_IDX                                DL_TIMER_CC_1_INDEX



/* Defines for TIMER_0 */
#define TIMER_0_INST                                                     (TIMG0)
#define TIMER_0_INST_IRQHandler                                 TIMG0_IRQHandler
#define TIMER_0_INST_INT_IRQN                                   (TIMG0_INT_IRQn)
#define TIMER_0_INST_LOAD_VALUE                                         (49999U)



/* Defines for UART_Debug */
#define UART_Debug_INST                                                    UART0
#define UART_Debug_INST_IRQHandler                              UART0_IRQHandler
#define UART_Debug_INST_INT_IRQN                                  UART0_INT_IRQn
#define GPIO_UART_Debug_RX_PORT                                            GPIOA
#define GPIO_UART_Debug_TX_PORT                                            GPIOA
#define GPIO_UART_Debug_RX_PIN                                    DL_GPIO_PIN_11
#define GPIO_UART_Debug_TX_PIN                                    DL_GPIO_PIN_10
#define GPIO_UART_Debug_IOMUX_RX                                 (IOMUX_PINCM22)
#define GPIO_UART_Debug_IOMUX_TX                                 (IOMUX_PINCM21)
#define GPIO_UART_Debug_IOMUX_RX_FUNC                  IOMUX_PINCM22_PF_UART0_RX
#define GPIO_UART_Debug_IOMUX_TX_FUNC                  IOMUX_PINCM21_PF_UART0_TX
#define UART_Debug_BAUD_RATE                                            (115200)
#define UART_Debug_IBRD_40_MHZ_115200_BAUD                                  (21)
#define UART_Debug_FBRD_40_MHZ_115200_BAUD                                  (45)
/* Defines for UART1 */
#define UART1_INST                                                         UART1
#define UART1_INST_IRQHandler                                   UART1_IRQHandler
#define UART1_INST_INT_IRQN                                       UART1_INT_IRQn
#define GPIO_UART1_RX_PORT                                                 GPIOA
#define GPIO_UART1_TX_PORT                                                 GPIOA
#define GPIO_UART1_RX_PIN                                          DL_GPIO_PIN_9
#define GPIO_UART1_TX_PIN                                          DL_GPIO_PIN_8
#define GPIO_UART1_IOMUX_RX                                      (IOMUX_PINCM20)
#define GPIO_UART1_IOMUX_TX                                      (IOMUX_PINCM19)
#define GPIO_UART1_IOMUX_RX_FUNC                       IOMUX_PINCM20_PF_UART1_RX
#define GPIO_UART1_IOMUX_TX_FUNC                       IOMUX_PINCM19_PF_UART1_TX
#define UART1_BAUD_RATE                                                 (115200)
#define UART1_IBRD_40_MHZ_115200_BAUD                                       (21)
#define UART1_FBRD_40_MHZ_115200_BAUD                                       (45)
/* Defines for UART2 */
#define UART2_INST                                                         UART2
#define UART2_INST_IRQHandler                                   UART2_IRQHandler
#define UART2_INST_INT_IRQN                                       UART2_INT_IRQn
#define GPIO_UART2_RX_PORT                                                 GPIOB
#define GPIO_UART2_TX_PORT                                                 GPIOB
#define GPIO_UART2_RX_PIN                                         DL_GPIO_PIN_16
#define GPIO_UART2_TX_PIN                                         DL_GPIO_PIN_15
#define GPIO_UART2_IOMUX_RX                                      (IOMUX_PINCM33)
#define GPIO_UART2_IOMUX_TX                                      (IOMUX_PINCM32)
#define GPIO_UART2_IOMUX_RX_FUNC                       IOMUX_PINCM33_PF_UART2_RX
#define GPIO_UART2_IOMUX_TX_FUNC                       IOMUX_PINCM32_PF_UART2_TX
#define UART2_BAUD_RATE                                                 (115200)
#define UART2_IBRD_40_MHZ_115200_BAUD                                       (21)
#define UART2_FBRD_40_MHZ_115200_BAUD                                       (45)
/* Defines for UART_JY901S */
#define UART_JY901S_INST                                                   UART3
#define UART_JY901S_INST_IRQHandler                             UART3_IRQHandler
#define UART_JY901S_INST_INT_IRQN                                 UART3_INT_IRQn
#define GPIO_UART_JY901S_RX_PORT                                           GPIOA
#define GPIO_UART_JY901S_TX_PORT                                           GPIOA
#define GPIO_UART_JY901S_RX_PIN                                   DL_GPIO_PIN_13
#define GPIO_UART_JY901S_TX_PIN                                   DL_GPIO_PIN_14
#define GPIO_UART_JY901S_IOMUX_RX                                (IOMUX_PINCM35)
#define GPIO_UART_JY901S_IOMUX_TX                                (IOMUX_PINCM36)
#define GPIO_UART_JY901S_IOMUX_RX_FUNC                 IOMUX_PINCM35_PF_UART3_RX
#define GPIO_UART_JY901S_IOMUX_TX_FUNC                 IOMUX_PINCM36_PF_UART3_TX
#define UART_JY901S_BAUD_RATE                                           (115200)
#define UART_JY901S_IBRD_80_MHZ_115200_BAUD                                 (43)
#define UART_JY901S_FBRD_80_MHZ_115200_BAUD                                 (26)





/* Defines for DMA_UART_JY901S */
#define DMA_UART_JY901S_CHAN_ID                                              (0)
#define UART_JY901S_INST_DMA_TRIGGER                         (DMA_UART3_RX_TRIG)



/* Port definition for Pin Group GPIO_RGB */
#define GPIO_RGB_PORT                                                    (GPIOA)

/* Defines for RED: GPIOA.29 with pinCMx 4 on package pin 36 */
#define GPIO_RGB_RED_PIN                                        (DL_GPIO_PIN_29)
#define GPIO_RGB_RED_IOMUX                                        (IOMUX_PINCM4)
/* Port definition for Pin Group BUZZER */
#define BUZZER_PORT                                                      (GPIOA)

/* Defines for buzzer: GPIOA.2 with pinCMx 7 on package pin 42 */
#define BUZZER_buzzer_PIN                                        (DL_GPIO_PIN_2)
#define BUZZER_buzzer_IOMUX                                       (IOMUX_PINCM7)
/* Port definition for Pin Group OLED */
#define OLED_PORT                                                        (GPIOA)

/* Defines for OLED_SCL: GPIOA.16 with pinCMx 38 on package pin 9 */
#define OLED_OLED_SCL_PIN                                       (DL_GPIO_PIN_16)
#define OLED_OLED_SCL_IOMUX                                      (IOMUX_PINCM38)
/* Defines for OLED_SDA: GPIOA.15 with pinCMx 37 on package pin 8 */
#define OLED_OLED_SDA_PIN                                       (DL_GPIO_PIN_15)
#define OLED_OLED_SDA_IOMUX                                      (IOMUX_PINCM37)
/* Port definition for Pin Group Motor */
#define Motor_PORT                                                       (GPIOB)

/* Defines for MotorA1: GPIOB.11 with pinCMx 28 on package pin 63 */
#define Motor_MotorA1_PIN                                       (DL_GPIO_PIN_11)
#define Motor_MotorA1_IOMUX                                      (IOMUX_PINCM28)
/* Defines for MotorA2: GPIOB.14 with pinCMx 31 on package pin 2 */
#define Motor_MotorA2_PIN                                       (DL_GPIO_PIN_14)
#define Motor_MotorA2_IOMUX                                      (IOMUX_PINCM31)
/* Defines for MotorC1: GPIOB.22 with pinCMx 50 on package pin 21 */
#define Motor_MotorC1_PIN                                       (DL_GPIO_PIN_22)
#define Motor_MotorC1_IOMUX                                      (IOMUX_PINCM50)
/* Defines for MotorB2: GPIOB.21 with pinCMx 49 on package pin 20 */
#define Motor_MotorB2_PIN                                       (DL_GPIO_PIN_21)
#define Motor_MotorB2_IOMUX                                      (IOMUX_PINCM49)
/* Defines for MotorB1: GPIOB.20 with pinCMx 48 on package pin 19 */
#define Motor_MotorB1_PIN                                       (DL_GPIO_PIN_20)
#define Motor_MotorB1_IOMUX                                      (IOMUX_PINCM48)
/* Defines for MotorC2: GPIOB.23 with pinCMx 51 on package pin 22 */
#define Motor_MotorC2_PIN                                       (DL_GPIO_PIN_23)
#define Motor_MotorC2_IOMUX                                      (IOMUX_PINCM51)
/* Defines for MotorD1: GPIOB.24 with pinCMx 52 on package pin 23 */
#define Motor_MotorD1_PIN                                       (DL_GPIO_PIN_24)
#define Motor_MotorD1_IOMUX                                      (IOMUX_PINCM52)
/* Defines for MotorD2: GPIOB.25 with pinCMx 56 on package pin 27 */
#define Motor_MotorD2_PIN                                       (DL_GPIO_PIN_25)
#define Motor_MotorD2_IOMUX                                      (IOMUX_PINCM56)
/* Port definition for Pin Group ENCODER */
#define ENCODER_PORT                                                     (GPIOB)

/* Defines for E3A: GPIOB.4 with pinCMx 17 on package pin 52 */
// pins affected by this interrupt request:["E3A","E3B","E4A","E4B"]
#define ENCODER_INT_IRQN                                        (GPIOB_INT_IRQn)
#define ENCODER_INT_IIDX                        (DL_INTERRUPT_GROUP1_IIDX_GPIOB)
#define ENCODER_E3A_IIDX                                     (DL_GPIO_IIDX_DIO4)
#define ENCODER_E3A_PIN                                          (DL_GPIO_PIN_4)
#define ENCODER_E3A_IOMUX                                        (IOMUX_PINCM17)
/* Defines for E3B: GPIOB.5 with pinCMx 18 on package pin 53 */
#define ENCODER_E3B_IIDX                                     (DL_GPIO_IIDX_DIO5)
#define ENCODER_E3B_PIN                                          (DL_GPIO_PIN_5)
#define ENCODER_E3B_IOMUX                                        (IOMUX_PINCM18)
/* Defines for E4A: GPIOB.6 with pinCMx 23 on package pin 58 */
#define ENCODER_E4A_IIDX                                     (DL_GPIO_IIDX_DIO6)
#define ENCODER_E4A_PIN                                          (DL_GPIO_PIN_6)
#define ENCODER_E4A_IOMUX                                        (IOMUX_PINCM23)
/* Defines for E4B: GPIOB.7 with pinCMx 24 on package pin 59 */
#define ENCODER_E4B_IIDX                                     (DL_GPIO_IIDX_DIO7)
#define ENCODER_E4B_PIN                                          (DL_GPIO_PIN_7)
#define ENCODER_E4B_IOMUX                                        (IOMUX_PINCM24)
/* Defines for D1: GPIOB.0 with pinCMx 12 on package pin 47 */
#define sensor_D1_PORT                                                   (GPIOB)
#define sensor_D1_PIN                                            (DL_GPIO_PIN_0)
#define sensor_D1_IOMUX                                          (IOMUX_PINCM12)
/* Defines for D2: GPIOB.27 with pinCMx 58 on package pin 29 */
#define sensor_D2_PORT                                                   (GPIOB)
#define sensor_D2_PIN                                           (DL_GPIO_PIN_27)
#define sensor_D2_IOMUX                                          (IOMUX_PINCM58)
/* Defines for D3: GPIOB.26 with pinCMx 57 on package pin 28 */
#define sensor_D3_PORT                                                   (GPIOB)
#define sensor_D3_PIN                                           (DL_GPIO_PIN_26)
#define sensor_D3_IOMUX                                          (IOMUX_PINCM57)
/* Defines for D4: GPIOA.26 with pinCMx 59 on package pin 30 */
#define sensor_D4_PORT                                                   (GPIOA)
#define sensor_D4_PIN                                           (DL_GPIO_PIN_26)
#define sensor_D4_IOMUX                                          (IOMUX_PINCM59)
/* Defines for D5: GPIOA.25 with pinCMx 55 on package pin 26 */
#define sensor_D5_PORT                                                   (GPIOA)
#define sensor_D5_PIN                                           (DL_GPIO_PIN_25)
#define sensor_D5_IOMUX                                          (IOMUX_PINCM55)
/* Defines for D6: GPIOA.24 with pinCMx 54 on package pin 25 */
#define sensor_D6_PORT                                                   (GPIOA)
#define sensor_D6_PIN                                           (DL_GPIO_PIN_24)
#define sensor_D6_IOMUX                                          (IOMUX_PINCM54)
/* Defines for D7: GPIOA.12 with pinCMx 34 on package pin 5 */
#define sensor_D7_PORT                                                   (GPIOA)
#define sensor_D7_PIN                                           (DL_GPIO_PIN_12)
#define sensor_D7_IOMUX                                          (IOMUX_PINCM34)
/* Defines for D8: GPIOA.7 with pinCMx 14 on package pin 49 */
#define sensor_D8_PORT                                                   (GPIOA)
#define sensor_D8_PIN                                            (DL_GPIO_PIN_7)
#define sensor_D8_IOMUX                                          (IOMUX_PINCM14)
/* Defines for Key1: GPIOA.30 with pinCMx 5 on package pin 37 */
#define Key_Key1_PORT                                                    (GPIOA)
#define Key_Key1_PIN                                            (DL_GPIO_PIN_30)
#define Key_Key1_IOMUX                                            (IOMUX_PINCM5)
/* Defines for Key2: GPIOA.31 with pinCMx 6 on package pin 39 */
#define Key_Key2_PORT                                                    (GPIOA)
#define Key_Key2_PIN                                            (DL_GPIO_PIN_31)
#define Key_Key2_IOMUX                                            (IOMUX_PINCM6)
/* Defines for key3: GPIOB.10 with pinCMx 27 on package pin 62 */
#define Key_key3_PORT                                                    (GPIOB)
#define Key_key3_PIN                                            (DL_GPIO_PIN_10)
#define Key_key3_IOMUX                                           (IOMUX_PINCM27)
/* Defines for key4: GPIOB.17 with pinCMx 43 on package pin 14 */
#define Key_key4_PORT                                                    (GPIOB)
#define Key_key4_PIN                                            (DL_GPIO_PIN_17)
#define Key_key4_IOMUX                                           (IOMUX_PINCM43)
/* Defines for key5: GPIOB.18 with pinCMx 44 on package pin 15 */
#define Key_key5_PORT                                                    (GPIOB)
#define Key_key5_PIN                                            (DL_GPIO_PIN_18)
#define Key_key5_IOMUX                                           (IOMUX_PINCM44)
/* Defines for key6: GPIOB.19 with pinCMx 45 on package pin 16 */
#define Key_key6_PORT                                                    (GPIOB)
#define Key_key6_PIN                                            (DL_GPIO_PIN_19)
#define Key_key6_IOMUX                                           (IOMUX_PINCM45)



/* clang-format on */

void SYSCFG_DL_init(void);
void SYSCFG_DL_initPower(void);
void SYSCFG_DL_GPIO_init(void);
void SYSCFG_DL_SYSCTL_init(void);
void SYSCFG_DL_PWM_init(void);
void SYSCFG_DL_Motor_PWM_init(void);
void SYSCFG_DL_Servo_PWM_init(void);
void SYSCFG_DL_TIMER_0_init(void);
void SYSCFG_DL_UART_Debug_init(void);
void SYSCFG_DL_UART1_init(void);
void SYSCFG_DL_UART2_init(void);
void SYSCFG_DL_UART_JY901S_init(void);
void SYSCFG_DL_DMA_init(void);

void SYSCFG_DL_SYSTICK_init(void);

bool SYSCFG_DL_saveConfiguration(void);
bool SYSCFG_DL_restoreConfiguration(void);

#ifdef __cplusplus
}
#endif

#endif /* ti_msp_dl_config_h */
