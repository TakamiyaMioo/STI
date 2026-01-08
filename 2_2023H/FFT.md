目录

一、H题要求

二、方案选择

三、具体方案

  1、FFT采样并判断频率波形

2、DDS输出

3、频率补偿

四、总结

一、H题要求




二、方案选择
  一开始我们是想要跟着一个国一方案完整做一遍的，但是做到一半发现用于反馈的锁相放大器模块实在太贵了，淘宝上要200多一块，还要用两个，实在是买不起，后来查了查，AD630只要60一块，其实我们可以自己做的（这也正是隔壁组的做法），也许是因为当时嘉立创用的不熟，我们下意识抛弃了这个做法，这是值得检讨的，我们错失了锻炼的机会。

  于是，为了省钱，我们参考山东大学队的测相位差变化的方法，自己做出了一套方案。

三、具体方案
  1、FFT采样并判断频率波形
          对采到的数据进行FFT变换后，首先观察它的模值，因为采样频率与数组长度的不同，频谱的密度也不一样，要求中信号的频率会以5kHZ为单位改变，所以密度不应该超过5KHZ一个点，同时要让所测的频率刚好落在点上。

          我们用的FFT是STM32的DSP库里的函数，但它只支持256个点及其倍数长度的变换，这就限制了采样频率的设置，好在最后还是配好了。
    
          值得一提的是采样的设置，我是用的ADC+DMA传输，用定时器触发采样，由于反馈的需要，三条ADC通道应同时采样，这就需要将它们的触发源设置为同一个定时器。在代码编写上，同步采样需要先写DMA的传输，然后再打开定时器。

以下是我参考的教程

https://blog.csdn.net/qq_34022877/article/details/124099296?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522171560456616800185822920%2522%252C%2522scm%2522%253A%252220140713.130102334..%2522%257D&request_id=171560456616800185822920&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~sobaiduend~default-1-124099296-null-null.142^v100^pc_search_result_base7&utm_term=%E5%90%8C%E6%AD%A5%E9%87%87%E6%A0%B7&spm=1018.2226.3001.4187

采样并判断波形

HAL_TIM_Base_Start(&htim2);
HAL_ADC_Start_DMA(&hadc1,(uint32_t *)&adc_buff1,256);

 int i,a;
		 a=0;
		 for( i=0;i<256;i++)
		 {
			adc_transport1[i] = (float32_t)(adc_buff1[i])*3.3/4095;
		 }
		 for(int j=0;j<256;j++)
		{
		adc_buffin1[j*2]=adc_transport1[j];
		}
		 arm_cfft_f32(&arm_cfft_sR_f32_len256,adc_buffin1,0,1);
		 arm_cmplx_mag_f32(adc_buffin1,adc_out,256);
		 for( i=1;i<128;i++)// ? ?  
		  {
			   if(2<adc_out[i])
			    {
				    f[a]=i;
						a++;
			    }
		   }		
			HAL_TIM_Base_Stop(&htim2);
//***************************************************/波形频率判断
			 a=0;
		for(i=0;i<10;i++)
    {
		 if(f[i]>0)
		 {a++;}
		}			 
		if(a==2)//两正弦
		{
		 FA=f[0]*5000;
			wavemode1=0;//正弦
			FB=f[1]*5000;
			wavemode2=0;
		}
		if(a==3)//正弦三角重合
		{
     FA=f[0]*5000;
     wavemode1=1;
		 wavemode2=0;
     if(adc_out[f[1]]>40)
		 {
		  FB=f[1]*5000;
		 }			 
		 else FB=f[2]*5000;
		}
		if(a==4)//一正弦一三角波未重合
		{
		 FA=f[0]*5000;
			if(f[1]==f[0]*3)
			{wavemode1=1;
       if(f[2]==f[0]*5)
			 {
			  FB=f[3]*5000;
				wavemode2=0;
			 }
       else{FB=f[2]*5000;
			 wavemode2=0;}			 
			}
			else
			{
			 FB=f[1]*5000;
				if(f[2]==f[0]*3)
				{
				 wavemode1=1;
				 wavemode2=0;
				}
				else
				{
				 wavemode1=0;
				 wavemode2=1;
				}
			}
		}
        if(a==5)//两三角波卡3倍频
		{
			FA=f[0]*5000;
			wavemode1=1;
			wavemode2=1;
			FB=FA*3;
		 
		
		}
		if(a==6)//两三角
		{
		 wavemode1=1;
		 wavemode2=1;
		 FA=f[0]*5000;
		 if(f[1]==f[0]*3) 
		 {
		  if(f[2]==f[0]*5) FB=f[3]*5000;
			else FB=f[2]*5000;
		 }
		 else FB=f[1]*5000;
			 
		}
