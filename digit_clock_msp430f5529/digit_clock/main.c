#include <msp430.h> 
#include "keyboard.h"
#include "setclock.h"
#include "oled.h"
#define  ONE (!(P2IN&BIT0))
#define TWO (!(P1IN&BIT2))
#define THREE (!(P1IN&BIT3))
#define AP (!(P1IN&BIT4))
#define CALADC12_15V_30C *((unsigned int *)0x1A1A) // Temperature Sensor Calibration-30 C
//See device datasheet for TLV table memory mapping
#define CALADC12_15V_85C *((unsigned int *)0x1A1C) // Temperature Sensor Calibration-85 C
//全局变量
int tick_count=1;//调整时钟
int hour=0;//时分秒
int min=0;
int sec=0;
int temp1,temp2,temp3;
int date[4]={2023,1,17,2};// year  month day  date（week）
int is_24=1;
int is_am=1;
int alarm_cnt=0;
int beep=0;
int alarm_time_count=5;
int alarm_hour[3]={0};
int alarm_min[3]={0};
int alarm_sec[3]={0};
int timer_second=0;
int start_timer=0;
int enable_alarm=1;

unsigned int temp;
volatile float temperatureDegC;
volatile float temperatureDegF;
int temtemp = 0;
////////
void flash_write_int8(int *ptr, int value);//flash写入函数声明
void flash_clr(int *ptr);//flash清除函数声明
void read_flash_int1(unsigned int addr, int *array, int count);//读flash函数声明
void store_time(void);
void read_time(void);
void show_time(void);
void show_date(void);
void get_week(int y,int m,int d);
void store_date(void);
void read_date(void);
void set_alarm(int n);
void show_alarm_select_ui(void);
int is_alarm_beeping(void);
int max_day_in_month(int m);
/**
 * main.c
 */
