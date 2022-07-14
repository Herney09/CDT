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
#include "Config.h"
#include "LCD.h"
#include "KEYPAD.h"
#include "dht11.h"



#define _XTAL_FREQ 8000000          /* define _XTAL_FREQ for using internal delay */

//Variables del Sistema de Security
const char password[5] ={'2','0','2','2',0};
char pass_user[5];
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
void Photoresist(void);
void Dht11();



void main(void) {
    
    //Configura Fosc = 8Mhz interno, Fuente de Fosc del sistema = interno
    OSCCON = 0x72; //Configura oscilador interno (FOSC = 8Mhz)
    TRISE = 0b000;
    TRISA =0x01;
    ADCON1bits.PCFG = 0b1110; //  Configura el Puerto como Entrada Analógica.
    LCD_Init(); //Inicializa el LCD
    keypad_init();
    Security(); //funcion del sistema de Security  
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
            LCD_Clear(); //limpia el LCD
            LCD_String_xy(0,0,"Acceso Concedido");
            
            PORTE = 0b10; // se enciente el led verde si la contraseña es correcta
            __delay_ms(3000);
            PORTE = 0b00; //se apaga el led verde despues de 3 segundos
            __delay_ms(500);
            LCD_Clear(); //limpia el LCD
            LCD_String_xy(0,1,"Bienvenido Al");
            LCD_String_xy(2,1,"Sistem Confort");
            __delay_ms(3000);
            while(1){
                
                Dht11();   
                __delay_ms(2000);
                Photoresist(); 
                                
            }
            Con_Error = 0; //inicializa en contador de error en cero si la contraseña es correcta
        }else{
            LCD_Clear(); //limpia el LCD
            LCD_String_xy(0,1,"Clave Invalida");
            PORTE = 0b01; //se enciente el led Rojo si la contraseña es incorrecta
            __delay_ms(3000);
            PORTE = 0b00; //despues de 3 segundos se apaga el led
            Con_Error = Con_Error + 1; //cuenta lacantidad de contraseña incorrecta 
            
            if(Con_Error == 3){
                LCD_Clear(); //limpia el LCD
                LCD_String_xy(0,0,"Clave Bloqueada");
                __delay_ms(15000); // despues que se bloquea la clave espera 30 segundos para volver a solicitarla
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

void Photoresist(void){
     
    OSCCONbits.IRCF = 0b111; //Configura oscilador interno (FOSC = 8Mhz)
    OSCCONbits.SCS = 0b10; // Oscilador del sistema = Fosc interno
  
    TRISA = 0x01;
    
    LCD_Init();
    LCD_Command(0xC0);
    
    //ADC = 10bits, Tad = 1us, Tacq = 4us, Vref = 5v-0v, RA=ANALOG
    ADCON1bits.PCFG = 0b1110; //  Configura el Puerto como Entrada Analógica.
    ADCON1bits.VCFG = 0b00; //  Selecciona Voltajes de Referencia (5v-0v).
    ADCON0bits.CHS = 0b0000; //  Selecciona el Canal Analógico.
    ADCON2bits.ACQT = 0b010; //  Tiempo de Adquisición 4Tad.
    ADCON2bits.ADCS = 0b001; //  Tiempo de Conversión Fosc/8.
    ADCON2bits.ADFM = 1; //  Justificación derecha (modo-10bits).
    ADCON0bits.ADON = 1; //  Habilita el Módulo AD.
    
    ADCON0bits.GO_DONE = 1; //Inicia la COnversió AD.
    while (ADCON0bits.GO_DONE); //  Espera a que termine la conversión AD.
    value_adc = ADRESH; //  Lectura de valor AD.
    value_adc = (value_adc << 8) + ADRESL;
    temp = value_adc;
    sprintf(Temperature,"%d",(int)temp);
    LCD_String_xy(0,0,"Luz: ");
    LCD_String(Temperature);
    __delay_ms(2000);
}

void Dht11(void){
    char RH_Decimal,RH_Integral,T_Decimal,T_Integral;
    char Checksum;
    char value[10];    
    
    DHT11_Start();                  /* send start pulse to DHT11 module */
    DHT11_CheckResponse();          /* wait for response from DHT11 module */
    
    /* read 40-bit data from DHT11 module */
    RH_Integral = DHT11_ReadData(); /* read Relative Humidity's integral value */
    RH_Decimal = DHT11_ReadData();  /* read Relative Humidity's decimal value */
    T_Integral = DHT11_ReadData();  /* read Temperature's integral value */
    T_Decimal = DHT11_ReadData();   /* read Relative Temperature's decimal value */
    Checksum = DHT11_ReadData();    /* read 8-bit checksum value */
    LCD_Clear();
    /* convert humidity value to ascii and send it to display*/    
    sprintf(value,"%d",RH_Integral);
    LCD_String_xy(1,4,value);
    sprintf(value,".%d ",RH_Decimal);
    LCD_String(value);
    LCD_Char('%');
    LCD_String_xy(1,0,"Hr:");
    
    /* convert temperature value to ascii and send it to display*/
    sprintf(value,"%d",T_Integral);
    LCD_String_xy(2,5,value);
    sprintf(value,".%d",T_Decimal);
    LCD_String(value);
    LCD_Char(0xdf);
    LCD_Char('C');
    LCD_String_xy(2,0,"Tem:");
    
    __delay_ms(2000);
    
}   


