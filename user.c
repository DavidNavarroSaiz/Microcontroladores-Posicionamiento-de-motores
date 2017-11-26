/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#if defined(__XC)
#include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
#include <htc.h>        /* HiTech General Include File */
#elif defined(__18CXX)
#include <p18cxxx.h>    /* C18 General Include File */
#endif

#if defined(__XC) || defined(HI_TECH_C)

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */

#endif

#include "user.h"

/******************************************************************************/
/* User Functions                                                             */
/******************************************************************************/

/* <Initialize variables in user.h and insert code for user algorithms.> */

void InitApp(void) {
    PORTA = 0;
    PORTB = 0;
    PORTC = 0;

    TRISBbits.RB0 = 0;
    TRISBbits.RB1 = 0;
    TRISBbits.RB2 = 0;
    TRISBbits.RB3 = 0;

    TRISA = 0;

    //TIMER 1 led estado
    TRISBbits.RB7 = 0; //LED DE ESTADO//
    T1CONbits.RD16 = 1; //read /write 16 bits
    T1CONbits.T1CKPS = 0b11; //preescaler 1:8
    T1CONbits.TMR1CS = 0; //timer interno   
    T1CONbits.T1SYNC = 1; //sincronizar con reloj externo
    T1CONbits.TMR1ON = 1; //iniciar timer
    PIE1bits.TMR1IE = 1; //activar interrupciones de overflow
    IPR1bits.TMR1IP = 1; //alta prioridad 
    TMR1 = 3036; //100 ms
    INTCONbits.GIE = 1; //ACTIVAR INTERRUPCIONES GLOBALES//
    INTCONbits.PEIE = 1; //ACTIVAR INTERRUPCIONES PERIFERICAS//



    TRISCbits.RC7 = 1; // RECEIVER RX INPUT. SE QUIERE ESTE PIN COMO ENTRADA PARA RECEPCIÒN//
    TRISCbits.RC6 = 0; //TRANSMITTER TX OUTPUT, SE QUIERE ESTE PIN COMO SALIDA PARA TRANSMISIÒN//
    SPBRG = 10; //BAUD RATE DE 115200 BITS POR SEGUNDO TEORICAMENTE//
    RCSTAbits.SPEN = 1; //Habilitar UART,ACTIVAR PUERTO SERIAL ASINCRONO //
    TXSTAbits.SYNC = 0; // ASYNCHRONOUS MODE//
    TXSTAbits.BRGH = 1; //ALTA RESOLUCIÒN, PUEDE AYUDAR A DISMINUIR EL ERROR //
    TXSTAbits.TXEN = 1; //Habilitar la transmisiòn//
    RCSTAbits.CREN = 1;
    PIE1bits.RCIE = 1; //Habilitamos interrupciones recepcion UART//
    IPR1bits.RCIP = 1; //ALTA PRIORIDAD//


    PIE2bits.EEIE = 1; //EEPROM write operation bit 
    
}

