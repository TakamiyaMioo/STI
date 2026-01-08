/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "adc.h"
#include "dma.h"
#include "memorymap.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

#include "arm_math.h"
#include "arm_const_structs.h"
#include "xFFT.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
ALIGN_32BYTES(uint16_t adc1_data[FFT_LENGTH])  __attribute__((section(".ARM.__at_0x30000000")));
__IO uint8_t AdcConvEnd=0;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
// double voltage;
//0是正弦波，1是三角波
int Wavemax1;//最大频点频率值1
int WaveS1;//波形1种类
int Wavemax2;//最大频点频率值2
int WaveS2;//波形2种类
//0是不同频，1是同频
int frediffornot;//同频与否

int waveptarr[21][2]={{0,0},{11,5},{23,10},{34,15},{46,20},{57,25},{68,30},{80,35},{91,40},{102,45},{114,50},{125,55},{137,60},{148,65},{159,70},{171,75},{182,80},{194,85},{205,90},{216,95},{228,100}};//标准频点
int boxlength=21;

double Forcal[FFT_LENGTH/2];//用于计算
double calinfw[FFT_LENGTH/2];//用于确定是否是同频信号

// int maxindex1;//波形频点角标1
// int maxindex2;//波形频点角标2
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

  void TwoOrOne(){//同频还是不同频
    for(int i=0;i<FFT_LENGTH/2;i++){
      calinfw[i]=Forcal[i];
    }
    double maxx=0.0;//最大值
    int maxxindex;//最大值的角标
    for(int k=6;k<FFT_LENGTH/2;k++){
      if(maxx<calinfw[k]){
        maxx=calinfw[k];
        maxxindex=k;
      }
    }//先找到第一个很大的值，是一个频点
    for(int i=maxxindex-5;i<maxxindex+5;i++){/////////////////////////
      calinfw[i]=0;
    }//把这一部分频率滤掉
    maxx=0.0;
    for(int k=6;k<FFT_LENGTH/2;k++){
      if(maxx<calinfw[k]){
        maxx=calinfw[k];
        maxxindex=k;
      }
    }
    if(maxx>200){//最大值大于两百，认为还有一个频点，也就是两个信号不同频/////////////////////////
      frediffornot=0;//不同频
    }else{
      frediffornot=1;//同频
    }
  }

int FindPoint(int thatindex){//找到最大频率的点的角标，这个点离哪个标准频点更近
  //计算和标准频点的频谱值的差值，找到差值最小的点，那个点是所求的频率的附近的点
  int mindiff=1e5;//最小差
  int diff;//差值
  int thatfre;//找到那个频率点
  for(int i=1;i<boxlength;i++){
    diff=abs(waveptarr[i][0]-thatindex);
    if(diff<mindiff){
      mindiff=diff;
      thatfre=waveptarr[i][1];
    }
  }
  return thatfre;
}

int Triornot(int Wavevalue){//判断是否为三角波
  //找到标准频率的角标，看该频率的三倍和五倍频的频谱值是否同时大于1/9和1/25
  int index=0;//标准频点角标
  for(int i=1;i<boxlength;i++){
    if(Wavevalue==waveptarr[i][1]){
      index=waveptarr[i][0];
      break;
    }
  }
  int index3=index*3;//三次谐波标准值,要出现大于60的数//////////////////////////
  int index5=index*5;//五次谐波标准值，要出现大于13的数/////////////////////////
  bool in3j=false,in5j=false;//判断3次、5次谐波是否满足
  for(int i=index3-5;i<index3+5;i++){/////////////////////////
    if(Forcal[i]>60){///////////////////////////////////
      in3j=true;
      break;
    }
  }
  for(int i=index5-5;i<index5+5;i++){////////////////////////
    if(Forcal[i]>13){///////////////////////////////////
      in5j=true;
      break;
    }
  }
  if(in3j&&in5j){
    return 1;//三角波
  }else{
    return 0;//正弦波
  }
}

