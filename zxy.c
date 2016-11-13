/*------------------------------------------------------------
    文件名称：TIMERl_INT.C 
    功能：16位定时器/计数器1，中断方式使用，控制蜂鸣器发声
    说明： 定时器/计数器1，8MHz下，时钟256分频，定时时间0.5s 
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
    函数名称：SIGNAL(SIG_OUTPUT_COMPARE1A)
    功能：定时器/计数器1比较匹配中断的中断服务程序
    说明：重置计数初值，并对PE3端口取反，开/关声响
    入口参数：无
    返回值：无
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
      TCNT1H = 0x00;      //T/C1计数值清零
      TCNT1L = 0x00;
      //对PE3取反，0.5秒改变一次; x ^= 1;
      if(ifsound == 0){ // PORTE & _BV(PE3)
	  	ifsound=1;
		//		sound(523);
        //PORTE &= ~_BV(PE3);  //输出低电平
		time++;
	}
      else {
	  	ifsound=0;
	    //PORTE |= (1<<PE3);   //输出高电平
	  }
    } 
     //主程序
int main(void) 
   {
   DIG_Init();
      DDRE = _BV(PE3);   //初始化端口PE3为输出模式
      PORTE &= _BV(PE3); //输出高电平，蜂鸣器禁止发声
      TCCR1B = 0x00;     //中断控制寄存器清零，停止T/Cl计数  
      OCR1AH = 0xF4;     //设置8MHz、256分频、定时0.5s的比较值， 
      OCR1AL = 0x24;     //与TCNTl的计数值进行比较，若匹配产生中断
      TCCR1A = 0x00;     //T/C1普通端口模式
      TCCR1B = 0x03;     //启动定时器T/C1，256分频
      TIMSK |= 0x10;     //使能C/Tl比较匹配中断
      sei();             //允许全局中断
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
				PORTE |= (1<<PE3);   //输出高电平
			}
		}
	  }          //等待中断的发生
    }

