#ifndef TPE_G1_SUBWAYADT_H
#define TPE_G1_SUBWAYADT_H

typedef struct subCDT * subADT;

subADT newSub(void);

// For the correct use of de TAD, you need to first use the function createStations, this will then enable you
// to use the function addData.
void createStations(subADT sub, char line, char * name, size_t stationID);

// This function adds the data that later will be processed.
void addData(subADT sub, char day, char month, size_t year, size_t stationID, size_t cantPassen, char start, char end);

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
