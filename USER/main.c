#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"
#include "fdc2214.h"
#include "timer.h"
#include "exti.h"
#include "beep.h"
#include "math.h"
 
/************************************************
 ALIENTEK精英STM32开发板实验13
 TFTLCD显示实验  
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/

#define Threshold1 150						//阀值1，
#define Threshold2 500						//阀值2,有手放
#define Threshold3 70						//阀值2
#define Threshold4 100						//阀值2
#define Threshold5 250						//阀值2

void Sys_Init(void);
float compute(void);

float res0, res1, res2, res3;				//传感器采值
float temp0, temp1, temp2, temp3;			//传感器初始值
float D_res0, D_res1, D_res2, D_res3;		//传感器差值
u8 decide_flag = 3, mode_flag = 0, decide_flag_l = 0, collect_flag = 0;//判定标志位；模式标志位,次数标志位，采集标志位
u8 n = 20, i = 0, j = 0, f = 0, train_flag, count = 0;

float collect[20] = {0}, collect_fist[9], collect_finger[15], fist[3] = {0.050, 0.680, 1.500}, finger[5] = {0.350, 0.580, 0.850, 1.050, 1.440};
//采集20个数--采9/15个平均值--石头，剪刀，布的平均值--1，2，3，4，5的平均值
      

int main(void)
{	
	
	u8 k = 0, l = 0;
	collect[19] = 0;
	//u8 lcd_id[12];			//存放LCD ID字符串
	delay_init();	    	 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(115200);	 	//串口初始化为115200
 	LED_Init();			     //LED端口初始化
	LCD_Init();
	BEEP_Init();
	Sys_Init();				//传感器初始化
	EXTIX_Init();         	//初始化外部中断输入 
	POINT_COLOR=BLACK;
	//sprintf((char*)lcd_id,"LCD ID:%04X",lcddev.id);//将LCD ID打印到lcd_id数组。
	LCD_Display_Dir(1);	
	while(mode_flag == 0)//初进系统给选项
	{
		LCD_ShowString(0, 60, 200, 16, 16, "Please select mode:   ");
		LCD_ShowString(0, 100, 300, 16, 16, "Rock-Paper-Scissors:      KEY0");
		LCD_ShowString(0, 120, 300, 16, 16, "finger-guessing:          KEY1");
		LCD_ShowString(0, 140, 300, 16, 16, "Rock-Paper-Scissors train:KEY2");
		LCD_ShowString(0, 160, 300, 16, 16, "finger-guessing train:    KEY3");
	}
	LCD_Clear(WHITE);//清屏
	
  	while(1) 
	{		 
//		LCD_ShowxNum(50, 100, (res0 - temp0) * 1000, 10, 16, 0);				//显示通道0采值
//		LCD_ShowxNum(50, 120, (res1 - temp1) * 1000, 10, 16, 0);				//显示通道1采值
//		LCD_ShowxNum(50, 140, (res2 - temp2) * 1000, 10, 16, 0);				//显示通道2采值
//		LCD_ShowxNum(50, 160, (res3 - temp3) * 1000, 10, 16, 0);				//显示通道3采值
//		LCD_ShowString(0, 180, 200, 16, 16, "mode_flag");						//显示模式标志位
//		LCD_ShowxNum(150, 180, mode_flag, 5, 16, 0);
//		LCD_ShowString(0, 220, 200, 16, 16, "decide_flag");						//显示判定标志位
//		LCD_ShowxNum(150, 220, decide_flag, 5, 16, 0);
		if(mode_flag == 3)//石头剪刀布训练模式
		{
			for(k = 0; k < 9; k++)//采9次手势
			{
				while(train_flag % 2 == 0);
				LED0 = !LED0;
				LCD_ShowString(0, 140, 300, 24, 24, "                           ");
				
				collect_flag = 1;//放上手
				while(collect[19] == 0);
				collect_flag = 0;
				collect_fist[k] = compute();//计算放一次手平均值
				collect[19] = 0;//清第19个元素，可以下次进采集函数
				train_flag++;	
				POINT_COLOR=RED;
				LCD_ShowString(0, 140, 300, 24, 24, "Acquisition complete       ");
				POINT_COLOR=BLACK;
			}
			fist[0] = (collect_fist[0] + collect_fist[1] + collect_fist[2]) / 3;//计算各个手势的平均值
			fist[1] = (collect_fist[3] + collect_fist[4] + collect_fist[5]) / 3;
			fist[2] = (collect_fist[6] + collect_fist[7] + collect_fist[8]) / 3;
			mode_flag = 10;//不再进来
			
		}
		if(mode_flag == 4)
		{
			for(l = 0; l < 15; l++)//采9次手势
			{
				while(train_flag % 2 == 0);
				LED0 = !LED0;
				LCD_ShowString(0, 140, 300, 24, 24, "                           ");
				
				collect_flag = 1;//放上手
				while(collect[19] == 0);
				collect_flag = 0;
				collect_finger[l] = compute();//计算放一次手平均值
				collect[19] = 0;//清第19个元素，可以下次进采集函数
				train_flag++;				
				POINT_COLOR=RED;
				LCD_ShowString(0, 140, 300, 24, 24, "Acquisition complete       ");
				POINT_COLOR=BLACK;
			} 
			finger[0] = (collect_finger[0] + collect_finger[1] + collect_finger[2]) / 3;
			finger[1] = (collect_finger[3] + collect_finger[4] + collect_finger[5]) / 3;
			finger[2] = (collect_finger[6] + collect_finger[7] + collect_finger[8]) / 3;
			finger[3] = (collect_finger[9] + collect_finger[10] + collect_finger[11]) / 3;
			finger[4] = (collect_finger[12] + collect_finger[13] + collect_finger[14]) / 3;
			mode_flag = 10;//不再进来
		}
		if(D_res1 > Threshold2)//手放上去光电对管有值才进来
		{
			if(mode_flag == 1)//石头剪刀布
			{
				switch(decide_flag)
				{
					case 0: 
					{
						POINT_COLOR=RED;
						LCD_ShowString(0, 140, 800, 24, 24, "scissor        	                              ");//剪刀
						POINT_COLOR=BLACK;
						if(decide_flag_l != decide_flag)//有变化识别出来之后蜂鸣器响200ms
						{
							BEEP = 1;
						}
					}break;
					case 1:
					{
						POINT_COLOR=RED;
						LCD_ShowString(0, 140, 300, 24, 24, "stone                                           ");//石头
						POINT_COLOR=BLACK;
						if(decide_flag_l != decide_flag)
						{
							BEEP = 1;
						}
					}break;
					case 2:
					{
						POINT_COLOR=RED;
						LCD_ShowString(0, 140, 300, 24, 24, "cloth                                           ");//布
						POINT_COLOR=BLACK;
						if(decide_flag_l != decide_flag)
						{
							BEEP = 1;
						}
					}break;
				}
			}
			if(mode_flag == 2)//划拳模式
			{
				if(D_res0 < 200)
				{
					LCD_ShowString(0, 140, 500, 24, 24, "                                           ");//没有手指-拳头，什么都不显示
				}
				else
				{
					switch(decide_flag)
					{
						case 4: 
						{
							POINT_COLOR=RED;
							LCD_ShowxNum(0, 140, decide_flag - 3, 24, 24, 0);//显示手指数
							POINT_COLOR=BLACK;
							if(decide_flag_l != decide_flag)
							{
								BEEP = 1;
							}
						}break;
						case 5: 
						{
							POINT_COLOR=RED;
							LCD_ShowxNum(0, 140, decide_flag - 3, 24, 24, 0);
							POINT_COLOR=BLACK;
							if(decide_flag_l != decide_flag)
							{
								BEEP = 1;
							}
						} break;
						case 6: 
						{
							POINT_COLOR=RED;
							LCD_ShowxNum(0, 140, decide_flag - 3, 24, 24, 0);
							POINT_COLOR=BLACK;
							if(decide_flag_l != decide_flag)
							{
								BEEP = 1;
							}
						}break;
						case 7: 
						{
							POINT_COLOR=RED;
							LCD_ShowxNum(0, 140, decide_flag - 3, 24, 24, 0);
							POINT_COLOR=BLACK;
							if(decide_flag_l != decide_flag)
							{
								BEEP = 1;
							}
						}break;
						case 8: 
						{
							POINT_COLOR=RED;
							LCD_ShowxNum(0, 140, decide_flag - 3, 24, 24, 0);
							POINT_COLOR=BLACK;
							if(decide_flag_l != decide_flag)
							{
								BEEP = 1;
							}
						}break;
					}
				}
			}
		}
		else//单个极板没信号，没有把手放上去
		{
			//delay_ms(100);
			if(D_res1 <= Threshold2)
			{
				if(mode_flag == 1)
				{
					LCD_ShowString(0, 80, 300, 24, 24, "Rock-Paper-Scissors      ");//石头剪刀布
					POINT_COLOR=RED;
					LCD_ShowString(0, 140, 300, 24, 24, "Please put your hand              ");
					POINT_COLOR=BLACK;
				}
				if(mode_flag == 2)
				{
					LCD_ShowString(0, 80, 300, 24, 24, "finger-guessing          ");//划拳
					POINT_COLOR=RED;
					LCD_ShowString(0, 140, 300, 24, 24, "Please put your hand              ");
					POINT_COLOR=BLACK;
				}
				if(mode_flag == 3)//划拳训练
				{
					LCD_ShowString(0, 80, 300, 24, 24, "Rock-Paper-Scissors train");	//石头剪刀布训练模式		
					POINT_COLOR=RED;
					LCD_ShowString(0, 140, 300, 24, 24, "Please put your hand              ");
					POINT_COLOR=BLACK;
				}
				if(mode_flag == 4)//猜拳训练
				{
					LCD_ShowString(0, 80, 300, 24, 24, "finger-guessing train    ");//划拳训练模式
					POINT_COLOR=RED;
					LCD_ShowString(0, 140, 300, 24, 24, "Please put your hand              ");
					POINT_COLOR=BLACK;
				}
				if(mode_flag == 5)
				{
					LCD_ShowString(0, 80, 300, 24, 24, "Rock-Paper-Scissors       ");//石头剪刀布训练模式后判决
					POINT_COLOR=RED;
					LCD_ShowString(0, 140, 300, 24, 24, "Please put your hand              ");
					POINT_COLOR=BLACK;
				}
				if(mode_flag == 6)
				{
					LCD_ShowString(0, 80, 300, 24, 24, "finger-guessing           ");//划拳训练模式后判决
					POINT_COLOR=RED;
					LCD_ShowString(0, 140, 300, 24, 24, "Please put your hand              ");
					POINT_COLOR=BLACK;
				}
			}
		}
		decide_flag_l = decide_flag;//配合蜂鸣器只响一次
	} 
}

void Sys_Init(void)
{	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	while(FDC2214_Init());
	delay_ms(100);
	temp0 = Cap_Calculate(0);//读取初始值
	temp1 = Cap_Calculate(1);
//	temp2 = Cap_Calculate(2);
//	temp3 = Cap_Calculate(3);
	TIM3_Int_Init(500-1,7200-1);//200ms中断，读一次通道值+转换一次通道值33ms，实际可能比这个短
}

//定时器5中断服务程序
void TIM3_IRQHandler(void)   //TIM5中断
{
	BEEP = 0;
	if(TIM3->SR&0X0001)//如果TIM3定时中断是使能的
	{
		res0 = Cap_Calculate(0);//采集数据
		res1 = Cap_Calculate(1);
		if(collect_flag == 1 && collect[19] == 0)//有标志位和数组最后一位为0；采满不再采集
		{
			collect[i] = res0 - temp0;//采集不扩大1000倍
//			printf("%3.3f \r\n", collect[i]);
//			while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET);
			i++;
		}
		if(i == 20)
		{
			i = 0;
		}
//		res2 = Cap_Calculate(2);
//		res3 = Cap_Calculate(3);
	//	Data_Send_Status(res,0,0);
	//	printf("%5d : %5d : %5d\r\n",res0,res1,res2);
		 LED1 = ~LED1;
	//	 printf("CH0;%3.3f CH1;%3.3f \r\n",res0-temp0,res1-temp1);
	//	 while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET);
	}
	D_res0 = (res0 - temp0) * 1000;//数值放大
	D_res1 = (res1 - temp1) * 1000;
	D_res2 = (res0 - temp0);
	D_res3 = (res1 - temp1);
	if(count == 10)
	{
		if(mode_flag == 1)//第一小题，猜拳判决
		{
			decide_flag = 3;
			if(fabs(D_res2 - fist[1]) < fabs(D_res2 - fist[0]) && fabs(D_res2 - fist[1]) < fabs(D_res2 - fist[2]))
			{
				decide_flag = 0;//剪刀---距离哪个最近就判定为哪个
			}
			else if(fabs(D_res2 - fist[0]) < fabs(D_res2 - fist[1]) && fabs(D_res2 - fist[0]) < fabs(D_res2 - fist[2]))
			{
				decide_flag = 1;//石头
			}
			else if(fabs(D_res2 - fist[2]) < fabs(D_res2 - fist[1]) && fabs(D_res2 - fist[2]) < fabs(D_res2 - fist[0]))
			{
				decide_flag = 2;//布
			}	
		}
		if(mode_flag == 2)//第一小题，猜拳判决
		{
			decide_flag = 9;
			if(fabs(D_res2 - finger[0]) < fabs(D_res2 - finger[1]) && fabs(D_res2 - finger[0]) < fabs(D_res2 - finger[2]) && fabs(D_res2 - finger[0]) < fabs(D_res2 - finger[3]) && fabs(D_res2 - finger[0]) < fabs(D_res2 - finger[4]))
			{
				decide_flag = 4;//1个手指
			}
			else if(fabs(D_res2 - finger[1]) < fabs(D_res2 - finger[0]) && fabs(D_res2 - finger[1]) < fabs(D_res2 - finger[2]) && fabs(D_res2 - finger[1]) < fabs(D_res2 - finger[3]) && fabs(D_res2 - finger[1]) < fabs(D_res2 - finger[4]))
			{
				decide_flag = 5;//2个手指
			}
			else if(fabs(D_res2 - finger[2]) < fabs(D_res2 - finger[0]) && fabs(D_res2 - finger[2]) < fabs(D_res2 - finger[1]) && fabs(D_res2 - finger[2]) < fabs(D_res2 - finger[3]) && fabs(D_res2 - finger[2]) < fabs(D_res2 - finger[4]))
			{
				decide_flag = 6;//3个手指
			}	
			else if(fabs(D_res2 - finger[3]) < fabs(D_res2 - finger[0]) && fabs(D_res2 - finger[3]) < fabs(D_res2 - finger[1]) && fabs(D_res2 - finger[3]) < fabs(D_res2 - finger[2]) && fabs(D_res2 - finger[3]) < fabs(D_res2 - finger[4]))
			{
				decide_flag = 7;//4个手指
			}
			else if(fabs(D_res2 - finger[4]) < fabs(D_res2 - finger[0]) && fabs(D_res2 - finger[4]) < fabs(D_res2 - finger[1]) && fabs(D_res2 - finger[4]) < fabs(D_res2 - finger[2]) && fabs(D_res2 - finger[4]) < fabs(D_res2 - finger[3]))
			{
				decide_flag = 8;//5个手指
			}
		}
		count = 0;
	}
	
	count++;
	TIM3->SR&=~(1<<0);  //中断标志位清0，等待下一次中断
}

float compute(void)           // 计算剪刀的均值方差和标准差
{
	u8 i = 0;
	float aver = 0, sum = 0;//均为求平均数和方差--差值的标准差和方差
	for(i = 0; i < n; i++)
	{
		sum += collect[i];
	}
	aver = sum / n;
	return aver;
} 










