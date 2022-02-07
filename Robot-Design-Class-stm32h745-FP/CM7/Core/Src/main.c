/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdlib.h>

#include "ServoControl.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#ifndef HSEM_ID_0
#define HSEM_ID_0 (0U) /* HW semaphore 0*/
#endif
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */
int x0, start, mv, action, getBall, throwInit, throwBall;
int wsc;
char command;
uint8_t uart3InputBuffer[10];

uint32_t servo_list[6] = {1, 2, 3, 4, 5, 6};
uint32_t servo_pwm_list[6] = {1500, 1700, 800, 1500, 2300, 1500};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */
// ref: https://blog.csdn.net/wschou/article/details/106594045
#undef PUTCHAR_PROTOTYPE
#undef GETCHAR_PROTOTYPE
#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#define GETCHAR_PROTOTYPE int __io_getchar(FILE *f)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#define GETCHAR_PROTOTYPE int fgetc(FILE *f)
#endif
// HAL库
PUTCHAR_PROTOTYPE {  // scanf() usually not use
    HAL_UART_Transmit(&huart3, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
    return ch;
}
GETCHAR_PROTOTYPE {  // printf()
    uint8_t ch;
    HAL_UART_Receive(&huart3, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
    return ch;
}

// my code
void init();
void grafcet();
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
    /* USER CODE BEGIN 1 */

    /* USER CODE END 1 */
    /* USER CODE BEGIN Boot_Mode_Sequence_0 */
    int32_t timeout;
    /* USER CODE END Boot_Mode_Sequence_0 */

    /* USER CODE BEGIN Boot_Mode_Sequence_1 */
    /* Wait until CPU2 boots and enters in stop mode or timeout*/
    timeout = 0xFFFF;
    while ((__HAL_RCC_GET_FLAG(RCC_FLAG_D2CKRDY) != RESET) && (timeout-- > 0))
        ;
    if (timeout < 0) {
        Error_Handler();
    }
    /* USER CODE END Boot_Mode_Sequence_1 */
    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();
    /* USER CODE BEGIN Boot_Mode_Sequence_2 */
    /* When system initialization is finished, Cortex-M7 will release Cortex-M4 by means of
    HSEM notification */
    /*HW semaphore Clock enable*/
    __HAL_RCC_HSEM_CLK_ENABLE();
    /*Take HSEM */
    HAL_HSEM_FastTake(HSEM_ID_0);
    /*Release HSEM in order to notify the CPU2(CM4)*/
    HAL_HSEM_Release(HSEM_ID_0, 0);
    /* wait until CPU2 wakes up from stop mode */
    timeout = 0xFFFF;
    while ((__HAL_RCC_GET_FLAG(RCC_FLAG_D2CKRDY) == RESET) && (timeout-- > 0))
        ;
    if (timeout < 0) {
        Error_Handler();
    }
    /* USER CODE END Boot_Mode_Sequence_2 */

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_USART3_UART_Init();
    MX_USART2_UART_Init();
    /* USER CODE BEGIN 2 */
    init();

    robotInit(&huart2, 2000);
    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1) {
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
        grafcet();
        //        HAL_Delay(100);
    }
    /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

    /** Supply configuration update enable
     */
    HAL_PWREx_ConfigSupply(PWR_DIRECT_SMPS_SUPPLY);
    /** Configure the main internal regulator output voltage
     */
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

    while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {
    }
    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }
    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 |
                                  RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_D3PCLK1 |
                                  RCC_CLOCKTYPE_D1PCLK1;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV1;
    RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
        Error_Handler();
    }
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART3 | RCC_PERIPHCLK_USART2;
    PeriphClkInitStruct.Usart234578ClockSelection = RCC_USART234578CLKSOURCE_D2PCLK1;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK) {
        Error_Handler();
    }
}

