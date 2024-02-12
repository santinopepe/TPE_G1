#include "subwayADT.h"
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <strings.h>

#define DIFF ('A')

#define POS(n) ((n) - DIFF) // This macro gives us the position in the vector according to the line.

#define PERIODSINTER 2 // This gives us the boundaries of the periods.

#define LEAPYEAR 1 // This is used as a return if it is a leap year.

#define NOID -1 // This is a return in case there is no station that meets with the requirements.

#define ERROR -1 // This is a return in case of an error.

#define FEB 2 // This is the number the month february occupies.

#define JAN 1 // This is the month number that january occupies.

#define TOTALMONTH 12


/*---------------------------------------------------------- STRUCTS ----------------------------------------------------------------*/
typedef struct node{
    char * name; // In this variable can carry the name of the station or the subway line, depending on the use.
    size_t numTot; // Amount of passengers.
    struct node * tail;
}Tnode;

typedef struct avgTop{
    char * name;
    char line;
    float avg; // Average is calculated Amount of passengers in a month divided by the amount of days of the month.
    int year;
    char month;
    struct avgTop * tail;
}avgTop;

typedef struct node * Tlist;

typedef struct Tmonth{
    size_t totalMonth; //Amount of passengers in a given month.
    int numDay; // The amount of days in a given month.
}Tmonth;

typedef struct Tstation{
    char * name;
    char line;
    size_t passenStation; // Total amount of passengers of a station.
    size_t days[CANTPERIODS][CANTWEEKDAYS]; // This matrix contains the amount of passengers on a given period and weekday,
                                            // of a specific station.
    Tmonth * historyMonth[TOTALMONTH]; // Matrix that contains the amount of passengers in a given month and year.
    int maxYear[TOTALMONTH]; // Vector that contains the maximum year of each month. (Inside the given range)
    int minYear[TOTALMONTH]; //  Vector that contains the minimum year of each month. (Inside the given range)

}Tstation;

typedef struct Tline{
    size_t passenTot; // Total passengers per line
    Tlist top; //Top 3 stations with most passengers per line
}Tline;


typedef struct subCDT{
    Tstation * station; // Vector where each position corresponds to the id of a station and has its information.
    size_t dimStation; // Dimension of the vector station.
    size_t minID; // Minimum id between all stations.

    Tline * lines; // Vector where each position corresponds to a subway line. (They are in alphabetical order).
    size_t dimLines; // Dimension of the lines vector.
    size_t it2; // Iterator that contains the position of each subway line, on the lines vector.

    Tlist list1; // List that is ordered in a decedent manner,
                // and contains the subway line and the amount of passengers recorded.
    Tlist it1; // Iterator of list1.

    int days[CANTPERIODS][CANTWEEKDAYS]; //Matrix that contains the id of the best station on a given period and weekday.

    avgTop * list4; //A list that is ordered in a decedent manner (taking into account the month average).
    avgTop * it4; //Iterator of list4.


    size_t yearEnd; // If end year is passed as a argument, this variable has its value, otherwise it is 0.
    size_t yearStart; // If a start year is passed as a argument, this variable has its value, otherwise it is 0.
}subCDT;


/*-------------------------------------------------------------------------- PROTOTYPES --------------------------------------------------------------------------------------------*/



// Recursive function that adds a node to a list of lines that goes
// from the one with most passengers to the one with the least passengers.
static Tlist addListAmountPassenRec(Tlist list, size_t numPassen, char line);

// Function that iterates in a vector with all the lines to create
// a list that goes from the line with the most passengers to the one with the least
// in case 2 lines have the same number of passengers they go in alphabetical order.
static void addListAmountPassen(subADT sub);

// This function gives a number from 0 to 3 indicating what period the journey was made.
// It returns a number from 0 to 3, this correlates with a given period.
static int getPeriod( char endHour);

// It checks if the year is a leap year or not.
// Returns LEAPYEAR or !LEAPYEAR.
static char leapYearCalc(size_t year);

// Given a month number and if it is a leap year it returns the number of days that the month haves.
static int getDayOfMonth(char month, char leap);

// This function gives us what day of a week a certain date represents.
// For example, for 2/02/2024 it will give us the number 5, as that number represents friday.
static int getDayOfWeek(size_t day, size_t month, size_t year, size_t leapYear);

// This function creates a vector with the lines with a list that
// contains the top 3 stations and the total of passengers of each line.
static void StationLineTop (subADT sub);