int main(void)
{
    //初始化
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	init_key();//按键输入初始化上拉
	OLED_Init();
	P3DIR|=BIT6;//要求1，闪烁
	P3OUT|=BIT6;
	///////////闹钟闪烁设置
    P3DIR|=BIT5;
    P3OUT|=BIT5;
    P8DIR|=BIT2;
    P8OUT|=BIT2;
    P3DIR|=BIT7;
    P3OUT|=BIT7;
    P4DIR|=BIT0;
    P4OUT|=BIT0;
    P8DIR|=BIT1;
    P8OUT&=~BIT1;//闹钟指示

	///////////
	P1OUT &= ~BIT5;//键盘4个按键控制初始化
	P2OUT &= ~(BIT4+BIT5);
	P4OUT &= ~BIT3;

	//时钟与中断设置
	SetClock_MCLK12MHZ_SMCLK12MHZ_ACLK32_768K();
	UCSCTL5|=DIVS__32;    //使用USC统一时钟系统进行预分频，将SMCLK进行32分频
	TA0EX0|=TAIDEX_4;    ////A0计数器分频，5分频
	TA0CTL|= TASSEL_2+ID_2+MC_1+TACLR;    //配置A0计数器，4分频，时钟源SMCLK，上升模式，同时清除计数器
	TA0CCTL0|=CCIE;    //使能定时器中断（CCR0单源中断）
	TA0CCR0 = 20000;    //20000  捕获比较寄存器0设置值为9735    原：38940：1s    19470
	//store_time();

////////////////////////////////////////测试
	REFCTL0 &= ~REFMSTR; // Reset REFMSTR to hand over control to
	// ADC12_A ref control registers
	ADC12CTL0 = ADC12SHT0_8 + ADC12REFON + ADC12ON;
	// 设置参考电压为1.5V，打开AD
	ADC12CTL1 = ADC12SHP;           // 采样保持脉冲设置为内部定时器   采用Aclock 12MHz->4MHz
	ADC12MCTL0 = ADC12SREF_1 + ADC12INCH_10;//ADC12MCTL0 = ADC12SREF_1 + ADC12INCH_10; // AD的A10通道连接至温度传感器输出
	ADC12IE = 0x001; // ADC_IFG upon conv resultADCMEMO
	__delay_cycles(100);
	ADC12CTL0 |= ADC12ENC; // 使能AD
//////////////////////////////////////

	read_time();//开始之前先读入上次的时间
	read_date();
	get_week(date[0],date[1],date[2]);
	show_date();
	__bis_SR_register(GIE);//使能全局中断
	//主循环
	while(1){
	    show_time();
	    show_date();
	    if(!is_24){
	        if(is_am)OLED_ShowString(90,6,"AM");
	        else OLED_ShowString(90,6,"PM");
	    }
	    if(is_alarm_beeping()){
	        beep=1;
	       // __delay_cycles(1000000000);
	        //beep=0;
	    }
	    __bis_SR_register(GIE);//使能全局中断

	    /////////////////////////////////////////////////测试
	    ADC12CTL0 &= ~ADC12SC;
	    ADC12CTL0 |= ADC12SC; // 开始采样
	    __bis_SR_register(LPM4_bits); // LPM0 with interrupts enabled
	    __no_operation();
	    // Temperature in Celsius. See the Device Descriptor Table section in the
	    // System Resets, Interrupts, and Operating Modes, System Control Module
	    // chapter in the device user's guide for background information on the
	    // used formula.
	    temperatureDegC = (float)(((long)temp - CALADC12_15V_30C) * (85 - 30)) /(CALADC12_15V_85C - CALADC12_15V_30C) + 30.0f; //摄氏度换算
	    temtemp = temperatureDegC*100;
	    OLED_ShowString(1,4,"Temp=");
	    OLED_ShowString(57,4,".");
	    OLED_ShowNum(41,4,temtemp/100,2,16);
	    OLED_ShowNum(65,4,temtemp%100,2,16); //温度显示
	    // Temperature in Fahrenheit Tf = (9/5)*Tc + 32
	    temperatureDegF = temperatureDegC * 9.0f / 5.0f + 32.0f;//华氏度换算
	    __delay_cycles(6000000);
	    ///////////////////////////////////////////////////

	    /////////////////////////////////////////////////计时器模式

	    ////////////////////////////////////////////////mode1
	    if(AP){//设置时间模式
	        __delay_cycles(10000000);
	        while(1){
	            _DINT();
	            if(AP){
	                __delay_cycles(10000000);
	                break;//按A退出

	            }
	            else if(ONE){
	                __delay_cycles(10000000);
	                hour=hour+1;
	                if(hour>=24&&is_24)hour=0;
	                else if(hour>11&&(!is_24)&&is_am)is_am=0;
	                else if((!is_am)&&(!is_24)&&hour>=12)hour=1;
	                show_time();
	            }
	            else if(TWO){
	                __delay_cycles(10000000);
	                min+=1;
	                if(min>59)min=0;
	                show_time();
	            }
	            else if(THREE){
	                __delay_cycles(10000000);
	                sec+=1;
	                if(sec>59)sec=0;
	                show_time();
	            }
	        }

	    }
	    else if(TWO && THREE){
	               __delay_cycles(10000000);
	               enable_alarm=1-enable_alarm;
	               P8OUT^=BIT1;

	           }
	    ////////////////////mode2日期设置///////////
	    else if(THREE){
	        __delay_cycles(10000000);
	        while(1){

	            //_DINT();
	            if(AP){
	                __delay_cycles(10000000);
	                break;//按A退出
	            }
	            else if(TWO&&ONE){
	                __delay_cycles(10000000);
	                date[0]=date[0]-1;
	                get_week(date[0],date[1],date[2]);
	                show_date();
	            }
	            else if(ONE){
	                __delay_cycles(10000000);
	                date[0]=date[0]+1;
	                get_week(date[0],date[1],date[2]);
	                show_date();
	            }

	            else if(TWO){
	                __delay_cycles(10000000);
	                date[1]=date[1]+1;
	                if(date[1]>12)date[1]=1;
	                get_week(date[0],date[1],date[2]);
	                show_date();
	            }
	            else if(THREE){
	                __delay_cycles(10000000);
	                date[2]=date[2]+1;
	                if(date[2]>max_day_in_month(date[1]))date[2]=1;
	                get_week(date[0],date[1],date[2]);
	                show_date();
	            }
	        }
	    }

	    ///////////////////12/24小时制切换功能
	    else if(TWO){
	        _DINT();
	        __delay_cycles(10000000);
	        is_24=1-is_24;
	        if(!is_24){
	            //is_24=0;
	            //OLED_ShowString(0,3,"AMm");
	            if(hour>=12){
	                is_am=0;
	                hour=hour-12;
	            }
	            else is_am=1;

	            if(is_am)OLED_ShowString(90,6,"AM");
	            else OLED_ShowString(90,6,"PM");

	        }
	        else{
	            OLED_Clear();
	            //is_24=1;
	            if(!is_am){
	               hour=hour+12;
	            }
	        }
	        _EINT();
	    }
/////////////////////////////闹钟功能
	    else if(ONE){
	        _DINT();
	        __delay_cycles(30000000);
            if(ONE&&TWO){
                __delay_cycles(10000000);
                OLED_Clear();
                OLED_ShowChar(60,6,'s');
                while(1){
                    _EINT();
                    OLED_ShowNum(0,6,timer_second,4,16);
                    if(AP){
                        __delay_cycles(10000000);
                        OLED_Clear();
                        break;
                    }
                    if(ONE){//开始计时
                        start_timer=1;
                    }
                    if(TWO){//暂停
                        start_timer=0;
                    }
                    if(THREE){//reset
                        start_timer=0;
                        timer_second=0;
                    }
                }
            }

	        OLED_Clear();
	        //show_alarm_select_ui();
	        while(1){

	            if(AP){
	                __delay_cycles(10000000);
	                break;//按A退出
	            }
	            if(ONE){//第一个闹钟设置
	                set_alarm(0);
	            }
	            if(TWO){//2
	                set_alarm(1);
	            }
	            if(THREE){//3
	                set_alarm(2);
	            }
	        }

	    }

	}
	return 0;
}

