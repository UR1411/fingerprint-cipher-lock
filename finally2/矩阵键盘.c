#include<reg52.h>
#include<intrins.h>
#define RS_CLR RS=0 
#define RS_SET RS=1

#define RW_CLR RW=0 
#define RW_SET RW=1 

#define EN_CLR EN=0
#define EN_SET EN=1

#define DataPort P0
#define uchar unsigned char
#define uint unsigned int
#define GPIO_KEY P2

uchar keyColValue=0;
uchar keyRowValue=0;
uchar display[10] = {0};

sbit Buzzer = P1^3;
sbit RS = P1^0;   //定义端口 
sbit RW = P1^1;
sbit EN = P2^5;

void delay(uint z)		//@11.0592MHz	//延时函数
{
	uint x,y;
	for(x=z;x>0;x--)
	for(y=114;y>0;y--);
}

void BUZZER(uint i)
{
	Buzzer = 0;
	delay(i);
	Buzzer = 1;
	delay(50);
}

/*uchar keyscan()
{
	uchar h,l;
	P2 = 0xf0;						 
	if(P2 & 0xf0!=0xf0)
	{
		delay(5);
		if(P2&0xf0!=0xf0)
		{
			l = P2 & 0xf0;
			P2 = l | 0x0f;
			h = P2 & 0x0f;
			while((P2&0x0f)!=0x0f);
			return (h + l);
		}
	}
}*/

void keyDown()
{
	char a;
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
delay(10);

}

/*void keypro()
{
	switch(keyscan())
	{
		case 0xff: BUZZER(1000);break;
		case 0x7d: BUZZER(500);break;
	}
} */
void DelayUs2x(unsigned char t)
{   
 while(--t);
}
/*------------------------------------------------
 mS延时函数，含有输入参数 unsigned char t，无返回值
 unsigned char 是定义无符号字符变量，其值的范围是
 0~255 这里使用晶振12M，精确延时请使用汇编
------------------------------------------------*/
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
              判忙函数
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
              写入命令函数
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
              写入数据函数
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
                清屏函数
------------------------------------------------*/
 void LCD_Clear(void) 
 { 
 LCD_Write_Com(0x01); 
 DelayMs(5);
 }
/*------------------------------------------------
              写入字符串函数
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
/*------------------------------------------------
              写入字符函数
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
              初始化函数
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

void main()
{
	LCD_Init(); 
LCD_Clear();//清屏
	while(1)
	{
		keyDown();
		if(keyColValue == 2)
		{
			LCD_Write_String(2,1,"11");
			delay(300);
			LCD_Clear();
			delay(300);
			keyColValue = 0;
		}
		
	}
}