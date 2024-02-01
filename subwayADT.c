//
// Created by Santino Pepe on 31/01/2024.
//

#include "subwayADT.h"
#include <stdio.h>

typedef enum{MORNING=0, LUNCH, NOON, NIGHT}HOURS;

typedef struct info{ //PONER MEJOR NOMBRE
    char * name;
    size_t numPasan;
}info;

typedef struct node{
    char * name; //para el query 1 es la linea y para el query 2 el nombre
    size_t numTot;
    struct node * tail;
}Tnode;

typedef struct avgTop{
    char * name;
    float avg;
    size_t year;
    char month;
    struct avgTop * tail;
}avgTop;

typedef struct node * Tlist;

typedef struct Tmonth{
    size_t totalMonth;
    int numDay;//dias que hay en el mes (debe haber una formula para sacarlo por lo cual esto no deberia ir probablemente)
}Tmonth;

typedef struct Tstation{
    char * name;
    size_t pasenStation; //para query 2 no va a ser lo mas eficiente pero hay que usar el id para la busqueda de mas 
    size_t days[4][7]; //matriz de dias de la semana con periodos del dia
    Tmonth * historyMonth[12]; //le puse el asterisco para que busque su año y mes y ponga ahi la cantidad de gente
    size_t yearEnd;
}Tstation;

typedef struct Tline{
    size_t pasaenTot; //pasajeros por linea
    Tstation * station; //vector donde los id de las estaciones son la posicion al estos ser mayor a 0 CREO
    size_t dimStation;
    Tlist top; //QUERY 2
}Tline;


typedef struct subCDT{
    char * line; //vector

    Tline * lines; //vector donde cada posicion es una linea de subte y estan los results
    size_t dimLines;

    Tlist list1; //QUERY 1

    info days[4][7]; //QUERY 3 

    avgTop list4; //QUERY 4
};