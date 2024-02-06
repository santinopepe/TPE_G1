
#include "subwayADT.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>


#define DIFF ('A')
#define POS(n) ((n) - DIFF) //This macro gives us the position in the vector according to the line.
#define PERIODSINTER 2 //This gives us the boundaries of the periods.
#define CANTPERIODS 4 //This gives us the number of periods.
#define LEAPYEAR 1
#define TOTALMONTH 12
#define CANTWEEKDAYS 7
#define NOID -1
#define NOTOPSTATION "S/D"
#define ERROR -1



typedef enum{MORNING=0, LUNCH, NOON, NIGHT}HOURS;

typedef struct node{
    char * name; //para el query 1 es la linea y para el query 2 el nombre
    size_t numTot;
    struct node * tail;
}Tnode;

typedef struct avgTop{
    char * name;
    char line;
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
    char line;
    size_t passenStation; //para query 2 no va a ser lo mas eficiente pero hay que usar el id para la busqueda de mas 
    size_t days[CANTPERIODS][CANTWEEKDAYS]; //matriz de dias de la semana con periodos del dia
    Tmonth * historyMonth[TOTALMONTH]; //le puse el asterisco para que busque su año y mes y ponga ahi la cantidad de gente
    size_t maxYear;
}Tstation;

typedef struct Tline{
    size_t passenTot; //Total passengers per line
    Tlist top; //Top 3 stations with most passengers per line
}Tline;