//中断服务函数
#pragma vector=ADC12_VECTOR
__interrupt void ADC12ISR (void)
{
switch(__even_in_range(ADC12IV,34))
{
case 0: break; // Vector 0: No interrupt
case 2: break; // Vector 2: ADC overflow
case 4: break; // Vector 4: ADC timing overflow
case 6: // Vector 6: ADC12IFG0
temp = ADC12MEM0; // 读取结果，中断标志已被清除
__bic_SR_register_on_exit(LPM4_bits); // Exit active CPU
case 8: break; // Vector 8: ADC12IFG1
case 10: break; // Vector 10: ADC12IFG2
case 12: break; // Vector 12: ADC12IFG3
case 14: break; // Vector 14: ADC12IFG4
case 16: break; // Vector 16: ADC12IFG5
case 18: break; // Vector 18: ADC12IFG6
case 20: break; // Vector 20: ADC12IFG7
case 22: break; // Vector 22: ADC12IFG8
case 24: break; // Vector 24: ADC12IFG9
case 26: break; // Vector 26: ADC12IFG10
case 28: break; // Vector 28: ADC12IFG11
case 30: break; // Vector 30: ADC12IFG12
case 32: break; // Vector 32: ADC12IFG13
case 34: break; // Vector 34: ADC12IFG14
default: break;
}
}
#pragma vector = TIMER0_A0_VECTOR//TA0CCR0中断服务函数
__interrupt void TIMER0_A0_ISR(void)
{

    if(hour>=24)hour=0;
    if(beep&&enable_alarm){

        P3OUT^=BIT5;
        P8OUT^=BIT2;
        P3OUT^=BIT7;
        P4OUT^=BIT0;
        if(alarm_time_count==0){
            alarm_time_count=5;
            beep=0;
        }
        else alarm_time_count--;
    }
        if(tick_count==0){
            //闪烁
            P3OUT^=(BIT6);
            if(start_timer==1)timer_second++;
            //时分秒控制
            sec++;
            if(sec==60){
                sec=0;
                min++;
                if(min==60){
                    min=0;
                    hour++;
                    if(is_24&&hour>=24){
                        hour=0;
                    }
                    else if((!is_24)&&hour>12&&(!is_am)){
                        hour=1;
                    }
                    else if((!is_24)&&hour>11&&(!is_am)){
                        hour=0;
                        is_am=1-is_am;
                    }
                    else if((!is_24)&&hour>=12&&is_am){
                        is_am=1-is_am;
                    }
                }
            }


            //定时控制归位
            tick_count=1;
        }
        else{
            tick_count--;
        }
        store_time();
        _EINT();
        //store_date();
        //_EINT();


}


