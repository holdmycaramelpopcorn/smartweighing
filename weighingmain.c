#include <lpc214x.h>
#include <stdio.h>
#include "LCD.h"
#include "matrix.h"
#define uint16_t unsigned int
	
typedef struct
{
	unsigned char sec;
  unsigned char min;
  unsigned char hour;
  unsigned char weekDay;
  unsigned char date;
  unsigned char month;
  unsigned int year;  
}rtc_t;

void SystemInit(void);//initialize CCLK and PCLK
void Board_Init(void);//initialize GPIO
void uart_init(void); 
void RTC_Init(void);
void timer1_Init(void);// generates interrupt every 1sec
void delay(int cnt);
void RTC_SetDateTime(rtc_t *rtc);
void RTC_GetDateTime(rtc_t *rtc);
unsigned int adc(int no,int ch);
void serialPrint(unsigned val);//print int on serialport
void serialPrintStr(char * buf);//print string on serialport
unsigned int matrixentry(unsigned int);

//global variables
rtc_t rtc; // declare a variable to store date,time
#define RELAY_ON (IO0SET = 1 << 11)
#define RELAY_OFF (IO0CLR = 1 << 11)
unsigned  int x=0;
unsigned int lookup_table[4][4]={{0,1,2,3},
																{4,5,6,7},
																{8,9,10,11},
																{12,13,14,15}};
unsigned char items[][50]={{"Okra"},{"Potato"},{"Tomato"},{"Brinjal"},{"Radish"}};
unsigned int price[]={40,12,10,60,30};
unsigned int rowsel=0,colsel=0;
// ISR Routine to blink LED D7 to indicate project working
__irq   void  Timer1_ISR(void)
 {
	x = ~x;//x ^ 1;
  if (x)   
    IO0SET  =  1u << 31;   //P0.31  =  1
  else   
    IO0CLR =   1u <<31;   // P0.31  = 0	 
	T1IR  =  0x01; // clear match0 interrupt, and get ready for the next interrupt
  VICVectAddr = 0x00000000 ; //End of interrupt 
 }
void Board_Init(void)
{
	
	//IO0DIR |= 1 << 11; // RELAY IS CONNECTED TO P0.11
	IO0DIR |= 1U<<15 | 1U << 31 | 0x00FF0000; // to set P0.15 to P0.23 as o/ps ,
  IO1DIR |= 1U << 25;	              // to set P1.25 as o/p used for EN
                                    // make D7 Led (P0.31) on off for testing
}
unsigned int adc(int no,int ch)
{
	unsigned int val;
	PINSEL0 |=  0x0F300000;   
	/* Select the P0_13 AD1.4 for ADC function */
  /* Select the P0_12 AD1.3 for ADC function */
	/* Select the P0_10 AD1.2 for ADC function */
  switch (no)        //select adc
    {
        case 0: AD0CR   = 0x00200600 | (1<<ch); //select channel
                AD0CR  |= (1<<24) ;            //start conversion
                while ( ( AD0GDR &  ( 1U << 31 ) ) == 0);
                val = AD0GDR;
                break;
 
        case 1: AD1CR = 0x00200600  | ( 1 << ch );       //select channel
                AD1CR |=  ( 1 << 24 ) ;                              //start conversion
                while ( ( AD1GDR & (1U << 31) ) == 0);
                val = AD1GDR;
                break;
    }
    val = (val  >>  6) & 0x03FF;         // bit 6:15 is 10 bit AD value
    return  val;
}

void RTC_Init(void)
{
   //enable clock and select external 32.768KHz
	   CCR = ((1<< 0 ) | (1<<4));//D0 - 1 enable, 0 disable
} 														// D4 - 1 external clock,0 from PCLK

// SEC,MIN,HOUR,DOW,DOM,MONTH,YEAR are defined in LPC214x.h
void RTC_SetDateTime(rtc_t *rtc)//to set date & time
{
     SEC   =  rtc->sec;       // Update sec value
     MIN   =  rtc->min;       // Update min value
     HOUR  =  rtc->hour;      // Update hour value 
     DOW   =  rtc->weekDay;   // Update day value 
     DOM   =  rtc->date;      // Update date value 
     MONTH =  rtc->month;     // Update month value
     YEAR  =  rtc->year;      // Update year value
}

