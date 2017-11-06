/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#if defined(__XC)
#include <xc.h>        /* XC8 General Include File */
#elif defined(HI_TECH_C)
#include <htc.h>       /* HiTech General Include File */
#elif defined(__18CXX)
#include <p18cxxx.h>   /* C18 General Include File */
#endif

#if defined(__XC) || defined(HI_TECH_C)

#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>       /* For true/false definition */       
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <pic18f252.h>
#endif

#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "user.h"          /* User funct/params, such as InitApp */


/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/

int cont_led;
int c;
int stepcont = 1;
int stepsPerRev = 4076; /*El 28BYJ-48 tiene un paso de 5.625 grados (64 pasos por vuelta).
                        *  El reductor interno tiene una relación de 1/64.
                        *  Combinados, la precisión total es de 4096 pasos por vuelta, equivalente a un paso de 0.088º , pere la relcion del reductor nos da 4076 pasos por vuelta*/


int vel = 150; /*velocidad del motor El 28BYJ-48 tiene un par máximo tras el reductor de 3N?cm (0.3Kgf?cm).
La frecuencia máxima es de 100Hz, lo que supone unos 40 segundos por vuelta, o equivalentemente una velocidad de giro máxima en torno a 1.5 rpm.*/

const char saludo [] = "bienvenido a posicionamiento de camara" ;
int numSteps = 8; //secuencia half step tiene 8 pasos
//int stepsLookup[8] = {0b1000, 0b1100, 0b0100, 0b0110, 0b0010, 0b0011, 0b0001, 0b1001};
const char moverx [] = "MOVERX";
int steps; // numero entre cero y 4076
int actualstep = 0b0001;
int n, o, p, help;
//char charstep;
void ledestado(int tiempoled);
void enviarTrama(char *datos);
void paso_Der();
char tam[20];
int led;
int r; //para escribir la palabra
int xtrue;
char digitos [];
/******************************************************************************/
/* Main Program                                                               */

/******************************************************************************/

void main(void) {
    /* Configure the oscillator for the device */
    ConfigureOscillator();

    /* Initialize I/O and Peripherals for application */
    InitApp();

    /* TODO <INSERT USER APPLICATION CODE HERE> */

    enviarTrama(saludo);
    while (1) {
        if  (led == 1){
            ledestado(10);
        }
        
        if (enter == 1 ){
        for (r = 0; r <= 5; r++) {
            tam[r] = comando[r];
        }
        xtrue = strcmp(tam, moverx);
        digitos[0] = comando[6] - '0';
        digitos[1] = comando[7] - '0';
        digitos[2] = comando[8] - '0';
        n = 0;
        o = 1;
        p = 2;

        while (p >= 0) {
            help = digitos[p] - '0';
            help = help*o;
            n = n + help;
            p = p - 1;
            o = o * 10;
        }
        steps = n;
        enter = 0 ;
        }
        if (xtrue == 0) {
            
            if (steps > 0) {
                while (stepcont <= steps) {

                    if (actualstep > 7) {
                        actualstep = 0;
                    }
                    switch (actualstep) {
                        case 0:
                            LATA = 0b0001;
                            for (int i = 0; i <= vel; i++);
                            stepcont++;
                            actualstep++;
                            break;
                        case 1:
                            LATA = 0b0011;
                            for (int i = 0; i <= vel; i++);
                            stepcont++;
                            actualstep++;
                            break;
                        case 2:
                            LATA = 0b0010;
                            for (int i = 0; i <= vel; i++);
                            stepcont++;
                            actualstep++;
                            break;
                        case 3:
                            LATA = 0b0110;
                            for (int i = 0; i <= vel; i++);
                            stepcont++;
                            actualstep++;
                            break;
                        case 4:
                            LATA = 0b0100;
                            for (int i = 0; i <= vel; i++);
                            stepcont++;
                            actualstep++;
                            break;
                        case 5:
                            LATA = 0b1100;
                            for (int i = 0; i <= vel; i++);
                            stepcont++;
                            actualstep++;
                            break;
                            
                        case 6:
                            LATA = 0b1000;
                            for (int i = 0; i <= vel; i++);
                            stepcont++;
                            actualstep++;
                            break;
                        case 7:
                            LATA = 0b1001;
                            for (int i = 0; i <= vel; i++);
                            stepcont++;
                            actualstep++;
                            break;
                    }

                }
                stepcont = 0;

            } else if (steps < 0) {

                while (stepcont >= steps) {

                    //            LATA = (actualstep << 1);
                    //            stepcont=stepcont - 1;
                    //            

                    if (actualstep < 0) {
                        actualstep = 7;
                    }
                    switch (actualstep) {
                        case 0:
                            LATA = 0b1001;
                            for (int i = 0; i <= vel; i++);
                            stepcont--;
                            actualstep--;
                            break;
                        case 1:
                            LATA = 0b1000;
                            for (int i = 0; i <= vel; i++);
                            stepcont--;
                            actualstep--;
                            break;
                        case 2:
                            LATA = 0b1100;
                            for (int i = 0; i <= vel; i++);
                            stepcont--;
                            actualstep--;
                            break;
                        case 3:
                            LATA = 0b0100;
                            for (int i = 0; i <= vel; i++);
                            stepcont--;
                            actualstep--;
                            break;
                        case 4:
                            LATA = 0b0110;
                            for (int i = 0; i <= vel; i++);
                            stepcont--;
                            actualstep--;
                            break;
                        case 5:
                            LATA = 0b0010;
                            for (int i = 0; i <= vel; i++);
                            stepcont--;
                            break;
                            actualstep--;
                        case 6:
                            LATA = 0b0011;
                            for (int i = 0; i <= vel; i++);
                            stepcont--;
                            actualstep--;
                            break;
                        case 7:
                            LATA = 0b0001;
                            for (int i = 0; i <= vel; i++);
                            stepcont--;
                            actualstep--;
                            break;
                    }
                }
                stepcont = 0;
            }
        }
    }

}

