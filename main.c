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

const char saludo[] = "Bienvenido, ya puede controlar el desplazador XY ";
const char rangox[] = " El rango de trabajo en el eje x : [0,18]mm"; //rango de movimiento mecanico eje x
const char rangoy[] = " El rango de trabajo en el eje y : [0,23]mm"; //rango de movimiento mecanico eje y
const char comandonoreconocido[] = "Comando no Reconocido";
const char moverx[] = "MOVERX";
const char movery[] = "MOVERY";
const char posicion_info[] = "POSICION";
const char devolver[] = "DEVOLVER";
const char norango[] = "el movimiento solicitado se encuentra fuera del rango de trabajo";
const char cero[] = "CERO";
int infotrue = 1;
long stepsx; // pasos en el eje x , el eje con engranaje delgado
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
int devolvertrue = 1;
int cerotrue = 1;
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
int posicionactualx;
int posicionactualy;
float relacionx = 11518.75; //75000pasos /6,4mm --- 75000 pasos avanza 6,4 mm
float relaciony = 10960.714; //100000/9,7 mm
///funciones
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
void writeRegister(int direccionW, int datoW);
int readRegister(int direccionR);
int k, z;

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
    k = readRegister(0);
    if (k != 255) {
        posicionx = readRegister(0);
        NOP();
    }
    z = readRegister(1);
    if (z != 255) {
        posiciony = readRegister(1);
    }


    while (1) {
        if (led == 1 && movimiento == 0) { //led de estado modo de funcionamiento normal
            NOP();
            ledestado(6);
        }
        if (enter == 1) {
            NOP();
            for (r = 0; r <= 5; r++) {
                tam[r] = comando[r]; // se cogen los primeros 6 caracteres de la palabra que introducimos
            }
            xtrue = strcmp(tam, moverx); //los 6 primeros caracteres se comparan con moverx[] y movery[] para saber si son iguales
            ytrue = strcmp(tam, movery);
            infotrue = strcmp(comando, posicion_info); // se compara el comando que introdujeron con posicion_info
            devolvertrue = strcmp(comando, devolver); // se compara el comando que introdujeron con devolver
            cerotrue = strcmp(comando, cero);
            enviarTrama(comando); // eco de la palabra que introducimos

            if (xtrue != 0 && ytrue != 0 && infotrue != 0 && devolvertrue != 0 && cerotrue !=0 ) { //si ninguna de las comparaciones dio positiva , entonces es un comando no reconocido
                enviarTrama(comandonoreconocido);
                enter = 0;
                tama = 0;
            }

            if (xtrue == 0) {//si introdujeron el comando moverx
                ptr_llegada = &comando[6]; // se coloca un puntero en la posicion 6 de comando
                if (*ptr_llegada == '-') { //si en la posicion 6 de comando hay un menos
                    ptr_llegada = &comando[7]; //// se coloca un puntero en la posicion 7 de comando la cual es donde empieza el numero  
                    while (*ptr_llegada != 0) {//mientras el puntero no sea un null
                        entrada[lleg] = *ptr_llegada; // se escribe el numero en el char entrada []
                        ptr_llegada++;
                        lleg++;
                    }
                    lleg = 0;

                    tama1 = tama - 7; //queremos saber el numero de numeros que introdujeron el comando moverx- tiene 7 caracteres
                    movimientox = stringtolong(entrada, tama1); //se llama a la funcion stringtolong
                    stepsx = movimientox * relacionx; //dependiendo de el numero que introdujeron se sabe cuantos pasos debe dar el motor
                    movimientox = movimientox * (-1); //introdujeron un numero negativo
                    posicionx = movimientox + posicionx; //se actualiza la posicion actual del motor
                    writeRegister(0, posicionx); //es escribe en la EEPROM la posicion actual
                    stepsx = (-1) * stepsx; //numero de pasos debe ser negativo debido a que introdujimos un (-) en la posicion 6)
                    enter = 0;
                    tama = 0;


                } else { //el numero es positivo y se hace el mismo procedimiento anterior sin multiplicar por (-1))
                    while (*ptr_llegada != 0) {
                        entrada[lleg] = *ptr_llegada;
                        ptr_llegada++;
                        lleg++;
                    }
                    lleg = 0;
                    tama2 = tama - 6;
                   
                    NOP();
                    movimientox = stringtolong(entrada, tama2);
                    posicionx = movimientox + posicionx;
                    writeRegister(0, posicionx);
                    stepsx = movimientox * relacionx;

                    enter = 0;
                    tama = 0;

//                  
                }
            } else if (ytrue == 0) {//escribieron MOVERY, se hace lo mismo que el caso anterior
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
                    writeRegister(1, posiciony);
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
                    writeRegister(1, posiciony);
                    stepsy = movimientoy*relaciony;
                    enter = 0;
                    tama = 0;
                    NOP();
                }
            } else if (infotrue == 0) {//escribieron posicion
                posicionactualx = readRegister(0); //se lee el registro de memoria en la posicion 0
                posicionactualy = readRegister(1); //se lee el reegistro de memoria en la posicion 1
                sprintf(salida, "posicion en eje x : %d ", posicionactualx); //se imprime cual es el valor de la posicion
                enviarTrama(salida);
                sprintf(salida, "posicion en eje y : %d ", posicionactualy);
                enviarTrama(salida);
                tama = 0;
               
                enter = 0;
            } else if (devolvertrue == 0) {//introdujeron el comando DEVOLVER, por lo cual quremos llevar los ejes a la posicion inicial
                stepsy_auxiliar = posiciony*relaciony; //llevar el eje y a cero
                stepsy_auxiliar = stepsy_auxiliar * (-1); //mover el carro justo la posicion contraria en la que se encuentra
                ejeynegativo(stepsy_auxiliar);
                
                stepcount = 0;
                stepsy_auxiliar = 0;
               
                stepsx_auxiliar = posicionx*relacionx; //llevar el eje y a cero
                stepsx_auxiliar = stepsx_auxiliar * (-1);
                ejexnegativo(stepsx_auxiliar);
                apagarM2();
                apagarM1();
                stepcount = 0;
                stepsx_auxiliar = 0;
                enter = 0;
                posiciony = 0;
                writeRegister(1, posiciony);
                 posicionx = 0;
                writeRegister(0, posicionx);
                sprintf(salida, "posicion en eje x : %d ", posicionx);
                enviarTrama(salida);
                sprintf(salida, "posicion en eje y : %d ", posiciony);
                enviarTrama(salida);
                devolvertrue = 1;
                tama = 0;
                movimiento = 0 ;
            } else if (cerotrue == 0) {
                posicionx = 0;
                posiciony = 0;
                writeRegister(1, posiciony);
                writeRegister(0, posicionx);
                cerotrue = 1;
                tama = 0;
                enter = 0;
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

        if (xtrue == 0) {
         
            if (posicionx >= 0 && posicionx <= 18) { //limites de movimiento mecanico
                stepsy_auxiliar = posiciony*relaciony; //llevar el eje y a cero debido a problema mecanico
                stepsy_auxiliar = stepsy_auxiliar * (-1);
                ejeynegativo(stepsy_auxiliar);
                apagarM2();
                stepcount = 0;
                stepsy_auxiliar = 0;
                //               
                if (stepsx > 0) { //mover hacia la derecha el motor

                    ejexpositivo(stepsx);

                    apagarM1();
                    stepcount = 0;
                    stepsx = 0;
                    movimiento = 0;
                    movimientox = 0;


                } else if (stepsx < 0) { //mover hacia la izquierda

                    ejexnegativo(stepsx);

                    apagarM1();
                    stepcount = 0;
                    stepsx = 0;
                    movimiento = 0;


                }
                stepsy_auxiliar = posiciony*relaciony; //llevar el eje y a POSICION en la que se encontraba
                stepsy_auxiliar = stepsy_auxiliar;
                ejeypositivo(stepsy_auxiliar);
                apagarM2();
                stepcount = 0;
                stepsy_auxiliar = 0;

                sprintf(salida, "posicion en eje x : %d ", posicionx);
                enviarTrama(salida);
                movimiento = 0;

            } else {
                enviarTrama(norango); //la posicion solicitada no se encuentra en el rango deseado
                posicionx = posicionx - movimientox;
                movimientox = 0;
                enviarTrama(rangox);
                writeRegister(0, posicionx);
            }
            xtrue = 1;
        }

        if (ytrue == 0) {
            if (posiciony >= 0 && posiciony <= 23) { //limites mecanicos del desplazador
                if (stepsy >= 0) {
                    ejeypositivo(stepsy); //movery positivamente
                    sprintf(salida, "posicion en eje y : %d ", posiciony);
                    enviarTrama(salida);
                    apagarM2();
                    stepcount = 0;
                    stepsy = 0;
                    movimiento = 0;
                    movimientoy = 0;

                }
                if (stepsy < 0) {
                    ejeynegativo(stepsy); //movery negativamente
                    sprintf(salida, "posicion en eje y : %d ", posiciony);
                    enviarTrama(salida);
                    apagarM2();
                    stepcount = 0;
                    stepsy = 0;
                    movimiento = 0;
                    movimientoy = 0;

                }

            } else {
                enviarTrama(norango); //la posicion solicitada no se encuentra en el rango mecanico
                enviarTrama(rangoy);
                posiciony = posiciony - movimientoy;
                movimientoy = 0;
                writeRegister(1, posiciony);
            }
            ytrue = 1;

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

long stringtolong(char string[], int tamanio) { //conversion de una cadena de caracteres de cierto tamaño a un entero 
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
    help = 0;
}

void apagarM1() { //apagar el motor 1
    LATA = 0;
}

void apagarM2() { //apagar el motor 2
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

int readRegister(int direccionR) { //leer un registro de la EEPROM
    int datoR;
    EEADR = direccionR;
    EECON1bits.EEPGD = 0;
    EECON1bits.CFGS = 0;
    EECON1bits.RD = 1;
    datoR = EEDATA; //EL VALOR SE ALMACENA EEDAT 
    return datoR;
}

void writeRegister(int direccionW, int datoW) { //escribir un registro en la EEPROM
    EEDATA = datoW; // guardamos la localizacion de memoria 
    EEADR = direccionW; //                   
    EECON1bits.EEPGD = 0; //Acceder a los datos de memoria
    EECON1bits.CFGS = 0;
    EECON1bits.WREN = 1; //HABILITAR ESCRITURA 
    INTCONbits.GIE = 0; //DESACTIVAR INTERRUPCIONES GLOBALES//
    EECON2 = 0x55;
    EECON2 = 0xAA;
    EECON1bits.WR = 1; //SOLO SE PUEDE ACTIVAR SI WREN ESTÁ EN 1  
    while (EECON1bits.WR != 0) {
    }
    EECON1bits.WREN = 0; //SI SE DESACTIVA NO AFECTA EL CICLO DE ESCRITURA                    
    PIR2bits.EEIF = 0; // 
    INTCONbits.GIE = 1; //ACTIVAR INTERRUPCIONES GLOBALES//
}


