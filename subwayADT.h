#ifndef TPE_G1_SUBWAYADT_H
#define TPE_G1_SUBWAYADT_H

typedef struct subCDT * subADT;

subADT newSub(size_t startYear, size_t endYear);

// For the correct use of de TAD, you need to first use the function addStations, to add the name, id and line of each station , this will then enable you
// to use the function addDataTrips.

//PODRIAMOS AGREGAR ARG DE STARTING Y ENDING YEAR Y ACLARAR QUE EN EL FRONT ES NECESAROP
// MANDAR UN 0 EN AMBOS PARAMETROS SI NO NOS PASARON NADA COMO ARGUMENTO, 
//asi nos ahorramos hacer la matriz de history tan grande ya desde el principio
void addStations(subADT sub, char line, char * name, size_t stationID);

// This function adds the data from each trip.


void addDataTrips(subADT sub, char day, char month, size_t year, size_t stationID, size_t cantPassen, char start, char end);

//sets an iterator to start looking at the lines from the one with most passengers to the one with the least
void toBeginLines(subADT sub); //CONVIENE HACERLO CON RETURN INT PARA CHEQUEAR QUE SALIO TODO OK?

//sets an iterator to start looking at the lines in 
//alphabetic order and with the 3 stations with most passengers from each line 
void toBeginTopbyLine(subADT sub);

//returns if there is another line next
int hasNextLine(subADT sub);
int hasNextTopbyLine(subADT sub);

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

//Prepares data to get the top stations of a given period and day of week.
void toBeginTopPeriod(subADT sub);


//This function receives an ADT, a period, a weekday and a pointer to a char so it gives the name of the line.
//It returns the name of the top station requested.
char * getTopStationPeriod (subADT sub, int period, int weekday, char * line)


//This function prepares the data to get the year and month of best average of each station.
void toBeginAvgTop(subADT sub);

//This function checks if there is a next line
//It returns 1 if there is, 0 if there isn't.
int hasNextAvgTop(subADT sub);


//This function gives the data needed, user must pass a char pointer to get name of the station, the line and the month
//on the other hand it must pass a size_t pointer to get the year.
//Function return the average of the best month. 
float NextAvgTop(subADT sub, char * station, char * line, size_t * year, char * month);



void freeSub(subADT sub);

//ENUM ERRORES:
enum{OK=0, MEMERR, TOKENERR, ARGERR, OPENERR, NEXTERR, ERRID, ERRDATE, PARAMERR};
/*
MEMERR: Memory error
TOKENERR: Token error
ARGERR: Error in the quantity of archives
OPENERR: Error opening archives
NEXTERR: Error on the iterator
ERRID: Error on a invalid id
ERRDATE: Error on a invalid date
PARAMERR: Error in the parameter provided by the frontend
*/

#endif //TPE_G1_SUBWAYADT_H
