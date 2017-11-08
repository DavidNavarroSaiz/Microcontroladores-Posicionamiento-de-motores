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
int stepcont = 0;
int stepsPerRev = 4076; /*El 28BYJ-48 tiene un paso de 5.625 grados (64 pasos por vuelta).
                        *  El reductor interno tiene una relación de 1/64.
                        *  Combinados, la precisión total es de 4096 pasos por vuelta, equivalente a un paso de 0.088º , pere la relcion del reductor nos da 4076 pasos por vuelta*/

char comando [25];
int vel = 50; /*velocidad del motor El 28BYJ-48 tiene un par máximo tras el reductor de 3N?cm (0.3Kgf?cm).
La frecuencia máxima es de 100Hz, lo que supone unos 40 segundos por vuelta, o equivalentemente una velocidad de giro máxima en torno a 1.5 rpm.*/

const char saludo[] = "bienvenido";
const char comandonoreconocido[] = "Comando no Reconocido";
int numSteps = 8; //secuencia half step tiene 8 pasos
//int stepsLookup[8] = {0b1000, 0b1100, 0b0100, 0b0110, 0b0010, 0b0011, 0b0001, 0b1001};
const char moverx[] = "MOVERX";
const char movery[] = "MOVERY";
int steps; // numero entre cero y 4076
int actualstep = 0;
int h, o, p, help;
//char charstep;
char tam[20];
int led;
int r; //para escribir la palabra
int xtrue = 1;
int ytrue = 1;
char digitos [3];
int enter;
int tama;
int tama1;
int tama2;
char salida [25];
char entrada[25];
int i;
int lleg;
int movimiento ;
char *ptr_llegada;
//const char prueba[] = "PUEBAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
void ledestado(int tiempoled);
void enviarTrama(char *datos);
void paso_Der();
int stringtoint(char string[], int tamanio);
void apagarM1();
void apagarM2();


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
        if (led == 1 && movimiento == 0) {
            NOP();
            ledestado(6);
        }
       

        if (enter == 1) {

            NOP();
            for (r = 0; r <= 5; r++) {
                tam[r] = comando[r];
            }
            xtrue = strcmp(tam, moverx);
            ytrue = strcmp(tam, movery);
            enviarTrama(comando);
            ptr_llegada = &comando[6];
            if (*ptr_llegada == '-') {
                ptr_llegada = &comando[7];
                while (*ptr_llegada != 0) {
                    entrada[lleg] = *ptr_llegada;
                    ptr_llegada++;
                    lleg++;
                }
                lleg = 0;

                tama1 = tama - 7;
                steps = stringtoint(entrada, tama1);
                steps = (-1) * steps;
                enter = 0;
                tama = 0;

            } else {
                while (*ptr_llegada != 0) {
                    entrada[lleg] = *ptr_llegada;
                    ptr_llegada++;
                    lleg++;
                }
                lleg = 0;
                tama2 = tama - 6;
                steps = stringtoint(entrada, tama2);

                enter = 0;
                tama = 0;
                NOP();
            }
            for (i = 0; i < 25; i++) { //limpio comando
                comando[i] = '\0';
            }
            for (i = 0; i < 25; i++) { //limpio comando
                entrada[i] = '\0';
            }
            if (xtrue != 0 && ytrue != 0) {
                enviarTrama(comandonoreconocido);
            }

        }
