/*------------------------------------------------------------
    �ļ����ƣ�TIMERl_INT.C 
    ���ܣ�16λ��ʱ��/������1���жϷ�ʽʹ�ã����Ʒ���������
    ˵���� ��ʱ��/������1��8MHz�£�ʱ��256��Ƶ����ʱʱ��0.5s 
    --------------------------------------------------------------*/
    #include <avr/io.h>
    #include <avr/io.h>
    #include <util/delay.h>
    #define DIG_CS1 PG3//en for duan
    #define DIG_CS2 PG4//en for time
    #define PB_MASK 0xFF
    #include <avr/interrupt.h>
	#include <math.h> 
    /********************************************
    �������ƣ�SIGNAL(SIG_OUTPUT_COMPARE1A)
    ���ܣ���ʱ��/������1�Ƚ�ƥ���жϵ��жϷ������
    ˵�������ü�����ֵ������PE3�˿�ȡ������/������
    ��ڲ�������
    ����ֵ����
    *********************************************/
uint8_t code[16] = { 0xC0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8, 0x80, 0x90, 0x88, 0x83, 0xc6, 0xa1, 0x86, 0x8e };
int tmp[8] = {0,0,0,0,0,0,0,0};
int hour = 0;
int min = 0;
int sec = 0;
int time = 0;
int ifsound = 0;
int ifbeep = 0;
void DIG_Init() {
    PORTB = ~PB_MASK;
    DDRB = PB_MASK;
    PORTG &= 0x00;
    DDRG |= _BV(DIG_CS1) | _BV(DIG_CS2);
}
void showchar(int pos, int c, int time){
	if (1 || tmp[pos] != c) {

	PORTG |= _BV(DIG_CS2);
	PORTG &= ~_BV(DIG_CS1);
	if (pos % 2 == 0) 
    	PORTB = code[c] & 0x7f;
	else
		PORTB = code[c];

	PORTG |= _BV(DIG_CS1);
	PORTG &= ~_BV(DIG_CS2);
    PORTB = 1<<(7-pos);

	PORTG &= ~_BV(DIG_CS1);
	PORTG &= ~_BV(DIG_CS2);
	}
	tmp[pos] = c;
    _delay_ms(time);
}
void print(int num, int mode, int time) {
	int c[7];
	int i,rest,tmp, s;
	tmp = num;
	int wait = 0;
	for (i=0; i <5; i++){
		if (wait == 1) {
			_delay_ms(time);
			continue;}
		rest = tmp % mode;
		showchar(i, rest, time);
		s = tmp / mode;
		if ( s == 0) {
			wait = 0;
		}
		tmp = s;
	}

}
int beep(double ms)
{


	if (TCNT1L % (int)(125*ms)){
		if (ifbeep){
			//PORTE &= ~(1 << PE3);
			ifbeep= 0;
		}
		else {
			ifbeep =1;
			//PORTE |= (1 << PE3);
		}
	}
	if (ifbeep)
		PORTE &= ~(1 << PE3);
	else
		PORTE |= (1 << PE3);
	return 0;
}
void sound(double freq)
{
int i=0;
double fff=500/freq;
beep(fff);
//while(i<=200)
//{beep(fff);
//++i;}
}
    SIGNAL(SIG_OUTPUT_COMPARE1A)
    {
      TCNT1H = 0x00;      //T/C1����ֵ����
      TCNT1L = 0x00;
      //��PE3ȡ����0.5��ı�һ��; x ^= 1;
      if(ifsound == 0){ // PORTE & _BV(PE3)
	  	ifsound=1;
		//		sound(523);
        //PORTE &= ~_BV(PE3);  //����͵�ƽ
		time++;
	}
      else {
	  	ifsound=0;
	    //PORTE |= (1<<PE3);   //����ߵ�ƽ
	  }
    } 
     //������
int main(void) 
   {
   DIG_Init();
      DDRE = _BV(PE3);   //��ʼ���˿�PE3Ϊ���ģʽ
      PORTE &= _BV(PE3); //����ߵ�ƽ����������ֹ����
      TCCR1B = 0x00;     //�жϿ��ƼĴ������㣬ֹͣT/Cl����  
      OCR1AH = 0xF4;     //����8MHz��256��Ƶ����ʱ0.5s�ıȽ�ֵ�� 
      OCR1AL = 0x24;     //��TCNTl�ļ���ֵ���бȽϣ���ƥ������ж�
      TCCR1A = 0x00;     //T/C1��ͨ�˿�ģʽ
      TCCR1B = 0x03;     //������ʱ��T/C1��256��Ƶ
      TIMSK |= 0x10;     //ʹ��C/Tl�Ƚ�ƥ���ж�
      sei();             //����ȫ���ж�
       PORTE = 0X08;
		DDRE = 0X08;
	  while(1){
	  	sec = time % 60;
		min = (time / 60) % 60;
		hour = (time / 60 / 60) % 60;
		hour * 10000 + min * 100 + sec;
	  	print(hour * 10000 + min * 100 + sec, 10,1);
		int k=0;
		for (;k<1;k++){
	  		if(ifsound) {
				sound(100);
			}
			else {
				PORTE |= (1<<PE3);   //����ߵ�ƽ
			}
		}
	  }          //�ȴ��жϵķ���
    }