typedef struct subCDT{
    Tstation * station; //vector where each position corresponds to the id of a station and has its information 
    size_t dimStation; // dimension of the vector station

    Tline * lines; //para el Q1 y Q2 (se guardan los datos en el mismo vector)
    size_t dimLines;
    size_t it2; //QUERY 2 (es el iterador para acceder a cada linea)(es el indice)

    Tlist list1;
    Tlist it1;

    size_t days[CANTPERIODS][CANTWEEKDAYS]; //QUERY 3

    avgTop * list4; //QUERY 4
    avgTop * it4; //Cambiar nombre.


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
static Tlist addListAmountPassenRec(Tlist list, size_t numPassen, char line);


//function that iterates in a vector with all all the lines to create
// a list that goes from the line with the most passengers to the one with the least 
//in case 2 lines have the same number of passengers they go in alphabetical order
static void addListAmountPassen(subADT sub);

//This function creates a vector with the lines with a list that 
//contains the top 3 stations and the total of passengers of each line.
static void StationLineTop (subADT sub);

//This is a recursive function that helps the creation of the top 3.
//It returns the top 3 stations of each line or less if there are less stations

static Tlist StationLineTopRec (Tlist top, size_t NumPassen, char * StationName, char  * TopFlag);

//function that puts the names of the top3 stations in a line in the vector res
//in case the line has less than 3 stations the positions in the vector stay empty
static void returnTopbyLine(Tlist list, char ** res);


//Q3:


//Returns the ID of the most popular station by period and weekday,
//it complements the following function PeriodWeekDayTop.
static size_t TopPeriodStation (subADT sub, int weekday, int period);


static size_t bestStationMonth(Tstation station, char * topMonth, float monthAvg, size_t start, size_t end);


static avgTop * createAvgTopRec(avgTop * list, char topMonth, size_t topYear, float monthAvg, char line, char * name);

static void freeList(Tlist list);



subADT newSub(size_t startYear, size_t endYear){
    subADT aux = calloc(1,sizeof(subCDT));
    aux->yearStart =  startYear;
    aux->yearEnd = endYear;
    return aux;
    
}


void addStations(subADT sub, char line, char * name, size_t stationID){
    errno=OK;
    if(stationID >= sub->dimStation){
        sub->station = realloc(sub->station, stationID*sizeof(Tstation));
        sub->dimStation=stationID;
        if(errno==ENOMEM || sub->station == NULL){
            errno = MEMERR;
            return; //ESTA BN ASI CHEQUEO DE ALLOC?????
        }
    }

    char lineUp=toupper(line);
    sub->station[stationID].line=lineUp;
    sub->station[stationID].name = malloc(strlen(name)+1);

    if(errno==ENOMEM || sub->station[stationID].name == NULL){
        errno = MEMERR;
        return;
    }
    sub->station[stationID].name=strcpy(sub->station[stationID].name, name);
    //ACA VA UN CHECK ERROR por el strcpy ????????????
}


void addDataTrips(subADT sub, char day, char month, size_t year, size_t stationID, size_t numPassen, char start, char end){
    errno = OK;
    
    sub->station[stationID].passenStation += numPassen; // Here the number of passengers of a station increases.

    char isLeapYear = leapYearCalc(year);  //This helps calculate the day of the week if it is a leap year.

    sub->station[stationID].days[getPeriod(start,end)][getDayOfWeek(day,month,year,isLeapYear)] += numPassen; //Here we add passengers to a given day and period.


    size_t largestYear = sub->station[stationID].maxYear; //This will help us know if we need to expand Tmonth * historyMonth[12] vector.

    //POSIBLE PROBLEMA cuando probamos nos fijamos
    if(sub->yearEnd != 0 && sub->yearStart != 0 && sub->station[stationID].maxYear == 0){
        //Aca creamos la matriz cunado nos pasan los dos parametros.
        sub->station[stationID].historyMonth = calloc(sub->yearEnd, sizeof(Tmonth *) ); //CHEQUEAR ESTO (le ponemos historyMonth[12] se soluciona)
        if (errno == ENOMEM || sub->station[stationID].historyMonth != NULL ){
            errno = MEMERR;
            return;
        }
        for (int i = sub->yearStart; i <=  sub->yearEnd; i++){
            sub->station[stationID].historyMonth[i] = calloc(TOTALMONTH, sizeof(Tmonth)); //Me parece jugado de ultima ponemos un for que rellene con 0 va de 0 a 12.
            if (errno == ENOMEM || sub->station[stationID].historyMonth[i] != NULL ){
                errno = MEMERR;
                return;
            }
        }
        sub->station[stationID].maxYear = -1;//igualo a 0 para q no vuelva a entrar
    }else if ( sub->station[stationID].maxYear != -1 &&  year >= largestYear){
        sub->station[stationID].historyMonth = realloc(sub->station[stationID].historyMonth, year * sizeof(Tmonth *)); //CHEQUEAR ES COMO EL DE LA LINEA 171
        if (errno == ENOMEM || sub->station[stationID].historyMonth == NULL ){ //ACA me dice esto: Comparison of array 'sub->lines[lineNum].station[stationID].historyMonth' equal to a null pointer is always false
            errno = MEMERR;
            return;
        }
        for (int i = largestYear; i <= year; i++){
            sub->station[stationID].historyMonth[i] = calloc(TOTALMONTH, sizeof(Tmonth)); //Me parece jugado de ultima ponemos un for que rellene con 0 va de 0 a 12.
            if (errno == ENOMEM || sub->station[stationID].historyMonth[i] != NULL ){
                errno = MEMERR;
                return;
            }
        }

    }
    if (sub->yearStart <= year && (sub->yearEnd >= year || sub->yearEnd == 0)){ //Preguntar si esta bien esto historyMonth[year][month] o  hay q poner historyMonth[year]->numday.

        sub->station[stationID].historyMonth[year][month].totalMonth += numPassen;

        if (sub->station[stationID].historyMonth[year][month].numDay == 0){
            //Creo que se podria hacer mejor (osea un solo vector).
            //podemos hacer una matriz osea -> char daysOfMonth[12][2]={{31,29,31,30,31,30,31,31,30,31,30,31},{31,28,31,30,31,30,31,31,30,31,30,31}}
            //tambn podemos considerar el caso de febrero aparte y chequear si es leap solo cuando toca mes 2. Pense en hacer esto pero queria hablarlo con ustedes.
            char daysOfMonthLeap[] = {31,29,31,30,31,30,31,31,30,31,30,31};
            char daysOfMonthNoLeap[] = {31,28,31,30,31,30,31,31,30,31,30,31};
            /*podes hacer if(month==2){
                sub->station[stationID].historyMonth[year][month].numDay = daysOfMonth[month]+isLeapYear;
            }
            y tenes un solo vector, si es q leapYear es 1 o 0
            */
            if (isLeapYear){
                sub->station[stationID].historyMonth[year][month].numDay = daysOfMonthLeap[month];
            } else{
                sub->station[stationID].historyMonth[year][month].numDay = daysOfMonthNoLeap[month];
            }
        }
    }
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
    StationLineTop(sub);
    //to make a list with the lines in order of how many passengers they have when the user/frontend  asks for it
    addListAmountPassen(sub);
    sub->it1=sub->list1;
}

//RARO HACER UN ITERADOR QUE SEA EL INDICE PERO SINO NO C COMO MANEJAR ESTO
void toBeginTopbyLine(subADT sub){
    sub->it2=0;
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
return (sub->it2 <= sub->dimLines); 
}


int nextLine(subADT sub, char * line){
    *line=sub->it1->name;
    int res=sub->it1->numTot;
    sub->it1=sub->it1->tail;
    return res;
}


static void addListAmountPassen(subADT sub){
    for (size_t i=0; i < sub->dimLines; i++){
        if(sub->dimStation != 0){
            sub->list1=addListAmountPassenRec(sub->list1, sub->lines[i].passenTot, (char)i + DIFF); 
        }
    }
}


static Tlist addListAmountPassenRec(Tlist list, size_t numPassen, char line){
    errno=OK;
    int c;
    if(list==NULL || list->numTot<numPassen){
        c = list->numTot - numPassen;
        if(list == NULL || c!=0 || (c == 0 && (line < list->name[0]))){ //Me parece que esta rara la ultima condicion.
              Tlist aux = malloc(sizeof(struct node));
            if(errno == ENOMEM){
                errno = MEMERR;
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
        for(size_t j = 0; j < sub->dimStation; j++){ //We move inside each line to every station.
            char * TopFlag = 0; //This flag helps not create all the list given that if a statiuon doesn't enter top 3  it quits the comparison.
            char line = sub->station[j].line;
            sub->lines[POS(line)].passenTot += sub->station[j].passenStation;
            sub->lines[POS(line)].top = StationLineTopRec(sub->lines[POS(line)].top, sub->lines[POS(line)].passenTot, sub->station[j].name, TopFlag);
        }
}



static Tlist StationLineTopRec (Tlist top, size_t NumPassen, char * StationName, char  * TopFlag){
    if (top == NULL || NumPassen > top->numTot){
        Tlist aux = malloc(sizeof(struct node));
        if(errno == ENOMEM){
            errno= MEMERR;
            return top;
        }
        aux->name = StationName;
        aux->numTot=NumPassen;
        aux->tail = top;
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
void toBeginTopPeriod(subADT sub){
    if (sub == NULL){
        errno = PARAMERR;
        return;
    }
    for (int i = 0; i < CANTWEEKDAYS; i++){
        for (int j = 0; j < CANTPERIODS; j++){
            sub->days[j][i] = TopPeriodStation(sub,i,j);
        }
    }
}

static size_t TopPeriodStation (subADT sub, int weekday, int period){
    size_t TopID;
    size_t TopStationPassen = 0;
        for (size_t j = 0;  j < sub->dimStation; j++){ //Recorro las estaciones dentro de las lineas
            if (TopStationPassen <= sub->station[j].days[period][weekday]){
                if ((TopStationPassen == sub->station[j].days[period][weekday] && (strcasecmp(sub->station[TopID].name,sub->station[j].name)) > 0) ||  TopStationPassen != sub->station[j].days[period][weekday]){ //Un quilombo el strcasecmp puede estar mal revisar.
                    TopStationPassen = sub->station[j].days[period][weekday];
                    TopID = j;
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
    (*line) = sub->station[id].line;
    return sub->station[id].name;
}



static void TopStationMonth(subADT sub){
        size_t yearEnd;
        for(size_t j=0; j < sub->dimStation; j++){
            char * topMonth = 0;
            float monthAvg = 0;
            if (sub->yearEnd == 0){
                yearEnd = sub->station[j].maxYear;
            } else{
                yearEnd = sub->yearEnd;
            }
            size_t topYear = bestStationMonth(sub->station[j], topMonth, monthAvg, sub->yearStart, yearEnd);

            sub->list4 = createAvgTopRec(sub->list4, &topMonth, topYear, monthAvg, sub->station[j].line, sub->station[j].name);
        }

}

static size_t bestStationMonth(Tstation station, char * topMonth, float monthAvg, size_t start, size_t end){
    size_t maxYear=0;
    for(size_t i=start; i < end; i++){ //Aca si end es
        for(int j=0; j < TOTALMONTH; j++){
            float tempAvg = station.historyMonth[i][j].totalMonth / station.historyMonth[i][j].numDay; //SI NO FUNCIONA Q4 DAR VUELTA I Y J
            if(monthAvg  <= tempAvg){
                if((monthAvg  == tempAvg && maxYear < i) || (maxYear == i && (*topMonth) < j) || monthAvg < tempAvg){
                    monthAvg  = tempAvg;
                    maxYear = i;
                    topMonth = j;
                }
            }
        }
    }
    return maxYear;
}

static avgTop * createAvgTopRec(avgTop * list, char topMonth, size_t topYear, float monthAvg, char line, char * name){
    if(list == NULL || monthAvg >= list->avg){
        if((monthAvg == list->avg && strcasecmp(list->name, name) > 0) || monthAvg >= list->avg || list == NULL){
            avgTop * aux = malloc(sizeof(struct avgTop));
            if(aux == NULL){
                errno = MEMERR;
                return list;
            }
            aux->avg = monthAvg;
            aux->line= line;
            aux->month = topMonth;
            aux->name = name;
            aux->year = topYear;
            aux->tail = list;
            return aux;
        }
    }
    list->tail = createAvgTopRec(list->tail, topMonth, topYear, monthAvg, line, name);
    return list;
}

void toBeginAvgTop(subADT sub){
    if (sub == NULL){
        errno = PARAMERR;
        return;
    }
    TopStationMonth(sub);
    sub->it4 = sub->list4;
}

int hasNextAvgTop(subADT sub){
    if (sub == NULL){
        errno = PARAMERR;
        return;
    }
    return sub->it4 != NULL && sub->it4->tail != NULL;
}

float NextAvgTop(subADT sub, char * station, char * line, size_t * year, char * month){
    if (sub == NULL){
        errno = PARAMERR;
        return -1; //Habria que cambiar esto.
    }
    float avg;
    (*station) = sub->it4->name;
    (*line) = sub->it4->line;
    (*year) = sub->it4->year;
    (*month) = sub->it4->month;
    avg = sub->it4->avg;
    if (hasNextAvgTop(sub)){
        sub->it4 =sub->it4->tail;
    }
    return avg;

}

void freeSub(subADT sub){
    for (int i = 0; i < sub->dimStation; i++){
        free(sub->station[i].name); //Here we free the names of the stations.
        for(int j = sub->yearStart; j < sub->yearEnd; j++){
            free(sub->station[i].historyMonth[j]);
        }
        free(sub->station[i]);
    }
    freeList(sub->list1);
    freeList(sub->list4); //Aca esta mal
    free(sub);

}

static void freeList(Tlist list){
    if (list == NULL){
        return;
    }
    freeList(list->tail);
    free(list);

}

//COMO CHEQUEAR QUE ME PASARON UN VECTOR QUE ESTA BIEN 
char nextTopbyLine(subADT sub, char * res[TOP]){
    errno=OK;
    if(sub==NULL){
        errno=PARAMERR;
        return ERROR;
    }
    int it=sub->it2;
    //To make sure the response matrix has a 0 if there are less than 3 stations in the line
    for(int i=0; i<TOP; i++){
        res[i]=0;
    }
    returnTopbyLine(sub->lines[it].top, res);
    sub->it2++;
    char line = ((char) it)+DIFF; 
    return line;
}

//REVISAR
static void returnTopbyLine(Tlist list, char ** res){
    if(list==NULL){
        return;
    }
    res[0]= list->name;
    returnTopbyLine(list->tail, res+1);
}

/* COSAS Q faltan:
 * Q1: en el toBegin del Q1 hay q llamar al StationLineTop, pq carga la cantindad de pasajeros por linea y llamar addListAmountPassen.
 * Q2: Falta terminar funciones de front.
 * Q4: Funciones de front, toBegin y los iteradores y next.
 *
 * FrontEnd:
 * Hacer las funciones de los Queries.
 *
 */