void FindWave(){
  //q1[i].real
  double max1=0.0;//第一个最大值
  double max2=0.0;//第二个最大值
  int index=0;

  TwoOrOne();

  if(frediffornot==0){//两个波频率不一样
    for(int k=6;k<FFT_LENGTH/2;k++){
      if(Forcal[k]>max1){
        max1=Forcal[k];//找到最大的那个点的值
        index=k;//找到最大的那个点的角标
      }
    }

    Wavemax1=FindPoint(index);//多少k
    WaveS1=Triornot(Wavemax1);//三角波还是正弦波

    for(int i=1;i<boxlength;i++){
      if(Wavemax1==waveptarr[i][1]){
        index=waveptarr[i][0];
        break;
      }
    }//找到这个频点对应的index

    for(int i=index-5;i<index+5;i++){//////////////////////////
      Forcal[i]=62;//////////////////////////////////////////////
    }

    for(int k=6;k<FFT_LENGTH/2;k++){
      if(Forcal[k]>max2){
        max2=Forcal[k];//找到最大的那个点的值
        index=k;//找到最大的那个点的角标
      }
    }

    Wavemax2=FindPoint(index);//多少k
    WaveS2=Triornot(Wavemax2);//三角波还是正弦波

  }else{
    for(int k=6;k<FFT_LENGTH/2;k++){
      if(Forcal[k]>max1){
        max1=Forcal[k];//找到最大的那个点的值
        index=k;//找到最大的那个点的角标
      }
    }

    Wavemax1=FindPoint(index);//多少k
    Wavemax2=Wavemax1;//同频

    for(int i=1;i<boxlength;i++){
      if(Wavemax1==waveptarr[i][1]){
        index=waveptarr[i][0];
        break;
      }
    }

    int index3=index*3;//三次谐波标准值，要出现大于60的数，才认为是两个三角波1；若只是大于10，则认为一三角一正弦2//////////////////////////
    int index5=index*5;//五次谐波标准值，要出现大于40的数，才认为是两个三角波1；若只是大于10，则认为一三角一正弦2/////////////////////////
    int in3j=0,in5j=0;//判断3次、5次谐波是否满足

    for(int i=index3-5;i<index3+5;i++){/////////////////////////
      if(Forcal[i]>10){///////////////////////////////////
        in3j=2;
        if(Forcal[i]>70){///////////////////////////////
          in3j=1;
          break;
        }
      }
    }
    for(int i=index5-5;i<index5+5;i++){////////////////////////
      if(Forcal[i]>10){///////////////////////////////////
        in5j=2;
        if(Forcal[i]>40){//////////////////////////////
          in5j=1;
          break;
        }
      }
    }

    if(in3j==0&&in5j==0){
      WaveS1=0;
      WaveS2=0;//两正弦
    }else if(in3j==1&&in5j==1){
      WaveS1=1;
      WaveS2=1;//两个三角波
    }else{
      WaveS1=1;
      WaveS2=0;//一三角一正弦
    }

  } 
  
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
  if(htim==&htim6){
    printf("Tim6 Triggered\r\n");
    HAL_TIM_Base_Stop(&htim6);
    __HAL_TIM_SET_COUNTER(&htim6,0);
    __HAL_TIM_SET_COUNTER(&htim15,0);
    HAL_TIM_Base_Start(&htim15);//打开定时器15
    HAL_ADCEx_Calibration_Start(&hadc1, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED);//校准ADC
    HAL_ADC_Start_DMA(&hadc1,(uint32_t*)adc1_data,FFT_LENGTH);//开始转换
  }
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc){
  if(hadc==&hadc1){
    HAL_ADC_Stop_DMA(&hadc1);
    HAL_TIM_Base_Stop(&htim15);//结束转换
    printf("In ADC1\r\n");
//    for(int i=0;i<FFT_LENGTH;i++){
//      voltage=adc1_data[i]*3.3/65535.0;
//      printf("%.6f\r\n",voltage);
//    }
		AdcConvEnd=1;//标志位置位，其他代码放在while(1)中
    
//    FFT_DIS();
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

  /* MPU Configuration--------------------------------------------------------*/
  MPU_Config();

  /* Enable the CPU Cache */

  /* Enable I-Cache---------------------------------------------------------*/
  SCB_EnableICache();

  /* Enable D-Cache---------------------------------------------------------*/
  SCB_EnableDCache();

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
  MX_DMA_Init();
  MX_TIM7_Init();
  MX_USART1_UART_Init();
  MX_UART7_Init();
  MX_ADC1_Init();
  MX_TIM15_Init();
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */

	
	HAL_TIM_Base_Start_IT(&htim6);
  HAL_TIM_Base_Start_IT(&htim15);
  HAL_TIM_Base_Stop(&htim6);//采样前延时
  HAL_TIM_Base_Stop(&htim15);//采样间隔

  //调试用
  //printf("In Main\r\n");
  __HAL_TIM_SET_COUNTER(&htim6,0);
  HAL_TIM_Base_Start(&htim6);//开启定时器

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    if(AdcConvEnd==1){
      AdcConvEnd=0;
      FFT_Init();
      // FFT_DIS();
      //q1[i].real
      for(int i=0;i<FFT_LENGTH/2;i++){
        Forcal[i]=q1[i].real;
      }
      FindWave();//找到这两个信号
      printf("Wave1fre: %d\r\n",Wavemax1);
      printf("Wave2fre: %d\r\n",Wavemax2);
      printf("Wave1sss: %d\r\n",WaveS1);
      printf("Wave2sss: %d\r\n",WaveS2);
    }
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

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

 /* MPU Configuration */

void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x24000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_512KB;
  MPU_InitStruct.SubRegionDisable = 0x0;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL1;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Number = MPU_REGION_NUMBER1;
  MPU_InitStruct.BaseAddress = 0x30000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_128KB;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Number = MPU_REGION_NUMBER2;
  MPU_InitStruct.BaseAddress = 0x30020000;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Number = MPU_REGION_NUMBER3;
  MPU_InitStruct.BaseAddress = 0x30040000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_32KB;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Number = MPU_REGION_NUMBER4;
  MPU_InitStruct.BaseAddress = 0x38000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_64KB;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

}

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
