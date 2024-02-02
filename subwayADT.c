
#include "subwayADT.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>

#define DIFF ('Z' - 'A')
#define POS(n) ((n) - DIFF ) //This macro gives us the number of the line.
#define PERIODSINTER 2 //This gives us the boundaries of the periods.
#define CANTPERIODS 4 //This gives us the number of periods.
#define LEAPYEAR 1
#define TOTALMONTH 12



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
    size_t maxYear;
}Tstation;

typedef struct Tline{
    size_t passenTot; //pasajeros por linea
    Tstation * station; //vector donde los id de las estaciones son la posicion al estos ser mayor a 0 CREO
    size_t dimStation;
    Tlist top; //QUERY 2
}Tline;


typedef struct subCDT{
    char * line; //vector donde se indica la linea a la que pertenece cada id

    size_t idMax;

    Tline * lines; //vector donde cada posicion es una linea de subte y estan los results
    size_t dimLines;

    Tlist list1; //QUERY 1

    info days[4][7]; //QUERY 3 

    avgTop list4; //QUERY 4

    size_t yearEnd;
    size_t yearStart;
}subCDT;



//Prototipos:


// This function gives a number from 0 to 3 indicating what period the journey was made.
// It returns a number from 0 to 3, this correlates with a given period.
static int getPeriod(char startHour, char endHour);

// It checks if the year is a leap year or not.
// Returns LEAPYEAR or !LEAPYEAR.
static char leapYearCalc(size_t year);

//This function gives us what day of a week a certain date represents.
// For example, for 2/02/2024 it will give us the number 5, as that number represents friday.
static int getDayOfWeek(size_t day, size_t month, size_t year, size_t leapYear)


subADT newSub(size_t startYear, size_t endYear){
    subADT aux = calloc(1,sizeof(subCDT));
    aux->startYear =  startYear;
    aux->endYear = endYear;
    return aux;
    
}


void addStations(subADT sub, char line, char * name, size_t stationID){
    errno=OK;
    if(stationID >= sub->idMax){
        sub->line = realloc(sub->line, stationID);//it is char so it is not necessary to multiply by sizeof
        sub->idMax=stationID;
        if(errno==ENOMEM || sub->line == NULL){
            errno = MEMERR;
            return; //ESTA BN ASI CHEQUEO DE ALLOC?????
        }
    }

    char lineUp=toupper(line);
    sub->line[stationID]=lineUp;
    size_t pos=POS(lineUp);
    if(sub->dimLines <= pos){
        sub->lines = realloc(sub->lines, pos*sizeof(Tline));
        sub->dimLines=pos;
        if(errno==ENOMEM || sub->lines == NULL){
            errno = MEMERR;
            return;
        }
    }
    sub->lines[pos].station->name=malloc(strlen(name));

    if(errno==ENOMEM || sub->lines[pos].station->name == NULL){
        errno = MEMERR;
        return;
    }
     sub->lines[POS(lineUp)].station->name=strcpy(sub->lines[POS(lineUp)].station->name, name);
    //ACA VA UN CHECK ERROR por el strcpy ????????????
    
}