// This is a recursive function that helps the creation of the top 3.
// It returns the top 3 stations of each line or less if there are fewer stations.
static Tlist StationLineTopRec (Tlist top, size_t NumPassen, char * StationName, int  * TopFlag);

// Function that puts the names of the top3 stations in a line in the vector res
// in case the line has less than 3 stations the positions in the vector stay empty
static void returnTopbyLine(Tlist list, char ** res, int * flag);

// Returns the ID of the most popular station by period and weekday,
// it complements the following function PeriodWeekDayTop.
static size_t TopPeriodStation (subADT sub, int weekday, int period);

// This function calls bestStationMonth in order to get the best month of every station and the call the function createAvgTopRec
// to sort the data.
static void TopStationMonth(subADT sub);


// This function receives a char pointer to get the best month a station, a float pointer to get the best month average
// of the station, also it receives the start year of the information.
// It returns the year of the best month average.
static size_t bestStationMonth(Tstation station, char * topMonth, float * monthAvg, size_t startYear);

// Creates a list that is ordered from the highest month average to the lowest one, in case it's the same it
// prioritize in alphabetical order. It receives the list, the best average for that station
// and the date when it happened, month and year, and the line and name of the station.
static avgTop * createAvgTopRec(avgTop * list, char * topMonth, size_t topYear, float * monthAvg, char line, char * name);

// Recursive function that frees the list.
static void freeList(Tlist list);

// Recursive function that frees the list.
static void freeLine(Tlist list);

// Recursive function that frees the top average list.
static void freeListAVG(avgTop * list);

