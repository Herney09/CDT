/* 
 * File:   dht11.h
 * Author: vente
 *
 * Created on 6 de julio de 2022, 04:40 PM
 */

#ifndef DHT11_H
#define	DHT11_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include <xc.h>
#include <pic18f4550.h>  /*Header file PIC18f4550 definitions*/
    
#define _XTAL_FREQ 8000000
    
#define Data_Out LATA1              /* assign Port pin for data*/
#define Data_In PORTAbits.RA1       /* read data from Port pin*/
#define Data_Dir TRISAbits.RA1      /* Port direction */

void DHT11_Start(void);
void DHT11_CheckResponse(void);
char DHT11_ReadData(void);
    
    
#ifdef	__cplusplus
}
#endif

#endif	/* DHT11_H */