void addDataTrips(subADT sub, char day, char month, size_t year, size_t stationID, size_t numPassen, char start, char end){
    errno = OK;

    size_t lineNum = POS(sub->line[stationID]); //Here we get the line of the station.
    
    sub->lines[lineNum].passenTot += numPassen; // Here the number of passengers of a line increases.

    char isLeapYear = leapYearCalc(year);  //This helps calculate the day of the week if it is a leap year.

    sub->lines[lineNum].station[stationID].days[getPeriod(start,end)][getDayOfWeek(day,month,year,isLeapYear)] += numPassen; //Here we add passengers to a given day and period.

    size_t largestYear = sub->lines[lineNum].station[stationID].maxYear; //This will help us know if we need to expand Tmonth * historyMonth[12] vector.

    //POSIBLE PROBLEMA cuando probamos nos fijamos.
    if(sub->yearEnd != 0 && sub->yearStart != 0 && sub->lines[lineNum].station[stationID].maxYear == 0){
        //Aca creamos la matriz cunado nos pasan los dos parametros.
        sub->lines[lineNum].station[stationID].historyMonth = calloc(sub->yearEnd, sizeof(Tmonth *) )
        if (errno == ENOMEM || sub->lines[lineNum].station[stationID].historyMonth != NULL ){
            errno = MEMERR;
            return;
        }
        for (int i = sub->yearStart; i <=  sub->yearEnd; i++){
            sub->lines[lineNum].station[stationID].historyMonth[i] = calloc(TOTALMONTH, sizeof(Tmonth)); //Me parece jugado de ultima ponemos un for que rellene con 0 va de 0 a 12.
            if (errno == ENOMEM || sub->lines[lineNum].station[stationID].historyMonth[i] != NULL ){
                errno = MEMERR;
                return;
            }
        }
        sub->lines[lineNum].station[stationID].maxYear = -1 //igualo a 0 para q no vuelva a entrar
    }else if ( sub->lines[lineNum].station[stationID].maxYear != -1 &&  year >= largestYear){
        sub->lines[lineNum].station[stationID].historyMonth = realloc(sub->lines[lineNum].station[stationID].historyMonth, year * sizeof(Tmonth *));
        if (errno == ENOMEM || sub->lines[lineNum].station[stationID].historyMonth == NULL ){ //ACA me dice esto: Comparison of array 'sub->lines[lineNum].station[stationID].historyMonth' equal to a null pointer is always false
            errno = MEMERR;
            return;
        }
        for (int i = largestYear; i <= year; i++){
            sub->lines[lineNum].station[stationID].historyMonth[i] = calloc(TOTALMONTH, sizeof(Tmonth)); //Me parece jugado de ultima ponemos un for que rellene con 0 va de 0 a 12.
            if (errno == ENOMEM || sub->lines[lineNum].station[stationID].historyMonth[i] != NULL ){
                errno = MEMERR;
                return;
            }
        }
    }

    if (sub->lines[lineNum].station[stationID].historyMonth[year][month].numDay == 0){ //Preguntar si esta bien esto historyMonth[year][month] o  hay q poner historyMonth[year]->numday.
        //Creo que se podria hacer mejor (osea un solo vector).
        //podemos hacer una matriz osea -> char daysOfMonth[12][2]={{31,29,31,30,31,30,31,31,30,31,30,31},{31,28,31,30,31,30,31,31,30,31,30,31}}
        //tambn podemos considerar el caso de febrero aparte y chequear si es leap solo cuando toca mes 2. Pense en hacer esto pero queria hablarlo con ustedes.
        char daysOfMonthLeap[] = {31,29,31,30,31,30,31,31,30,31,30,31};
        char daysOfMonthNoLeap[] = {31,28,31,30,31,30,31,31,30,31,30,31};
        if (isLeapYear){
            sub->lines[lineNum].station[stationID].historyMonth[year][month].numDay = daysOfMonthLeap[month];
        } else{
            sub->lines[lineNum].station[stationID].historyMonth[year][month].numDay = daysOfMonthNoLeap[month];
        }
    }
    sub->lines[lineNum].station[stationID].historyMonth[year][month].totalMonth += numPassen;

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
    if ( (year % 4 == 0 && year % 100 != 0) || year % 400 == 0 ){ //This are the conditions to know if a given year is a leap year. 
        return LEAPYEAR;
    }
    return !LEAPYEAR;
}

static int getDayOfWeek(size_t day, size_t month, size_t year, size_t leapYear){
        static size_t t[] = { 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };
        year -= month < 3;
        size_t dayOfWeek = ( (y + y / 4 - y / 100 + y / 400 + t[m - 1] + d) % 7 ) - leapYear;
        if (day == 0){ //With formula 0 position is given to sundays, in our case sundays need to have the value of 6.
            return 6;
        }
        return dayOfWeek - 1; //We subtract 1 given that as we need sundays to have the value 6.
}