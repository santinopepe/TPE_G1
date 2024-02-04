
#include "subwayADT.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>

#define DIFF ('A')
#define POS(n) ((n) - DIFF) //This macro gives us the number of the line.
#define PERIODSINTER 2 //This gives us the boundaries of the periods.
#define CANTPERIODS 4 //This gives us the number of periods.
#define LEAPYEAR 1
#define TOTALMONTH 12
#define CANTWEEKDAYS 7
#define NOID -1
#define NOTOPSTATION "S/D"


typedef enum{MORNING=0, LUNCH, NOON, NIGHT}HOURS;


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
    size_t days[CANTPERIODS][CANTWEEKDAYS]; //matriz de dias de la semana con periodos del dia
    Tmonth * historyMonth[TOTALMONTH]; //le puse el asterisco para que busque su año y mes y ponga ahi la cantidad de gente
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
    Tlist it1;

    size_t it2; //QUERY 2 
    //PREG SI ESTA OK EL IT 2 QUE VA A SER NOMAS EL INDICE EN EL VECTOR DE LINEAS
    //No entiendo este iterador osea como funca.
    size_t days[CANTPERIODS][CANTWEEKDAYS]; //QUERY 3

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
static int getDayOfWeek(size_t day, size_t month, size_t year, size_t leapYear);

//recursive function to add a node to a list of lines that goes 
//from the one with most passengers to the one with the least passengers
static Tlist addListAmountPassenRec(Tlist list, size_t numPassen, char * line);

//function that iterates in a vector with all the information from all the lines to create
// a list that goes from the line with the most passengers to the one with the least
static void addListAmountPassen(subADT sub);

//This function creates the list that contains the top 3 stations of each line.
static void StationLineTop (subADT sub);

//This is a recursive function that helps the creation of the top 3.
//It returns the top 3 stations of each line.
static Tlist StationLineTopRec (Tlist top, size_t NumPassen, char * StationName, char  * TopFlag);

//This function completes the days matrix
static void PeriodWeekDayTop(subADT sub);

//Returns the ID of the most popular station by period and weekday,
//it complements the following function PeriodWeekDayTop.
static size_t TopPeriodStation (subADT sub, int weekday, int period);


subADT newSub(size_t startYear, size_t endYear){
    subADT aux = calloc(1,sizeof(subCDT));
    aux->yearStart =  startYear;
    aux->yearEnd = endYear;
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
        sub->lines[lineNum].station[stationID].historyMonth = calloc(sub->yearEnd, sizeof(Tmonth *) );
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
        sub->lines[lineNum].station[stationID].maxYear = -1;//igualo a 0 para q no vuelva a entrar
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
        size_t dayOfWeek = ( (year + year / 4 - year / 100 + year / 400 + t[month - 1] + day) % 7 ) - leapYear;
        if (day == 0){ //With formula 0 position is given to sundays, in our case sundays need to have the value of 6.
            return 6;
        }
        return dayOfWeek - 1; //We subtract 1 given that as we need sundays to have the value 6.
}


void toBeginLines(subADT sub){
    //armar lista en orden de cantidad de pasajeros
    sub->it1=sub->list1;
}

void toBeginTopbyLine(subADT sub){
    //Armar lista con orden alfabetico.

}

//returns if there is another line next
int hasNextLine(subADT sub){
    errno = OK;
    if(sub != NULL){ //Creo q hay que hacer esto que es progrmacion defensiva q es algo q nos corrigieron
        return sub->it1 != NULL && sub->it1->tail != NULL;
    } else{
        errno = ARGERR; //Esto podria ser otro tipo de error, un PARAMERROR.
        return 0;
    }
}

int hasNextTopbyLine(subADT sub){

}

/*
returns the number of passengers in the line and uses the parameter line to return the line letter
to which the amount of passengers belong
and changes the iterator to the next line in order to start with the line with
the most assengers and finish with the one with least
*/

int nextLine(subADT sub, char * line); 
// uses the matrix res to return the top 3 stations with most passengers from one line
// and changes the iterator to the next line in alphabetic order
void nextTopbyLine(subADT sub, char * res[3]);

