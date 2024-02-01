
#include "subwayADT.h"
#include <stdio.h>
#include <stdlib.h>


#define DIFF ('Z' - 'A') //Ver como cata carga los letras de la lineas si en mayuscula o miniscula.
#define POS(n) ((n) - DIFF ) //This macro gives us the number of the line.
#define PERIODSINTER 2 //This gives us the boundaries of the periods.
#define CANTPERIODS 4 //This gives us the number of periods.
#define LEAPYEAR 1




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
}subCDT;



//Prototipos:

//This function checks from what line a station is part of.
// Returns a letter meaning the name of the line.
static char getLine(size_t stationID, char * lines);

//This function gives a number from 0 to 3 indicating what period the journey was made.
//It returns a number from 0 to 3, this correlates with a given period.
static int getPeriod(char startHour, char endHour);

//It checks if the year is a leap year or not.
//Returns LEAPYEAR or !LEAPYEAR.
static char leapYearCalc(size_t year)









void addData(subADT sub, char day, char month, size_t year, size_t stationID, size_t cantPassen, char start, char end){
    
    size_t lineNum = POS(getLine(stationID,sub->line)); //Here we get the line of the station. 
    
    sub->lines[lineNum] += cantPassen; // Here the amount of passengers of a line increases.

    sub->lines[lineNum].station[stationID].days[getPeriod(start,end)][day] += cantPassen //Here we add passengers to a given day and period.

    size_t largestYear = sub->lines[lineNum].station[stationID].yearEnd; //This will help us know if we need to expand Tmonth * historyMonth[12] vector.

    //Here we expand the vector so every year is part of it.
    if (year >= largestYear){
        size_t newLargestYear = year+1;
        for (int i = largestYear; i < newLargestYear; i++ ){
            sub->lines[lineNum].station[stationID].historyMonth[i] = calloc(1,Tmonth);
        }
        sub->lines[lineNum].station[stationID].yearEnd = newLargestYear;
    }
    sub->lines[lineNum].station[stationID].historyMonth[year]->totalMonth += cantPassen;

    if (sub->lines[lineNum].station[stationID].historyMonth[year][month].numDay == 0){ //Preguntar si esta bien esto historyMonth[year][month] o  hay q poner historyMonth[year]->numday.
        //Creo que se podria hacer mejor (osea un solo vector).
        char daysOfMonthLeap[] = {31,29,31,30,31,30,31,31,30,31,30,31};
        char daysOfMonthNoLeap[] = {31,28,31,30,31,30,31,31,30,31,30,31};
        if (leapYearCalc(year)){
            sub->lines[lineNum].station[stationID].historyMonth[year][month].numDay = daysOfMonthLeap[month];
        } else{
            sub->lines[lineNum].station[stationID].historyMonth[year][month].numDay = daysOfMonthNoLeap[month];
        }
    }

}

static char getLine(size_t stationID, char * lines){
    return lines[stationID]; //Preguntar a Cata como va a cargar los datos de las lineas si es stationID o stationID - 1. 
}


static int getPeriod(char startHour, char endHour){
  char periods [][PERIODSINTER] = {{6,11}, {11,13},{13,17},{17,6}}; //Ver que onda esto usa magic numbers.
    for (int i = 0; i < CANTPERIODS ; i++) {
        if ( (startHour >= periods[i][0]) && (endHour <= periods[i][1]) ){
            return i;
        }
    }
}

static char leapYearCalc(size_t year){
    if ( year % 4 == 0 && year % 100 != 0 || year % 400 == 0 ){
        return LEAPYEAR;
    }
    return !LEAPYEAR;
}
