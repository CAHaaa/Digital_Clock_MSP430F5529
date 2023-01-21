/*
 * oled.h
 *
 *  Created on: 2021��12��25��
 *      Author: IvanZheng
 */
#ifndef __OLED_H
#define __OLED_H
#include <msp430f5529.h>
#define  u8 unsigned char 
#define  u32 unsigned int 
#define OLED_CMD  0
#define OLED_DATA 1



#define     Set_Bit(val, bitn)      (val |= (/*1 <<*/(bitn)))
#define     Clr_Bit(val, bitn)      (val &= ~(/*1<<*/(bitn)))
#define     Get_Bit(val, bitn)      (val & (1<<(bitn)) )
//----------------------------------------------------------------------------------
//OLED SSD1306 SPI  D0\CLK

#define     OLED_SSD1306_SCLK_PIN_NUM       (BIT3)

#define     OLED_SSD1306_SCLK_IO_INIT       (Set_Bit(P3DIR,OLED_SSD1306_SCLK_PIN_NUM))
#define     OLED_SCLK_Set()                 (Set_Bit(P3OUT,OLED_SSD1306_SCLK_PIN_NUM))
#define     OLED_SCLK_Clr()             (Clr_Bit(P3OUT,OLED_SSD1306_SCLK_PIN_NUM))

//----------------------------------------------------------------------------------
//OLED SSD1306 SPI D1\MOSI

#define     OLED_SSD1306_SDIN_PIN_NUM       (BIT2)

#define     OLED_SSD1306_SDIN_IO_INIT       (Set_Bit(P3DIR,OLED_SSD1306_SDIN_PIN_NUM))
#define     OLED_SDIN_Set()             (Set_Bit(P3OUT,OLED_SSD1306_SDIN_PIN_NUM))
#define     OLED_SDIN_Clr()                 (Clr_Bit(P3OUT,OLED_SSD1306_SDIN_PIN_NUM))

//----------------------------------------------------------------------------------
//OLED SSD1306 DC

#define     OLED_SSD1306_DC_PIN_NUM         (BIT0)

#define     OLED_SSD1306_DC_IO_INIT         (Set_Bit(P3DIR,OLED_SSD1306_DC_PIN_NUM))
#define     OLED_DC_Set()               (Set_Bit(P3OUT,OLED_SSD1306_DC_PIN_NUM))
#define     OLED_DC_Clr()               (Clr_Bit(P3OUT,OLED_SSD1306_DC_PIN_NUM))

//----------------------------------------------------------------------------------
//OLED SSD1306 CS

#define     OLED_SSD1306_CE_PIN_NUM         (BIT2)

#define     OLED_SSD1306_CE_IO_INIT         (Set_Bit(P2DIR,OLED_SSD1306_CE_PIN_NUM))
#define     OLED_CS_Set()               (Set_Bit(P2OUT,OLED_SSD1306_CE_PIN_NUM))
#define     OLED_CS_Clr()               (Clr_Bit(P2OUT,OLED_SSD1306_CE_PIN_NUM))

//----------------------------------------------------------------------------------
//OLED SSD1306 RES

#define     OLED_SSD1306_RST_PIN_NUM        (BIT1)

#define     OLED_SSD1306_RST_IO_INIT        (Set_Bit(P3DIR,OLED_SSD1306_RST_PIN_NUM))
#define     OLED_RST_Set()              (Set_Bit(P3OUT,OLED_SSD1306_RST_PIN_NUM))
#define     OLED_RST_Clr()              (Clr_Bit(P3OUT,OLED_SSD1306_RST_PIN_NUM))

#define SIZE 16
#define XLevelL     0x02
#define XLevelH     0x10
#define Max_Column  130
#define Max_Row     64
#define Brightness  0xFF
#define X_WIDTH     130
#define Y_WIDTH     64

void delay_ms(unsigned int ms);




void OLED_WR_Byte(u8 dat,u8 cmd);
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Init(void);
void OLED_Clear(void);
void OLED_DrawPoint(u8 x,u8 y,u8 t);
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot);
void OLED_ShowChar(u8 x,u8 y,u8 chr);
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size2);
void OLED_ShowString(u8 x,u8 y, u8 *p);
void OLED_Set_Pos(unsigned char x, unsigned char y);
void OLED_ShowCHinese(u8 x,u8 y,u8 no);
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[]);
#endif  