static void addListAmountPassen(subADT sub){
    for (int i=0; i<sub->dimLines; i++){
        if(sub->lines->dimStation!=0){
            //chequear como hacer la conversion de la i q es la posicion del vector a la letra que es
            //Me parece que si haces sub->line[i] te va a dar la letra de la linea.
            sub->list1=addListAmountPassenRec(sub->list1, sub->lines[i].passenTot,POS(i)); 
        }
    }
}


static Tlist addListAmountPassenRec(Tlist list, size_t numPassen, char * line){
    errno=OK;
    int c;
    if(list==NULL || list->numTot<numPassen){
        c = list->numTot-numPassen;
        if(list == NULL || c!=0 || (c == 0 && (*line<list->name))){ //Me parece que esta rara la ultima condicion.
              Tlist aux = malloc(sizeof(struct node));
            if(errno == ENOMEM){
                errno= MEMERR;
                return list; 
            }
            aux->name=malloc(sizeof(char));
            aux->name=strcpy(aux->name,line); //Me parece raro mejor no seria apuntar a la posicion del nombre ???
            aux->numTot=numPassen;
            aux->tail = list;
            return aux;
        }
    }

    //si no cumple con las condiciones pasa al siguinte
         list->tail = addListAmountPassenRec(list->tail, numPassen, line);
         return list;
}
    
static void StationLineTop (subADT sub){
    for (size_t i = 0; i < sub->dimLines; i++){ //We move to each line.
        for(size_t j = 0; j < sub->lines[i].dimStation; j++){ //We move inside each line to every station.
            char * TopFlag = 0; //This flag helps not create all the list given that if a statiuon doesn't enter top 3  it quits the comparison.
            sub->lines[i].top = StationLineTopRec(sub->lines[i].top, sub->lines[i].passenTot, sub->lines[i].station[j].name, TopFlag);
        }
    }
}
static Tlist StationLineTopRec (Tlist top, size_t NumPassen, char * StationName, char  * TopFlag){
    if (top == NULL || NumPassen > top->numTot){
        Tlist aux = malloc(sizeof(struct node));
        if(errno == ENOMEM){
            errno= MEMERR;
            return list;
        }
        aux->name = StationName;
        aux->numTot=NumPassen;
        aux->tail = list;
        return aux;

    }
    (*TopFlag)++;
    if (*TopFlag == 3){ //If this condition is true the station doesn't enter top 3, so we don't care.
        return top;
    }
    top->tail = StationLineTopRec(top->tail, NumPassen, StationName, TopFlag);
    return top;
}


// Esto es un kilombo y bastante feo a mi parecer PENSAR DEVUElTA
// Podriamos hacer un vector cuando cargamos los datos y usar swaps??? Me parce mejor.
static void PeriodWeekDayTop(subADT sub){
    for (int i = 0; i < CANTWEEKDAYS; i++){
        for (int j = 0; j < CANTPERIODS; j++){
            sub->days[j][i] = TopPeriodStation(sub,i,j);
        }
    }
}

static size_t TopPeriodStation (subADT sub, int weekday, int period){
    size_t TopID;
    size_t TopStationPassen = 0;
    for (size_t i = 0; i < sub->dimLines; i++){ //Recorro las lineas
        for (size_t j = 0;  j < sub->lines[i].dimStation; j++){ //Recorro las estaciones dentro de las lineas
            if (TopStationPassen <= sub->lines[i].station[j].days[period][weekday]){
                if ((TopStationPassen == sub->lines[i].station[j].days[period][weekday] && (strcasecmp(sub->lines[POS(sub->line[TopID])].station[TopID].name,sub->lines[i].station[j].name)) > 0) ||  TopStationPassen != sub->lines[i].station[j].days[period][weekday]){ //Un quilombo el strcasecmp puede estar mal revisar.
                    TopStationPassen = TopStationPassen == sub->lines[i].station[j].days[period][weekday];
                    TopID = j;
                }
            }
        }
    }
    if (TopStationPassen == 0){
        return NOID;
    }
    return TopID;
}

char * getTopStationPeriod (subADT sub, int period, int weekday, char * line){
    if ( (period < 0 || period > CANTPERIODS) || (weekday > CANTWEEKDAYS || weekday < 0)){
        (*line) = NULL;
        return NULL;
    }
    size_t id = sub->days[period][weekday];
    if (id == NOID){
        (*line) = NULL;
        return NOTOPSTATION;
    }
    (*line) = sub->line[id];
    return sub->lines[POS((*line))].station[id].name;
}

