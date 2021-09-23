/*
 * test load cell.c
 *
 * Created: 9/20/2021 3:29:05 PM
 * Author : CHANDAN
 */ 




#define F_CPU                           (4000000UL)         /* using default clock 4MHz*/
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include "UART_1_AVR128DA64.h"

extern void Init_Hx711();
extern unsigned long ReadCount(void);
extern unsigned int Get_Weight();
extern void Get_OFFSET();

long HX711_Buffer = 0;
long Weight_OFFSET = 0,Weight_Real = 0;
int Weight = 0;
unsigned long M_Weight=0, ADC_Value=0;
unsigned long Count;

void Get_OFFSET()
{
	HX711_Buffer = ReadCount();
	Weight_OFFSET = HX711_Buffer;
}
unsigned int Get_Weight()
{
	HX711_Buffer = ReadCount();
	HX711_Buffer = HX711_Buffer;

	Weight_Real = HX711_Buffer;
	Weight_Real = Weight_Real - Weight_OFFSET;				//Get AD sampling value in kind .
	
	Weight_Real = (unsigned int)((float)Weight_Real/7.35+0.05);
	// Calculate the actual physical weight
	// Because different sensor characteristic curve is not the same, so that each sensor and to correct the divisor 4.30 here
	// When they find out of the test weight is too large, increase the value.
	// If the test out of the weight is too small, decrease the value change .
	// This value is generally about 7.16 . Because different sensors may be.
	// +0.05 Percentile to rounding

	return Weight_Real;
}

unsigned long ReadCount(void)
{
	unsigned long Count;
	unsigned char i;
	//PORTA.OUT |= (1 << 2); // DT AS HIGH
	PORTA.OUT &= ~(1 << 3); // SCK AS LOW
	//USART1_sendInt(Count);
	Count=0;
	while((PORTA.IN & (1 << 2)));
	for (i=0;i<24;i++)
	{
		PORTA.OUT |=(1 << 3);
		_delay_us(5);
		Count=Count<<1;
		PORTA.OUT &= ~(1 << 3);
		_delay_us(5);
		if((PORTA.IN & (1 << 2)))
		{
			Count++;
		}
	}
	
	PORTA.OUT |= (1 << 3);
	Count=Count^0x800000;
	PORTA.OUT &= ~(1 << 3);
	// USART1_sendInt(Count);
	return(Count);
}
int main(void)
{
	USART1_init(9600);
	// int adcVal;
	//ADC0_init();
	//ADC0_read( channel_3);
	//unsigned long Count=0;
	PORTA.DIR &= ~(1 << 2); //DT AS INPUT
	PORTA.DIR |= (1 << 3);  // SCK AS OUTPUT
	
	while (1)
	{
		
		//	USART1_sendString("Gram..");
		//Count = ReadCount();
		//USART1_sendInt(Count);
		
			ADC_Value=ReadCount();
		M_Weight=(ADC_Value-8401819)*0.00793457031;
// 		Weight = Get_Weight();
// 		////USART1_sendInt(Count);
		float wt =0;
	for(int i=1;i<=500;i++)
		{
		wt =wt+M_Weight;
	} 	
		wt =wt/500.00;
		wt =(wt-1520.00);
// 		
		USART1_sendFloat(wt,2 );
	//	USART1_sendInt(ADC_Value);
		
		
		_delay_ms(500);
	}

}