/**
 * @brief USART2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART2_UART_Init(void) {
    /* USER CODE BEGIN USART2_Init 0 */

    /* USER CODE END USART2_Init 0 */

    /* USER CODE BEGIN USART2_Init 1 */

    /* USER CODE END USART2_Init 1 */
    huart2.Instance = USART2;
    huart2.Init.BaudRate = 9600;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;
    huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    huart2.Init.ClockPrescaler = UART_PRESCALER_DIV1;
    huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    if (HAL_UART_Init(&huart2) != HAL_OK) {
        Error_Handler();
    }
    if (HAL_UARTEx_SetTxFifoThreshold(&huart2, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK) {
        Error_Handler();
    }
    if (HAL_UARTEx_SetRxFifoThreshold(&huart2, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK) {
        Error_Handler();
    }
    if (HAL_UARTEx_DisableFifoMode(&huart2) != HAL_OK) {
        Error_Handler();
    }
    /* USER CODE BEGIN USART2_Init 2 */

    /* USER CODE END USART2_Init 2 */
}

/**
 * @brief USART3 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART3_UART_Init(void) {
    /* USER CODE BEGIN USART3_Init 0 */

    /* USER CODE END USART3_Init 0 */

    /* USER CODE BEGIN USART3_Init 1 */

    /* USER CODE END USART3_Init 1 */
    huart3.Instance = USART3;
    huart3.Init.BaudRate = 115200;
    huart3.Init.WordLength = UART_WORDLENGTH_8B;
    huart3.Init.StopBits = UART_STOPBITS_1;
    huart3.Init.Parity = UART_PARITY_NONE;
    huart3.Init.Mode = UART_MODE_TX_RX;
    huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart3.Init.OverSampling = UART_OVERSAMPLING_16;
    huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    huart3.Init.ClockPrescaler = UART_PRESCALER_DIV1;
    huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    if (HAL_UART_Init(&huart3) != HAL_OK) {
        Error_Handler();
    }
    if (HAL_UARTEx_SetTxFifoThreshold(&huart3, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK) {
        Error_Handler();
    }
    if (HAL_UARTEx_SetRxFifoThreshold(&huart3, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK) {
        Error_Handler();
    }
    if (HAL_UARTEx_DisableFifoMode(&huart3) != HAL_OK) {
        Error_Handler();
    }
    /* USER CODE BEGIN USART3_Init 2 */

    /* USER CODE END USART3_Init 2 */
}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();

    /*Configure GPIO pins : PC1 PC4 PC5 */
    GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /*Configure GPIO pins : PA1 PA2 PA7 */
    GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /*Configure GPIO pin : PB13 */
    GPIO_InitStruct.Pin = GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /*Configure GPIO pins : PA8 PA11 PA12 */
    GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_11 | GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF10_OTG1_FS;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /*Configure GPIO pins : PG11 PG13 */
    GPIO_InitStruct.Pin = GPIO_PIN_11 | GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
}

/* USER CODE BEGIN 4 */
void init() {
    x0 = 1;
    start = 0;
    mv = 0;
    action = 0;
    getBall = 0;
    throwInit = 0;
    throwBall = 0;
}

void grafcet() {
    if (x0 == 1) {  // 0
        printf("start or not(y/n): ");
        fflush(stdout);
        while (HAL_OK != HAL_UART_Receive(&huart3, uart3InputBuffer, 1, 100))
            ;

        char tmp[10];
        snprintf(tmp, "%d", uart3InputBuffer);
        printf(tmp);
        printf("\r\n");
        fflush(stdout);
        if (strcmp(tmp, "y") == 0) {
            printf("program started\r\n");
            fflush(stdout);
            start = 1, x0 = 0;
        }

    } else if (start == 1) {  // 1
        printf("夾子移動成功\r\n");
        fflush(stdout);
        servo_pwm_list[0] = 1500;
        servo_pwm_list[1] = 1700;
        servo_pwm_list[2] = 800;
        servo_pwm_list[3] = 1500;
        servo_pwm_list[4] = 2300;
        servo_pwm_list[5] = 950;
        robot_control_cmd(&huart2, 6, 1000, servo_list, servo_pwm_list);
        wsc = 0;
        mv = 1, start = 0;
    } else if (mv == 1) {  // 2
        printf("接收從電腦指令(w/s/a/d/space): ");
        fflush(stdout);
        while (HAL_OK != HAL_UART_Receive(&huart3, uart3InputBuffer, 1, 100))
            ;

        char tmp[10];
        snprintf(tmp, "%d", uart3InputBuffer);
        printf(tmp);
        printf("\r\n");
        fflush(stdout);
        command = tmp[0];
        printf("接收電腦指令成功\r\n");
        fflush(stdout);
        action = 1, mv = 0;

    } else if (action == 1) {  // 3
        switch (command) {
            case ' ':  // space
                printf("detect space pressed\r\n");
                fflush(stdout);
                getBall = 1, action = 0;
                break;
            case 'w':
                wsc++;
                printf("complete forward\r\n");
                fflush(stdout);
                servo_pwm_list[1] += 50;
                servo_pwm_list[2] += 100;
                mv = 1, action = 0;
                break;
            case 's':
                wsc--;
                printf("complete backward\r\n");
                fflush(stdout);
                servo_pwm_list[1] -= 50;
                servo_pwm_list[2] -= 100;
                mv = 1, action = 0;
                break;
            case 'a':
                printf("complete left\r\n");
                fflush(stdout);
                servo_pwm_list[0] += 100;
                mv = 1, action = 0;
                break;
            case 'd':
                printf("complete right\r\n");
                fflush(stdout);
                servo_pwm_list[0] -= 100;
                mv = 1, action = 0;
                break;
            default:
                printf("nothing done\r\n");
                fflush(stdout);
                mv = 1, action = 0;
                break;
        }
        robot_control_cmd(&huart2, 6, 1000, servo_list, servo_pwm_list);
    } else if (getBall == 1) {  // 4
        printf("完成take up the ball\r\n");
        fflush(stdout);

        servo_pwm_list[1] -= 100 + ((wsc > 0) ? (wsc * -50) : ((int)(wsc / 2) * 50));
        //- (wc) * 50 - sc * 25;
        servo_pwm_list[2] = (servo_pwm_list[2] - 300) < 400 ? 400 : (servo_pwm_list[2] - 300);
        servo_pwm_list[3] += 350 + ((wsc > 0) ? (wsc * -50) : (wsc * 100));
        // -(wc)*50 - (sc)*100;
        robot_control_cmd(&huart2, 6, 1000, servo_list, servo_pwm_list);

        servo_pwm_list[5] = 1700;
        robot_control_cmd(&huart2, 6, 1000, servo_list, servo_pwm_list);

        HAL_Delay(1000);
        throwInit = 1, getBall = 0;
    } else if (throwInit == 1) {                          // 5
        printf("完成move the ball to 投球預備位置\r\n");  // print 每個字有100 delay
        fflush(stdout);

        servo_pwm_list[0] = 1500;
        servo_pwm_list[1] = 1700;
        servo_pwm_list[2] = 2000;
        servo_pwm_list[3] = 1500;
        servo_pwm_list[4] = 2300;
        servo_pwm_list[5] = 2500;
        robot_control_cmd(&huart2, 6, 500, servo_list, servo_pwm_list);

        throwBall = 1, throwInit = 0;
    } else if (throwBall == 1) {  // 6
        servo_pwm_list[0] = 1500;
        servo_pwm_list[1] = 1400;
        servo_pwm_list[2] = 2000;
        servo_pwm_list[3] = 2100;
        servo_pwm_list[4] = 2500;
        servo_pwm_list[5] = 950;
        robot_control_cmd(&huart2, 6, 200, servo_list, servo_pwm_list);

        printf("完成throw the ball\r\n");
        fflush(stdout);
        x0 = 1, throwBall = 0;
    }
}
/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1) {
    }
    /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line) {
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
