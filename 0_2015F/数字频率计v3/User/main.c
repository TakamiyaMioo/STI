
#include "stm32f10x.h"
#include "./lcd/bsp_ili9341_lcd.h"
#include "./usart/bsp_usart.h" 
#include "./led/bsp_led.h"
#include "bsp_SysTick.h"
#include <stdio.h>

void Printf_Charater(void)   ;

extern uint8_t rx_buf[20];
extern uint16_t num;
extern uint8_t receive_flg;

uint32_t dutyCnt;
uint32_t delay_cnt;
uint8_t  stage;
uint32_t fxCnt;
uint32_t fbaseCnt;

float  frequency;  //频率
float  period   ;  //周期
float  duty      ;  // 占空比
float  time;     //时间间隔
float  time_start;     //时间间隔
float  time_end;     //时间间隔
/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main ( void )
{
    static char dispBuff[50];
  SysTick_Init();
	ILI9341_Init ();         //LCD 初始化
	USART_Config();		
	LED_GPIO_Config();
	Meter_Config ();
	
   //其中 6 模式为大部分液晶例程的默认显示方向  
   ILI9341_GramScan ( 6 );	
   LCD_SetFont(&Font8x16); //设置字体
   ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	/* 清屏，显示全黑 */
   LCD_SetColors(RED,WHITE);//设置前景色背景色
   ILI9341_DispStringLine_EN(LINE(2),"   Digital Frequency Meter   ");
   LCD_SetColors(BLUE,WHITE);//设置前景色背景色
   ILI9341_DispStringLine_EN(LINE(4),"  Frequency :  xxx.xx");
   ILI9341_DispStringLine_EN(LINE(7),"  Period    :  xxx.xx");
   ILI9341_DispStringLine_EN(LINE(10),"  Duty     : xxx.xx");
   ILI9341_DispStringLine_EN(LINE(13),"  Time     : xxx.xx");
	

	
	while ( 1 )
	{    
		if(receive_flg==1)
			{
			LED2_ON;
			num=0;
			receive_flg=0 ;
			Delay_ms(50);

			stage = rx_buf[1];
			dutyCnt=   (rx_buf[2] << 24)+(rx_buf[3] << 16)+(rx_buf[4] << 8)+(rx_buf[5]  << 0);
			delay_cnt =(rx_buf[6] << 24)+(rx_buf[7] << 16)+(rx_buf[8] << 8)+(rx_buf[9]  << 0);
			fxCnt  =  (rx_buf[10] << 24)+(rx_buf[11]<< 16)+(rx_buf[12]<< 8)+(rx_buf[13] << 0);
			fbaseCnt =(rx_buf[14] << 24)+(rx_buf[15]<< 16)+(rx_buf[16]<< 8)+(rx_buf[17] << 0);
		
			frequency=200000000*((float)fxCnt / (float)fbaseCnt);//200M
			period =  1 / frequency;//显示单位微妙
			duty   = 100* (float)dutyCnt / (float)fbaseCnt;
			switch(stage>>4){//高8位
				case(0x00): time_start=0;
				case(0x01): time_start=0.005*0.25;
				case(0x11): time_start=0.005*0.5;
				case(0x10): time_start=0.005*0.75;
				default   : time_start =0;
			}
			switch((stage&0x0f)){//低8位
				case(0x10): time_end=0;
				case(0x11): time_end=-0.005*0.25;
				case(0x01): time_end=-0.005*0.5;
				case(0x00): time_end=-0.005*0.75;
				default   : time_end =-0;
			}				
			time   =   0.005 * (float)delay_cnt  +time_start +time_end;//单位是微妙
			
			//显示单元
			//ILI9341_Clear(0,60,LCD_X_LENGTH,LCD_Y_LENGTH-60);	/* 清屏，显示全黑 */
			//显示频率
			if(frequency<1000)
						{
						sprintf(dispBuff,"  Frequency : %.3f Hz",frequency);
						LCD_ClearLine(LINE(4));
						ILI9341_DispStringLine_EN(LINE(4),dispBuff);
						}
			else if(frequency<1000000)
						{
						sprintf(dispBuff,"  Frequency : %.3f kHz",frequency/1000);
						LCD_ClearLine(LINE(4));
						ILI9341_DispStringLine_EN(LINE(4),dispBuff);
						}
			else if(0<frequency<1000000000)
						{
						sprintf(dispBuff,"  Frequency : %.3f MHz",frequency/1000000);
						LCD_ClearLine(LINE(4));
						ILI9341_DispStringLine_EN(LINE(4),dispBuff);
						}
			//显示周期
	
			 if(period>0.001)
					{
					sprintf(dispBuff,"  Period : %.3f ms",period*1000);
					LCD_ClearLine(LINE(7));
					ILI9341_DispStringLine_EN(LINE(7),dispBuff);
					}
			else if(0.001>period>0.000001)
					{
					sprintf(dispBuff,"  Period : %.3f us",period*1000000);
					LCD_ClearLine(LINE(7));
					ILI9341_DispStringLine_EN(LINE(7),dispBuff);
					}				
			else //(0.000001>period)
					{
					sprintf(dispBuff,"  Period : %.3f ns",period*1000000000);
					LCD_ClearLine(LINE(7));
					ILI9341_DispStringLine_EN(LINE(7),dispBuff);
					}					
					
			//显示占空比
		    sprintf(dispBuff,"  Duty   : %.2f %%",duty);
			 LCD_ClearLine(LINE(10));
			ILI9341_DispStringLine_EN(LINE(10),dispBuff);		
			//显示占空比
		  sprintf(dispBuff,"  Time   : %.4f us ",time);
		  LCD_ClearLine(LINE(13));
			ILI9341_DispStringLine_EN(LINE(13),dispBuff);
			
			printf("\r\n  dutyCnt  = %d \r\n",dutyCnt);   
			printf("\r\n  delay_cnt= %x \r\n",delay_cnt); 
			printf("\r\n  fxCnt    = %x \r\n",fxCnt);    
			printf("\r\n  fbaseCnt = %x \r\n",fbaseCnt);  
			LED2_OFF;

			}
		else
		{
			Delay_ms(1200);
			Meter_OFF;
			Delay_ms(10);
			Meter_ON;
      Delay_ms(500);
		}


	 }
	
	
}