void enviarTrama(char *datos) {

    //saludo[0] = *datos:

    while (*datos != 0) {
        TXREG = *datos;
        while (TXSTAbits.TRMT == 0);
        *datos++;
    }

    TXREG = 0x0A;
    while (TXSTAbits.TRMT == 0);
    TXREG = 0x0D;
    while (TXSTAbits.TRMT == 0);

}

void ledestado(int tiempoled) {
    cont_led++;
    if (cont_led >= tiempoled) {
        LATBbits.LB7 = ~PORTBbits.RB7;
        cont_led = 0;
    }
    led = 0;
}

//void paso_Der() {
//    LATA = 0b0001;
//    for (int i = 0; i <= vel; i++);
//    StepCounter++;
//    LATA = 0b0011;
//    for (int i = 0; i <= vel; i++);
//    StepCounter++;
//    LATA = 0b0010;
//    for (int i = 0; i <= vel; i++);
//    StepCounter++;
//    LATA = 0b0110;
//    for (int i = 0; i <= vel; i++);
//    StepCounter++;
//    LATA = 0b0100;
//    for (int i = 0; i <= vel; i++);
//    StepCounter++;
//    LATA = 0b1100;
//    for (int i = 0; i <= vel; i++);
//    StepCounter++;
//    LATA = 0b1000;
//    for (int i = 0; i <= vel; i++);
//    StepCounter++;
//    LATA = 0b1001;
//    for (int i = 0; i <= vel; i++);
//    StepCounter++;


//}

//void paso_Izq() {


    //    LATA = 0b1001;
    //    for (int i = 0; i <= vel; i++);
    //    StepCounter++;
    //    LATA = 0b1000;
    //    for (int i = 0; i <= vel; i++);
    //    StepCounter++;
    //    LATA = 0b1100;
    //    for (int i = 0; i <= vel; i++);
    //    StepCounter++;
    //    LATA = 0b0100;
    //    for (int i = 0; i <= vel; i++);
    //    StepCounter++;
    //    LATA = 0b0110;
    //    for (int i = 0; i <= vel; i++);
    //    StepCounter++;
    //    LATA = 0b0010;
    //    for (int i = 0; i <= vel; i++);
    //    StepCounter++;
    //    LATA = 0b0011;
    //    for (int i = 0; i <= vel; i++);
    //    StepCounter++;
    //    LATA = 0b0001;
    //    for (int i = 0; i <= vel; i++);
    //    StepCounter++;





//}

//void apagar() {
//    LATA = 0;
//}