/*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

subADT newSub(size_t startYear, size_t endYear){
    errno = OK;
    subADT aux = calloc(1,sizeof(subCDT));

    if (errno == ENOMEM || aux == NULL){
        errno = MEMERR;
        return NULL;
    }

    aux->yearStart = startYear;
    aux->yearEnd = endYear;
    return aux;

}


void addStations(subADT sub, char line, char * name, size_t stationID){
    errno=OK;

    if(sub->minID == 0 || sub->minID > stationID){
        sub->minID = stationID;
    }

    if(stationID >= sub->dimStation){
        sub->station = realloc(sub->station, (stationID+1)*sizeof(Tstation));
        if(errno==ENOMEM || sub->station == NULL){
            errno = MEMERR;
            return;
        }
        for(size_t j=sub->dimStation; j<=stationID; j++){
        sub->station[j].passenStation=0; // passenStation counts the passengers so if it has
        sub->station[j].name=NULL;                                //  rubbish data in it the results won't be accurate
        for(int i=0; i<TOTALMONTH; i++){
            //  MaxYear indicates the highest year of each month, minYear indicates the lowest year it appears of each month,
            //  there are used to know the size of the vector and where start looking for the information.
            //  If it has rubbish it won't be possible to enlarge it correctly,
            sub->station[j].maxYear[i] = 0;
            sub->station[j].minYear[i] = 0;
            sub->station[j].historyMonth[i] = NULL;
        }
        for(int i = 0; i < CANTWEEKDAYS; i++){
            for(int k = 0; k < CANTPERIODS; k++){
                sub->station[j].days[k][i] = 0; // We set on 0 the matrix as it is used adding amount of passengers.
            }
        }
    }
    sub->dimStation=stationID+1; //As it is the highest id recorded it dimension should be it.
    }

    line = toupper(line); // It is used just in case a lowercase letter is passed as a parameter.
    sub->station[stationID].line = line;
    sub->station[stationID].name = malloc(strlen(name)+1);
    if(errno==ENOMEM || sub->station[stationID].name == NULL){
        errno = MEMERR;
        return;
    }
    strcpy(sub->station[stationID].name, name);
}




void addDataTrips(subADT sub, char day, char month, int year, int stationID, int numPassen, char end){
    errno = OK;

    if (sub->dimStation >= stationID && sub->station[stationID].name != NULL ){ // This condition helps us know if the station id
                                                                              // is from a valid station.
        sub->station[stationID].passenStation += numPassen;
        int max = sub->station[stationID].maxYear[month-1]; //Max year of a given month.

        char leap = leapYearCalc(year);

        sub->station[stationID].days[getPeriod(end)][getDayOfWeek(day,month,year,leap)] += numPassen;

        //It is used to enlarge the history month matrix.
        if(max < year && (sub->yearEnd==0 || year <= sub->yearEnd) && (year >= sub->yearStart)){
            sub->station[stationID].historyMonth[month-1] = realloc(sub->station[stationID].historyMonth[month-1], (year-sub->yearStart + 1)*sizeof(Tmonth));
            if (errno == ENOMEM || sub->station[stationID].historyMonth[(int)month-1] == NULL ){
                errno = MEMERR;
                return;
            }
            if(max != 0){
                max-=sub->yearStart; //Used to fill correctly the data.
            }
            for(int i=max; i <= year-sub->yearStart; i++){
                sub->station[stationID].historyMonth[month-1][i].totalMonth = 0;
                sub->station[stationID].historyMonth[month-1][i].numDay = getDayOfMonth(month, leap);
            }
            if((year<=sub->yearEnd || sub->yearEnd==0) && year > sub->station[stationID].maxYear[month-1]){
                sub->station[stationID].maxYear[month-1]=year;
            }
        }

        if((sub->yearEnd==0 || year <= sub->yearEnd) && (year >= sub->yearStart)){
            sub->station[stationID].historyMonth[month-1][year-sub->yearStart].totalMonth += numPassen;
        }

        if((year <= sub->yearEnd || sub->yearEnd==0) && (year >= sub->yearStart) && (sub->station[stationID].minYear[month-1] == 0  || sub->station[stationID].minYear[month-1] > year)){
            sub->station[stationID].minYear[month-1] = year;
        }


    }

}

static int getDayOfMonth(char month, char leap){
    char daysOfMonth[] = {31,28,31,30,31,30,31,31,30,31,30,31};
    if(month==FEB){
        return daysOfMonth[(int)month-1]+leap; //If it is february and a leap year then the month has 29 days.
    }
        return daysOfMonth[(int)month-1];
}



static int getPeriod(char endHour){
    errno = OK;
    //Matrix that contains the boundaries of the intervals.
    char periods [][PERIODSINTER] = {{6,11}, {11,13},{13,17},{17,6}};

    for (int i = 0; i < CANTPERIODS-1; i++) {
        if ( (periods[i][0]<=endHour) && (endHour <= periods[i][1]) ){
            return i;
        }
    }
    if(periods[CANTPERIODS-1][0]<=endHour || periods[CANTPERIODS-1][1]>=endHour){
        return CANTPERIODS-1; //If the hour of the data falls in the last period it needs a different condition.
    }
    errno = PARAMERR;
    return ERROR;
}

static char leapYearCalc(size_t year){
    if ( (year % 4 == 0 && year % 100 != 0) || year % 400 == 0 ){ //These are the conditions to know if a given year is a leap year.
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
    //to make a list with the lines in order of how many passengers they have when the user/frontend asks for it
    addListAmountPassen(sub);
    sub->it1=sub->list1;
}


static void StationLineTop (subADT sub){
    errno = OK;
    for(size_t j = sub->minID; j < sub->dimStation; j++){ // We move inside each line to every station.
        if(sub->station[j].name != NULL){ // We check that the station exists.
            
            char line = sub->station[j].line;

            if(sub->dimLines <= POS(line)){
                sub->lines = realloc(sub->lines, (POS(line)+1)*sizeof(Tline));
                if(sub->lines == NULL || errno==ENOMEM){
                    errno=MEMERR;
                    return;
                }
                for(size_t i=sub->dimLines; i < POS(line) + 1; i++){
                    sub->lines[i].passenTot = 0;
                    sub->lines[i].top = NULL;
                }
                sub->dimLines = POS(line)+1;
            }

            int TopFlag = 0; //This flag helps not create all the list given that if a station doesn't enter top 3  it quits the comparison.
            sub->lines[POS(line)].passenTot += sub->station[j].passenStation;
            sub->lines[POS(line)].top = StationLineTopRec(sub->lines[POS(line)].top, sub->station[j].passenStation, sub->station[j].name, &TopFlag);
        }
    }
}


static Tlist StationLineTopRec (Tlist top, size_t NumPassen, char * StationName, int * TopFlag){
    errno = OK;
    if (top == NULL || NumPassen > top->numTot || (NumPassen == top->numTot && strcasecmp(top->name,StationName) > 0)){
        Tlist aux = malloc(sizeof(struct node));
        if(errno == ENOMEM || aux == NULL){
            errno= MEMERR;
            return top;
        }
        aux->name = StationName;
        aux->numTot = NumPassen;
        aux->tail = top;
        return aux;
    }

    (*TopFlag)++;
    if ((*TopFlag) == 3){ // If this condition is true the station doesn't enter top 3, so we don't consider it.
        return top;
    }
    top->tail = StationLineTopRec(top->tail, NumPassen, StationName, TopFlag);
    return top;
}


static void addListAmountPassen(subADT sub){
    for (size_t i=0; i < sub->dimLines; i++){
        if(sub->lines[i].passenTot != 0){ // We check that the line has passengers, if they don't we don't include them.
            sub->list1=addListAmountPassenRec(sub->list1, sub->lines[i].passenTot, (char)i + DIFF);
        }
    }
}


static Tlist addListAmountPassenRec(Tlist list, size_t numPassen, char line){
    errno=OK;

    if(list==NULL || list->numTot < numPassen || (list->numTot == numPassen && (line < list->name[0]))){
        Tlist aux = malloc(sizeof(struct node));
        if(errno == ENOMEM || aux == NULL){
            errno = MEMERR;
            return list;
        }
        aux->name=malloc(sizeof(char)+1);
        if(errno == ENOMEM || aux->name == NULL){
            errno = MEMERR;
            return list;
        }
        aux->name[0]= line;
        aux->name[1] = '\0';
        aux->numTot=numPassen;
        aux->tail = list;

        return aux;
    }

    list->tail = addListAmountPassenRec(list->tail, numPassen, line);
    return list;
}

int hasNextLine(subADT sub){
    errno = OK;
    if(sub != NULL){
        return sub->it1 != NULL;
    } else{
        errno = PARAMERR;
        return 0;
    }
}

int nextLine(subADT sub, char * line){
    errno = OK;
    if (sub == NULL){
        errno = PARAMERR;
        return ERROR;

    }
    strcpy(line, sub->it1->name);
    int res=sub->it1->numTot;
    sub->it1=sub->it1->tail;
    return res;
}


void toBeginTopbyLine(subADT sub){
    errno = OK;
    if (sub == NULL){
        errno = PARAMERR;
        return;
    }
    sub->it2=0;
}


int hasNextTopbyLine(subADT sub){
    errno = OK;
    if (sub == NULL){
        errno = PARAMERR;
        return ERROR;

    }
    return (sub->it2 < sub->dimLines);
}

char nextTopbyLine(subADT sub, char * res[TOP]){
    errno=OK;
    if(sub==NULL || res == NULL){
        errno = PARAMERR;
        return ERROR;
    }
    while(sub->lines[sub->it2].top==NULL){
        sub->it2++;
    }
    int it=sub->it2;
    //To make sure the response matrix has a 0 if there are less than 3 stations in the line
    for(int i=0; i<TOP; i++){
        res[i]=NULL;
    }
    int flag = 0;


    returnTopbyLine(sub->lines[it].top, res, &flag);

    sub->it2++;

    char line = ((char) it)+DIFF; //As it is a number, we add DIFF to get the subway line.
    return line;
}


static void returnTopbyLine(Tlist list, char ** res, int * flag){
    if(list==NULL || (*flag) == TOP){ //If flag is equal to TOP, it means that the information needed is stored.
        return;
    }
    res[(*flag)]= list->name;
    (*flag)++;
    returnTopbyLine(list->tail, res, flag);
}



void toBeginTopPeriod(subADT sub){
    errno = OK;
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
    size_t TopID=sub->minID;
    size_t TopStationPassen = 0;
    for (size_t j = sub->minID;  j < sub->dimStation; j++){
        if (sub->station[j].name != NULL && TopStationPassen <= sub->station[j].days[period][weekday]){
            if (TopStationPassen != sub->station[j].days[period][weekday] || (TopStationPassen == sub->station[j].days[period][weekday] && (strcasecmp(sub->station[TopID].name,sub->station[j].name) > 0))){
                TopStationPassen = sub->station[j].days[period][weekday];
                TopID = j;
            }
        }
    }
    if (TopStationPassen == 0){ // If the value never was modified, there is no data for that weekday period.
        return NOID;
    }
    return TopID;
}

char * getTopStationPeriod (subADT sub, int period, int weekday, char * line){
    errno = OK;
    if ( (period < 0 || period > CANTPERIODS) || (weekday > CANTWEEKDAYS || weekday < 0)){ // We check parameters
        line = NULL;
        errno = PARAMERR;
        return NULL;
    }
    int id = sub->days[period][weekday];
    if (id == NOID){ // In the case where there is no data we should print NOTOPSTATION ("S/D").
        line = NULL;
        return NOTOPSTATION;
    }
    line[0] = sub->station[id].line;
    line[1] = '\0';
    return sub->station[id].name;
}


void toBeginAvgTop(subADT sub){
    errno = OK;
    if (sub == NULL){
        errno = PARAMERR;
        return;
    }
    TopStationMonth(sub);
    sub->it4 = sub->list4;
}



static void TopStationMonth(subADT sub){
    for(size_t j=sub->minID; j < sub->dimStation; j++){
        if(sub->station[j].name != NULL){
            char  topMonth = 0;
            float  monthAvg = 0;
            size_t topYear = bestStationMonth(sub->station[j], &topMonth, &monthAvg, sub->yearStart);
            if(monthAvg!=0){
                sub->list4 = createAvgTopRec(sub->list4, &topMonth, topYear, &monthAvg, sub->station[j].line, sub->station[j].name);
            }
        }
    }
}



static size_t bestStationMonth(Tstation station, char * topMonth, float * monthAvg, size_t startYear){
    size_t maxYear=0;
    float mAvg = 0;
    char tMonth=0;
    for(int j=0; j < TOTALMONTH ; j++){
        int start = station.minYear[j];
        int end=station.maxYear[j];
          for(size_t i=start-startYear; i <= end-startYear && station.minYear[j]!=0 && station.maxYear[j]!=0 ; i++){

                float num = station.historyMonth[j][i].totalMonth;
                float denom = station.historyMonth[j][i].numDay;
                float tempAvg = (num / denom);
                if(mAvg<=tempAvg){
                    if(mAvg  < tempAvg || (mAvg == tempAvg && maxYear < i) || (mAvg == tempAvg && maxYear == i && tMonth < j)){
                        mAvg = tempAvg;
                        maxYear = i;
                        tMonth = j;
                    }
                }

        }
    }
    (*topMonth) = tMonth;
    (*monthAvg) = mAvg;
    return maxYear + startYear; // As max year is a position of a vector, which is affected with the startYear,
                                // to get the correct date we should add startYear.
}

static avgTop * createAvgTopRec(avgTop * list, char * topMonth, size_t topYear, float * monthAvg, char line, char * name){
    errno = OK;
    if(list == NULL || (*monthAvg)  > list->avg || (((*monthAvg) == list->avg) && strcasecmp(list->name, name) > 0) ){
            avgTop * aux = malloc(sizeof(struct avgTop));
            if(aux == NULL){
                errno = MEMERR;
                return list;
            }
            aux->avg = (*monthAvg);
            aux->line = line;
            aux->month = (*topMonth);
            aux->name = name;
            aux->year = topYear;
            aux->tail = list;
            return aux;

    }
    list->tail = createAvgTopRec(list->tail, topMonth, topYear, monthAvg, line, name);
    return list;
}


int hasNextAvgTop(subADT sub){
    errno = OK;
    if (sub == NULL){
        errno = PARAMERR;
        return ERROR;
    }
    return sub->it4 != NULL;
}

float NextAvgTop(subADT sub, char * station, char * line, size_t * year, char * month){
    errno = OK;
    if (sub == NULL){
        errno = PARAMERR;
        return ERROR;
    }
    float avg;
    strcpy(station, sub->it4->name);
    line[0] = sub->it4->line;
    line[1] = '\0';
    (*year) = sub->it4->year;

    if (sub->it4->month == 0){ //Inside our program january is represented as 0, so we need to change it.
        (*month) = JAN;
    } else{
        (*month) = sub->it4->month;
    }

    avg = sub->it4->avg;
    sub->it4 =sub->it4->tail;

    return avg;
}



void freeSub(subADT sub){
    for (int i = sub->minID; i < sub->dimStation; i++){
        free(sub->station[i].name);
        for(int j = 0; j < TOTALMONTH; j++){
            free(sub->station[i].historyMonth[j]);
        }

    }
    free(sub->station); //After we free the name and the matrix we free the station.

    for(int i =0; i<sub->dimLines; i++){
        freeList(sub->lines[i].top);
    }
    free(sub->lines); //After we free the top list we free the line.

    freeLine(sub->list1);

    freeListAVG(sub->list4);
    free(sub); //After we free the contents of the CDT we free the ADT.
}

static void freeList(Tlist list){
    if (list == NULL){
        return;
    }
    freeList(list->tail);
    free(list);

}

static void freeListAVG(avgTop * list){
    if (list == NULL){
        return;
    }
    freeListAVG(list->tail);
    free(list);

}

static void freeLine(Tlist list){
    if (list == NULL){
        return;
    }
    freeLine(list->tail);
    free(list->name);
    free(list);
}
