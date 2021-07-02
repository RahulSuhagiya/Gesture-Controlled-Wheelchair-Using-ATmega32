#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
 void senddata(unsigned char data)
 {
 	_delay_ms(2);
	PORTC |= 0x01;
	PORTD=data;
	PORTC |= 0x02;
	PORTC &= 0xFD;
 }
void sendcmd(unsigned char cmd)
 {
	_delay_ms(2);
	PORTC &= 0xFE;
	PORTD=cmd;
	PORTC |= 0x02;
	PORTC &= 0xFD;
 }
void printstr(char *s)
{
	uint8_t l,i;
	l = strlen(s);				// get the length of string
	for(i=0;i<l;i++)
	{
		senddata(*s);			// write every char one by one
		s++;
	}
}
void lcd_init()
  {
  	 DDRD = 0xFF;
	 DDRC = 0x03;
	 PORTC = 0x00;
	 PORTD = 0x00;
	 sendcmd(0x3E);
	 sendcmd(0x0E);
	 sendcmd(0x06);
	 sendcmd(0x01);
	 printstr("Gesture Control");
	 sendcmd(0xC3);
	 printstr("Wheel Chair");
	 _delay_ms(2000);
  }
void adcinitliz()
  {
  	ADMUX = (1<<REFS0);								// select reference for ADC
	ADCSRA =(0<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);	// select clock s
  }
unsigned int convert()
  {
  	unsigned int tmp1,tmp2,tmp3,t;
	tmp1 = (ADCL & 0x0F);
	tmp2 = (ADCL >> 4) & 0x0F;
	tmp3 = (ADCH & 0x0F);
	tmp2 = tmp2*16;
	tmp3 = tmp3*256;
	t = tmp1+tmp2+tmp3;
	return t;
  }

void main()
 {
 	unsigned int adc_value,ch_count=0,fwd_flag=0,rev_flag=0,right_flag=0,left_flag=0;
	unsigned int msg1_flag=0,msg1_count=0,msg2_flag=0,msg2_count=0,msg3_flag=0,msg3_count=0,msg4_flag=0,msg4_count=0,msg5_count=0;
	DDRB = 0xFF;
	PORTB = 0x00;
//	TCCR0=0x6B;
    lcd_init();
	adcinitliz();
	while(1)
	  {
		ch_count++;
		if(ch_count==1) ADMUX = 0x40 ;
		else if (ch_count==2) ADMUX = 0x41 ;
		else if (ch_count==3) ADMUX = 0x42;
		else if (ch_count==4) ADMUX = 0x43;
		ADCSRA = (1<<ADEN) | (1<<ADSC);
		while(!(ADCSRA & (1<<ADIF)));
		ADCSRA = (1<<ADIF);
		adc_value = convert();
		if(ch_count==1)
		  {
		  	if(adc_value>820)
			  {
			  	PORTB = 0x15;
				fwd_flag=1;
				msg1_flag = 1;
			  }
			else
			  	{fwd_flag=0;msg1_count=0;msg1_flag=0;}
		   }
		else if(ch_count==2)
		  {
		  	if(adc_value>820)
			  {
			  	PORTB = 0x21;
				left_flag=1;
				msg2_flag=1;
			  }
			else {left_flag=0;msg2_count=0;msg2_flag=0;}
		  }
		else if(ch_count==3)
		  {
		  	if(adc_value>820)
			  {
			  	PORTB = 0x44;
				right_flag=1;
				msg3_flag=1;
			  }
			else {right_flag=0;msg3_count=0;msg3_flag=0;}
		  }
		else if(ch_count==4)
		  {
		  	if(adc_value>820)
			  {
			  	PORTB = 0x8A;
				rev_flag=1;
				msg4_flag=1;
			  }
			else {rev_flag=0;msg4_count=0;msg4_flag=0;}
			ch_count=0;
		  }
		if((fwd_flag==0) && (rev_flag==0) && (left_flag==0) && (right_flag==0))
		  {
		  	PORTB = 0x00;
			msg1_flag=0;
		  }
		if(msg1_flag==1)
		{
		 if(msg1_count==0)
		   {
		   	  {
				sendcmd(0x01);
	            printstr("Wheel Chair is");
	            sendcmd(0xC0);
	            printstr("moving forward");
				//msg1_flag=0;
				msg1_count++;
				msg5_count=0;
			  }
		  }
        }
		else if(msg2_flag==1)
		{
		 if(msg2_count==0)
		   {
		   	  {
				sendcmd(0x01);
	            printstr("Wheel Chair is");
	            sendcmd(0xC0);
	            printstr("turning left");
				//msg2_flag=0;
				msg2_count++;
				msg5_count=0;
			  }
		  }
        }
	 else if(msg3_flag==1)
		{
		 if(msg3_count==0)
		   {
		   	  {
				sendcmd(0x01);
	            printstr("Wheel Chair is");
	            sendcmd(0xC0);
	            printstr("turning right");
				msg3_count++;
				msg5_count=0;
			  }
		  }
        }
     else if(msg4_flag==1)
		{
		 if(msg4_count==0)
		   {
		   	  {
				sendcmd(0x01);
	            printstr("Wheel Chair is");
	            sendcmd(0xC0);
	            printstr("moving reverse");
				//msg4_flag=0;
				msg4_count++;
				msg5_count=0;
			  }
		  }
        }
		else
		  {
			if(msg5_count==0)
		   	  {
				sendcmd(0x01);
	        	printstr("Wheel Chair");
	        	sendcmd(0xC0);
	        	printstr("is stop");
				msg5_count++;
		      }
		 }
	 }
  }

