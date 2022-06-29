/*
 * File:   LCD.h
 * Author: Kevin Herney Vente Angulo
 *
 * Created on 24 de junio de 2022, 10:02 PM
 */


#ifndef LCD_H
#define	LCD_H

#ifdef	__cplusplus
extern "C" {
#endif

   
#define RS LATD0  /*PIN 0 of PORTB is assigned for register select Pin of LCD*/
#define EN LATD1  /*PIN 1 of PORTB is assigned for enable Pin of LCD */
#define ldata LATD  /*PORTB(PB4-PB7) is assigned for LCD Data Output*/ 
#define LCD_Port TRISD  /*define macros for PORTB Direction Register*/
    
void LCD_Init();                   /*Initialize LCD*/
void LCD_Command(unsigned char );  /*Send command to LCD*/
void LCD_Char(unsigned char x);    /*Send data to LCD*/
void LCD_String(const char *);     /*Display data string on LCD*/
void LCD_String_xy(char, char , const char *);
void LCD_Clear();                  /*Clear LCD Screen*/


#ifdef	__cplusplus
}
#endif

#endif	/* LCD_H */