void flash_write_int8(int *ptr, int value)
{
     _DINT();
     FCTL3 = 0x0A500;          // Lock = 0 开锁
     FCTL1 = 0x0A540;          // Write = 1使能写入
    *((int *) ptr) = value;       // 写入数据
}
void flash_clr(int *ptr)
{
    _DINT();                            //关中断
    FCTL3 = 0x0A500;              //* Lock = 0 开锁
    FCTL1 = 0x0A502;              //* Erase = 1 使能擦除
    *((int *) ptr) = 0;           //* 擦除段
}
void read_flash_int1(unsigned int addr, int *array, int count)
{
    int *address = (int *)addr;
    int *temp=array;
    int i;
    for(i = 0; i<count; i++)
    {
        *array = *address++;
        array++;
    }
    array=temp;
}
void store_time(void){
    //flash_clr((int*)0x00187c);
    //flash_clr((int*)0x00187e);
    //flash_clr((int*)0x001880);
    //flash_clr((int*)0x001894);
    flash_clr((int*)0x001894);
    flash_write_int8((int*)0x001894,hour);
    flash_write_int8((int*)0x001896,min);
    flash_write_int8((int*)0x001898,sec);
    flash_write_int8((int*)0x0018A0,date[0]);
    flash_write_int8((int*)0x0018A2,date[1]);
    flash_write_int8((int*)0x0018A4,date[2]);
    flash_write_int8((int*)0x0018A6,date[3]);
}
void read_time(void){
    read_flash_int1(0x001894,&hour,1);
    read_flash_int1(0x001896,&min,1);
    read_flash_int1(0x001898,&sec,1);
    if(hour>=24){
        hour=0;
        min=0;
        sec=0;
    }
}
void show_time(void){
    OLED_ShowNum(10,6,hour,2,16);
    OLED_ShowChar(30,6,':');
    OLED_ShowNum(40,6,min,2,16);
    OLED_ShowChar(60,6,':');
    OLED_ShowNum(70,6,sec,2,16);

}
void show_date(void){
    OLED_ShowNum(0,1,date[0],4,20);
    OLED_ShowChar(40,1,'.');
    OLED_ShowNum(45,1,date[1],2,16);
    OLED_ShowChar(60,1,'.');
    OLED_ShowNum(70,1,date[2],2,16);
    OLED_ShowChar(90,1,'.');
    OLED_ShowNum(100,1,date[3],2,4);

}
void get_week(int y,int m,int d){

    if((m==1)||(m==2)){
        m=12+m;
        y=y-1;
    }
    int c=y/100;
    y=y%100;
    date[3]=(y+y/4+c/4-2*c+26*(m+1)/10+d-1)%7;////w=y+[y/4]+[c/4]-2c+[26(m+1）/10]+d-1
    if(date[3]==0)date[3]=7;
}

int max_day_in_month(int m){

    int cnt;
    switch(m){
    case 1:cnt=31;break;
    case 2:cnt=29;break;
    case 3:cnt=31;break;
    case 4:cnt=30;break;
    case 5:cnt=31;break;
    case 6:cnt=30;break;
    case 7:cnt=31;break;
    case 8:cnt=31;break;
    case 9:cnt=30;break;
    case 10:cnt=31;break;
    case 11:cnt=30;break;
    case 12:cnt=31;break;
    }
    return cnt;
}
void store_date(void){
    flash_clr((int*)0x0019A2);
    flash_write_int8((int*)0x0019A2,0x0001);
    flash_write_int8((int*)0x0019A4,5);
    flash_write_int8((int*)0x0019A6,5);
}
void read_date(void){
    read_flash_int1(0x0018A0,&date[0],1);
    read_flash_int1(0x0018A2,&date[1],1);
    read_flash_int1(0x0018A4,&date[2],1);
    read_flash_int1(0x0018A6,&date[3],1);
    if(date[0]>=3000){
        date[0]=2023;
        date[1]=1;
        date[2]=1;
    }

}
void show_alarm(int n){
    OLED_ShowNum(10,6,alarm_hour[n],2,16);
    OLED_ShowChar(30,6,':');
    OLED_ShowNum(40,6,alarm_min[n],2,16);
    OLED_ShowChar(60,6,':');
    OLED_ShowNum(70,6,alarm_sec[n],2,16);

}
void show_alarm_select_ui(void){
    OLED_ShowString(0,6,"Press1-3 to Select");
}
void set_alarm(int n){
                        __delay_cycles(10000000);
                        show_alarm(n);
                        while(1){
                            if(AP){
                                __delay_cycles(10000000);
                                OLED_Clear();
                                //show_alarm_select_ui();
                                break;
                            }
                            else if(ONE){
                                __delay_cycles(10000000);
                                alarm_hour[n]+=1;
                                if(alarm_hour[n]==24)alarm_hour[0]=0;
                                show_alarm(n);
                            }
                            else if(TWO){
                                __delay_cycles(10000000);
                                alarm_min[n]+=1;
                                if(alarm_min[n]==60)alarm_min[0]=0;
                                show_alarm(n);
                            }
                            else if(THREE){
                                __delay_cycles(10000000);
                                alarm_sec[n]+=1;
                                if(alarm_sec[n]==60)alarm_sec[0]=0;
                                show_alarm(n);
                            }
                        }
}
int is_alarm_beeping(void){
    int beep=0;
    if(hour==alarm_hour[0]&&min==alarm_min[0]&&sec==alarm_sec[0]){
        beep=1;
    }
    else if(hour==alarm_hour[1]&&min==alarm_min[1]&&sec==alarm_sec[1])beep=1;
    else if(hour==alarm_hour[2]&&min==alarm_min[2]&&sec==alarm_sec[2])beep=1;
    return beep;
}
