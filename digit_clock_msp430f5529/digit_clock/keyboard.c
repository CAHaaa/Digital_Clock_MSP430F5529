/*
 * keyboard.c
 *
 *  Created on: 2021��12��25��
 *      Author: IvanZheng
 */
#include "msp430.h"
#include "keyboard.h"
unsigned  char KeyVal;
void init_key(void)
{
    //�������������
    //��Ϊ��� P1.5 P2.4 P2.5 P4.3
    P1DIR |= BIT5;
    P2DIR |= (BIT4+BIT5);
    P4DIR |= BIT3;
    //��Ϊ���� P2.0 P1.2 P1.3 P1.4
    P2DIR &= ~BIT0;
    P1DIR &= ~(BIT2+BIT3+BIT4);
    //�������������������������Ϊ�ߵ�ƽ �������¾ͻ���͵�ƽ ����Ϊ�ߵ�ƽ
    P2REN |=BIT0;
    P1REN |= (BIT2+BIT3+BIT4);
    P2OUT |=BIT0;
    P1OUT|=(BIT2+BIT3+BIT4);
}
unsigned short int key()
{
    unsigned char ReadData=0x00;
    //ɨ���һ��,��һ������͵�ƽ���ڶ�����������ߵ�ƽ
    P1OUT |= BIT5;
    P2OUT |= (BIT4+BIT5);
    P4OUT |= BIT3;
    P1OUT &= ~BIT5;
    //ֻ������λ
    ReadData= ((P2IN | ~BIT0) & (P1IN | ~(BIT2+BIT3+BIT4)))^0xff;
    switch(ReadData)
    {
    //��һ��
    case 0x01:
    KeyVal=1;
    _delay_cycles(300000);
    return KeyVal;
    //�ڶ���
    case 0x04:
    KeyVal=2;
    _delay_cycles(300000);
    return KeyVal;
    //������
    case 0x08:
    KeyVal=3;
    _delay_cycles(300000);
    return KeyVal;
    //������
    case 0x10:
    KeyVal=4;
    _delay_cycles(300000);
    return KeyVal;
    default:
    KeyVal=0;
    break;
   }
    ReadData=0x00;
    P1OUT |= BIT5;
    P2OUT |= (BIT4+BIT5);
    P4OUT |= BIT3;
    //ɨ��ڶ���,�ڶ�������͵�ƽ����һ����������ߵ�ƽ
    P2OUT &=~BIT4;
    //ֻ������λ
    ReadData= ((P2IN | ~BIT0) & (P1IN | ~(BIT2+BIT3+BIT4)))^0xff;
    switch(ReadData)
    {
    //��һ��
    case 0x01:
    KeyVal=5;
    _delay_cycles(300000);
    return KeyVal;
    //�ڶ���
    case 0x04:
    KeyVal=6;
    _delay_cycles(300000);
    return KeyVal;
    //������
    case 0x08:
    KeyVal=7;
    _delay_cycles(300000);
    return KeyVal;
    //������
    case 0x10:
    KeyVal=8;
    _delay_cycles(300000);
    return KeyVal;
    default:
    KeyVal=0;
    break;
    }
    ReadData=0x00;
    P1OUT |= BIT5;
    P2OUT |= (BIT4+BIT5);
    P4OUT |= BIT3;
    //ɨ�������,����������͵�ƽ����һ����������ߵ�ƽ
    P2OUT &=~BIT5;
    //ֻ������λ
    ReadData= ((P2IN | ~BIT0) & (P1IN | ~(BIT2+BIT3+BIT4)))^0xff;
    switch(ReadData)
    {
    //��һ��
    case 0x01:
    KeyVal=9;
    _delay_cycles(300000);
    return KeyVal;
    //�ڶ���
    case 0x04:
    KeyVal=10;
    _delay_cycles(300000);
    return KeyVal;
    //������
    case 0x08:
    KeyVal=11;
    _delay_cycles(300000);
    return KeyVal;
    //������
    case 0x10:
    KeyVal=12;
    _delay_cycles(300000);
    return KeyVal;
    default:
    KeyVal=0;
    break;
    }
    ReadData=0x00;
    P1OUT |= BIT5;
    P2OUT |= (BIT4+BIT5);
    P4OUT |= BIT3;
    //ɨ�������,����������͵�ƽ����һ����������ߵ�ƽ
    P4OUT &= ~BIT3;
    //ֻ������λ
    ReadData= ((P2IN | ~BIT0) & (P1IN | ~(BIT2+BIT3+BIT4)))^0xff;
    switch(ReadData)
    {
    //��һ��
    case 0x01:
    KeyVal=13;
    _delay_cycles(300000);
     return KeyVal;
    //�ڶ���
    case 0x04:
    KeyVal=14;
    _delay_cycles(300000);
    return KeyVal;
    //������
    case 0x08:
    KeyVal=15;
    _delay_cycles(300000);
    return KeyVal;
    //������
    case 0x10:
    KeyVal=16;
    _delay_cycles(300000);
    return KeyVal;
    default:
    KeyVal=0;
    break;
    }


    return KeyVal;

}




