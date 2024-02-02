#include <stdio.h>
#include <string.h>
#include "subwayADT.h"


#define MAX_CHARS 50
#define DELIM ";"
#define CHANGE_LINE "\n"
#define DATE_DELIM "/"
#define HOUR_DELIM ":"

subADT readStations(FILE * stations, subADT sub);
void readTurnstiles(subADT sub, FILE * turnstiles);
char * copyName(char * name, char * aux);

void query1(subADT sub);
void query2(subADT sub);
void query3(subADT sub);


int main(int numArg, char * argv[]){
    if(numArg <= 3 && numArg >= 5){
        fprintf(stderr, "Error, amount of arguments not valid");
        exit(ARGERR);
    }

    FILE * fileTurnstiles = fopen(argv[1], "rt");
    FILE * fileStations = fopen(argv[2], "rt");

    if(fileTurnstiles == NULL || fileStations == NULL){
        fprintf(stderr, "Error, could not open files");
        exit(OPENERR);
    }
    
    int year_start = 0, year_end = 0;

    if (numArg == 4) {
        year_start = atoi(argv[3]);
        if (year_start <= 0) {
            fprintf(stderr, "Error, imput date not valid");
            exit(ERRDATE);
        }
    }else if (numArg == 5) {
        year_end = atoi(argv[4]);
        if (year_end <= 0 && year_start <= 0 && year_start >= year_end) {
            fprintf(stderr, "Error, imput date not valid");
            exit(ERRDATE);
        }
    }

    subADT sub = newSub(year_start, year_end);
    if(sub == NULL){
        fprintf(stderr, "Error, could not assign memory");
        exit(MEMERR);
    }

    sub = readStations(fileStations, sub);
    readTurnstiles(sub, fileTurnstiles);

    fclose(fileTurnstiles);
    fclose(fileStations);

    query1(sub);
    query2(sub);
    query3(sub);

    free(sub);
}

subADT readStations(FILE * stations, subADT sub){
    char text[MAX_CHARS];

    fscanf(stations, "%s\n", text);

    char * temp;
    char * name = NULL;
    while(fgets(text, MAX_CHARS, stations) != NULL){
        temp = strtok(text, DELIM);
        if(temp == NULL){
            fprintf(stderr, "Token error");
            exit(TOKENERR);
        }
        size_t id = atoi(temp);

        name = copyName(name, NULL);

        temp = strtok(NULL, DELIM);
        char line = temp[0];

        strtok(NULL, CHANGE_LINE);

        addStations(sub, line, name, id);
    }
    free(name);
    return sub;
}

void readTurnstiles(subADT sub, FILE * turnstiles){
    char text[MAX_CHARS];
    char day, month, start, end;
    size_t year, id, numPassen;
    char * temp;
    fscanf(turnstiles, "%s\n", text);

    while(fgets(text, MAX_CHARS, turnstiles) != NULL){
        temp = strtok(text, DATE_DELIM);
        if(temp == NULL){
            fprintf(stderr, "Token error");
            exit(TOKENERR);
        }
        day = atoi(temp);

        temp = strtok(NULL, DATE_DELIM);
        month = atoi(temp);

        temp = strtok(NULL, DELIM);
        year = atoi(temp);

        temp = strtok(NULL, HOUR_DELIM);
        start = atoi(temp);
        strtok(NULL, DELIM); //salteo los minutos

        temp = strtok(NULL, HOUR_DELIM);
        end = atoi(temp);
        strtok(NULL, DELIM); //salteo los minutos

        temp = strtok(NULL, DELIM);
        id = atoi(temp);

        temp = strtok(NULL, CHANGE_LINE);
        numPassen = atoi(temp);

        addDataTrips(sub, day, month, year, id, numPassen, start, end);
    }
}

char * copyName(char * name, char * aux){
    char * tok = strtok(aux, DELIM);
    if(tok == NULL){
        fprintf(stderr, "Token error");
        exit(TOKENERR);
    }

    name = realloc(name, strlen(tok)+1);
    if(name == NULL){
        fprintf(stderr, "Error, could not assign memory");
        exit(MEMERR);    
    }
    strcpy(name, tok);

    return name;
}
