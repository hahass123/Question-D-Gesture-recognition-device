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
 ALIENTEK��ӢSTM32������ʵ��13
 TFTLCD��ʾʵ��  
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/

#define Threshold1 150						//��ֵ1��
#define Threshold2 500						//��ֵ2,���ַ�
#define Threshold3 70						//��ֵ2
#define Threshold4 100						//��ֵ2
#define Threshold5 250						//��ֵ2

void Sys_Init(void);
float compute(void);

float res0, res1, res2, res3;				//��������ֵ
float temp0, temp1, temp2, temp3;			//��������ʼֵ
float D_res0, D_res1, D_res2, D_res3;		//��������ֵ
u8 decide_flag = 3, mode_flag = 0, decide_flag_l = 0, collect_flag = 0;//�ж���־λ��ģʽ��־λ,������־λ���ɼ���־λ
u8 n = 20, i = 0, j = 0, f = 0, train_flag, count = 0;

float collect[20] = {0}, collect_fist[9], collect_finger[15], fist[3] = {0.050, 0.680, 1.500}, finger[5] = {0.350, 0.580, 0.850, 1.050, 1.440};
//�ɼ�20����--��9/15��ƽ��ֵ--ʯͷ������������ƽ��ֵ--1��2��3��4��5��ƽ��ֵ
      