AI写代码
cpp
运行

2、DDS输出
  我们采用的DDS是AD9833，在1MHZ下精度可以达到0.004HZ，但从50KHZ开始波形出现严重变形，让我们不得不加了两个滤波器。更值得注意的是，在对商家给的标注库驱动改到HAL库过程中，用于代替writedata函数的HAL库SPI写函数效果并不好，输入时好时坏，更是没有浮点数传输能力，让人难蚌。最后是用的引脚模拟SPI才成功（有没有大佬知道为什么）

以下是.C和.H文件


#include "ad9833.h"

/**************************************
*   函 数 名: ad9833_init
*   功能说明: ad9833初始化
*   形    参: 无
*   返 回 值: 无
  *************************************/
  void AD9833_Init(void)
  {
     GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6, GPIO_PIN_SET);
  /*Configure GPIO pins : PA4 PA5 PA6 */
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/**************************************
*   函 数 名: AD9833_Delay
*   功能说明: ad9833延迟
*   形    参: 无
*   返 回 值: 无
*************************************/
static void AD9833_Delay(void)
{
    uint16_t i;
    for (i = 0; i < 1; i++);
}
/**************************************
*   函 数 名: ad9833_write_data
*   功能说明: ad9833写入16位数据
*   形    参: txdata：待写入的16位数据
*   返 回 值: 无
*************************************/
void AD9833_WriteData(uint16_t txdata)
{
    int i;
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
    //写16位数据
    for(i=0;i<16;i++)
    {
        
        if (txdata & 0x8000)
           HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
        else
           HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
        AD9833_Delay();
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
        AD9833_Delay();
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
        txdata<<=1;
    }
   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
}
void AD9833_WriteData1(uint16_t txdata)
	{
    int i;
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
    //写16位数据
    for(i=0;i<16;i++)
    {
        
        if (txdata & 0x8000)
           HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
        else
           HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
        AD9833_Delay();
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
        AD9833_Delay();
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
        txdata<<=1;
    }
   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
}
/**************************************
*   函 数 名: AD9833_SetFrequency
*   功能说明: ad9833设置频率寄存器
*   形    参: reg：待写入的频率寄存器   
              fout：频率值
*   返 回 值: 无
*************************************/
void AD9833_SetFrequency(unsigned short reg, double fout)
{
    int frequence_LSB,frequence_MSB;
    double   frequence_mid,frequence_DATA;
    long int frequence_hex;

    /*********************************计算频率的16进制值***********************************/
    frequence_mid=268435456/1;//适合25M晶振
    //如果时钟频率不为25MHZ，修改该处的频率值，单位MHz ，AD9833最大支持25MHz
    frequence_DATA=fout;
    frequence_DATA=frequence_DATA/1000000;
    frequence_DATA=frequence_DATA*frequence_mid;
    frequence_hex=frequence_DATA;  //这个frequence_hex的值是32位的一个很大的数字，需要拆分成两个14位进行处理；
    frequence_LSB=frequence_hex; //frequence_hex低16位送给frequence_LSB
    frequence_LSB=frequence_LSB&0x3fff;//去除最高两位，16位数换去掉高位后变成了14位
    frequence_MSB=frequence_hex>>14; //frequence_hex高16位送给frequence_HSB
    frequence_MSB=frequence_MSB&0x3fff;//去除最高两位，16位数换去掉高位后变成了14位
    frequence_LSB=frequence_LSB|reg;
    frequence_MSB=frequence_MSB|reg;
    AD9833_WriteData(0x2100); //选择数据一次写入，B28位和RESET位为1
    AD9833_WriteData(frequence_LSB);
    AD9833_WriteData(frequence_MSB);
}
void AD9833_SetFrequency1(unsigned short reg, double fout)
{
    int frequence_LSB,frequence_MSB;
    double   frequence_mid,frequence_DATA;
    long int frequence_hex;

    /*********************************计算频率的16进制值***********************************/
    frequence_mid=268435456/1;//适合25M晶振
    //如果时钟频率不为25MHZ，修改该处的频率值，单位MHz ，AD9833最大支持25MHz
    frequence_DATA=fout;
    frequence_DATA=frequence_DATA/1000000;
    frequence_DATA=frequence_DATA*frequence_mid;
    frequence_hex=frequence_DATA;  //这个frequence_hex的值是32位的一个很大的数字，需要拆分成两个14位进行处理；
    frequence_LSB=frequence_hex; //frequence_hex低16位送给frequence_LSB
    frequence_LSB=frequence_LSB&0x3fff;//去除最高两位，16位数换去掉高位后变成了14位
    frequence_MSB=frequence_hex>>14; //frequence_hex高16位送给frequence_HSB
    frequence_MSB=frequence_MSB&0x3fff;//去除最高两位，16位数换去掉高位后变成了14位
    frequence_LSB=frequence_LSB|reg;
    frequence_MSB=frequence_MSB|reg;
    AD9833_WriteData1(0x2100); //选择数据一次写入，B28位和RESET位为1
    AD9833_WriteData1(frequence_LSB);
    AD9833_WriteData1(frequence_MSB);
}