//        if (xtrue == 0 || ytrue == 0) {


            if (xtrue == 0) {

                if (steps > 0) {
                    NOP();
                    while (stepcont <= steps) {
                        movimiento = 1 ;
                        if (actualstep > 7) {
                            actualstep = 0;
                        }
                        sprintf(salida, "stepcount : %d ", stepcont);
                        enviarTrama(salida);

                        switch (actualstep) {
                            case 0:
                                LATA = 0b0001;
                                for (i = 0; i <= vel; i++);
                                NOP();
                                stepcont++;
                                NOP();
                                actualstep++;
                                break;
                            case 1:
                                LATA = 0b0011;
                                for (i = 0; i <= vel; i++);
                                stepcont++;
                                actualstep++;
                                break;
                            case 2:
                                LATA = 0b0010;
                                for (i = 0; i <= vel; i++);
                                stepcont++;
                                actualstep++;
                                break;
                            case 3:
                                LATA = 0b0110;
                                for (i = 0; i <= vel; i++);
                                stepcont++;
                                actualstep++;
                                break;
                            case 4:
                                LATA = 0b0100;
                                for (i = 0; i <= vel; i++);
                                stepcont++;
                                actualstep++;
                                break;
                            case 5:
                                LATA = 0b1100;
                                for (i = 0; i <= vel; i++);
                                stepcont++;
                                actualstep++;
                                break;

                            case 6:
                                LATA = 0b1000;
                                for (i = 0; i <= vel; i++);
                                stepcont++;
                                actualstep++;
                                break;
                            case 7:
                                LATA = 0b1001;
                                for (i = 0; i <= vel; i++);
                                stepcont++;
                                actualstep++;
                                break;
                        }

                    }
                    apagarM1();
                    stepcont = 0;
                    steps = 0;
                    movimiento = 0 ;
                }//            D
                else if (steps < 0) {


                    while (stepcont >= steps) {
                        movimiento = 1 ;
                        if (actualstep > 7) {
                            actualstep = 0;
                        }
                        sprintf(salida, "stepcount : %d ", stepcont);
                        enviarTrama(salida);

                        switch (actualstep) {
                            case 0:
                                LATA = 0b1001;
                                for (i = 0; i <= vel; i++);
                                stepcont--;
                                actualstep++;

                                break;
                            case 1:
                                LATA = 0b1000;
                                for (i = 0; i <= vel; i++);
                                stepcont--;
                                actualstep++;
                                break;
                            case 2:
                                LATA = 0b1100;
                                for (i = 0; i <= vel; i++);
                                stepcont--;
                                actualstep++;

                                break;
                            case 3:
                                LATA = 0b0100;
                                for (i = 0; i <= vel; i++);
                                stepcont--;
                                actualstep++;
                                break;
                            case 4:
                                LATA = 0b0110;
                                for (i = 0; i <= vel; i++);
                                stepcont--;
                                actualstep++;
                                break;
                            case 5:
                                LATA = 0b0010;
                                for (i = 0; i <= vel; i++);
                                stepcont--;
                                actualstep++;
                                break;

                            case 6:
                                LATA = 0b0011;
                                for (i = 0; i <= vel; i++);
                                stepcont--;
                                actualstep++;
                                break;
                            case 7:
                                LATA = 0b0001;
                                for (i = 0; i <= vel; i++);
                                stepcont--;
                                actualstep++;
                                break;
                        }


                    }
                    apagarM1();
                    stepcont = 0;
                    steps = 0;
                    movimiento = 0;
                }
            }
            if (ytrue == 0) {

                if (steps > 0) {
                    NOP();
                    while (stepcont <= steps) {
                        movimiento = 1 ;
                        if (actualstep > 7) {
                            actualstep = 0;
                        }
                        sprintf(salida, "stepcount : %d ", stepcont);
                        enviarTrama(salida);

                        switch (actualstep) {
                            case 0:
                                LATB = 0b0001;
                                for (i = 0; i <= vel; i++);
                                NOP();
                                stepcont++;
                                NOP();
                                actualstep++;
                                break;
                            case 1:
                                LATB = 0b0011;
                                for (i = 0; i <= vel; i++);
                                stepcont++;
                                actualstep++;
                                break;
                            case 2:
                                LATB = 0b0010;
                                for (i = 0; i <= vel; i++);
                                stepcont++;
                                actualstep++;
                                break;
                            case 3:
                                LATB = 0b0110;
                                for (i = 0; i <= vel; i++);
                                stepcont++;
                                actualstep++;
                                break;
                            case 4:
                                LATB = 0b0100;
                                for (i = 0; i <= vel; i++);
                                stepcont++;
                                actualstep++;
                                break;
                            case 5:
                                LATB = 0b1100;
                                for (i = 0; i <= vel; i++);
                                stepcont++;
                                actualstep++;
                                break;

                            case 6:
                                LATB = 0b1000;
                                for (i = 0; i <= vel; i++);
                                stepcont++;
                                actualstep++;
                                break;
                            case 7:
                                LATB = 0b1001;
                                for (i = 0; i <= vel; i++);
                                stepcont++;
                                actualstep++;
                                break;
                        }

                    }
                    apagarM2();
                    stepcont = 0;
                    steps = 0;
                    movimiento = 0;
                }//            D
                else if (steps < 0) {


                    while (stepcont >= steps) {
                        movimiento = 1 ;
                        if (actualstep > 7) {
                            actualstep = 0;
                        }
                        sprintf(salida, "stepcount : %d ", stepcont);
                        enviarTrama(salida);

                        switch (actualstep) {
                            case 0:
                                LATB = 0b1001;
                                for (i = 0; i <= vel; i++);
                                stepcont--;
                                actualstep++;

                                break;
                            case 1:
                                LATB = 0b1000;
                                for (i = 0; i <= vel; i++);
                                stepcont--;
                                actualstep++;
                                break;
                            case 2:
                                LATB = 0b1100;
                                for (i = 0; i <= vel; i++);
                                stepcont--;
                                actualstep++;

                                break;
                            case 3:
                                LATB = 0b0100;
                                for (i = 0; i <= vel; i++);
                                stepcont--;
                                actualstep++;
                                break;
                            case 4:
                                LATB = 0b0110;
                                for (i = 0; i <= vel; i++);
                                stepcont--;
                                actualstep++;
                                break;
                            case 5:
                                LATB = 0b0010;
                                for (i = 0; i <= vel; i++);
                                stepcont--;
                                actualstep++;
                                break;

                            case 6:
                                LATB = 0b0011;
                                for (i = 0; i <= vel; i++);
                                stepcont--;
                                actualstep++;
                                break;
                            case 7:
                                LATB = 0b0001;
                                for (i = 0; i <= vel; i++);
                                stepcont--;
                                actualstep++;
                                break;
                        }


                    }
                    apagarM2();
                    stepcont = 0;
                    steps = 0;
                    movimiento = 0;
                }
            }

//        } 
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

int stringtoint(char string[], int tamanio) {
    h = 0;
    o = 1;
    p = tamanio - 1; //sizeof (string) - 1; //2

    while (p >= 0) {
        help = string[p] - '0';
        help = help*o;
        h = h + help;
        p = p - 1;
        o = o * 10;
    }
    return h;
}

void apagarM1() {
    LATA = 0;
}
void apagarM2() {
    LATBbits.LB0 = 0;
    LATBbits.LB1 = 0;
    LATBbits.LB2 = 0;
    LATBbits.LB3 = 0;
}