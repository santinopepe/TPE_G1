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

void freeSub(subADT sub);

//ENUM ERRORES:
enum{OK=0, MEMERR, TOKENERR, ARGERR, OPENERR, NEXTERR,ERRID, ERRDATE};
/*
MEMERR: error de memoria
TOKENERR: error en funcion strtok
ARGERR: error cant archivos le pasan
OPENERR: error abrir archivos
NEXTERR: error en next (iterador)
ERRID: te pasan un id no valido
ERRDATE: te pasan un dia no valido
*/

#endif //TPE_G1_SUBWAYADT_H