/**************************************
*   函 数 名: ad9833_write_data
*   功能说明: ad9833设置相位寄存器
*   形    参: reg：待写入的相位寄存器   
              fout：相位值
*   返 回 值: 无
*************************************/
void AD9833_SetPhase(unsigned short reg, unsigned short val)
{
    unsigned short phase = reg;
    phase |= val;
    AD9833_WriteData(phase);
}
void AD9833_SetPhase1(unsigned short reg, unsigned short val)
{
    unsigned short phase = reg;
    phase |= val;
    AD9833_WriteData1(phase);
}
/**************************************
*   函 数 名: AD9833_SetWave
*   功能说明: ad9833设置波形
*   形    参: WaveMode：输出波形类型 
              Freq_SFR：输出的频率寄存器类型
              Phase_SFR：输出的相位寄存器类型
*   返 回 值: 无
*************************************/
void AD9833_SetWave(unsigned int WaveMode,unsigned int Freq_SFR,unsigned int Phase_SFR)
{
    unsigned int val = 0;
    val = (val | WaveMode | Freq_SFR | Phase_SFR);
    AD9833_WriteData(val);
}
void AD9833_SetWave1(unsigned int WaveMode,unsigned int Freq_SFR,unsigned int Phase_SFR)
{
    unsigned int val = 0;
    val = (val | WaveMode | Freq_SFR | Phase_SFR);
    AD9833_WriteData1(val);
}

/**************************************
*   函 数 名: AD9833_Setup
*   功能说明: ad9833设置输出
*   形    参: Freq_SFR：频率寄存器类型
              Freq    ：频率值
              Phase_SFR：相位寄存器类型
              Phase：相位值
              WaveMode：波形类型
*   返 回 值: 无
*************************************/
void AD9833_Setup(unsigned int Freq_SFR,double Freq,unsigned int Phase_SFR,unsigned int Phase,unsigned int WaveMode)
{
    unsigned int Fsel,Psel;
    AD9833_WriteData(0x0100); //复位AD9833,即RESET位为1
    AD9833_WriteData(0x2100); //选择数据一次写入，B28位和RESET位为1
    AD9833_SetFrequency(Freq_SFR,Freq);
    AD9833_SetPhase(Phase_SFR,Phase);
    if(Freq_SFR == AD9833_REG_FREQ0)
    {
        Fsel = AD9833_FSEL0;
    }
    else 
    {
        Fsel = AD9833_FSEL1;
    }
    if(Phase_SFR == AD9833_REG_PHASE0)
    {
        Psel = AD9833_PSEL0;
    }
    else 
    {
        Psel = AD9833_PSEL1;
    }

    AD9833_SetWave(WaveMode,Fsel,Psel);
}
void AD9833_Setup1(unsigned int Freq_SFR,double Freq,unsigned int Phase_SFR,unsigned int Phase,unsigned int WaveMode)
{
    unsigned int Fsel,Psel;
    AD9833_WriteData1(0x0100); //复位AD9833,即RESET位为1
    AD9833_WriteData1(0x2100); //选择数据一次写入，B28位和RESET位为1
    AD9833_SetFrequency1(Freq_SFR,Freq);
    AD9833_SetPhase1(Phase_SFR,Phase);
    if(Freq_SFR == AD9833_REG_FREQ0)
    {
        Fsel = AD9833_FSEL0;
    }
    else 
    {
        Fsel = AD9833_FSEL1;
    }
    if(Phase_SFR == AD9833_REG_PHASE0)
    {
        Psel = AD9833_PSEL0;
    }
    else 
    {
        Psel = AD9833_PSEL1;
    }

    AD9833_SetWave1(WaveMode,Fsel,Psel);
}
AI写代码
cpp
运行

