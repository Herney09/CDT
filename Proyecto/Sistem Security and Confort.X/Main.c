/*
 * File:   Main.c
 * Author: Kevin Herney Vente Angulo
 *
 * Created on 24 de junio de 2022, 10:02 PM
 */


#include <xc.h>
#include <pic18f4550.h>  /*Header file PIC18f4550 definitions*/
#include <stdio.h>
#include <string.h>
#include "CONFIG.h"
#include "LCD.h"
#include "KEYPAD.h"

#define _XTAL_FREQ 8000000

//Variables del Sistema de Security
unsigned char password[5] ={'2','0','2','2',0};
unsigned char pass_user[5];
unsigned char idx = 0;
unsigned char estado;
char buffer [3];
char key = '0';
int Con_Error = 0;
int Contador = 0;


//Variables del Sistema de Confort
unsigned int value_adc = 0;
unsigned char counter_digito = 0;
double temp = 0.0;
unsigned char lm35 = 0;
char Temperature[10];
char Raw[10];

//definicion de funciones
void Security(void);
void Confort(void);


void main(void) {
    
    //Configura Fosc = 8Mhz interno, Fuente de Fosc del sistema = interno
    OSCCON = 0x72; //Configura oscilador interno (FOSC = 8Mhz)
    TRISE = 0b000;
     
    LCD_Init(); //Inicializa el LCD
    keypad_init();
    
    Security(); // llamamos a la funcion Security  
}

void Security(void){
    while(1) {
        
        Contador = 2 - Con_Error;
        
        LCD_String_xy(0,1,"Digite Su Clave");
        LCD_Command(0xC0); 
        do{
            key = keypad_getkey();
            if(key != 0){
                //LCD_Char('*');/* display pressed key on LCD16x2 */
                LCD_Char(key);/* display pressed key on LCD16x2 */
                pass_user[idx++] = key;
            }
            __delay_ms(100);
        }while(idx < 5);
        
        if(strncmp(pass_user,password,4)==0){
            LCD_Clear();
            LCD_String_xy(0,0,"Acceso Concedido");
            LCD_String_xy(2,3,"Bienvenido");
            PORTE = 0b10; // se enciente el led verde si la contraseña es correcta
            __delay_ms(3000);
            PORTE = 0b00; //despues de 3 segundos se apaga el led
            __delay_ms(500);
            Confort(); // llamando a funcion de Confort
            Con_Error = 0; //inicializa en contador de error en cero si la contraseña es correcta
        }else{
            LCD_String_xy(0,1,"Clave Invalida ");
            PORTE = 0b01; //se enciente el led Rojo si la contraseña es incorrecta
            __delay_ms(3000);
            PORTE = 0b00; //despues de 3 segundos se apaga el led
            Con_Error = Con_Error + 1; //cuenta lacantidad de contraseña incorrecta 
            
            if(Con_Error == 3){
                LCD_String_xy(0,0,"Clave Bloqueada");
                __delay_ms(15000); // despues que se bloquea la clave espera 15 segundos para volver a solicitarla
                Con_Error = 0; //inicializa en contador de error en cero si la clave se bloquea
            }else{
                sprintf(buffer,"%d ", (int)(Contador));
                LCD_String_xy(2,0,"Intentos:"); // imprime numero de intentos para que la clave se bloquee
                LCD_String(buffer);
            }
        }

        __delay_ms(1000);
        LCD_Clear();
        idx = 0;
    }
}

void Confort(void){
    
    OSCCONbits.IRCF = 0b111; //Configura oscilador interno (FOSC = 8Mhz)
    OSCCONbits.SCS = 0b10; // Oscilador del sistema = Fosc interno

   // TRISD = 0x00;    
    TRISA = 0xFF;
    
    LCD_Init();
    LCD_String_xy(0,1,"Sistem Confort");
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
        
        __delay_ms(1000);
        sprintf(Temperature,"%d", (int)lm35);
        LCD_String_xy(2,0,"Tem:");
        LCD_String(Temperature);
        
        /*sprintf(Raw,"%d ", (int)value_adc);
        LCD_String_xy(2,8,"RH:");
        LCD_String(Raw);/*/
                
    }
    
}