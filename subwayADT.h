#ifndef TPE_G1_SUBWAYADT_H
#define TPE_G1_SUBWAYADT_H

#include <stdlib.h>
#include <stddef.h>

typedef struct subCDT * subADT;

#define TOP 3 // This define is used for the best 3 stations of each line.

#define CANTWEEKDAYS 7 // This gives us the amount of days in a week.

#define CANTPERIODS 4 // This gives us the number of periods.

#define NOTOPSTATION "S/D" // This appears there is no data for a specific station in queries.


// This function creates a new ADT, it receives as parameters startYear and endYear,
// in case there are no extra parameters that represent the starting and/or ending year to compare the month
// with the highest average of each station please enter a 0 (zero) in the corresponding argument.
subADT newSub(size_t startYear, size_t endYear);

// For the correct use of de TAD, you need to first use the function addStations, to add the name, id and line of each station ,
// this will then enable you to use the function addDataTrips.

// This function adds the information of each station.
// It receives the line, the station name and the station id.
void addStations(subADT sub, char line, char * name, size_t stationID);


// This function saves the data from each trip.
// It receives a date: day, month, year. The station id which the information corresponds,
// the amount of passengers between the periods start and end hour.
void addDataTrips(subADT sub, char day, char month, int year, int stationID, int cantPassen, char end);

// Sets an iterator to start looking at the lines from the one with
// most passengers to the one with the least and sorts the data.
void toBeginLines(subADT sub);

// Returns if there is another subway line next with fewer passengers than the one before.
// Returns 1 in case there is another line, 0 if there isn't.
int hasNextLine(subADT sub);

// Returns the number of passengers in the line and uses the parameter line to return the line letter
// to which the amount of passengers belongs and passes to the next subway line.
int nextLine(subADT sub, char * line);

// Sets an iterator to start looking at the lines in
// alphabetic order and with the 3 stations with most passengers from each line.
// In the case there are less than 3 stations it points to NULL.
void toBeginTopbyLine(subADT sub);

// Returns if there is another subway line next in alphabethic order.
// Returns 1 in case there is another line, 0 if there isn't.
int hasNextTopbyLine(subADT sub);

// Uses the matrix res to return the top 3 stations with most passengers from one line.
// If there is an empty space in the matrix is because the line has less than 3 stations,
// returns the letter of the line and changes the iterator to the next line in alphabetic order.
char nextTopbyLine(subADT sub, char * res[TOP]);

// Prepares data to get the top stations of a given period and day of week.
void toBeginTopPeriod(subADT sub);

// This function receives an ADT, a period, a weekday and a pointer to a char, it gives the name of the line.
// It returns the name of the top station requested.
char * getTopStationPeriod (subADT sub, int period, int weekday, char * line);

// This function sorts the data to get the best average of station,
// followed by the year and month when occurred.
void toBeginAvgTop(subADT sub);

// This function checks if there is a next station
// It returns 1 if there is, 0 if there isn't.
int hasNextAvgTop(subADT sub);

// This function gives the data needed, user must pass a various char pointer to get name of the station, the line and the month,
// on the other hand it must pass a size_t pointer to get the year.
// Function return the average of the best month.
float NextAvgTop(subADT sub, char * station, char * line, size_t * year, char * month);

// This function frees the space reserve by the TAD.
void freeSub(subADT sub);


//ERRORS ENUM:
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