#ifndef _AD9833_H
#define _AD9833_H
#include "main.h"
//AD9833 GPIO
#define AD9833_SDATA  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET)  //SDATA  PB6
#define AD9833_SCLK   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET)  //SCLK   PB5
#define AD9833_FSYNC  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET)  //FSYNC  PB4

/* WaveMode */
#define AD9833_OUT_SINUS    ((0 << 5) | (0 << 1) | (0 << 3))
#define AD9833_OUT_TRIANGLE ((0 << 5) | (1 << 1) | (0 << 3))
#define AD9833_OUT_MSB      ((1 << 5) | (0 << 1) | (1 << 3))
#define AD9833_OUT_MSB2     ((1 << 5) | (0 << 1) | (0 << 3))

/* Registers */

#define AD9833_REG_CMD      (0 << 14)
#define AD9833_REG_FREQ0    (1 << 14)
#define AD9833_REG_FREQ1    (2 << 14)
#define AD9833_REG_PHASE0   (6 << 13)
#define AD9833_REG_PHASE1   (7 << 13)

/* Command Control Bits */

#define AD9833_B28          (1 << 13)
#define AD9833_HLB          (1 << 12)
#define AD9833_FSEL0        (0 << 11)
#define AD9833_FSEL1        (1 << 11)
#define AD9833_PSEL0        (0 << 10)
#define AD9833_PSEL1        (1 << 10)
#define AD9833_PIN_SW       (1 << 9)
#define AD9833_RESET        (1 << 8)
#define AD9833_CLEAR_RESET  (0 << 8)
#define AD9833_SLEEP1       (1 << 7)
#define AD9833_SLEEP12      (1 << 6)
#define AD9833_OPBITEN      (1 << 5)
#define AD9833_SIGN_PIB     (1 << 4)
#define AD9833_DIV2         (1 << 3)
#define AD9833_MODE         (1 << 1)

void AD9833_Init(void);
static void AD9833_Delay(void);
void AD9833_WriteData(uint16_t txdata);
void AD9833_SetFrequency(unsigned short reg, double fout);
void AD9833_SetPhase(unsigned short reg, unsigned short val);
void AD9833_SetWave(unsigned int WaveMode,unsigned int Freq_SFR,unsigned int Phase_SFR);
void AD9833_Setup(unsigned int Freq_SFR,double Freq,unsigned int Phase_SFR,unsigned int Phase,unsigned int WaveMode);
void AD9833_WriteData1(uint16_t txdata);
void AD9833_SetFrequency1(unsigned short reg, double fout);
void AD9833_SetPhase1(unsigned short reg, unsigned short val);
void AD9833_SetWave1(unsigned int WaveMode,unsigned int Freq_SFR,unsigned int Phase_SFR);
void AD9833_Setup1(unsigned int Freq_SFR,double Freq,unsigned int Phase_SFR,unsigned int Phase,unsigned int WaveMode);
#endif
AI写代码
cpp
运行

3、频率补偿
  因为DDS和信号源使用的晶振不同，难免有一定的频率差。对于两频率相近的信号，它们的不同时刻相位差的差表示为【频率差】*【时间差】，所以，只要延时预定的时间分别对原信号和重建信号同时采样，再使用FFT获取相位信息，可以算出频率差。

以下是代码