void RTC_GetDateTime(rtc_t *rtc)
{
     rtc->sec     = SEC ;       // Read sec value
     rtc->min     = MIN ;       // Read min value
     rtc->hour    = HOUR;       // Read hour value 
     rtc->weekDay = DOW;      // Read day value 
     rtc->date    = DOM;       // Read date value 
     rtc->month   = MONTH;       // Read month value
     rtc->year    = YEAR;       // Read year value

}
void SystemInit(void)
{
   PLL0CON = 0x01; 
   PLL0CFG = 0x24; 
   PLL0FEED = 0xAA; 
   PLL0FEED = 0x55; 
   while( !( PLL0STAT & 0x00000400 ))
   { ; }
   PLL0CON = 0x03;
   PLL0FEED = 0xAA;  // lock the PLL registers after setting the required PLL
   PLL0FEED = 0x55;
   VPBDIV = 0x01;      // PCLK is same as CCLK i.e 60Mhz  
}
void uart_init(void)
{
 //configurations to use serial port
 PINSEL0 |= 0x00000005;  // P0.0 & P0.1 ARE CONFIGURED AS TXD0 & RXD0
 U0LCR = 0x83;   /* 8 bits, no Parity, 1 Stop bit    */
 U0DLM = 0; U0DLL = 8; // 115200 baud rate,PCLK = 15MHz
 U0LCR = 0x03;  /* DLAB = 0 	*/
 U0FCR = 0x07;
}
void timer1_Init()
{
	T1TCR = 0X00;
	T1MCR = 0X03;  //011 
	T1MR0 = 150000;
	T1TC  = 0X00;
	VICIntEnable = 0x0000020;  //00100000 Interrupt Souce No:5, D5=1
	VICVectAddr5 = (unsigned long)Timer1_ISR;  // set the timer ISR vector address
	VICVectCntl5 = 0x0000025;  // set the channel,D5=1,D4-D0->channelNo-5
	T1TCR = 0X01;
}
void serialPrint(unsigned val)
{
	int i=0;
	unsigned char buf[50],ch;
	sprintf((char *)buf,"%d\x0d\xa",val);
	while((ch = buf[i++])!= '\0')
	  {
		while((U0LSR & (0x01<<5))== 0x00){}; 
    U0THR= ch;                         
	  }
}
void serialPrintStr(char * buf)
{
	int i=0;
	char ch;
	while((ch = buf[i++])!= '\0')
	  {
		  while((U0LSR & (1u<<5))== 0x00){}; 
      U0THR= ch;   
	  }
	//send new line
	//while(U0LSR & (0x01<<5)){};U0THR = 13;
	//while(U0LSR & (0x01<<5)){};U0THR = 10;	
	
}
void delay(int cnt)
{
	T0MR0 = 1000;//14926; // some arbitrary count for delay
	T0MCR = 0x0004; // set Tiimer 0 Stop after Match
	while(cnt--)
	{
		T0TC = 0X00;
	  T0TCR = 1; // start the timer (enbale)
		while(!(T0TC == T0MR0)){};// wait for the match
	  T0TCR = 2;// stop the timer		
	}
}
unsigned int matrixentry(unsigned int choice)
{
	unsigned int ans=0,i=0,x=0;
	do
	{
		while(1)
		{
			rowsel=0;
			IO0SET=0x00F00000;
			IO0CLR=1<<20;
			if(COL0==0){colsel=0;break;}; if(COL1==0){colsel=1;break;};
			if(COL2==0){colsel=2;break;}; if(COL3==0){colsel=3;break;};
			rowsel=1;
			IO0SET=0x00F00000;
			IO0CLR=1<<21;
			if(COL0==0){colsel=0;break;}; if(COL1==0){colsel=1;break;};
			if(COL2==0){colsel=2;break;}; if(COL3==0){colsel=3;break;};
			rowsel=2;
			IO0SET=0x00F00000;
			IO0CLR=1<<22;
			if(COL0==0){colsel=0;break;}; if(COL1==0){colsel=1;break;};
			if(COL2==0){colsel=2;break;}; if(COL3==0){colsel=3;break;};
			rowsel=3;
			IO0SET=0x00F00000;
			IO0CLR=1<<23;
			if(COL0==0){colsel=0;break;}; if(COL1==0){colsel=1;break;};
			if(COL2==0){colsel=2;break;}; if(COL3==0){colsel=3;break;};
		};
		delay_ms(50);
		while(COL0==0||COL1==0||COL2==0||COL3==0);
		delay_ms(50);
		IO0SET = 0x00F00000;
		x=lookup_table[rowsel][colsel];
		if(x<=9)
		ans=(ans*10)+x;
	}while(x!=15||choice!=1);
	if(x==13)
		return x;
	return ans;
}
int main() 
{
    char msg[100];
	unsigned char *item;
	  unsigned int wt,cost,total;
	unsigned int ch=0,pinent=0,newcost=0;
  
 // initialize the peripherals & the board GPIO
	
    Board_Init();
    SystemInit();
    uart_init();
	  RTC_Init();
	  timer1_Init(); 
	  LCD_Reset();
		LCD_Init();

	// set date & time 
	
		rtc.hour = 11;rtc.min =  00;rtc.sec =  00;//11:00:00hrs
    rtc.date = 11;rtc.month = 06;rtc.year = 2020;//11th June 2020
    RTC_SetDateTime(&rtc);  // comment this line after first use
	 
	
	  while(1)
    {
      RTC_GetDateTime(&rtc);//get current date & time stamp
			sprintf((char *)msg,"time:%2d:%2d:%2d  Date:%2d/%2d/%2d \x0d\xa",(uint16_t)rtc.hour,(uint16_t)rtc.min,(uint16_t)rtc.sec,(uint16_t)rtc.date,(uint16_t)rtc.month,(uint16_t)rtc.year);
			// use the time stored in the variable rtc for date & time stamping
			serialPrintStr((char*)msg);
			LCD_CmdWrite(0x80); LCD_DisplayString((char*)msg);
			delay(2000);
			sprintf((char *)msg,"Enter 1 to bill and 2 to change rate");
				LCD_CmdWrite(0xC0); LCD_DisplayString((char*)msg);
				delay(2000);
			ch=matrixentry(1);
			if(ch==1)
			{
				while(1)
				{
		  //Weight Measurement
			IO0DIR|=1<<13;
			IO0SET|=1<<13;
			wt=adc(1,3);
			delay_ms(1);
			IO0CLR|=1<<13;
			sprintf((char *)msg,"Enter Item Code");
			LCD_CmdWrite(0x94); LCD_DisplayString((char*)msg);
			delay(2000);
			pinent=matrixentry(1);
			if(pinent<=4)
			{
				item=items[pinent];
				cost=price[pinent]*wt;
				sprintf((char*)msg,"%s - wt:%d - price:%d",item,wt,cost);
				serialPrintStr((char*)msg);
				LCD_CmdWrite(0xD4); LCD_DisplayString((char*)msg);
				delay(2000);
			}
			else
			{
				sprintf((char *)msg,"Wrong Entry");
				serialPrintStr((char*)msg);
				LCD_CmdWrite(0xD4); LCD_DisplayString((char*)msg);
				delay(2000);
			}
			pinent=matrixentry(1);
			if(pinent==13)
			{
				total+=cost;
				sprintf((char *)msg,"Total: %d",total);
				serialPrintStr((char*)msg);
				LCD_CmdWrite(0xC0); LCD_DisplayString((char*)msg);delay(2000);
			}
			else if(pinent==12)
			{
				total+=cost;
				if(total>=500)
					total-=100;
				if(total>=450&&total<500)
				{sprintf((char *)msg,"Buy for %d more to avail discount",(500-total));
					serialPrintStr((char*)msg);
					LCD_CmdWrite(0xC0); LCD_DisplayString((char*)msg);delay(2000);
					pinent=matrixentry(1);
					if(pinent==12)
					{
						sprintf((char *)msg,"Total cost is %d",total);
					serialPrintStr((char*)msg);
					LCD_CmdWrite(0x94); LCD_DisplayString((char*)msg);delay(2000);
						sprintf((char *)msg,"Thank you for shopping.");
					LCD_CmdWrite(0xD4); LCD_DisplayString((char*)msg);delay(2000);
						break;
					}
					else
						continue;
				}
					break;
			}
		}
	}
			if(ch==2)
			{
				sprintf((char *)msg,"Enter Item Code");
			LCD_CmdWrite(0x94); LCD_DisplayString((char*)msg);
			delay(2000);
			pinent=matrixentry(1);
			if(pinent<=4)
			{
				item=items[pinent];
				cost=price[pinent];
				sprintf((char*)msg,"%s - price:%d",item,cost);
				LCD_CmdWrite(0xD4); LCD_DisplayString((char*)msg);
				delay(2000);
				newcost=matrixentry(2);
				price[pinent]=newcost;
			}
			else
			{
				sprintf((char *)msg,"Wrong Entry");
				LCD_CmdWrite(0xD4); LCD_DisplayString((char*)msg);
				delay(2000);
			}
			}
					
		}
	}
