#include<reg52.h>
#include<intrins.h>
#define uchar unsigned char
#define uint unsigned int
//#define PWM_duty 5	 // 控制舵机正向（逆时针）最大速度旋转 3：舵机停止旋转 5：舵机顺时针最大速度旋转
#define CYCLE 8
#define RS_CLR RS=0 	// 与LCD有关的宏定义
#define RS_SET RS=1

#define RW_CLR RW=0 
#define RW_SET RW=1 

#define EN_CLR EN=0
#define EN_SET EN=1

#define DataPort P0
#define GPIO_KEY P2



// 定义常量
uchar count = 0;											 // 与舵机有关的计时器
uchar pack_head[6] = {0xef,0x01,0xff,0xff,0xff,0xff};			  // AS608通讯协议包头
uchar PS_IdendityBackdata[6] = {0x01,0x00,0x03,0x11,0x00,0x15};	// PS_Identify函数数据发送包
uchar PassWord[10] = {0};
uchar PassWordDefine[10] = {1,3,3,4};
uchar PWM_duty = 1;
uchar display[5] = {0};
uchar keyColValue=0;
uchar keyRowValue=0;

// 定义引脚
sbit PWM = P1^2;
sbit Buzzer = P1^3;
sbit RS = P1^0;   //定义端口 
sbit RW = P1^1;
sbit EN = P3^7;


// 定义函数
void delay(uint z)		//@11.0592MHz	//延时函数
{
	uint x,y;
	for(x=z;x>0;x--)
	for(y=114;y>0;y--);
}

void DelayUs2x(unsigned char t)
{   
 while(--t);
}
void DelayMs(unsigned char t)
{
     
 while(t--)
 {
     //大致延时1mS
     DelayUs2x(245);
	 DelayUs2x(245);
 }
}

/*------------------------------------------------
          LCD判忙函数
------------------------------------------------*/
 bit LCD_Check_Busy(void) 
 { 
 DataPort= 0xFF; 
 RS_CLR; 
 RW_SET; 
 EN_CLR; 
 _nop_(); 
 EN_SET;
 return (bit)(DataPort & 0x80);
 }
/*------------------------------------------------
              LCD写入命令函数
------------------------------------------------*/
 void LCD_Write_Com(unsigned char com) 
 {  
 while(LCD_Check_Busy()); //忙则等待
 RS_CLR; 
 RW_CLR; 
 EN_SET; 
 DataPort= com; 
 _nop_(); 
 EN_CLR;
 }
/*------------------------------------------------
              LCD写入数据函数
------------------------------------------------*/
 void LCD_Write_Data(unsigned char Data) 
 { 
 while(LCD_Check_Busy()); //忙则等待
 RS_SET; 
 RW_CLR; 
 EN_SET; 
 DataPort= Data; 
 _nop_();
 EN_CLR;
 }
 /*------------------------------------------------
                LCD清屏函数
------------------------------------------------*/
 void LCD_Clear(void) 
 { 
 LCD_Write_Com(0x01); 
 DelayMs(5);
 }
/*------------------------------------------------
              LCD写入字符串函数
------------------------------------------------*/
 void LCD_Write_String(unsigned char x,unsigned char y,unsigned char *s) 
 {     
 if (y == 0) 
 	{     
	 LCD_Write_Com(0x80 + x);     //表示第一行
 	}
 else 
 	{      
 	LCD_Write_Com(0xC0 + x);      //表示第二行
 	}        
 while (*s) 
 	{     
 LCD_Write_Data( *s);     
 s ++;     
 	}
 }
void LCD_Write_StringShort(unsigned char x,unsigned char y,unsigned char *s,uint i)
{
	uint w,k;
	LCD_Clear();
	for(w=i;w>0;w--)
	for(k=114;k>0;k--)
	{
		LCD_Write_String(x,y,s);
	}
	LCD_Clear();
}
void LCD_Write_StringShortDouble(unsigned char x,unsigned char y,unsigned char *s,unsigned char f,unsigned char g,unsigned char *h,uint i)
{
	uint w,k;
	LCD_Clear();
	for(w=i;w>0;w--)
	for(k=114;k>0;k--)
	{
		LCD_Write_String(x,y,s);
		LCD_Write_String(f,g,h);
	}
	LCD_Clear();
}
/*------------------------------------------------
              LCD写入字符函数
------------------------------------------------*/
 void LCD_Write_Char(unsigned char x,unsigned char y,unsigned char Data) 
 {     
 if (y == 0) 
 	{     
 	LCD_Write_Com(0x80 + x);     
 	}    
 else 
 	{     
 	LCD_Write_Com(0xC0 + x);     
 	}        
 LCD_Write_Data( Data);  
 }
