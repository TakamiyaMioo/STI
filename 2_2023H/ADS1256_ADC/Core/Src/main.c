/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usart.h"
#include "gpio.h"
#include "ADS1256.h"
#include "delay.h"
#include "oled.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h> //用到了printf函数
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* save data from UART receive interrupt*/
#define UART1_REC_LEN 200 //定义最大接收字节数 200
uint8_t UART1_RX_Buffer[UART1_REC_LEN];//接受缓存 最大为UART1_REC_LEN个字节
uint16_t UART1_RX_STA=0;// 接收状态标记
//接收状态
//bit15置1 0x8000 接收到0x0a 即\n转移符 表示接收完成
//bit14置1 0x4000 接收到0x0d 即\r转义符
//bit13~0 表示可以接受到的数据个数最大为 2^13

uint8_t Res=0;//保存接受到的1BYTE数据
uint32_t data_in0;
uint32_t data_in1;
uint32_t data_in2;
uint32_t data_in3;
uint32_t data_in4;
uint32_t data_in5;
uint32_t data_in6;
uint32_t data_in7;
double v0;
double v1;
double v2;
double v3;
double v4;
double v5;
double v6;
double v7;

/*printf函数重定义，使用用UART1打印到上位机*/

int fputc(int ch, FILE *f)
{      
	unsigned char temp[1]={ch};
	HAL_UART_Transmit(&huart1,temp,1,0xffff);//循环发送,直到发送完毕   
	return ch;
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
	HAL_UART_Receive_IT(&huart1, &Res, 1);//将Res作为UART中断接收数据的变量
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin,GPIO_PIN_RESET);
	Delay_Init();
	
//	OLED_Init();
//	OLED_DisPlay_On();
//	OLED_ShowNum(20,20,852,4,12,1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
			if(UART1_RX_STA & 0x8000)//判断UART接收是否完成
			{
				printf("Get data:");
				HAL_UART_Transmit(&huart1,UART1_RX_Buffer,UART1_RX_STA&0x3fff, 0xffff);//正常发送，非中断发送
				while(huart1.gState != HAL_UART_STATE_READY);//等待发送完成MCU->上位机
				printf("\r\n");
				UART1_RX_STA=0;
			}
	HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
	printf("test\r\n");
	ADS1256_Init(ADS1256_GAIN_1, ADS1256_DRATE_30000SPS);
	Delay_ms(100);
	data_in0=ADS1256ReadData(ADS1256_MUXP_AIN0, ADS1256_MUXN_AINCOM);
	Delay_ms(100);
	data_in1=ADS1256ReadData(ADS1256_MUXP_AIN1, ADS1256_MUXN_AINCOM);
	Delay_ms(100);
	data_in2=ADS1256ReadData(ADS1256_MUXP_AIN2, ADS1256_MUXN_AINCOM);
	Delay_ms(100);
	data_in3=ADS1256ReadData(ADS1256_MUXP_AIN3, ADS1256_MUXN_AINCOM);
	Delay_ms(100);
	data_in4=ADS1256ReadData(ADS1256_MUXP_AIN4, ADS1256_MUXN_AINCOM);
	Delay_ms(100);
	data_in5=ADS1256ReadData(ADS1256_MUXP_AIN5, ADS1256_MUXN_AINCOM);
	Delay_ms(100);
	data_in6=ADS1256ReadData(ADS1256_MUXP_AIN6, ADS1256_MUXN_AINCOM);
	Delay_ms(100);
	data_in7=ADS1256ReadData(ADS1256_MUXP_AIN7, ADS1256_MUXN_AINCOM);
	Delay_ms(100);
	v0=ADS1256GetVolt(ADS1256_MUXP_AIN0, ADS1256_MUXN_AINCOM, ADS1256_GAIN_1);
	Delay_ms(100);
	v1=ADS1256GetVolt(ADS1256_MUXP_AIN1, ADS1256_MUXN_AINCOM, ADS1256_GAIN_1);
	Delay_ms(100);
	v2=ADS1256GetVolt(ADS1256_MUXP_AIN2, ADS1256_MUXN_AINCOM, ADS1256_GAIN_1);
	Delay_ms(100);
	v3=ADS1256GetVolt(ADS1256_MUXP_AIN3, ADS1256_MUXN_AINCOM, ADS1256_GAIN_1);
	Delay_ms(100);
	v4=ADS1256GetVolt(ADS1256_MUXP_AIN4, ADS1256_MUXN_AINCOM, ADS1256_GAIN_1);
	Delay_ms(100);
	v5=ADS1256GetVolt(ADS1256_MUXP_AIN5, ADS1256_MUXN_AINCOM, ADS1256_GAIN_1);
	Delay_ms(100);
	v6=ADS1256GetVolt(ADS1256_MUXP_AIN6, ADS1256_MUXN_AINCOM, ADS1256_GAIN_1);
	Delay_ms(100);
	v7=ADS1256GetVolt(ADS1256_MUXP_AIN7, ADS1256_MUXN_AINCOM, ADS1256_GAIN_1);
	printf("AIN0=%d",data_in0);
	printf("\r\n");
	printf("AIN1=%d",data_in1);
	printf("\r\n");
	printf("AIN2=%d",data_in2);
	printf("\r\n");
	printf("AIN3=%d",data_in3);
	printf("\r\n");
	printf("AIN4=%d",data_in4);
	printf("\r\n");
	printf("AIN5=%d",data_in5);
	printf("\r\n");
	printf("AIN6=%d",data_in6);
	printf("\r\n");
	printf("AIN7=%d",data_in7);
	printf("\r\n");
	printf("V0=%f",v0);
	printf("\r\n");
	printf("V1=%f",v1);
	printf("\r\n");
	printf("V2=%f",v2);
	printf("\r\n");
	printf("V3=%f",v3);
	printf("\r\n");
	printf("V4=%f",v4);
	printf("\r\n");
	printf("V5=%f",v5);
	printf("\r\n");
	printf("V6=%f",v6);
	printf("\r\n");
	printf("V7=%f",v7);
	printf("\r\n");
	printf("\r\n");
	Delay_ms(1500); 
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)//自定义回调函数 在UART_Receive_IT()中调用
{
	//判断是哪个串口触发的中断  huart1.Instance = USART1;定义在MX_USART1_UART_Init中
	if(huart==&huart1)//huart ->Instance == USART1两种判断条件等价
	{
		if((UART1_RX_STA & 0x8000)==0)//接收未完成&位运算符 &&短路与判断
		{
			if(UART1_RX_STA & 0x4000)//接收到 \r
			{
				if(Res==0x0a)//下一个必须是接收 \n
					UART1_RX_STA|=0x8000;
				else
					UART1_RX_STA=0;
			}
			else //未接收到\r
			{
				if(Res==0x0d)//Receive \r
				{
					UART1_RX_STA|=0x4000;
				}
				else
				{
					UART1_RX_Buffer[UART1_RX_STA&0X3FFF]=Res;
					UART1_RX_STA++;
					if(UART1_RX_STA>UART1_REC_LEN-1) UART1_RX_STA=0;//如果接收数据大于200Byte 重新开始接收
				}
			}
		}
		HAL_UART_Receive_IT(&huart1, &Res, 1);//完成一次接受，再此开启中断
	}
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