float xiangweicha(int f)//     ?     ?  
{ 
	 float hudu1,jiaodu1,hudu2,jiaodu2,jiaoducha1,jiaoducha2;


   HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_buff1, 256);//adc 
	 HAL_ADC_Start_DMA(&hadc2, (uint32_t *)adc_buff2, 256);
	 
	 HAL_TIM_Base_Start(&htim2);
	 HAL_Delay(500);
	 HAL_TIM_Base_Stop(&htim2);
	 HAL_Delay(500);
	 HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_buff3, 256);//adc 
	 HAL_ADC_Start_DMA(&hadc2, (uint32_t *)adc_buff4, 256);
	
	 HAL_TIM_Base_Start(&htim2);
	 HAL_Delay(50);
	 HAL_TIM_Base_Stop(&htim2);
	
	//*****************************对第一次信号求相位差
		 for( int i=0;i<256;i++)
		 {
			adc_transport1[i] = (float32_t)(adc_buff1[i])*3.3/4095;
			 adc_transport2[i] = (float32_t)(adc_buff2[i])*3.3/4095;
		 }
		  for(int j=0;j<256*2;j++)
		{
		adc_buffin1[j]=0;
			adc_buffin2[j]=0;
		}
		 
		 for(int j=0;j<256;j++)
		{
		adc_buffin1[j*2]=adc_transport1[j];
			adc_buffin2[j*2]=adc_transport2[j];
		}
		 arm_cfft_f32(&arm_cfft_sR_f32_len256,adc_buffin1,0,1);
		arm_cfft_f32(&arm_cfft_sR_f32_len256,adc_buffin2,0,1);
		
		hudu1 = atan2(adc_buffin1[(f/5000)*2+1] ,adc_buffin1[(f/5000)*2]);//    ? ??  ?     ???    
	jiaodu1 = hudu1 * 180.0 / 3.1415926;
		hudu2 = atan2(adc_buffin2[(f/5000)*2+1] ,adc_buffin2[(f/5000)*2]);//    ? ??  ?     ???    
	jiaodu2 = hudu2 * 180.0 / 3.1415926;
		jiaoducha1=jiaodu2-jiaodu1;
		//*************************对第二次信号求相位差
		for( int i=0;i<256;i++)
		 {
			adc_transport1[i] = (float32_t)(adc_buff3[i])*3.3/4095;
			 adc_transport2[i] = (float32_t)(adc_buff4[i])*3.3/4095;
		 }
		  for(int j=0;j<256*2;j++)
		{
		adc_buffin1[j]=0;
			adc_buffin2[j]=0;
		}
		 
		 for(int j=0;j<256;j++)
		{
		adc_buffin1[j*2]=adc_transport1[j];
			adc_buffin2[j*2]=adc_transport2[j];
		}
		 arm_cfft_f32(&arm_cfft_sR_f32_len256,adc_buffin1,0,1);
		arm_cfft_f32(&arm_cfft_sR_f32_len256,adc_buffin2,0,1);
		
		hudu1 = atan2(adc_buffin1[(f/5000)*2+1] ,adc_buffin1[(f/5000)*2]);//    ? ??  ?     ???    
	jiaodu1 = hudu1 * 180.0 / 3.1415926;
		hudu2 = atan2(adc_buffin2[(f/5000)*2+1] ,adc_buffin2[(f/5000)*2]);//    ? ??  ?     ???    
	jiaodu2 = hudu2 * 180.0 / 3.1415926;		
		jiaoducha2=jiaodu2-jiaodu1;
		if(jiaoducha1>180)jiaoducha1-=360;
		if(jiaoducha1<180)jiaoducha1+=360;
		if(jiaoducha2>180)jiaoducha2-=360;
		if(jiaoducha2<180)jiaoducha2+=360;
		
	  jiaoducha1=(jiaoducha1-jiaoducha2)/(360);
	
	return jiaoducha1;
}
AI写代码
cpp
运行

这是本方案的核心，至此，所需的硬件只有加法器、单片机、两个DDS、两个滤波器，可以说是非常简单的方案，但效果还可以，除了设定相位差因为时间不够没有做，其他要求都可以满足。

四、总结


  这篇博客用来给这段时间的工作进行总结，这是我们组第一次做电赛题，虽然耗时长了点，但总算做出了点东西，同时，我们也暴露出不少问题，在这里就不一一叙述。就在我写这篇博客的明天，我们就要验收，然后去做第二道题，希望下一次可以实现全要求。
————————————————

                            版权声明：本文为博主原创文章，遵循 CC 4.0 BY-SA 版权协议，转载请附上原文出处链接和本声明。

原文链接：https://blog.csdn.net/m0_74966966/article/details/137696949