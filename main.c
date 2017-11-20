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
long stepcount = 0;
int stepsPerRev = 4076; /*El 28BYJ-48 tiene un paso de 5.625 grados (64 pasos por vuelta).
                        *  El reductor interno tiene una relación de 1/64.
                        *  Combinados, la precisión total es de 4096 pasos por vuelta, equivalente a un paso de 0.088º , pere la relcion del reductor nos da 4076 pasos por vuelta*/

char comando [25];
int vel = 500; /*velocidad del motor El 28BYJ-48 tiene un par máximo tras el reductor de 3N?cm (0.3Kgf?cm).
La frecuencia máxima es de 100Hz, lo que supone unos 40 segundos por vuelta, o equivalentemente una velocidad de giro máxima en torno a 1.5 rpm.*/

const char saludo[] = "Bienvenido,";
const char rangox[] = " El rango de trabajo en el eje x : [0,20]mm";
const char rangoy[] = " El rango de trabajo en el eje y : [0,30]mm";
const char comandonoreconocido[] = "Comando no Reconocido";
const char moverx[] = "MOVERX";
const char movery[] = "MOVERY";
const char posicion_info[] = "POSICION";
const char devolver[] = "DEVOLVER";
const char norango[] = "el movimiento solicitado se encuentra fuera del rango de trabajo";
int infotrue = 1;
long stepsx; // pasos en el eje x , eel eje con engranaje delgado
long stepsy; //pasos en el eje y , el eje con engranaje grueso
long stepsy_auxiliar; //cuando se necesite mover el eje x se debe llevar el eje y a cero 
long stepsx_auxiliar;
int currentstep = 0;
long h, o, p, help;
//char charstep;
char tam[20];
int led;
int r; //para escribir la palabra
int xtrue = 1;
int ytrue = 1;
int devolvertrue;
char digitos [3];
int enter;
int tama;
int tama1;
int tama2;
char salida [25];
char entrada[25];
int i;
int lleg;
int movimiento;
char *ptr_llegada;
int movimientoy;
int movimientox;
int posiciony;
int posicionx; //numero que introduce el usuario , esta en unidades de mm
float relacionx = 10548.52321; //100000 / 9,48mm --- 100000 pasos avanza 9,48 mm
unsigned int relaciony = 11160.714; //100000/8,96mm
//const char prueba[] = "PUEBAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
void ledestado(int tiempoled);
void enviarTrama(char *datos);
void paso_Der();
long stringtolong(char string[], int tamanio);
void apagarM1();
void apagarM2();
void ejexpositivo(long paso);
void ejexnegativo(long paso2);
void ejeypositivo(long paso3);
void ejeynegativo(long paso4);


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
    enviarTrama(rangox);
    enviarTrama(rangoy);
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
            infotrue = strcmp(comando, posicion_info);
            devolvertrue = strcmp(comando, devolver);
            enviarTrama(comando);
            if (xtrue != 0 && ytrue != 0 && infotrue != 0 && devolvertrue != 0) {
                enviarTrama(comandonoreconocido);
                enter = 0;
            }

            if (xtrue == 0) {
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
                    movimientox = stringtolong(entrada, tama1);
                    stepsx = movimientox * relacionx;
                    movimientox = movimientox * (-1);
                    posicionx = movimientox + posicionx;
                    stepsx = (-1) * stepsx;
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
                    movimientox = stringtolong(entrada, tama2);
                    posicionx = movimientox + posicionx;
                    stepsx = movimientox * relacionx;

                    enter = 0;
                    tama = 0;
                }
            } else if (ytrue == 0) {
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
                    movimientoy = stringtolong(entrada, tama1);
                    stepsy = movimientoy*relaciony;
                    movimientoy = movimientoy * (-1);
                    posiciony = posiciony + movimientoy;
                    stepsy = (-1) * stepsy;
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
                    movimientoy = stringtolong(entrada, tama2);
                    posiciony = posiciony + movimientoy;
                    stepsy = movimientoy*relaciony;
                    enter = 0;
                    tama = 0;
                    NOP();
                }
            } else if (infotrue == 0) {
                sprintf(salida, "posicion en eje x : %d ", posicionx);
                enviarTrama(salida);
                sprintf(salida, "posicion en eje y : %d ", posiciony);
                enviarTrama(salida);
                enter = 0;
            }else if(devolvertrue == 0){
                stepsy_auxiliar = posiciony*relaciony; //llevar el eje y a cero
                stepsy_auxiliar = stepsy_auxiliar * (-1);
                ejeynegativo(stepsy_auxiliar);
                apagarM2();
                stepcount = 0;
                stepsy_auxiliar = 0;
                posiciony = 0 ;
                stepsx_auxiliar = posicionx*relacionx; //llevar el eje y a cero
                stepsx_auxiliar = stepsx_auxiliar * (-1);
                ejexnegativo(stepsx_auxiliar);
                apagarM2();
                stepcount = 0;
                stepsx_auxiliar = 0;                
                enter = 0 ;
                posiciony = 0;
                sprintf(salida, "posicion en eje x : %d ", posicionx);
                enviarTrama(salida);
                sprintf(salida, "posicion en eje y : %d ", posiciony);
                enviarTrama(salida);
            }
            for (i = 0; i < 25; i++) { //limpio comando
                comando[i] = '\0';
            }
            for (i = 0; i < 25; i++) { //limpio comando
                entrada[i] = '\0';
            }


        }
        //        if (xtrue == 0 || ytrue == 0) {


        if (xtrue == 0) {
            if (posicionx >= 0 && posicionx <= 20) {
                stepsy_auxiliar = posiciony*relaciony; //llevar el eje y a cero
                stepsy_auxiliar = stepsy_auxiliar * (-1);
                ejeynegativo(stepsy_auxiliar);
                apagarM2();
                stepcount = 0;
                stepsy_auxiliar = 0;
//                sprintf(salida, "stepsxxxxxx x : %ld ", stepsx);
//                enviarTrama(salida);
                if (stepsx > 0) {
                    //                if (posicionx < 20) { //aun falta definir los limites
                    ejexpositivo(stepsx);

                    apagarM1();
                    stepcount = 0;
                    stepsx = 0;
                    movimiento = 0;
                    movimientox = 0;
                    //                } else {
                    //                    enviarTrama(norango);
                    //                    posicionx = posicionx - movimientox;
                    //                    movimientox = 0;
                    //                    enviarTrama(rangox);
                    //                }


                } else if (stepsx < 0) {
                    //                    if (posicionx >= 0) {
                    ejexnegativo(stepsx);

                    apagarM1();
                    stepcount = 0;
                    stepsx = 0;
                    movimiento = 0;
                    //                    } else {
                    //                        enviarTrama(norango);
                    //                        enviarTrama(rangox);
                    //                        posicionx = posicionx - movimientox;
                    //                        movimientox = 0;
                    //                    }


                }
                stepsy_auxiliar = posiciony*relaciony; //llevar el eje y a POSICION
                stepsy_auxiliar = stepsy_auxiliar;
                ejeypositivo(stepsy_auxiliar);
                apagarM2();
                stepcount = 0;
                stepsy_auxiliar = 0;

                sprintf(salida, "posicion en eje x : %d ", posicionx);
                enviarTrama(salida);
                movimiento = 0;

            } else {
                enviarTrama(norango);
                posicionx = posicionx - movimientox;
                movimientox = 0;
                enviarTrama(rangox);
            }
            xtrue = 1;
        }

        if (ytrue == 0) {
            if (posiciony >= 0 && posiciony <= 30) {
                if (stepsy >= 0) {
                    //                    if (posiciony <= 30) {
                    NOP();
                    ejeypositivo(stepsy);
                    sprintf(salida, "posicion en eje y : %d ", posiciony);
                    enviarTrama(salida);
                    apagarM2();
                    stepcount = 0;
                    stepsy = 0;
                    movimiento = 0;
                    movimientoy = 0;
                    //                    } else {
                    //                        enviarTrama(norango);
                    //                        enviarTrama(rangoy);
                    //                        posiciony = posiciony - movimientoy;
                    //                        movimientoy = 0;
                    //
                    //                    }
                }
                if (stepsy < 0) {
                    //                        if (posiciony >= 0) {

                    ejeynegativo(stepsy);
                    sprintf(salida, "posicion en eje y : %d ", posiciony);
                    enviarTrama(salida);
                    apagarM2();
                    stepcount = 0;
                    stepsy = 0;
                    movimiento = 0;
                    movimientoy = 0;
                    //                        } else {
                    //                            enviarTrama(norango);
                    //                            enviarTrama(rangoy);
                    //                            posiciony = posiciony - movimientoy;
                    //                            movimientoy = 0;
                    //                        }
                }

            } else {
                enviarTrama(norango);
                enviarTrama(rangoy);
                posiciony = posiciony - movimientoy;
                movimientoy = 0;
            }
            ytrue =1;

            //        } 
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

    long stringtolong(char string[], int tamanio) {
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

    void ejexpositivo(long paso) {
        while (stepcount <= paso) {
            movimiento = 1;
            if (currentstep > 7) {
                currentstep = 0;
            }
            //        sprintf(salida, "stepcount : %ld ", stepcount);
            //        enviarTrama(salida);

            switch (currentstep) {
                case 0:
                    LATA = 0b0001;
                    for (i = 0; i <= vel; i++);
                    NOP();
                    stepcount++;
                    NOP();
                    currentstep++;
                    break;
                case 1:
                    LATA = 0b0011;
                    for (i = 0; i <= vel; i++);
                    stepcount++;
                    currentstep++;
                    break;
                case 2:
                    LATA = 0b0010;
                    for (i = 0; i <= vel; i++);
                    stepcount++;
                    currentstep++;
                    break;
                case 3:
                    LATA = 0b0110;
                    for (i = 0; i <= vel; i++);
                    stepcount++;
                    currentstep++;
                    break;
                case 4:
                    LATA = 0b0100;
                    for (i = 0; i <= vel; i++);
                    stepcount++;
                    currentstep++;
                    break;
                case 5:
                    LATA = 0b1100;
                    for (i = 0; i <= vel; i++);
                    stepcount++;
                    currentstep++;
                    break;

                case 6:
                    LATA = 0b1000;
                    for (i = 0; i <= vel; i++);
                    stepcount++;
                    currentstep++;
                    break;
                case 7:
                    LATA = 0b1001;
                    for (i = 0; i <= vel; i++);
                    stepcount++;
                    currentstep++;
                    break;
            }

        }

    }

    void ejexnegativo(long paso2) {
        while (stepcount >= paso2) {
            movimiento = 1;
            if (currentstep > 7) {
                currentstep = 0;
            }
            //        sprintf(salida, "stepcount : %ld ", stepcount);
            //        enviarTrama(salida);

            switch (currentstep) {
                case 0:
                    LATA = 0b1001;
                    for (i = 0; i <= vel; i++);
                    stepcount--;
                    currentstep++;

                    break;
                case 1:
                    LATA = 0b1000;
                    for (i = 0; i <= vel; i++);
                    stepcount--;
                    currentstep++;
                    break;
                case 2:
                    LATA = 0b1100;
                    for (i = 0; i <= vel; i++);
                    stepcount--;
                    currentstep++;

                    break;
                case 3:
                    LATA = 0b0100;
                    for (i = 0; i <= vel; i++);
                    stepcount--;
                    currentstep++;
                    break;
                case 4:
                    LATA = 0b0110;
                    for (i = 0; i <= vel; i++);
                    stepcount--;
                    currentstep++;
                    break;
                case 5:
                    LATA = 0b0010;
                    for (i = 0; i <= vel; i++);
                    stepcount--;
                    currentstep++;
                    break;

                case 6:
                    LATA = 0b0011;
                    for (i = 0; i <= vel; i++);
                    stepcount--;
                    currentstep++;
                    break;
                case 7:
                    LATA = 0b0001;
                    for (i = 0; i <= vel; i++);
                    stepcount--;
                    currentstep++;
                    break;
            }


        }

    }

    void ejeypositivo(long paso3) {
        while (stepcount <= paso3) {
            movimiento = 1;
            if (currentstep > 7) {
                currentstep = 0;
            }
            //        sprintf(salida, "stepcount : %ld ", stepcount);
            //        enviarTrama(salida);

            switch (currentstep) {
                case 0:
                    LATB = 0b0001;
                    for (i = 0; i <= vel; i++);
                    NOP();
                    stepcount++;
                    NOP();
                    currentstep++;
                    break;
                case 1:
                    LATB = 0b0011;
                    for (i = 0; i <= vel; i++);
                    stepcount++;
                    currentstep++;
                    break;
                case 2:
                    LATB = 0b0010;
                    for (i = 0; i <= vel; i++);
                    stepcount++;
                    currentstep++;
                    break;
                case 3:
                    LATB = 0b0110;
                    for (i = 0; i <= vel; i++);
                    stepcount++;
                    currentstep++;
                    break;
                case 4:
                    LATB = 0b0100;
                    for (i = 0; i <= vel; i++);
                    stepcount++;
                    currentstep++;
                    break;
                case 5:
                    LATB = 0b1100;
                    for (i = 0; i <= vel; i++);
                    stepcount++;
                    currentstep++;
                    break;

                case 6:
                    LATB = 0b1000;
                    for (i = 0; i <= vel; i++);
                    stepcount++;
                    currentstep++;
                    break;
                case 7:
                    LATB = 0b1001;
                    for (i = 0; i <= vel; i++);
                    stepcount++;
                    currentstep++;
                    break;
            }

        }
    }

    void ejeynegativo(long paso4) {
        while (stepcount >= paso4) {
            movimiento = 1;
            if (currentstep > 7) {
                currentstep = 0;
            }
            //        sprintf(salida, "stepcount : %ld ", stepcount);
            //        enviarTrama(salida);

            switch (currentstep) {
                case 0:
                    LATB = 0b1001;
                    for (i = 0; i <= vel; i++);
                    stepcount--;
                    currentstep++;

                    break;
                case 1:
                    LATB = 0b1000;
                    for (i = 0; i <= vel; i++);
                    stepcount--;
                    currentstep++;
                    break;
                case 2:
                    LATB = 0b1100;
                    for (i = 0; i <= vel; i++);
                    stepcount--;
                    currentstep++;

                    break;
                case 3:
                    LATB = 0b0100;
                    for (i = 0; i <= vel; i++);
                    stepcount--;
                    currentstep++;
                    break;
                case 4:
                    LATB = 0b0110;
                    for (i = 0; i <= vel; i++);
                    stepcount--;
                    currentstep++;
                    break;
                case 5:
                    LATB = 0b0010;
                    for (i = 0; i <= vel; i++);
                    stepcount--;
                    currentstep++;
                    break;

                case 6:
                    LATB = 0b0011;
                    for (i = 0; i <= vel; i++);
                    stepcount--;
                    currentstep++;
                    break;
                case 7:
                    LATB = 0b0001;
                    for (i = 0; i <= vel; i++);
                    stepcount--;
                    currentstep++;
                    break;
            }


        }
    }