///*用于测试各种液晶的函数*/
//void LCD_Test(void)
//{
//	/*演示显示变量*/
//	static uint8_t testCNT = 0;	
//	char dispBuff[100];
//	
//	testCNT++;	
//	
//	LCD_SetFont(&Font8x16);
//	LCD_SetColors(RED,BLACK);

//  ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	/* 清屏，显示全黑 */
//	/********显示字符串示例*******/
//  ILI9341_DispStringLine_EN(LINE(0),"BH 3.2 inch LCD para:");
//  ILI9341_DispStringLine_EN(LINE(1),"Image resolution:240x320 px");
//  ILI9341_DispStringLine_EN(LINE(2),"ILI9341 LCD driver");
//  ILI9341_DispStringLine_EN(LINE(3),"XPT2046 Touch Pad driver");
//  
//	/********显示变量示例*******/
//	LCD_SetFont(&Font16x24);
//	LCD_SetTextColor(GREEN);

//	/*使用c标准库把变量转化成字符串*/
//	sprintf(dispBuff,"Count : %d ",testCNT);
//  LCD_ClearLine(LINE(4));	/* 清除单行文字 */
//	
//	/*然后显示该字符串即可，其它变量也是这样处理*/
//	ILI9341_DispStringLine_EN(LINE(4),dispBuff);

//	/*******显示图形示例******/
//	LCD_SetFont(&Font24x32);
//  /* 画直线 */
//  
//  LCD_ClearLine(LINE(4));/* 清除单行文字 */
//	LCD_SetTextColor(BLUE);

//  ILI9341_DispStringLine_EN(LINE(4),"Draw line:");
//  
//	LCD_SetTextColor(RED);
//  ILI9341_DrawLine(50,170,210,230);  
//  ILI9341_DrawLine(50,200,210,240);
//  
//	LCD_SetTextColor(GREEN);
//  ILI9341_DrawLine(100,170,200,230);  
//  ILI9341_DrawLine(200,200,220,240);
//	
//	LCD_SetTextColor(BLUE);
//  ILI9341_DrawLine(110,170,110,230);  
//  ILI9341_DrawLine(130,200,220,240);
//  
//  Delay(0xFFFFFF);
//  
//  ILI9341_Clear(0,16*8,LCD_X_LENGTH,LCD_Y_LENGTH-16*8);	/* 清屏，显示全黑 */
//  
//  
//  /*画矩形*/

//  LCD_ClearLine(LINE(4));	/* 清除单行文字 */
//	LCD_SetTextColor(BLUE);

//  ILI9341_DispStringLine_EN(LINE(4),"Draw Rect:");

//	LCD_SetTextColor(RED);
//  ILI9341_DrawRectangle(50,200,100,30,1);
//	
//	LCD_SetTextColor(GREEN);
//  ILI9341_DrawRectangle(160,200,20,40,0);
//	
//	LCD_SetTextColor(BLUE);
//  ILI9341_DrawRectangle(170,200,50,20,1);
//  
//  
//  Delay(0xFFFFFF);
//	
//	ILI9341_Clear(0,16*8,LCD_X_LENGTH,LCD_Y_LENGTH-16*8);	/* 清屏，显示全黑 */

//  /* 画圆 */
//  LCD_ClearLine(LINE(4));	/* 清除单行文字 */
//	LCD_SetTextColor(BLUE);
//	
//  ILI9341_DispStringLine_EN(LINE(4),"Draw Cir:");

//	LCD_SetTextColor(RED);
//  ILI9341_DrawCircle(100,200,20,0);
//	
//	LCD_SetTextColor(GREEN);
//  ILI9341_DrawCircle(100,200,10,1);
//	
//	LCD_SetTextColor(BLUE);
//	ILI9341_DrawCircle(140,200,20,0);

//  Delay(0xFFFFFF);
//  
//  ILI9341_Clear(0,16*8,LCD_X_LENGTH,LCD_Y_LENGTH-16*8);	/* 清屏，显示全黑 */

//}


///**
//  * @brief  简单延时函数
//  * @param  nCount ：延时计数值
//  * @retval 无
//  */	
//static void Delay ( __IO uint32_t nCount )
//{
//  for ( ; nCount != 0; nCount -- );
//	
//}

/* ------------------------------------------end of file---------------------------------------- */