int main(void)
{	
	
	u8 k = 0, l = 0;
	collect[19] = 0;
	//u8 lcd_id[12];			//���LCD ID�ַ���
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 	//���ڳ�ʼ��Ϊ115200
 	LED_Init();			     //LED�˿ڳ�ʼ��
	LCD_Init();
	BEEP_Init();
	Sys_Init();				//��������ʼ��
	EXTIX_Init();         	//��ʼ���ⲿ�ж����� 
	POINT_COLOR=BLACK;
	//sprintf((char*)lcd_id,"LCD ID:%04X",lcddev.id);//��LCD ID��ӡ��lcd_id���顣
	LCD_Display_Dir(1);	
	while(mode_flag == 0)//����ϵͳ��ѡ��
	{
		LCD_ShowString(0, 60, 200, 16, 16, "Please select mode:   ");
		LCD_ShowString(0, 100, 300, 16, 16, "Rock-Paper-Scissors:      KEY0");
		LCD_ShowString(0, 120, 300, 16, 16, "finger-guessing:          KEY1");
		LCD_ShowString(0, 140, 300, 16, 16, "Rock-Paper-Scissors train:KEY2");
		LCD_ShowString(0, 160, 300, 16, 16, "finger-guessing train:    KEY3");
	}
	LCD_Clear(WHITE);//����
	
  	while(1) 
	{		 
//		LCD_ShowxNum(50, 100, (res0 - temp0) * 1000, 10, 16, 0);				//��ʾͨ��0��ֵ
//		LCD_ShowxNum(50, 120, (res1 - temp1) * 1000, 10, 16, 0);				//��ʾͨ��1��ֵ
//		LCD_ShowxNum(50, 140, (res2 - temp2) * 1000, 10, 16, 0);				//��ʾͨ��2��ֵ
//		LCD_ShowxNum(50, 160, (res3 - temp3) * 1000, 10, 16, 0);				//��ʾͨ��3��ֵ
//		LCD_ShowString(0, 180, 200, 16, 16, "mode_flag");						//��ʾģʽ��־λ
//		LCD_ShowxNum(150, 180, mode_flag, 5, 16, 0);
//		LCD_ShowString(0, 220, 200, 16, 16, "decide_flag");						//��ʾ�ж���־λ
//		LCD_ShowxNum(150, 220, decide_flag, 5, 16, 0);
		if(mode_flag == 3)//ʯͷ������ѵ��ģʽ
		{
			for(k = 0; k < 9; k++)//��9������
			{
				while(train_flag % 2 == 0);
				LED0 = !LED0;
				LCD_ShowString(0, 140, 300, 24, 24, "                           ");
				
				collect_flag = 1;//������
				while(collect[19] == 0);
				collect_flag = 0;
				collect_fist[k] = compute();//�����һ����ƽ��ֵ
				collect[19] = 0;//���19��Ԫ�أ������´ν��ɼ�����
				train_flag++;	
				POINT_COLOR=RED;
				LCD_ShowString(0, 140, 300, 24, 24, "Acquisition complete       ");
				POINT_COLOR=BLACK;
			}
			fist[0] = (collect_fist[0] + collect_fist[1] + collect_fist[2]) / 3;//����������Ƶ�ƽ��ֵ
			fist[1] = (collect_fist[3] + collect_fist[4] + collect_fist[5]) / 3;
			fist[2] = (collect_fist[6] + collect_fist[7] + collect_fist[8]) / 3;
			mode_flag = 10;//���ٽ���
			
		}
		if(mode_flag == 4)
		{
			for(l = 0; l < 15; l++)//��9������
			{
				while(train_flag % 2 == 0);
				LED0 = !LED0;
				LCD_ShowString(0, 140, 300, 24, 24, "                           ");
				
				collect_flag = 1;//������
				while(collect[19] == 0);
				collect_flag = 0;
				collect_finger[l] = compute();//�����һ����ƽ��ֵ
				collect[19] = 0;//���19��Ԫ�أ������´ν��ɼ�����
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
			mode_flag = 10;//���ٽ���
		}
		if(D_res1 > Threshold2)//�ַ���ȥ���Թ���ֵ�Ž���
		{
			if(mode_flag == 1)//ʯͷ������
			{
				switch(decide_flag)
				{
					case 0: 
					{
						POINT_COLOR=RED;
						LCD_ShowString(0, 140, 800, 24, 24, "scissor        	                              ");//����
						POINT_COLOR=BLACK;
						if(decide_flag_l != decide_flag)//�б仯ʶ�����֮���������200ms
						{
							BEEP = 1;
						}
					}break;
					case 1:
					{
						POINT_COLOR=RED;
						LCD_ShowString(0, 140, 300, 24, 24, "stone                                           ");//ʯͷ
						POINT_COLOR=BLACK;
						if(decide_flag_l != decide_flag)
						{
							BEEP = 1;
						}
					}break;
					case 2:
					{
						POINT_COLOR=RED;
						LCD_ShowString(0, 140, 300, 24, 24, "cloth                                           ");//��
						POINT_COLOR=BLACK;
						if(decide_flag_l != decide_flag)
						{
							BEEP = 1;
						}
					}break;
				}
			}
			if(mode_flag == 2)//��ȭģʽ
			{
				if(D_res0 < 200)
				{
					LCD_ShowString(0, 140, 500, 24, 24, "                                           ");//û����ָ-ȭͷ��ʲô������ʾ
				}
				else
				{
					switch(decide_flag)
					{
						case 4: 
						{
							POINT_COLOR=RED;
							LCD_ShowxNum(0, 140, decide_flag - 3, 24, 24, 0);//��ʾ��ָ��
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
		else//��������û�źţ�û�а��ַ���ȥ
		{
			//delay_ms(100);
			if(D_res1 <= Threshold2)
			{
				if(mode_flag == 1)
				{
					LCD_ShowString(0, 80, 300, 24, 24, "Rock-Paper-Scissors      ");//ʯͷ������
					POINT_COLOR=RED;
					LCD_ShowString(0, 140, 300, 24, 24, "Please put your hand              ");
					POINT_COLOR=BLACK;
				}
				if(mode_flag == 2)
				{
					LCD_ShowString(0, 80, 300, 24, 24, "finger-guessing          ");//��ȭ
					POINT_COLOR=RED;
					LCD_ShowString(0, 140, 300, 24, 24, "Please put your hand              ");
					POINT_COLOR=BLACK;
				}
				if(mode_flag == 3)//��ȭѵ��
				{
					LCD_ShowString(0, 80, 300, 24, 24, "Rock-Paper-Scissors train");	//ʯͷ������ѵ��ģʽ		
					POINT_COLOR=RED;
					LCD_ShowString(0, 140, 300, 24, 24, "Please put your hand              ");
					POINT_COLOR=BLACK;
				}
				if(mode_flag == 4)//��ȭѵ��
				{
					LCD_ShowString(0, 80, 300, 24, 24, "finger-guessing train    ");//��ȭѵ��ģʽ
					POINT_COLOR=RED;
					LCD_ShowString(0, 140, 300, 24, 24, "Please put your hand              ");
					POINT_COLOR=BLACK;
				}
				if(mode_flag == 5)
				{
					LCD_ShowString(0, 80, 300, 24, 24, "Rock-Paper-Scissors       ");//ʯͷ������ѵ��ģʽ���о�
					POINT_COLOR=RED;
					LCD_ShowString(0, 140, 300, 24, 24, "Please put your hand              ");
					POINT_COLOR=BLACK;
				}
				if(mode_flag == 6)
				{
					LCD_ShowString(0, 80, 300, 24, 24, "finger-guessing           ");//��ȭѵ��ģʽ���о�
					POINT_COLOR=RED;
					LCD_ShowString(0, 140, 300, 24, 24, "Please put your hand              ");
					POINT_COLOR=BLACK;
				}
			}
		}
		decide_flag_l = decide_flag;//��Ϸ�����ֻ��һ��
	} 
}

void Sys_Init(void)
{	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	while(FDC2214_Init());
	delay_ms(100);
	temp0 = Cap_Calculate(0);//��ȡ��ʼֵ
	temp1 = Cap_Calculate(1);
//	temp2 = Cap_Calculate(2);
//	temp3 = Cap_Calculate(3);
	TIM3_Int_Init(500-1,7200-1);//200ms�жϣ���һ��ͨ��ֵ+ת��һ��ͨ��ֵ33ms��ʵ�ʿ��ܱ������
}

//��ʱ��5�жϷ������
void TIM3_IRQHandler(void)   //TIM5�ж�
{
	BEEP = 0;
	if(TIM3->SR&0X0001)//���TIM3��ʱ�ж���ʹ�ܵ�
	{
		res0 = Cap_Calculate(0);//�ɼ�����
		res1 = Cap_Calculate(1);
		if(collect_flag == 1 && collect[19] == 0)//�б�־λ���������һλΪ0���������ٲɼ�
		{
			collect[i] = res0 - temp0;//�ɼ�������1000��
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
	D_res0 = (res0 - temp0) * 1000;//��ֵ�Ŵ�
	D_res1 = (res1 - temp1) * 1000;
	D_res2 = (res0 - temp0);
	D_res3 = (res1 - temp1);
	if(count == 10)
	{
		if(mode_flag == 1)//��һС�⣬��ȭ�о�
		{
			decide_flag = 3;
			if(fabs(D_res2 - fist[1]) < fabs(D_res2 - fist[0]) && fabs(D_res2 - fist[1]) < fabs(D_res2 - fist[2]))
			{
				decide_flag = 0;//����---�����ĸ�������ж�Ϊ�ĸ�
			}
			else if(fabs(D_res2 - fist[0]) < fabs(D_res2 - fist[1]) && fabs(D_res2 - fist[0]) < fabs(D_res2 - fist[2]))
			{
				decide_flag = 1;//ʯͷ
			}
			else if(fabs(D_res2 - fist[2]) < fabs(D_res2 - fist[1]) && fabs(D_res2 - fist[2]) < fabs(D_res2 - fist[0]))
			{
				decide_flag = 2;//��
			}	
		}
		if(mode_flag == 2)//��һС�⣬��ȭ�о�
		{
			decide_flag = 9;
			if(fabs(D_res2 - finger[0]) < fabs(D_res2 - finger[1]) && fabs(D_res2 - finger[0]) < fabs(D_res2 - finger[2]) && fabs(D_res2 - finger[0]) < fabs(D_res2 - finger[3]) && fabs(D_res2 - finger[0]) < fabs(D_res2 - finger[4]))
			{
				decide_flag = 4;//1����ָ
			}
			else if(fabs(D_res2 - finger[1]) < fabs(D_res2 - finger[0]) && fabs(D_res2 - finger[1]) < fabs(D_res2 - finger[2]) && fabs(D_res2 - finger[1]) < fabs(D_res2 - finger[3]) && fabs(D_res2 - finger[1]) < fabs(D_res2 - finger[4]))
			{
				decide_flag = 5;//2����ָ
			}
			else if(fabs(D_res2 - finger[2]) < fabs(D_res2 - finger[0]) && fabs(D_res2 - finger[2]) < fabs(D_res2 - finger[1]) && fabs(D_res2 - finger[2]) < fabs(D_res2 - finger[3]) && fabs(D_res2 - finger[2]) < fabs(D_res2 - finger[4]))
			{
				decide_flag = 6;//3����ָ
			}	
			else if(fabs(D_res2 - finger[3]) < fabs(D_res2 - finger[0]) && fabs(D_res2 - finger[3]) < fabs(D_res2 - finger[1]) && fabs(D_res2 - finger[3]) < fabs(D_res2 - finger[2]) && fabs(D_res2 - finger[3]) < fabs(D_res2 - finger[4]))
			{
				decide_flag = 7;//4����ָ
			}
			else if(fabs(D_res2 - finger[4]) < fabs(D_res2 - finger[0]) && fabs(D_res2 - finger[4]) < fabs(D_res2 - finger[1]) && fabs(D_res2 - finger[4]) < fabs(D_res2 - finger[2]) && fabs(D_res2 - finger[4]) < fabs(D_res2 - finger[3]))
			{
				decide_flag = 8;//5����ָ
			}
		}
		count = 0;
	}
	
	count++;
	TIM3->SR&=~(1<<0);  //�жϱ�־λ��0���ȴ���һ���ж�
}

float compute(void)           // ��������ľ�ֵ����ͱ�׼��
{
	u8 i = 0;
	float aver = 0, sum = 0;//��Ϊ��ƽ�����ͷ���--��ֵ�ı�׼��ͷ���
	for(i = 0; i < n; i++)
	{
		sum += collect[i];
	}
	aver = sum / n;
	return aver;
} 