/*------------------------------------------------
              LCD初始化函数
------------------------------------------------*/
 void LCD_Init(void) 
 {
   LCD_Write_Com(0x38);    /*显示模式设置*/ 
   DelayMs(5); 
   LCD_Write_Com(0x38); 
   DelayMs(5); 
   LCD_Write_Com(0x38); 
   DelayMs(5); 
   LCD_Write_Com(0x38);  
   LCD_Write_Com(0x08);    /*显示关闭*/ 
   LCD_Write_Com(0x01);    /*显示清屏*/ 
   LCD_Write_Com(0x06);    /*显示光标移动设置*/ 
   DelayMs(5); 
   LCD_Write_Com(0x0C);    /*显示开及光标设置*/
   }

void Timer0Init(void)		//0.5毫秒@11.0592MHz   // 定时器0 工作模式1 产生舵机PWM方波
{
	TMOD = 0x01;		//设置定时器模式
	IE = 0x82;
	TH0 = 0xf6;		//设置定时初值
	TL0 = 0xdc;	
//	EA = 1;
//	ET0 = 1;	//设置定时初值
	//TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
}  

void uart_init()		 //串行口初始化
{
	TMOD=0x20;
	TH1=0xfd;
	TL1=0xfd;
	PCON=0x00;
	SCON=0x50;
	EA=1;
	ES=1;
	TR1=1;
}

void sent_byte(uchar date)		  //发送一字节数据
{
	SBUF=date;
	while(!TI);
	TI=0;
}
uchar receive_byte()			  //接收一字节数据
{
	uchar date;
	while(!RI);
	RI=0;
	date=SBUF;
	return date;
}

void PS_Identify()
{
	int i;
	for(i=0;i<6;i++)
	{
		sent_byte(pack_head[i]);
	}

	for(i=0;i<6;i++)
	{
		sent_byte(PS_IdendityBackdata[i]);
	}
}

/*-------------------------------------
			蜂鸣器函数
--------------------------------------*/
void BUZZER(uint i)
{
	Buzzer = 0;
	delay(i);
	Buzzer = 1;
	delay(50);
}

/*------------------------------------
			矩阵键盘扫描函数
-------------------------------------*/
void keyDown()
{

	GPIO_KEY = 0x0f;
	if(GPIO_KEY != 0x0f)
	{
		delay(10);
		if(GPIO_KEY != 0x0f)						// 检测行数
		{
			switch(GPIO_KEY)
			{
				case 0x07: keyRowValue = 1;break;
				case 0x0b: keyRowValue = 2;break;
				case 0x0d: keyRowValue = 3;break;
				case 0x0e: keyRowValue = 4;break;
			}

		}
	}
	GPIO_KEY = 0xf0;
	if(GPIO_KEY != 0xf0)
	{
		delay(10);
		if(GPIO_KEY != 0xf0)						  // 检测列数
		{
			switch(GPIO_KEY)
			{
				case 0xe0: keyColValue = 1;break;
				case 0xd0: keyColValue = 2;break;
				case 0xb0: keyColValue = 3;break;
				case 0x70: keyColValue = 4;break;
			}
		}
	}
/*	while((a < 50) && (GPIO_KEY != 0xf0))  //延时，确保没有按键再按下
  {
   delay(50);
   a ++;
  }	*/
  delay(10);

}

/*------------------------------------
            主函数
------------------------------------*/

