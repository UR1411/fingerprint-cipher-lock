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
sbit RS = P1^0;   //����˿� 
sbit RW = P1^1;
sbit EN = P2^5;

void delay(uint z)		//@11.0592MHz	//��ʱ����
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
		if(GPIO_KEY != 0x0f)						// �������
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
		if(GPIO_KEY != 0xf0)						  // �������
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
 mS��ʱ����������������� unsigned char t���޷���ֵ
 unsigned char �Ƕ����޷����ַ���������ֵ�ķ�Χ��
 0~255 ����ʹ�þ���12M����ȷ��ʱ��ʹ�û��
------------------------------------------------*/
void DelayMs(unsigned char t)
{
     
 while(t--)
 {
     //������ʱ1mS
     DelayUs2x(245);
	 DelayUs2x(245);
 }
}
/*------------------------------------------------
              ��æ����
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
              д�������
------------------------------------------------*/
 void LCD_Write_Com(unsigned char com) 
 {  
 while(LCD_Check_Busy()); //æ��ȴ�
 RS_CLR; 
 RW_CLR; 
 EN_SET; 
 DataPort= com; 
 _nop_(); 
 EN_CLR;
 }
/*------------------------------------------------
              д�����ݺ���
------------------------------------------------*/
 void LCD_Write_Data(unsigned char Data) 
 { 
 while(LCD_Check_Busy()); //æ��ȴ�
 RS_SET; 
 RW_CLR; 
 EN_SET; 
 DataPort= Data; 
 _nop_();
 EN_CLR;
 }
 /*------------------------------------------------
                ��������
------------------------------------------------*/
 void LCD_Clear(void) 
 { 
 LCD_Write_Com(0x01); 
 DelayMs(5);
 }
/*------------------------------------------------
              д���ַ�������
------------------------------------------------*/
 void LCD_Write_String(unsigned char x,unsigned char y,unsigned char *s) 
 {     
 if (y == 0) 
 	{     
	 LCD_Write_Com(0x80 + x);     //��ʾ��һ��
 	}
 else 
 	{      
 	LCD_Write_Com(0xC0 + x);      //��ʾ�ڶ���
 	}        
 while (*s) 
 	{     
 LCD_Write_Data( *s);     
 s ++;     
 	}
 }
/*------------------------------------------------
              д���ַ�����
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
              ��ʼ������
------------------------------------------------*/
 void LCD_Init(void) 
 {
   LCD_Write_Com(0x38);    /*��ʾģʽ����*/ 
   DelayMs(5); 
   LCD_Write_Com(0x38); 
   DelayMs(5); 
   LCD_Write_Com(0x38); 
   DelayMs(5); 
   LCD_Write_Com(0x38);  
   LCD_Write_Com(0x08);    /*��ʾ�ر�*/ 
   LCD_Write_Com(0x01);    /*��ʾ����*/ 
   LCD_Write_Com(0x06);    /*��ʾ����ƶ�����*/ 
   DelayMs(5); 
   LCD_Write_Com(0x0C);    /*��ʾ�����������*/
   }

void main()
{
	LCD_Init(); 
LCD_Clear();//����
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