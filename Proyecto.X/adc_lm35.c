/*
 * File:   adc_lm35.c
 * Author: vente
 *
 * Created on 14 de junio de 2022, 09:41 AM
 */


#include <xc.h>
#include <pic18f4550.h>
#include "config.h"
#include <stdio.h>
#include <string.h>
#include "lcd.h"

unsigned int value_adc = 0;
unsigned char counter_digito = 0;
double temp = 0.0;
unsigned char lm35 = 0;

char Temperature[10];
char Raw[10];

void main(void) {

    OSCCONbits.IRCF = 0b111; //Configura oscilador interno (FOSC = 8Mhz)
    OSCCONbits.SCS = 0b10; // Oscilador del sistema = Fosc interno

   // TRISD = 0x00;    
    TRISA = 0xFF;
    
    LCD_Init();
    LCD_String_xy(0,0,"Value Analogico");
    LCD_Command(0xC0);
    
    //ADC = 10bits, Tad = 1us, Tacq = 4us, Vref = 5v-0v, RA=ANALOG
    ADCON1bits.PCFG = 0b1110; //  Configura el Puerto como Entrada Analógica.
    ADCON1bits.VCFG = 0b00; //  Selecciona Voltajes de Referencia (5v-0v).
    ADCON0bits.CHS = 0b0000; //  Selecciona el Canal Analógico.
    ADCON2bits.ACQT = 0b010; //  Tiempo de Adquisición 4Tad.
    ADCON2bits.ADCS = 0b001; //  Tiempo de Conversión Fosc/8.
    ADCON2bits.ADFM = 1; //  Justificación derecha (modo-10bits).
    ADCON0bits.ADON = 1; //  Habilita el Módulo AD.

    while (1) {
        ADCON0bits.GO_DONE = 1; //Inicia la COnversió AD.
        while (ADCON0bits.GO_DONE); //  Espera a que termine la conversión AD.
        value_adc = ADRESH; //  Lectura de valor AD.
        value_adc = (value_adc << 8) + ADRESL;
        temp = value_adc;
        temp = (temp * 500.0) / 1023.0;
        lm35 = (char) temp;
        
        __delay_ms(2000);
        sprintf(Temperature,"%d ", (int)lm35);
        LCD_String_xy(2,0,"TEMP:");
        LCD_String(Temperature);
        
        sprintf(Raw,"%d ", (int)value_adc);
        LCD_String_xy(2,9,"R:");
        LCD_String(Raw);
        
    }
}