void main()
{
	int i=0;
	uint j = 0;
	uint k = 0;
	uint passWordFlag = 1;
	uint passWordKeep = 1;
	
	unsigned char date[32];
	LCD_Init(); 
	LCD_Clear();//清屏
	while(1)
	{
		keyDown();
		/*---------------------------------------------
						密码模式
		----------------------------------------------*/
		if((keyRowValue == 3) && (keyColValue == 4))   // 密码模式
		{
			keyRowValue = 0;
			keyColValue = 0;
			LCD_Write_StringShortDouble(1,0,"Mode:",1,1,"PassWord",50);
			while(passWordFlag)
			{

				while(!keyRowValue)				  // 停顿直到行数改变
				{
				LCD_Write_StringShort(0,0,"keydown",15);
					keyDown();
					
				}
				if((keyRowValue == 1) && (keyColValue == 1))
				{
					PassWord[i] = 1;
					i++;
					keyRowValue = 0;
					keyColValue = 0;
				//	delay(200);
					LCD_Write_StringShort(0,0,"1",25);

				}
				else if((keyRowValue == 1) && (keyColValue == 2))
				{
					PassWord[i] = 2;
					i++;
					keyRowValue = 0;
					keyColValue = 0;
					LCD_Write_StringShort(0,0,"2",25);
				}
				else if((keyRowValue == 1) && (keyColValue == 3))
				{
					PassWord[i]	= 3;
					i++;
					keyRowValue = 0;
					keyColValue = 0;
					LCD_Write_StringShort(0,0,"3",25);
				}
				else if((keyRowValue == 1) && (keyColValue == 4))
				{
					PassWord[i] = 4;
					i++;
					keyRowValue = 0;
					keyColValue = 0;
				//	delay(200);	
					LCD_Write_StringShort(0,0,"4",25);									 
				}
				else if((keyRowValue == 2) && (keyColValue == 1))
				{
					PassWord[i]	= 5;
					i++;
					keyRowValue = 0;
					keyColValue = 0;
					LCD_Write_StringShort(0,0,"5",25);
				}
				else if((keyRowValue == 2) && (keyColValue == 2))
				{
					PassWord[i]	= 6;
					i++;
					keyRowValue = 0;
					keyColValue = 0;
					LCD_Write_StringShort(0,0,"6",25);
				}
				else if((keyRowValue == 2) && (keyColValue == 3))
				{
					PassWord[i]	= 7;
					i++;
					keyRowValue = 0;
					keyColValue = 0;
					LCD_Write_StringShort(0,0,"7",25);
				}
				else if((keyRowValue == 2) && (keyColValue == 4))
				{
					PassWord[i]	= 8;
					i++;
					keyRowValue = 0;
					keyColValue = 0;
					LCD_Write_StringShort(0,0,"8",25);
				}
				else if((keyRowValue == 3) && (keyColValue == 1))
				{
					PassWord[i]	= 9;
					i++;
					keyRowValue = 0;
					keyColValue = 0;
					LCD_Write_StringShort(0,0,"9",25);
				}
				else if((keyRowValue == 3) && (keyColValue == 2))
				{
					PassWord[i]	= 0;
					i++;
					keyRowValue = 0;
					keyColValue = 0;
					LCD_Write_StringShort(0,0,"0",25);
				}
				if(i==4)
				{
					for(j=0;j<4;j++)
					{
						if((PassWord[j]==PassWordDefine[j]) && j==3)
						{
							passWordFlag = 0;
						}
						if(PassWord[j]!=PassWordDefine[j])
						{
							break;
						}
					}

				}

				if(i==4 && passWordFlag==1)
				{
					BUZZER(1000);
					LCD_Clear();
					while(1)
					{
						LCD_Write_String(0,0,"wrong password!");
					}
				}
			}
			if(passWordFlag == 0)
			{
				LCD_Clear();
				LCD_Write_StringShort(1,0,"Success!",25);
				Timer0Init();
				delay(2000);
				ET0 = 0;
				TR0 = 0;
			}
		}
		/*---------------------------------------------
						指纹模式
		----------------------------------------------*/
		if((keyRowValue == 4) && (keyColValue == 1))				   // 指纹模式
		{
			 LCD_Write_StringShortDouble(1,0,"Mode:",1,1,"Fingerprint",50);
			 uart_init();
			PS_Identify();				// 调用自动验证指纹函数

			for(i=0;i<16;i++)
			{
				date[i] = receive_byte();
 		
			}
			if(date[9]==0x00)			// 指纹验证成功时
			{	
				
				LCD_Clear();
				LCD_Write_StringShort(1,0,"Success!",25);
				Timer0Init();
				delay(2000);
				ET0 = 0;
				TR0 = 0;
				while(1);
			}
			else if(date[9]==0x09)						// 指纹验证失败或接收包有错时
			{
				BUZZER(1000);
				LCD_Clear();
				while(1)
				{
					LCD_Write_String(0,0,"wrong Finger!");
				}

			}
		}
		
	}
	while(1);
}

void Timer0()interrupt 1	   // 与舵机有关的计时器1中断
{
	TH0 = 0xf6;
	TL0 = 0xdc;
	//TF0 = 0;
	if(count<PWM_duty)
	{
		PWM = 1;
	}
	else
	{
		PWM = 0;
		        
	}
	count++;
	count%=CYCLE;
}

void uart() interrupt 4	   // 重要！没有中断函数 中断无法正常进行
{
	
}

















































