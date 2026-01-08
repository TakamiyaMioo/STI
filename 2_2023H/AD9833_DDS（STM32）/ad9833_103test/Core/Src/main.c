/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "ad9833.h"
#include <stdbool.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
int fputc(int ch, FILE *f)
 
{
 
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xffff);
 
  return ch;
 
}

int fgetc(FILE *f)
{
  uint8_t ch = 0;
  HAL_UART_Receive(&huart1, &ch, 1, 0xffff);
  return ch;
}



/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint32_t rx_data[10];
uint32_t buf[1];
int num = 0;
int flag = 0;
int i=0;
int test=1;
uint32_t freq;

uint8_t RxBuffer[8];
uint8_t fre0=0;//通道0的频率
uint8_t fre1=0;//通道1的频率

uint8_t mod0=0;//通道0的波形
uint8_t mod1=0;//通道0的波形
// 0是正弦 1是三角
 bool freflag=0;
 double frearr[2]={10.73,10.74};
 double freletter;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
//	if(huart==&huart1){
//		if(RxBuffer[0]==0xff&&RxBuffer[1]==0xff&&RxBuffer[2]==0xff&&RxBuffer[3]==0xff){
//			//AD9833_WaveSeting(2000.0,0,SIN_WAVE,0,0);
//			fre0=RxBuffer[4]*1000;
//			fre1=RxBuffer[5]*1000;
//			mod0=RxBuffer[6];
//			mod1=RxBuffer[7];
//			//通道0输出
//			if(mod0==0)AD9833_WaveSeting(fre0,0,SIN_WAVE,0,0);//正弦
//			if(mod0==1)AD9833_WaveSeting(fre0,0,TRI_WAVE,0,0);//三角

//			//通道1输出
//			if(mod1==0)AD9833_WaveSeting(fre1,0,SIN_WAVE,0,1);//正弦
//			if(mod1==1)AD9833_WaveSeting(fre1,0,TRI_WAVE,0,1);//三角			
//		}
//	}
//}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if(htim==&htim4){
		freflag=!freflag;
		freletter=frearr[freflag];
	}
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
  MX_TIM1_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
	HAL_UART_Receive_IT(&huart1,RxBuffer,8);
	///////////////////////////////////////////////////////////////////////
	AD9833_WaveSeting_Double(1000, 0, SIN_WAVE, 0);

	 
	   freletter=10.73;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
      
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
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL8;
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

//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
//    //printf("num = %d", num);
//    //rx_data[num++] = buf[0];
//    //num = num%10;
//    /*
//    for(i=0;i<10;i++){
//        printf("data[%d] = %d", i, rx_data[i]);
//    }
//    */
//    /*
//    printf("recieve num0 %d\r\n",rx_data[0] );
//    printf("recieve num1 %d\r\n",rx_data[1] );
//    printf("recieve num2 %d\r\n",rx_data[2] );*/
//    /*
//    if(num>0){
//        HAL_UART_Receive_IT(&huart1, (uint8_t *)buf, 1);
//        //HAL_UART_Receive_IT(&huart1, (uint8_t *)rx_data, 10);
//    }
//    else {
//        flag = 1;
//    }*/
//}


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
