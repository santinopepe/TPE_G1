//
// Created by Santino Pepe on 31/01/2024.
//

#include "subwayADT.h"
#include <stdio.h>

enum{MORNING=0, LUNCH, NOON, NIGHT};

typedef struct Tmonth{
    size_t totalMonth;
    int numDay;//dias que hay en el mes (debe haber una formula para sacarlo por lo cual esto no deberia ir probablemente)
}Tmonth;

typedef struct Tweek{ //quedamos con la duda si hacerlo como dice cata (consultar pepe)
    size_t period[4]; //para cada periodo de el dia donde almacenar la cant de pasajeros en ese periodo (QUERY 3)
}Tweek;

typedef struct Tstation{
    char * name;
    size_t pasenStation; //para query 2 no va a ser lo mas eficiente pero hay que usar el id para la busqueda de mas 
    Tweek days[7]; //dias de la semana para almacenar la cant de gente por estacion
    Tmonth * historyMonth[12]; //le puse el asterisco para que busque su año y mes y ponga ahi la cantidad de gente
    size_t yearEnd;
}Tstation;

typedef struct Tline{
    size_t pasaenTot; //pasajeros por linea
    Tstation * station; //vector donde los id de las estaciones son la posicion al estos ser mayor a 0 CREO
    size_t numStation;
    size_t dimStation;
}Tline;

typedef struct subADT{
    size_t numLines;
    Tline * lines; //vector donde cada posicion es una linea de subte
    size_t dimLines;
};