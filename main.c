
#define ADD_WRITE 0b11010000
#define ADD_READ 0b11010001
#define F_CPU 1000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <I2C_lib.h>
#include <avr/interrupt.h>

int s = 0;
int m = 0;
int h = 0;



unsigned char RTC_ConvertFromDec(unsigned char c)
{
	unsigned char ch = ((c>>4)*10+(0b00001111&c));
	return ch;
}

unsigned char RTC_ConvertFromBinDec(unsigned char c)
{
	unsigned char ch = ((c/10)<<4)|(c%10);
	return ch;
}

void DS1307_set_time(char sec, char min, char hr, char dweek, char date, char mon, char yr)
{
	
	I2C_start();
	I2C_send_data(0b11010000);
	I2C_send_data(0);
	I2C_send_data(RTC_ConvertFromBinDec(sec));
	I2C_send_data(RTC_ConvertFromBinDec(min));
	I2C_send_data(RTC_ConvertFromBinDec(hr));
	I2C_send_data(RTC_ConvertFromBinDec(dweek));
	I2C_send_data(RTC_ConvertFromBinDec(date));
	I2C_send_data(RTC_ConvertFromBinDec(mon));
	I2C_send_data(RTC_ConvertFromBinDec(yr));
	I2C_stop();
}

int DS1307_read_time(int k)
{
	I2C_start();
	I2C_send_data(0b11010000);
	I2C_send_data(0);
	I2C_stop();
	
	I2C_start();
	I2C_send_data(0b11010001);
	s = I2C_read_data();
	m = I2C_read_data();
	h = I2C_read_data();
	int dw = I2C_read_data();
	int dm = I2C_read_data();
	int mo = I2C_read_data();
	int y = I2C_read_data();
	I2C_stop();
	
	s = RTC_ConvertFromDec(s); //Преобразуем в десятичный формат
	m = RTC_ConvertFromDec(m);
	h = RTC_ConvertFromDec(h);
	dw = RTC_ConvertFromDec(dw);
	dm = RTC_ConvertFromDec(dm);
	mo = RTC_ConvertFromDec(mo);
	y = RTC_ConvertFromDec(y);
	
	dw = 0; dm = 0; mo = 0; y = 0;
	
	switch(k)
	{
		case 0:
		return s;
		break;
		
		case 1:
		return m;
		break;
		
		case 2:
		return h;
		break;
		
		case 3:
		return dw;
		break;
		
		case 4:
		return dm;
		break;
		
		case 5:
		return mo;
		break;
		
		case 6:
		return y;
		break;
		
		default:
		return 0;
	}
}

void chislo (int a) 
{
	switch (a)
	{
		case 0:
		PORTD &= ~((1<<6) | (1<<7) | (1<<5) | (1<<4));
		break;
		
		case 1:
		PORTD &= ~((1<<6) | (1<<7) | (1<<5));
		PORTD |= (1<<4);
		break;
		
		case 2:
		PORTD &= ~((1<<7) | (1<<5));
		PORTD |= (1<<6) | (1<<4);
		break;
		
		case 3:
		PORTD &= ~((1<<7) | (1<<5) | (1<<4));
		PORTD |= (1<<6);
		break;
		
		case 4:
		PORTD &= ~((1<<6) | (1<<5) | (1<<4));
		PORTD |= (1<<7);
		break;
		
		case 5:
		PORTD &= ~((1<<6) | (1<<5));
		PORTD |= (1<<4) | (1<<7);
		break;
		
		case 6:
		PORTD &= ~((1<<5) | (1<<4));
		PORTD |= (1<<6) | (1<<7);
		break;
		
		case 7:
		PORTD &= ~(1<<5);
		PORTD |= (1<<6) | (1<<7) | (1<<4);
		break;
		
		case 8:
		PORTD &= ~((1<<6) | (1<<7) | (1<<4));
		PORTD |= (1<<5);
		break;
		
		case 9:
		PORTD |= (1<<5) | (1<<4);
		PORTD &= ~((1<<6) | (1<<7));
		break;
		
		default: 
		PORTD &= ~((1<<6) | (1<<7) | (1<<5) | (1<<4));
		
			//PD4 - X1
			//PD5 - X4
			//PD6 - X2
			//PD7 - X3
		
	}
}

void out (int port, int num)
{
	PORTB |= (1 << port);
	chislo(num);
	_delay_ms(3);
	PORTB &= ~(1 << port);
	_delay_us(100);
}

ISR (INT0_vect)
{
	DDRD &= ~(1<<2);
	PORTD |= (1<<2); 
	DDRC &= ~((1<<1) | (1<<0));
	PORTC |= (1<<1) | (1<<0);
	h = 0;
	m = 0;
	int j = 0;
	
    while(~PIND & (1<<2))
	{
		if ((~PINC & (1<<1)) && (~PINC & (1<<0)))
		{
			j++;
			_delay_ms(1000);
			
			if (j == 3)
			{
				j = 0;
			}
		}
		
		if (j == 0)
		{
			if (~PINC & (1<<1))
			{
				h++;
				_delay_ms(100);
			}
			
			if (~PINC & (1<<0))
			{
				h--;
				_delay_ms(100);
			}
			
			if (h < 0)
			{
				h = 23;
			}
			
			if (h == 24)
			{
				h = 0;
			}
			
			out(0, h/10);
			out(3, h%10);
		}
		
		if (j == 1)
		{
			if (~PINC & (1<<1))
			{
				m++;
				_delay_ms(100);
			}
			
			if (~PINC & (1<<0))
			{
				m--;
				_delay_ms(100);
			}
			
			if (m < 0)
			{
				m = 59;
			}
			
			if (m == 60)
			{
				m = 0;
			}
			DS1307_set_time(0, m, h, 0, 0, 0, 0);
			out(0, h/10);
			out(3, h%10);
			out(1, m/10);
			out(2, m%10);
		}
	}
}


int main(void)
{

	
	TCCR0 |= (1<<CS02) | (1<<CS00);
	TCCR0 &= ~((1<<CS01));
	
	DDRB |= (1<<0) | (1<<1) | (1<<2) | (1<<3);
	DDRD |= (1<<6) | (1<<7) | (1<<5) | (1<<4);
	PORTD |= (1<<2); 
	GICR |= (1<<INT0);
	MCUCR |= (1<<ISC01);
	MCUCR &= ~(1<<ISC00);
	sei();
	
	int z = 0;

    while (1) 
    {
		
		s = DS1307_read_time(0);
		m = DS1307_read_time(1);
		h = DS1307_read_time(2);
		
		while (z < 301)
		{
			out(0, h/10);
			out(3, h%10);
			out(1, m/10);
			out(2, m%10);
			z++;
			
			if (m == 10 || m == 20 || m == 30 || m == 40 || m == 50 || m == 0 || m == 5 || m == 15 || m == 25 || m == 35 || m == 45 || m == 55)
			{
				if (s < 25)
				{
					TCNT0 = 0;
					int o = 0;
					
					if (TCNT0 == 255)
					{
						o++;
					}
					
					for (int y = 9; y >= 0; y--)
					{
						if (o < 300)
						{
							out(0, y);
							out(2, y);
							out(1, y);
							out(3, y);
						}
						
						if (o == 300)
						{
							o = 0;
						}
					}
				}
			}
			
		}
		
		if (z == 301)
		{
			z = 0;
		}
    }
}

